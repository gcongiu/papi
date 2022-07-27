/**
 * @file    cpu.c
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 * @brief   This is the CPU component
 *
 */
#include "papi.h"
#include "papi_internal.h"
#include "papi_vector.h"
#include "papi_memory.h"
#include "cpu_prof.h"

static int cpu_init_component(int);
static int cpu_shutdown_component(void);
static int cpu_init_thread(hwd_context_t *);
static int cpu_init_control_state(hwd_control_state_t *);
static int cpu_dispatch_timer(int, hwd_siginfo_t *, void *);
static int cpu_start(hwd_context_t *, hwd_control_state_t *);
static int cpu_stop(hwd_context_t *, hwd_control_state_t *);
static int cpu_read(hwd_context_t *, hwd_control_state_t *, long long **, int);
static int cpu_shutdown_thread(hwd_context_t *);
static int cpu_ctl(hwd_context_t *, int , _papi_int_option_t *);
static int cpu_update_control_state(hwd_control_state_t *, NativeInfo_t *, int,
                                    hwd_context_t *);
static int cpu_set_domain(hwd_context_t *, int);
static int cpu_set_overflow(EventSetInfo_t *, int, int);
static int cpu_set_profile(EventSetInfo_t *, int, int);
static int cpu_stop_profiling(ThreadInfo_t *, EventSetInfo_t *);
static int cpu_write(hwd_context_t *, hwd_control_state_t *);
static int cpu_ntv_enum_events(unsigned int *, int);
static int cpu_ntv_name_to_code(const char *, unsigned int *);
static int cpu_ntv_code_to_name(unsigned int, char *name, int len);
static int cpu_ntv_code_to_descr(unsigned int, char *, int);
static int cpu_ntv_code_to_info(unsigned int, PAPI_event_info_t *);

static cpu_event_table_t ntv_table;

typedef struct {
    int initialized;
    int state;
    int cidx;
    cpu_event_table_t *ntv_table;
} cpu_context_t;

typedef struct {
    prof_info_t prof_info;
    prof_ctx_t *prof_ctx;
} cpu_control_t;

#define CPU_EVENTS_OPENED  0x1
#define CPU_EVENTS_RUNNONG 0x2

papi_vector_t _cpu_vector = {
    .cmp_info = {
        .name = "cpu",
        .short_name = "cpu",
        .version = "1.0",
        .description = "Access to CPU performance counters",

        .default_domain = PAPI_DOM_USER,
        .available_domains = PAPI_DOM_USER | PAPI_DOM_KERNEL | PAPI_DOM_SUPERVISOR,
        .default_granularity = PAPI_GRN_THR,
        .available_granularities = PAPI_GRN_THR | PAPI_GRN_SYS,

        .hardware_intr = 1,
        .kernel_profile = 1,

        /* component specific cmp_info initializations */
        .fast_virtual_timer = 0,
        .attach = 1,
        .attach_must_ptrace = 1,
        .cpu = 1,
        .inherit = 1,
        .cntr_umasks = 1,

        .kernel_multiplex = 1,
        .num_mpx_cntrs = PERF_EVENT_MAX_MPX_COUNTERS,
    },

    /* sizes of framework-opaque component-private structures */
    .size = {
        .context = sizeof(cpu_context_t),
        .control_state = sizeof(cpu_control_t),
        .reg_value = sizeof(int),
        .reg_alloc = sizeof(int),
    },

    .init_component = cpu_init_component,
    .shutdown_component = cpu_shutdown_component,
    .init_thread = cpu_init_thread,
    .init_control_state = cpu_init_control_state,
    .dispatch_timer = cpu_dispatch_timer,

    .start = cpu_start,
    .stop = cpu_stop,
    .read = cpu_read,
    .shutdown_thread = cpu_shutdown_thread,
    .ctl = cpu_ctl,
    .update_control_state =  cpu_update_control_state,
    .set_domain = cpu_set_domain,
    .reset = cpu_reset,
    .set_overflow = cpu_set_overflow,
    .set_profile = cpu_set_profile,
    .stop_profiling = cpu_stop_profiling,
    .write = cpu_write,

    .ntv_enum_events = cpu_ntv_enum_events,
    .ntv_name_to_code = cpu_ntv_name_to_code,
    .ntv_code_to_name = cpu_ntv_code_to_name,
    .ntv_code_to_descr = cpu_ntv_code_to_descr,
    .ntv_code_to_info = cpu_ntv_code_to_info,
};

int
cpu_init_component(int)
{
    int papi_errno = PAPI_OK;
    const char *err_string = NULL;

    papi_hwi_lock(COMPONENT_LOCK);

    papi_errno = prof_init(&ntv_table, &err_string);
    if (papi_errno != PAPI_OK) {
        int expect = snprintf(_cpu_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN, "%s", err_string);
        if (expect > PAPI_MAX_STR_LEN) {
            SUBDBG("disabled_reason truncated");
        }
        goto fn_fail;
    }

    _cpu_vector.cmp_info.num_native_events = ntv_table.count;
    _cpu_vector.cmp_info.num_cntrs = ntv_table.count;

  fn_exit:
    _cpu_vector.cmp_info.initialized = 1;
    _cpu_vector.cmp_info.disabled = papi_errno;
    papi_hwi_unlock(COMPONENT_LOCK);
    return papi_errno;
  fn_fail:
    goto fn_exit;
}

int
cpu_shutdown_component(void)
{
    int papi_errno = PAPI_OK;

    papi_errno = prof_shutdown();

    return papi_errno;
}

int
cpu_init_thread(hwd_context_t *ctx)
{
    int papi_errno = PAPI_OK;
    cpu_context_t *cpu_ctx = (cpu_context_t *) ctx;

    memset(cpu_ctx, 0, sizeof(*cpu_ctx));
    cpu_ctx->initialized = 1;
    cpu_ctx->event_table = &ntv_table;
    cpu_ctx->cidx = _cpu_vector.cmp_info.CmpIdx;

    return papi_errno;
}

int
cpu_init_control_state(hwd_control_state_t *ctl)
{
    int papi_errno = PAPI_OK;
    cpu_control_t *cpu_ctl = (cpu_control_t *) ctl;

    memset(cpu_ctl, 0, sizeof(*cpu_ctl));
    cpu_ctl->prof_info.domain = _cpu_vector.cmp_info.default_domain;
    cpu_ctl->prof_info.granularity = _cpu_vector.cmp_info.default_granularity;
    cpu_ctl->prof_info.overflow_signal = _cpu_vector.cmp_info.hardware_intr_sig;
    cpu_ctl->prof_info.cidx = _cpu_vector.cmp_info.CmpIdx;
    cpu_ctl->prof_info.ntv_table = &ntv_table;
    cpu_ctl->prof_info.cpu = -1;

    return papi_errno;
}

int
cpu_dispatch_timer(int n, hwd_siginfo_t *info, void *uc)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_start(hwd_context_t *ctx, hwd_control_state_t *ctl)
{
    int papi_errno = PAPI_OK;
    cpu_context_t *cpu_ctx = (cpu_context_t *) ctx;
    cpu_control_t *cpu_ctl = (cpu_control_t *) ctl;

    if (cpu_ctx->state & CPU_EVENTS_OPENED) {
        SUBDBG("Error! Cannot PAPI_start more than one eventset at a time for every component");
        return PAPI_ECNFLCT;
    }

    papi_errno = prof_ctx_open(cpu_ctl->prof_info, &cpu_ctl->prof_ctx);
    if (papi_errno != PAPI_OK) {
        goto fn_fail;
    }

    cpu_ctx->state = CPU_EVENTS_OPENED;

    papi_errno = prof_ctx_start(cpu_ctl->prof_ctx);
    if (papi_errno != PAPI_OK) {
        goto fn_fail;
    }

    cpu_ctx->state |= CPU_EVENTS_RUNNING;

  fn_exit:
    return papi_errno;
  fn_fail:
    goto fn_exit;
}

int
cpu_stop(hwd_context_t *ctx, hwd_control_state_t *ctl)
{
    int papi_errno = PAPI_OK;
    cpu_context_t *cpu_ctx = (cpu_context_t *) ctx;
    cpu_control_t *cpu_ctl = (cpu_control_t *) ctl;

    if (!(cpu_ctx->state & CPU_EVENTS_OPENED)) {
        SUBDBG("Error! Cannot PAPI_stop counters for an eventset that has not been PAPI_start'ed");
        return PAPI_ECNFLCT;
    }

    papi_errno = prof_ctx_stop(cpu_ctx->prof_ctx);
    if (papi_errno != PAPI_OK) {
        return papi_errno;
    }

    cpu_ctx->state &= ~CPU_EVENTS_RUNNING;

    papi_errno = prof_ctx_close(cpu_ctl->prof_ctx);
    if (papi_errno != PAPI_OK) {
        return papi_errno;
    }

    cpu_ctx->state = 0;
    cpu_ctl->prof_ctx = NULL;

    return papi_errno;
}

int
cpu_read(hwd_context_t *ctx, hwd_control_state_t *ctl, long long **counters,
         int flags)
{
    int papi_errno = PAPI_OK;
    cpu_context_t *cpu_ctx = (cpu_context_t *) ctx;
    cpu_control_t *cpu_ctl = (cpu_control_t *) ctl;

    if (cpu_ctl->prof_ctx == NULL) {
        SUBDGB("Error! Cannot PAPI_read counters for an eventset that has not been PAPI_start'ed");
        return PAPI_EMISC;
    }

    return prof_read(ctl->prof_ctx, counters);
}

int
cpu_shutdown_thread(hwd_context_t *ctx)
{
    int papi_errno = PAPI_OK;
    cpu_context_t *cpu_ctx = (cpu_context_t *) ctx;

    cpu_ctx->initialized = 0;

    return papi_errno;
}

int
cpu_ctl(hwd_context_t *ctx __attribute__((unused)), int code,
        _papi_int_option_t *option)
{
    int papi_errno;
    cpu_control_t *cpu_ctl = (cpu_control_t *) option->multiplex.ESI->ctl_state;

    papi_errno = prof_ctx_ctl(code, cpu_ctl->prof_ctx);

    return papi_errno;
}

int
cpu_update_control_state(hwd_control_state_t *ctl, NativeInfo_t *native,
                         int count, hwd_context_t *ctx)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_set_domain(hwd_control_t *ctl, int domain)
{
    int papi_errno = PAPI_OK;
    cpu_control_t *cpu_ctl = (cpu_control_t *) ctl;
    cpu_ctl->domain = domain;
    return papi_errno;
}

int
cpu_set_overflow(EventSetInfo_t *ESI, int EventIndex, int threshold)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_set_profile(EventSetInfo_t *ESI, int EventIndex, int threshold)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_stop_profiling(ThreadInfo_t *thread, EventSetInfo_t *ESI)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_write(hwd_context_t *ctx __attribute__((unused)),
          hwd_control_state_t *ctl __attribute__((unused)))
{
    int papi_errno = PAPI_ENOSUPP;
    return papi_errno;
}

int
cpu_ntv_enum_events(unsigned int *event_code, int modifier)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_ntv_name_to_code(const char *name, unsigned int *event_code)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_ntv_code_to_name(unsigned int event_code, char *name, int len)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_ntv_code_to_descr(unsigned int event_code, char *descr, int len)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
cpu_ntv_code_to_info(unsigned int event_code, PAPI_event_info_t *event_info)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}
