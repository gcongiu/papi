/*/
 * @file    linux-rocm.c
 *
 * @ingroup rocm_components
 *
 * @brief This implements a PAPI component that enables PAPI-C to
 *  access hardware monitoring counters for AMD ROCM GPU devices
 *  through the ROC-profiler library.
 *
 * The open source software license for PAPI conforms to the BSD
 * License template.
 */

#include <dlfcn.h>
#include <hsa.h>
#include <rocprofiler.h>
#include <string.h>
#include <sys/stat.h>

#include "papi.h"
#include "papi_memory.h"
#include "papi_internal.h"
#include "papi_vector.h"

#define HANDLE_STRING_ERROR {fprintf(stderr,"%s:%i unexpected string function error.\n",__FILE__,__LINE__); exit(-1);}

// Choose one.
#define ROCMDBG(format, args...) {;}
// #define ROCMDBG(format, args...) fprintf(stderr, format, ## args)

/* Macros for error checking... each arg is only referenced/evaluated once */
#define CHECK_PRINT_EVAL(checkcond, str, evalthis)                      \
    do {                                                                \
        int _cond = (checkcond);                                        \
        if (_cond) {                                                    \
            ROCMDBG("%s:%i error: condition %s failed: %s.\n",          \
                    __FILE__, __LINE__, #checkcond, str);               \
            evalthis;                                                   \
        }                                                               \
    } while (0)

// Choose one.
#define LIBCALL_DBG(format, args...) {;}
//#define LIBCALL_DBG(format, args...) fprintf(stderr, format, ## args)

#define ROCM_CALL_CK(call, args, handleerror)                           \
    do {                                                                \
        hsa_status_t _status = (*call##Ptr)args;                        \
        if (_status != HSA_STATUS_SUCCESS &&                            \
            _status != HSA_STATUS_INFO_BREAK) {                         \
            LIBCALL_DBG("%s:%i error: function %s failed with error "   \
                        "%d.\n",                                        \
                        __FILE__, __LINE__, #call, _status);            \
            handleerror;                                                \
        }                                                               \
    } while (0)

// Roc Profiler call.
#define ROCP_CALL_CK(call, args, handleerror)                           \
    do {                                                                \
        hsa_status_t _status = (*call##Ptr)args;                        \
        if (_status != HSA_STATUS_SUCCESS &&                            \
            _status != HSA_STATUS_INFO_BREAK) {                         \
            const char *profErr;                                        \
            (*rocprofiler_error_stringPtr)(&profErr);                   \
            LIBCALL_DBG("%s:%i error: function %s failed with error "   \
                        "%d [%s].\n",                                   \
                        __FILE__, __LINE__, #call, _status, profErr);   \
            handleerror;                                                \
        }                                                               \
    } while (0)

#define DLSYM_AND_CHECK(dllib, name)                                    \
    do {                                                                \
        name##Ptr = dlsym(dllib, #name);                                \
        if (dlerror()!=NULL) {                                          \
            int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,\
                                PAPI_MAX_STR_LEN,                       \
                                "The ROCM required function '%s' was "  \
                                "not found in dynamic libs",            \
                                #name);                                 \
            if (strErr > PAPI_MAX_STR_LEN) HANDLE_STRING_ERROR;         \
            return PAPI_ENOSUPP;                                        \
        }                                                               \
    } while (0)

typedef rocprofiler_t *Context;
typedef rocprofiler_feature_t EventID;

/* Contains device list, pointer to device description, and the list of available events.
 * Note that "indexed variables" in ROCM are read with eventname[%d], where %d is
 * 0 to #instances. This is what we store in the EventID.name element. But the PAPI name
 * doesn't use brackets; so in the ev_name_desc.name we store the user-visible name,
 * something like "eventname:device=%d:instance=%d". */
typedef struct _rocm_context {
    uint32_t availAgentSize;
    hsa_agent_t *availAgentArray;
    uint32_t availEventSize;
    int *availEventDeviceNum;
    EventID *availEventIDArray; /* The EventID struct has its own .name element for ROCM internal operation. */
    uint32_t *availEventIsBeingMeasuredInEventset;
    struct ev_name_desc *availEventDesc; /* This is where the PAPI name is stored; for user consumption. */
} _rocm_context_t;

/* Store the name and description for an event */
typedef struct ev_name_desc {
    char name[PAPI_MAX_STR_LEN];
    char description[PAPI_2MAX_STR_LEN];
} ev_name_desc_t;

/* this number assumes that there will never be more events than indicated */
#define PAPIROCM_MAX_COUNTERS 512

/* Control structure tracks array of active contexts, records active events and their values */
typedef struct _rocm_control {
    uint32_t countOfActiveContexts;
    struct _rocm_active_context_s *arrayOfActiveContexts[PAPIROCM_MAX_COUNTERS];
    uint32_t activeEventCount;
    int activeEventIndex[PAPIROCM_MAX_COUNTERS];
    long long activeEventValues[PAPIROCM_MAX_COUNTERS];
    uint64_t startTimestampNs;
    uint64_t readTimestampNs;
} _rocm_control_t;

/* For each active context, which ROCM events are being measured, context eventgroups containing events */
typedef struct _rocm_active_context_s {
    Context ctx;
    int deviceNum;
    uint32_t conEventsCount;
    EventID conEvents[PAPIROCM_MAX_COUNTERS];
    int conEventIndex[PAPIROCM_MAX_COUNTERS];
} _rocm_active_context_t;

/* ******  CHANGE PROTOTYPES TO DECLARE ROCM LIBRARY SYMBOLS AS WEAK  **********
 *  This is done so that a version of PAPI built with the rocm component can   *
 *  be installed on a system which does not have the rocm libraries installed. *
 *                                                                             *
 *  If this is done without these prototypes, then all papi services on the    *
 *  system without the rocm libraries installed will fail.  The PAPI libraries *
 *  contain references to the rocm libraries which are not installed.  The     *
 *  load of PAPI commands fails because the rocm library references can not be *
 *  resolved.                                                                  *
 *                                                                             *
 *  This also defines pointers to the rocm library functions that we call.     *
 *  These function pointers will be resolved with dlopen/dlsym calls at        *
 *  component initialization time.  The component then calls the rocm library  *
 *  functions through these function pointers.                                 *
 *******************************************************************************/

#define DECLAREROCMFUNC(funcname, funcsig) \
    hsa_status_t __attribute__((weak)) funcname funcsig; \
    hsa_status_t(*funcname##Ptr) funcsig;

// ROCR API declaration
DECLAREROCMFUNC(hsa_init, ());
DECLAREROCMFUNC(hsa_shut_down, ());
DECLAREROCMFUNC(hsa_iterate_agents, (hsa_status_t (*)(hsa_agent_t, void *),
                                     void *));
DECLAREROCMFUNC(hsa_system_get_info, (hsa_system_info_t, void *));
DECLAREROCMFUNC(hsa_agent_get_info, (hsa_agent_t, hsa_agent_info_t, void *));
DECLAREROCMFUNC(hsa_queue_destroy, (hsa_queue_t *));

// ROC-profiler API declaration
DECLAREROCMFUNC(rocprofiler_get_info, (const hsa_agent_t *,
                                       rocprofiler_info_kind_t,
                                       void *));
DECLAREROCMFUNC(rocprofiler_iterate_info, (const hsa_agent_t *,
                                           rocprofiler_info_kind_t,
                                           hsa_status_t (*)(const rocprofiler_info_data_t,
                                                            void *),
                                           void *));
DECLAREROCMFUNC(rocprofiler_open, (hsa_agent_t agent, rocprofiler_feature_t *,
                                   uint32_t, rocprofiler_t **, uint32_t,
                                   rocprofiler_properties_t *));
DECLAREROCMFUNC(rocprofiler_close, (rocprofiler_t*));
DECLAREROCMFUNC(rocprofiler_group_count, (const rocprofiler_t *, uint32_t *));
DECLAREROCMFUNC(rocprofiler_start, (rocprofiler_t *, uint32_t));
DECLAREROCMFUNC(rocprofiler_read, (rocprofiler_t *, uint32_t));
DECLAREROCMFUNC(rocprofiler_stop, (rocprofiler_t *, uint32_t));
DECLAREROCMFUNC(rocprofiler_get_data, (rocprofiler_t *, uint32_t));
DECLAREROCMFUNC(rocprofiler_get_metrics, (const rocprofiler_t *));
DECLAREROCMFUNC(rocprofiler_reset, (rocprofiler_t *, uint32_t));
DECLAREROCMFUNC(rocprofiler_error_string, (const char **));

/* The PAPI side (external) variable as a global */
papi_vector_t _rocm_vector;

/* Global variable for hardware description, event and metric lists */
static _rocm_context_t *global__rocm_context;
static uint32_t maxEventSize;
static rocprofiler_properties_t global__ctx_properties = {
    NULL, // queue
    128,  // queue depth
    NULL, // handler on completion
    NULL  // handler_arg
};

static _rocm_control_t *global__rocm_control = NULL;

/* component init utility functions prototypes */
static int check_n_initialize( void );
static int libc_statically_linked( void );
static int link_rocm_libraries( void );
static int link_rocm_hsa( char *rocm_root[], int rocm_roots, char *hsa_root );
static int open_rocm_hsa_override_path( void );
static int open_rocm_hsa_sys_path( void );
static int open_rocm_hsa_root_path( char *rocm_root[], int rocm_roots );
static void get_hsa_root_path( char *hsa_root );
static int link_rocm_prof( char *rocm_root[], int rocm_roots, char *hsa_root,
                           char *profiler_root );
static int open_rocm_prof_override_path( void );
static int open_rocm_prof_sys_path( void );
static int open_rocm_prof_root_path( char *rocm_root[], int rocm_roots );
static int open_rocm_prof_hsa_path( char *hsa_root );
static void get_profiler_root_path( char *profiler_root );
static int check_rocp_env( char *rocm_root[], int rocm_roots,
                           char *profiler_root );
static int check_n_set_rocp_metrics( char *rocm_root[], int rocm_roots,
                                     char *profiler_root );
static int check_rocp_hsa_intercept( void );
static int check_rocp_log( void );
static int check_hsa_aql_prof_log( void );
static int check_aql_prof_read( void );
static int check_hsa_tools_lib( char *profiler_root );
static int init_hsa( void );
static int init_rocm_context( void );
static int get_rocm_gpu_count( void );
static int add_rocm_native_events( void );
static int add_native_events( void );
static hsa_status_t count_native_events_cb( const rocprofiler_info_data_t info,
                                            void *arg );
static hsa_status_t add_native_events_cb( const rocprofiler_info_data_t info,
                                          void *arg );
static hsa_status_t get_gpu_count_cb( hsa_agent_t agent, void *arg );
static int check_rocm_num_native_events( void );

/* component update utility functions prototypes */
static int foreach_event_do_update( hwd_control_state_t *ctrl,
                                    NativeInfo_t *nativeInfo,
                                    int nativeCount );
static int find_or_create_context( const char *eventName, int eventDeviceNum );
static int get_users_ntv_event( int usr_idx, int ntv_range,
                                NativeInfo_t *ntv_info,
                                const char **eventName, int *eventDeviceNum );
static int get_event_control( int ntv_event_idx, int eventContextIdx,
                              _rocm_active_context_t **eventctrl );
static int open_rocm_event( int eventDeviceNum,
                            _rocm_active_context_t *eventctrl,
                            hwd_control_state_t *ctrl );

/* component control utility functions prototypes */
static int get_event_interval_length( uint64_t *durationNs );
static int foreach_context_do_read( uint64_t durationNs, long long **values );
static void foreach_event_do_read( int eventDeviceNum, int ctx_idx,
                                   uint64_t durationNs );

static void *dlp_rocm_hsa;
static void *dlp_rocm_prof;
static char rocm_hsa[] = PAPI_ROCM_HSA;
static char rocm_prof[] = PAPI_ROCM_PROF;

#define ROCM_CHECK_N_INIT() do {      \
    int err = check_n_initialize();   \
    if (PAPI_OK != err) return err;   \
} while(0)

/*
 * papi_vector functions start here
 */
static int
_rocm_init_component( int cidx )
{
    ROCMDBG("Entering _rocm_init_component\n");

    _rocm_vector.cmp_info.CmpIdx = cidx;
    _rocm_vector.cmp_info.num_native_events = -1;
    _rocm_vector.cmp_info.num_cntrs = -1;
    _rocm_vector.cmp_info.num_mpx_cntrs = -1;

    return PAPI_OK;
}

static int
_rocm_init_thread( hwd_context_t *ctx __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_init_thread\n");
    return PAPI_OK;
}

static int
_rocm_init_private( void )
{
    int err = PAPI_OK;

    PAPI_lock(COMPONENT_LOCK);
    if (_rocm_vector.cmp_info.initialized) {
        err = _rocm_vector.cmp_info.disabled;
        goto rocm_init_private_exit;
    }

    ROCMDBG("Entering _rocm_init_private\n");

    if(link_rocm_libraries() != PAPI_OK) {
        SUBDBG("Dynamic link of ROCM libraries failed, component will be "
               "disabled.\n");
        SUBDBG("See disable reason in papi_component_avail output for more "
                "details.\n");
        err = PAPI_ENOSUPP;
        goto rocm_init_private_exit;
    }

    err = init_hsa();
    if (err != PAPI_OK) {
        goto rocm_init_private_exit;
    }

    err = init_rocm_context();
    if (err != PAPI_OK) {
        goto rocm_init_private_exit;
    }

    err = get_rocm_gpu_count();
    if (err != PAPI_OK) {
        goto rocm_init_private_exit;
    }

    /* Get list of all native ROCM events supported */
    err = add_rocm_native_events();
    if (err != PAPI_OK) {
        goto rocm_init_private_exit;
    }

    _rocm_vector.cmp_info.num_native_events = global__rocm_context->availEventSize;
    _rocm_vector.cmp_info.num_cntrs = _rocm_vector.cmp_info.num_native_events;
    _rocm_vector.cmp_info.num_mpx_cntrs = _rocm_vector.cmp_info.num_native_events;

    ROCMDBG("Exiting _rocm_init_component cidx %d num_native_events %d "
            "num_cntrs %d num_mpx_cntrs %d\n",
            _rocm_vector.cmp_info.CmpIdx,
            _rocm_vector.cmp_info.num_native_events,
            _rocm_vector.cmp_info.num_cntrs,
            _rocm_vector.cmp_info.num_mpx_cntrs);

    err = check_rocm_num_native_events();

  rocm_init_private_exit:
    _rocm_vector.cmp_info.initialized = 1;
    _rocm_vector.cmp_info.disabled = err;

    PAPI_unlock(COMPONENT_LOCK);

    return err;
}

static int
_rocm_init_control_state( hwd_control_state_t *ctrl __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_init_control_state\n");

    ROCM_CHECK_N_INIT();

    _rocm_context_t *gctxt = global__rocm_context;
    _rocm_control_t *gctrl = global__rocm_control;

    CHECK_PRINT_EVAL((gctxt == NULL), "Error: The PAPI ROCM component needs to "
                                      "be initialized first",
                     return PAPI_ENOINIT);

    if (global__rocm_context->availEventSize <= 0) {
        strncpy(_rocm_vector.cmp_info.disabled_reason,
                "ERROR ROCM: No events exist",
                PAPI_MAX_STR_LEN);
        return PAPI_EMISC;
    }

    if (gctrl == NULL) {
        gctrl = papi_calloc(1, sizeof(_rocm_control_t));
        gctrl->countOfActiveContexts = 0;
        gctrl->activeEventCount = 0;
    }

    return PAPI_OK;
}

static int
_rocm_shutdown_thread( hwd_context_t *ctx __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_shutdown_thread\n");
    return PAPI_OK;
}

static int
_rocm_shutdown_component( void )
{
    ROCMDBG("Entering _rocm_shutdown_component\n");

    _rocm_control_t *gctrl = global__rocm_control;
    _rocm_context_t *gctxt = global__rocm_context;

    if(gctxt != NULL) {
        papi_free(gctxt->availEventIDArray);
        papi_free(gctxt->availEventDeviceNum);
        papi_free(gctxt->availEventIsBeingMeasuredInEventset);
        papi_free(gctxt->availEventDesc);
        papi_free(gctxt);
        global__rocm_context = gctxt = NULL;
    }

    if(gctrl != NULL) {
        uint32_t cc;
        for(cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
            if(gctrl->arrayOfActiveContexts[cc] != NULL) {
                papi_free(gctrl->arrayOfActiveContexts[cc]);
            }
        }

        papi_free(gctrl);
        global__rocm_control = gctrl = NULL;
    }

    if (hsa_shut_downPtr) {
        ROCM_CALL_CK(hsa_shut_down, (), return PAPI_EMISC);
    }

    if (dlp_rocm_hsa) {
        dlclose(dlp_rocm_hsa);
    }
    if (dlp_rocm_prof) {
        dlclose(dlp_rocm_prof);
    }

    return PAPI_OK;
}

/* Triggered by eventset operations like add or remove.  For ROCM,
 * needs to be called multiple times from each seperate ROCM context
 * with the events to be measured from that context.  For each
 * context, create eventgroups for the events.
 */
static int
_rocm_update_control_state( hwd_control_state_t *ctrl, NativeInfo_t *nativeInfo,
                            int nativeCount,
                            hwd_context_t *ctx __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_update_control_state with nativeCount %d\n",
            nativeCount);

    ROCM_CHECK_N_INIT();

    if (nativeCount == 0) {
        return PAPI_OK;
    }

    return foreach_event_do_update(ctrl, nativeInfo, nativeCount);
}

/* Triggered by PAPI_start().
 * For ROCM component, switch to each context and start all eventgroups.
 */
static int
_rocm_start( hwd_context_t *ctx __attribute__((unused)),
             hwd_control_state_t *ctrl __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_start\n");

    _rocm_control_t *gctrl = global__rocm_control;

    ROCMDBG("Reset all active event values\n");

    uint32_t ii;
    for (ii = 0; ii < gctrl->activeEventCount; ii++)
        gctrl->activeEventValues[ii] = 0;

    ROCM_CALL_CK(hsa_system_get_info,
                 (HSA_SYSTEM_INFO_TIMESTAMP, &gctrl->startTimestampNs),
                 return PAPI_EMISC);

    uint32_t cc;
    for (cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        Context eventCtx = gctrl->arrayOfActiveContexts[cc]->ctx;

        ROCMDBG("Start device %d ctx %p ts %lu\n",
                gctrl->arrayOfActiveContexts[cc]->deviceNum, eventCtx,
                gctrl->startTimestampNs);

        if (eventCtx == NULL) {
            abort();
        }

        ROCP_CALL_CK(rocprofiler_start, (eventCtx, 0), return PAPI_EMISC);
    }

    return PAPI_OK;
}

/* Triggered by PAPI_read(). For ROCM component, switch to each
 * context, read all the eventgroups, and put the values in the
 * correct places. */
static int
_rocm_read( hwd_context_t *ctx __attribute__((unused)),
            hwd_control_state_t *ctrl __attribute__((unused)),
            long long **values,
            int flags __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_read\n");

    uint64_t durationNs;
    int status = get_event_interval_length(&durationNs);
    if (status != PAPI_OK) {
        return status;
    }

    return foreach_context_do_read(durationNs, values);
}

/* Triggered by PAPI_stop() */
static int
_rocm_stop( hwd_context_t *ctx __attribute__((unused)),
            hwd_control_state_t *ctrl __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_stop\n");

    _rocm_control_t *gctrl = global__rocm_control;

    uint32_t cc;
    for (cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        Context eventCtx = gctrl->arrayOfActiveContexts[cc]->ctx;
        ROCMDBG("Stop device %d ctx %p \n",
                gctrl->arrayOfActiveContexts[cc]->deviceNum, eventCtx);

        ROCP_CALL_CK(rocprofiler_stop, (eventCtx, 0), return PAPI_EMISC);
    }

    return PAPI_OK;
}

/*
 * Disable and destroy the ROCM eventGroup
 */
static int
_rocm_cleanup_eventset( hwd_control_state_t *ctrl __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_cleanup_eventset\n");

    _rocm_control_t *gctrl = global__rocm_control;

    uint32_t cc;
    for(cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        Context eventCtx = gctrl->arrayOfActiveContexts[cc]->ctx;
        ROCMDBG("Destroy device %d ctx %p \n",
                gctrl->arrayOfActiveContexts[cc]->deviceNum, eventCtx);
        ROCP_CALL_CK(rocprofiler_close, (eventCtx), return PAPI_EMISC);
        papi_free( gctrl->arrayOfActiveContexts[cc] );
    }

    if (global__ctx_properties.queue != NULL) {
        ROCM_CALL_CK(hsa_queue_destroy, (global__ctx_properties.queue),
                     return PAPI_EMISC);
        global__ctx_properties.queue = NULL;
    }

    /* Record that there are no active contexts or events */
    gctrl->countOfActiveContexts = 0;
    gctrl->activeEventCount = 0;

    /* Clear all indicators of event being measured. */
    _rocm_context_t *gctxt = global__rocm_context;

    uint32_t i;
    for (i = 0; i < maxEventSize; i++) {
        gctxt->availEventIsBeingMeasuredInEventset[i] = 0;
    }

    return PAPI_OK;
}

/* Triggered by PAPI_reset() but only if the EventSet is currently
 *  running. If the eventset is not currently running, then the saved
 *  value in the EventSet is set to zero without calling this
 *  routine.  */
static int
_rocm_reset( hwd_context_t *ctx __attribute__((unused)),
             hwd_control_state_t *ctrl __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_reset\n");

    _rocm_control_t *gctrl = global__rocm_control;
    uint32_t ii, cc;

    ROCMDBG("Reset all active event values\n");
    for(ii = 0; ii < gctrl->activeEventCount; ii++)
        gctrl->activeEventValues[ii] = 0;

    for(cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        Context eventCtx = gctrl->arrayOfActiveContexts[cc]->ctx;
        ROCMDBG("Reset device %d ctx %p \n",
                gctrl->arrayOfActiveContexts[cc]->deviceNum, eventCtx);
        ROCP_CALL_CK(rocprofiler_reset, (eventCtx, 0), return PAPI_EMISC);
    }

    return PAPI_OK;
}

/* This function sets various options in the component - Does nothing in the ROCM component.
    @param[in] ctx -- hardware context
    @param[in] code valid are PAPI_SET_DEFDOM, PAPI_SET_DOMAIN, PAPI_SETDEFGRN, PAPI_SET_GRANUL and PAPI_SET_INHERIT
    @param[in] option -- options to be set
*/
static int
_rocm_ctrl( hwd_context_t *ctx __attribute__((unused)),
            int code __attribute__((unused)),
            _papi_int_option_t *option __attribute__((unused)) )
{
    ROCMDBG("Entering _rocm_ctrl\n");
    return PAPI_OK;
}

/*
 * This function has to set the bits needed to count different domains
 * In particular: PAPI_DOM_USER, PAPI_DOM_KERNEL PAPI_DOM_OTHER
 * By default return PAPI_EINVAL if none of those are specified
 * and PAPI_OK with success
 * PAPI_DOM_USER is only user context is counted
 * PAPI_DOM_KERNEL is only the Kernel/OS context is counted
 * PAPI_DOM_OTHER is Exception/transient mode (like user TLB misses)
 * PAPI_DOM_ALL is all of the domains
 */
static int
_rocm_set_domain( hwd_control_state_t *ctrl __attribute__((unused)),
                  int domain )
{
    int status = PAPI_OK;
    ROCMDBG("Entering _rocm_set_domain\n");

    if(!((PAPI_DOM_USER   & domain) ||
         (PAPI_DOM_KERNEL & domain) ||
         (PAPI_DOM_OTHER  & domain) ||
         (PAPI_DOM_ALL    & domain)))
        status = PAPI_EINVAL;

    return status;
}

/* Enumerate Native Events.
 * @param EventCode is the event of interest
 * @param modifier is one of PAPI_ENUM_FIRST, PAPI_ENUM_EVENTS
 */
static int
_rocm_ntv_enum_events( unsigned int *EventCode, int modifier )
{
    int status = PAPI_OK;

    ROCM_CHECK_N_INIT();

    switch (modifier) {
        case PAPI_ENUM_FIRST:
            *EventCode = 0;
            break;
        case PAPI_ENUM_EVENTS:
            if (global__rocm_context == NULL) {
                status = PAPI_ENOEVNT;
            } else if (*EventCode < global__rocm_context->availEventSize - 1) {
                ++(*EventCode);
            } else {
                status = PAPI_ENOEVNT;
            }
            break;
        default:
            status = PAPI_EINVAL;
    }

    return status;
}

/* Takes a native event code and passes back the name, but the PAPI version
 * of the name in availEventDesc[], not the ROCM internal name (in
 * availEventIDArray[].name).
 * @param EventCode is the native event code
 * @param name is a pointer for the name to be copied to
 * @param len is the size of the name string
 */
static int
_rocm_ntv_code_to_name( unsigned int EventCode, char *name, int len )
{
    //ROCMDBG("Entering EventCode %d\n", EventCode );
    ROCM_CHECK_N_INIT();

    unsigned int index = EventCode;
    _rocm_context_t *gctxt = global__rocm_context;

    if (gctxt != NULL && index < gctxt->availEventSize) {
        strncpy(name, gctxt->availEventDesc[index].name, len);
    } else {
        return PAPI_EINVAL;
    }

    return PAPI_OK;
}

/* Takes a native event code and passes back the event description
 * @param EventCode is the native event code
 * @param descr is a pointer for the description to be copied to
 * @param len is the size of the descr string
 */
static int
_rocm_ntv_code_to_descr( unsigned int EventCode, char *name, int len )
{
    unsigned int index = EventCode;
    _rocm_context_t *gctxt = global__rocm_context;
    if (gctxt != NULL && index < gctxt->availEventSize) {
        strncpy(name, gctxt->availEventDesc[index].description, len);
    } else {
        return PAPI_EINVAL;
    }
    return PAPI_OK;
}


/* Vector that points to entry points for the component */
papi_vector_t _rocm_vector = {
    .cmp_info = {
        /* default component information (unspecified values are initialized to 0) */
        .name                    = "rocm",
        .short_name              = "rocm",
        .version                 = "1.0",
        .description             = "GPU events and metrics via AMD ROCm-PL API",
        .num_mpx_cntrs           = PAPIROCM_MAX_COUNTERS,
        .num_cntrs               = PAPIROCM_MAX_COUNTERS,
        .default_domain          = PAPI_DOM_USER,
        .default_granularity     = PAPI_GRN_THR,
        .available_granularities = PAPI_GRN_THR,
        .hardware_intr_sig       = PAPI_INT_SIGNAL,
        /* component specific cmp_info initializations */
        .fast_real_timer    = 0,
        .fast_virtual_timer = 0,
        .attach             = 0,
        .attach_must_ptrace = 0,
        .available_domains  = PAPI_DOM_USER | PAPI_DOM_KERNEL,
        .initialized        = 0,
    }
    ,
    /* sizes of framework-opaque component-private structures... these are all unused in this component */
    .size = {
        .context       = 1,    /* sizeof( _rocm_context_t ), */
        .control_state = 1,    /* sizeof( _rocm_control_t ), */
        .reg_value     = 1,    /* sizeof( _rocm_register_t ), */
        .reg_alloc     = 1,    /* sizeof( _rocm_reg_alloc_t ), */
    }
    ,
    /* function pointers in this component */
    .start                = _rocm_start,             /* ( hwd_context_t * ctx, hwd_control_state_t * ctrl ) */
    .stop                 = _rocm_stop,              /* ( hwd_context_t * ctx, hwd_control_state_t * ctrl ) */
    .read                 = _rocm_read,              /* ( hwd_context_t * ctx, hwd_control_state_t * ctrl, long_long ** events, int flags ) */
    .reset                = _rocm_reset,             /* ( hwd_context_t * ctx, hwd_control_state_t * ctrl ) */
    .cleanup_eventset     = _rocm_cleanup_eventset,  /* ( hwd_control_state_t * ctrl ) */

    .init_component       = _rocm_init_component,        /* ( int cidx ) */
    .init_thread          = _rocm_init_thread,           /* ( hwd_context_t * ctx ) */
    .init_private         = _rocm_init_private,          /* (void) */
    .init_control_state   = _rocm_init_control_state,    /* ( hwd_control_state_t * ctrl ) */
    .update_control_state = _rocm_update_control_state,  /* ( hwd_control_state_t * ptr, NativeInfo_t * native, int count, hwd_context_t * ctx ) */

    .ctl                  = _rocm_ctrl,               /* ( hwd_context_t * ctx, int code, _papi_int_option_t * option ) */
    .set_domain           = _rocm_set_domain,         /* ( hwd_control_state_t * cntrl, int domain ) */
    .ntv_enum_events      = _rocm_ntv_enum_events,    /* ( unsigned int *EventCode, int modifier ) */
    .ntv_code_to_name     = _rocm_ntv_code_to_name,   /* ( unsigned int EventCode, char *name, int len ) */
    .ntv_code_to_descr    = _rocm_ntv_code_to_descr,  /* ( unsigned int EventCode, char *name, int len ) */
    .shutdown_thread      = _rocm_shutdown_thread,    /* ( hwd_context_t * ctx ) */
    .shutdown_component   = _rocm_shutdown_component, /* ( void ) */
};

/*
 * Component utility functions implementation starts here
 */
int
check_n_initialize( void )
{
    if (!_rocm_vector.cmp_info.initialized && _rocm_vector.init_private) {
        return _rocm_vector.init_private();
    }
    return PAPI_OK;
}

void (*_dl_non_dynamic_init)( void ) __attribute__((weak));

int
libc_statically_linked( void )
{
    if (_dl_non_dynamic_init) {
        char *strCpy = strncpy(_rocm_vector.cmp_info.disabled_reason,
                               "ROCM component can't link statically to libc.",
                               PAPI_MAX_STR_LEN);
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;

        if (strCpy == NULL) {
            HANDLE_STRING_ERROR;
        }

        return 1;
    }

    return 0;
}

int
link_rocm_libraries( void )
{
    int status;
    char hsa_root[PATH_MAX];
    char profiler_root[PATH_MAX];

    ROCMDBG("Entering link_rocm_libraries\n");

    if (libc_statically_linked()) {
       return PAPI_ENOSUPP;
    }

#define MAX_ROCM_ROOT_COUNT 4
    char *rocm_root[MAX_ROCM_ROOT_COUNT] = {
        getenv("PAPI_ROCM_ROOT"),
        getenv("ROCM_PATH"),
        getenv("ROCM_DIR"),
        getenv("ROCMDIR"),
    };

    status = link_rocm_hsa(rocm_root, MAX_ROCM_ROOT_COUNT, hsa_root);
    if (status != PAPI_OK) {
        return status;
    }

    status = link_rocm_prof(rocm_root, MAX_ROCM_ROOT_COUNT, hsa_root,
                            profiler_root);
    if (status != PAPI_OK) {
        return status;
    }

    status = check_rocp_env(rocm_root, MAX_ROCM_ROOT_COUNT, profiler_root);
    if (status != PAPI_OK) {
        return status;
    }

    return PAPI_OK;
}

int
link_rocm_hsa( char *rocm_root[], int rocm_roots, char *hsa_root )
{
    int status;

    status = open_rocm_hsa_override_path();
    if (status != PAPI_OK) {
        return status;
    }

    if (dlp_rocm_hsa == NULL) {
        status = open_rocm_hsa_sys_path();
        if (status != PAPI_OK) {
            return status;
        }
    }

    if (dlp_rocm_hsa == NULL) {
        status = open_rocm_hsa_root_path(rocm_root, rocm_roots);
        if (status != PAPI_OK) {
            return status;
        }
    }

    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_init);
    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_iterate_agents);
    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_system_get_info);
    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_agent_get_info);
    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_shut_down);
    DLSYM_AND_CHECK(dlp_rocm_hsa, hsa_queue_destroy);

    get_hsa_root_path(hsa_root);

    return PAPI_OK;
}

int
open_rocm_hsa_override_path( void )
{
    if (strlen(rocm_hsa) > 0) {
        dlp_rocm_hsa = dlopen(rocm_hsa, RTLD_NOW | RTLD_GLOBAL);
        if (dlp_rocm_hsa == NULL) {
            int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                  PAPI_MAX_STR_LEN,
                                  "PAPI_ROCM_HSA override '%s' given in "
                                  "Rules.rocm not found.",
                                  rocm_hsa);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
open_rocm_hsa_sys_path( void )
{
    dlp_rocm_hsa = dlopen("libhsa-runtime64.so", RTLD_NOW | RTLD_GLOBAL);
    return PAPI_OK;
}

int
open_rocm_hsa_root_path( char *rocm_root[], int rocm_roots )
{
    int i;
    int strErr;
    char path_name[PATH_MAX];

    for (i = 0; i < rocm_roots; i++) {
        if (rocm_root[i] != NULL) {
            strErr = snprintf(path_name, PATH_MAX,
                              "%s/lib/libhsa-runtime64.so",
                              rocm_root[i]);
            if (strErr > PATH_MAX) {
                HANDLE_STRING_ERROR;
            }

            dlp_rocm_hsa = dlopen(path_name, RTLD_NOW | RTLD_GLOBAL);
            if (dlp_rocm_hsa) {
                break;
            }
        }
    }

    if (dlp_rocm_hsa == NULL) {
        /* if we got a faulty path we fail */
        strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                          PAPI_MAX_STR_LEN,
                          "libhsa-runtime64.so not found. Need LD_LIBRARY_PATH "
                          "set, or Env Var PAPI_ROCM_ROOT set, or module load "
                          "rocm.");
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
        return PAPI_ENOSUPP;
    }

    return PAPI_OK;
}

void
get_hsa_root_path( char *hsa_root )
{
    Dl_info hsa_rt_info;

    dladdr(hsa_initPtr, &hsa_rt_info);
    strncpy(hsa_root, hsa_rt_info.dli_fname, PATH_MAX - 1);

    char *strCpy = strstr(hsa_root, "/hsa/lib/libhsa-runtime64.so");
    if (strCpy) {
        strCpy[0] = 0;
    } else {
        hsa_root[0] = 0;
    }
}

int
link_rocm_prof( char *rocm_root[], int rocm_roots, char *hsa_root, char *profiler_root )
{
    int status;

    status = open_rocm_prof_override_path();
    if (status != PAPI_OK) {
        return status;
    }

    if (dlp_rocm_prof == NULL) {
        status = open_rocm_prof_sys_path();
        if (status != PAPI_OK) {
            return status;
        }
    }

    if (dlp_rocm_prof == NULL) {
        status = open_rocm_prof_root_path(rocm_root, rocm_roots);
        if (status != PAPI_OK) {
            return status;
        }
    }

    if (dlp_rocm_prof == NULL) {
        status = open_rocm_prof_hsa_path(hsa_root);
        if (status != PAPI_OK) {
            return status;
        }
    }

    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_get_info);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_iterate_info);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_open);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_close);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_group_count);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_start);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_read);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_stop);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_get_data);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_get_metrics);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_reset);
    DLSYM_AND_CHECK(dlp_rocm_prof, rocprofiler_error_string);

    get_profiler_root_path(profiler_root);

    return PAPI_OK;
}

int
open_rocm_prof_override_path( void )
{
    if (strlen(rocm_prof) > 0) {
        dlp_rocm_prof = dlopen(rocm_prof, RTLD_NOW | RTLD_GLOBAL);
        if (dlp_rocm_hsa == NULL) {
            int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                  PAPI_MAX_STR_LEN,
                                  "PAPI_ROCM_PROF override '%s' given int "
                                  "Rules.rocm not found.",
                                  rocm_prof);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
open_rocm_prof_sys_path( void )
{
    dlp_rocm_prof = dlopen("librocprofiler64.so", RTLD_NOW | RTLD_GLOBAL);
    return PAPI_OK;
}

int
open_rocm_prof_root_path( char *rocm_root[], int rocm_roots )
{
    int i;
    int strErr;
    char path_name[PATH_MAX];

    for (i = 0; i < rocm_roots; i++) {
        if (rocm_root[i] != NULL) {
            strErr = snprintf(path_name, PATH_MAX,
                              "%s/lib/librocprofiler64.so",
                              rocm_root[i]);
            path_name[PATH_MAX - 1] = 0;
            if (strErr > PATH_MAX) {
                HANDLE_STRING_ERROR;
            }

            dlp_rocm_prof = dlopen(path_name, RTLD_NOW | RTLD_GLOBAL);
            if (dlp_rocm_prof) {
                break;
            }
        }
    }

    if (dlp_rocm_prof == NULL) {
        strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                          PAPI_MAX_STR_LEN,
                          "libhsa-runtime64.so not found. Need LD_LIBRARY_PATH "
                          "set, or Env Var PAPI_ROCM_ROOT set, or module load "
                          "rocm.");
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
        return PAPI_ENOSUPP;
    }

    return PAPI_OK;
}

int
open_rocm_prof_hsa_path( char *hsa_root )
{
    char path_name[PATH_MAX];

    if (hsa_root[0] != 0) {
        int strErr = snprintf(path_name, PATH_MAX,
                              "%s/lib/librocprofiler64.so",
                              hsa_root);
        path_name[PATH_MAX - 1] = 0;
        if (strErr > PATH_MAX) {
            HANDLE_STRING_ERROR;
        }

        dlp_rocm_prof = dlopen(path_name, RTLD_NOW | RTLD_GLOBAL);
        if (dlp_rocm_prof == NULL) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "librocprofiler64.so not found. Need "
                              "LD_LIBRARY_PATH set, or Env Var PAPI_ROCM_ROOT "
                              "set, or module load rocm.");
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }

            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

void
get_profiler_root_path( char *profiler_root )
{
    Dl_info rocprofiler_info;

    dladdr(rocprofiler_get_infoPtr, &rocprofiler_info);
    char *strCpy = strncpy(profiler_root, rocprofiler_info.dli_fname,
                           PATH_MAX - 1);
    if (strCpy == NULL) {
        HANDLE_STRING_ERROR;
    }

    strCpy = strstr(profiler_root, "/lib/");
    if (strCpy != NULL) {
        strCpy[5] = 0;
    } else {
        profiler_root[0] = 0;
    }
}

int
check_rocp_env( char *rocm_root[], int rocm_roots, char *profiler_root )
{
    int status;

    status = check_n_set_rocp_metrics(rocm_root, rocm_roots, profiler_root);
    if (status != PAPI_OK) {
        return status;
    }

    status = check_rocp_hsa_intercept();
    if (status != PAPI_OK) {
        return status;
    }

    status = check_rocp_log();
    if (status != PAPI_OK) {
        return status;
    }

    status = check_hsa_aql_prof_log();
    if (status != PAPI_OK) {
        return status;
    }

    status = check_aql_prof_read();
    if (status != PAPI_OK) {
        return status;
    }

    status = check_hsa_tools_lib(profiler_root);
    if (status != PAPI_OK) {
        return status;
    }

    return PAPI_OK;
}

int
check_n_set_rocp_metrics( char *rocm_root[], int rocm_roots,
                          char *profiler_root )
{
    char path_name[PATH_MAX];
    struct stat myStat;

    char *rocp_metrics = getenv("ROCP_METRICS");
    if (rocp_metrics == NULL) {
        // Attempt 1: Look for metrics.xml in same directory as library.
        int strErr = snprintf(path_name, PATH_MAX, "%smetrics.xml",
                              profiler_root);
        path_name[PATH_MAX - 1] = 0;
        if (strErr > PATH_MAX) {
            HANDLE_STRING_ERROR;
        }

        int err = stat(path_name, &myStat);
        if (err < 0) {
            // Attempt 2: Might have been in ROOT/lib, try in ROOT/rocprofiler/lib/
            strErr = snprintf(path_name, PATH_MAX,
                              "%s../rocprofiler/lib/metrics.xml",
                              profiler_root);
            path_name[PATH_MAX - 1] = 0;
            if (strErr > PATH_MAX) {
                HANDLE_STRING_ERROR;
            }

            err = stat(path_name, &myStat);
            if (err < 0) {
                // Attempt 3: Might have been in /usr or something, Try roots[]/rocprofiler/lib
                int i;
                for (i = 0; i < rocm_roots; i++) {
                    if (rocm_root[i] != NULL) {
                        strErr = snprintf(path_name, PATH_MAX,
                                          "%s/rocprofiler/lib/metrics.xml",
                                          rocm_root[i]);
                        path_name[PATH_MAX - 1] = 0;
                        if (strErr > PATH_MAX) {
                            HANDLE_STRING_ERROR;
                        }
                        err = stat(path_name, &myStat);
                        if (!err) {
                            break;
                        }
                    }
                }
            }

            if (err < 0) {
                strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                  PAPI_MAX_STR_LEN,
                                  "file 'metrics.xml' not found; set in Env. "
                                  "Var. ROCP_METRICS, or ensure PAPI_ROCM_ROOT "
                                  "is valid.");
                _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
                if (strErr > PAPI_MAX_STR_LEN) {
                    HANDLE_STRING_ERROR;
                }
                return PAPI_ENOSUPP;
            } else {
                err = setenv("ROCP_METRICS", path_name, 0);
                if (err != 0) {
                    int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                          PAPI_MAX_STR_LEN,
                                          "Cannot set Env. Var. ROCP_METRICS required "
                                          "for rocprofiler operation. Must be set "
                                          "manually.");
                    _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
                    if (strErr > PAPI_MAX_STR_LEN) {
                        HANDLE_STRING_ERROR;
                    }
                    return PAPI_ENOSUPP;
                }
            }
        }
    } else {
        int err = stat(rocp_metrics, &myStat);
        if (err < 0) {
            int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                  PAPI_MAX_STR_LEN,
                                  "File '%s' given in Env. var. ROCP_METRICS "
                                  "missing; required for rocprofiler.",
                                  rocp_metrics);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
check_rocp_hsa_intercept( void )
{
    int strErr;

    char *env_value = getenv("ROCP_HSA_INTERCEPT");
    if (env_value == NULL) {
        int err = setenv("ROCP_HSA_INTERCEPT", "1", 0);
        if (err != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Cannot set Env. Var. ROCP_HSA_INTERCEPT=1; "
                              "required for rocprofiler operation. Must be "
                              "set manually.");
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    } else {
        if (strcmp(env_value, "1") != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Env. Var. ROCP_HSA_INTERCEPT='%s' is not a "
                              "supported value; must be '1'.", env_value);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
check_rocp_log( void )
{
    int strErr;

    char *env_value = getenv("ROCPROFILER_LOG");
    if (env_value == NULL) {
        int err = setenv("ROCPROFILER_LOG", "1", 0);
        if (err != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Cannot set Env. Var. ROCPROFILER_LOG=1; required "
                              "for rocprofiler operation. Must be set manually.");
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    } else {
        if (strcmp(env_value, "1") != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Env. Var. ROCPROFILER_LOG='%s' is not a supported "
                              "value; must be '1'.", env_value);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN-1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
check_hsa_aql_prof_log( void )
{
    int strErr;

    char *env_value = getenv("HSA_VEN_AMD_AQLPROFILE_LOG");
    if (env_value == NULL) {
        int err = setenv("HSA_VEN_AMD_AQLPROFILE_LOG", "1", 0);
        if (err != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Cannot set Env. Var. HSA_VEN_AMD_AQLPROFILE_LOG=1; "
                              "required for rocprofiler operation. Must be set "
                              "manually.");
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    } else {
        if (strcmp(env_value, "1") != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Env. Var. HSA_VEN_AMD_AQLPROFILE_LOG=%s is not a "
                              "supported value; must be '1'.", env_value);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
check_aql_prof_read( void )
{
    int strErr;

    char *env_value = getenv("AQLPROFILE_READ_API");
    if (env_value == NULL) {
        int err = setenv("AQLPROFILE_READ_API", "1", 0);
        if (err != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Cannot set Env. Var. AQLPROFILE_READ_API=1; "
                              "required for rocprofiler operation. Must be "
                              "set manually.");
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    } else {
        if (strcmp(env_value, "1") != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Env. Var. AQLPROFILE_READ_API=%s is not a "
                              "supported value; must be '1'.", env_value);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
check_hsa_tools_lib( char *profiler_root )
{
    /*
     * Note we still have a valid rocprofiler_info. dli_fname, we need to strip
     * away path info to set HSA_TOOLS_LIB.
     * Actual example:
     *  rocprofiler_info dli_fname='/opt/rocm/rocprofiler/lib/librocprofiler64.so'
     */
    int strErr;

    char *env_value = getenv("HSA_TOOLS_LIB");
    if (env_value == NULL) {
        int i = strlen(profiler_root);
        while (i > 1 && profiler_root[i - 1] != '/')
            i--;

        int err = setenv("HSA_TOOLS_LIB", profiler_root + i, 0);
        if (err != 0) {
            strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "Cannot set Env. Var. HSA_TOOLS_LIB='%s' required "
                              "for rocprofiler operation. Must be set manually.",
                              profiler_root + i);
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOSUPP;
        }
    }

    return PAPI_OK;
}

int
init_hsa( void )
{
    hsa_status_t status = (*hsa_initPtr)();
    if (status != HSA_STATUS_SUCCESS && status != HSA_STATUS_INFO_BREAK) {
        int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "ROCM hsa_init() failed with error %d.",
                              status);
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
        return PAPI_EMISC;
    }

    return PAPI_OK;
}

int
init_rocm_context( void )
{
    if(global__rocm_context == NULL) {
        global__rocm_context = papi_calloc(1, sizeof(_rocm_context_t));
        if (global__rocm_context == NULL) {
            int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                                  PAPI_MAX_STR_LEN,
                                  "%s:%i global__rocm_context alloc of %lu "
                                  "bytes failed.", __FILE__, __LINE__,
                                  sizeof(_rocm_context_t));
            _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
            if (strErr > PAPI_MAX_STR_LEN) {
                HANDLE_STRING_ERROR;
            }
            return PAPI_ENOMEM;
        }
    }

    return PAPI_OK;
}

int
get_rocm_gpu_count( void )
{
    hsa_status_t status = (*hsa_iterate_agentsPtr)(get_gpu_count_cb,
                                                   global__rocm_context);
    if (status != HSA_STATUS_SUCCESS && status != HSA_STATUS_INFO_BREAK) {
        int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "ROCM hsa_iterate_agents() failed with "
                              "error %d.",
                              status);
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
        return PAPI_EMISC;
    }

    return PAPI_OK;
}

hsa_status_t
get_gpu_count_cb( hsa_agent_t agent, void *arg )
{
    _rocm_context_t * gctxt = (_rocm_context_t*) arg;

    hsa_device_type_t type;
    ROCM_CALL_CK(hsa_agent_get_info, (agent, HSA_AGENT_INFO_DEVICE, &type),
                 return PAPI_EMISC);

    if (type == HSA_DEVICE_TYPE_GPU) {
        gctxt->availAgentSize += 1;
        gctxt->availAgentArray =
            papi_realloc(gctxt->availAgentArray,
                         gctxt->availAgentSize * sizeof(hsa_agent_t));
        gctxt->availAgentArray[gctxt->availAgentSize - 1] = agent;
    }

    return HSA_STATUS_SUCCESS;
}

int
add_rocm_native_events( void )
{
    int err = add_native_events();
    if(err != PAPI_OK) {
        int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "ROCM component routine add_native_events() "
                              "failed.");
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
    }

    return err;
}

/* events callback argument */
typedef struct {
    int device_num;
    int count;
    _rocm_context_t *ctx;
} events_cb_arg_t;

int
add_native_events( void )
{
    ROCMDBG("Entering add_native_events\n");

    _rocm_context_t *ctx = global__rocm_context;

    uint32_t i;
    for (i = 0; i < ctx->availAgentSize; i++) {
        ROCP_CALL_CK(rocprofiler_iterate_info, (&(ctx->availAgentArray[i]),
                                                ROCPROFILER_INFO_KIND_METRIC,
                                                count_native_events_cb,
                                                &maxEventSize),
                     return PAPI_EMISC);
    }

    ctx->availEventDeviceNum = papi_calloc(maxEventSize, sizeof(int));
    CHECK_PRINT_EVAL((ctx->availEventDeviceNum == NULL),
                     "ERROR ROCM: Could not allocate memory",
                     return PAPI_ENOMEM);

    ctx->availEventIDArray = papi_calloc(maxEventSize, sizeof(EventID));
    CHECK_PRINT_EVAL((ctx->availEventIDArray == NULL),
                     "ERROR ROCM: Could not allocate memory",
                     return PAPI_ENOMEM);

    ctx->availEventIsBeingMeasuredInEventset = papi_calloc(maxEventSize,
                                                           sizeof(uint32_t));
    CHECK_PRINT_EVAL((ctx->availEventIsBeingMeasuredInEventset == NULL),
                     "ERROR ROCM: Could not allocate memory",
                     return PAPI_ENOMEM);

    ctx->availEventDesc = papi_calloc(maxEventSize, sizeof(ev_name_desc_t));
    CHECK_PRINT_EVAL((ctx->availEventDesc == NULL),
                     "ERROR ROCM: Could not allocate memory",
                     return PAPI_ENOMEM);

    for (i = 0; i < ctx->availAgentSize; ++i) {
        events_cb_arg_t arg;
        arg.device_num = i;
        arg.count = maxEventSize;
        arg.ctx = ctx;
        ROCP_CALL_CK(rocprofiler_iterate_info, (&(ctx->availAgentArray[i]),
                                                ROCPROFILER_INFO_KIND_METRIC,
                                                add_native_events_cb,
                                                &arg),
                    return PAPI_EMISC);
    }

    return PAPI_OK;
}

hsa_status_t
count_native_events_cb( const rocprofiler_info_data_t info, void *arg )
{
    const uint32_t instances = info.metric.instances;
    uint32_t *count = (uint32_t *) arg;
    (*count) += instances;
    return HSA_STATUS_SUCCESS;
}


hsa_status_t
add_native_events_cb( const rocprofiler_info_data_t info, void *arg )
{
    uint32_t ui;
    events_cb_arg_t *cb_arg = (events_cb_arg_t *) arg;
    _rocm_context_t *ctx = cb_arg->ctx;
    const uint32_t eventDeviceNum = cb_arg->device_num;
    const uint32_t count = cb_arg->count;
    uint32_t index = ctx->availEventSize;
    const uint32_t instances = info.metric.instances;


    if (index + instances > count) {
        return HSA_STATUS_ERROR;
    }

    for (ui = 0; ui < instances; ui++) {
        char ROCMname[PAPI_MAX_STR_LEN];

        if (instances > 1) {
            snprintf(ctx->availEventDesc[index].name,
                     PAPI_MAX_STR_LEN, "%s:device=%d:instance=%d",
                     info.metric.name, eventDeviceNum, ui);
            snprintf(ROCMname, PAPI_MAX_STR_LEN, "%s[%d]",
                     info.metric.name, ui); /* indexed version */
        } else {
            snprintf(ctx->availEventDesc[index].name,
                     PAPI_MAX_STR_LEN, "%s:device=%d",
                     info.metric.name, eventDeviceNum);
            snprintf(ROCMname, PAPI_MAX_STR_LEN, "%s",
                     info.metric.name); /* non-indexed version */
        }

        ROCMname[PAPI_MAX_STR_LEN - 1] = '\0';
        strncpy(ctx->availEventDesc[index].description, info.metric.description,
                PAPI_2MAX_STR_LEN);
        ctx->availEventDesc[index].description[PAPI_2MAX_STR_LEN - 1] = '\0';

        EventID eventId;
        eventId.kind = ROCPROFILER_FEATURE_KIND_METRIC;
        eventId.name = strdup(ROCMname);
        eventId.parameters = NULL;
        eventId.parameter_count = 0;

        ctx->availEventDeviceNum[index] = eventDeviceNum;
        ctx->availEventIDArray[index] = eventId;
        index++;
        ctx->availEventSize = index;
    }

    return HSA_STATUS_SUCCESS;
}

int
check_rocm_num_native_events( void )
{
    if (_rocm_vector.cmp_info.num_native_events == 0) {
        char *metrics = getenv("ROP_METRICS");
        int strErr = snprintf(_rocm_vector.cmp_info.disabled_reason,
                              PAPI_MAX_STR_LEN,
                              "No events. Ensure ROCP_METRICS=%s is correct.",
                              metrics);
        _rocm_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN - 1] = 0;
        if (strErr > PAPI_MAX_STR_LEN) {
            HANDLE_STRING_ERROR;
        }
        return PAPI_EMISC;
    }

    return PAPI_OK;
}

int
foreach_event_do_update( hwd_control_state_t *ctrl, NativeInfo_t *nativeInfo,
                         int nativeCount )
{
    int status;

    int ii;
    for (ii = 0; ii < nativeCount; ii++) {

        const char *eventName;
        int eventDeviceNum;
        int ntv_event_idx = get_users_ntv_event(ii, nativeCount, nativeInfo,
                                                &eventName, &eventDeviceNum);
        if (ntv_event_idx < 0) {
            continue;
        }

        int eventContextIdx = find_or_create_context(eventName, eventDeviceNum);

        _rocm_active_context_t *eventctrl;
        status = get_event_control(ntv_event_idx, eventContextIdx,
                                   &eventctrl);
        if (status != PAPI_OK)
            return status;
        }

        status = open_rocm_event(eventDeviceNum, eventctrl, ctrl);
        if (status != PAPI_OK) {
            return status;
        }
    }

    return PAPI_OK;
}

int
get_users_ntv_event( int usr_idx, int ntv_range __attribute__((unused)),
                     NativeInfo_t *ntv_info,
                     const char **eventName __attribute__((unused)),
                     int *eventDeviceNum )
{
    _rocm_context_t *gctxt = global__rocm_context;

    /* Get the PAPI event index from the user */
    int index = ntv_info[usr_idx].ni_event;
    *eventName = gctxt->availEventDesc[index].name;
    *eventDeviceNum = gctxt->availEventDeviceNum[index];

    if (gctxt->availEventIsBeingMeasuredInEventset[index] == 1) {
        ROCMDBG("Skipping event %s (%i of %i) which is already added\n",
                *eventName, user_idx, ntv_range);
        return -1;
    }

    gctxt->availEventIsBeingMeasuredInEventset[index] = 1;
    ROCMDBG("Need to add event %d %s to the context\n", index, *eventName);

    return index;
}

int
find_or_create_context( const char *eventName __attribute__((unused)),
                        int eventDeviceNum )
{
    _rocm_control_t *gctrl = global__rocm_control;

    /* Find context/control in papirocm, creating it if does not exist */
    uint32_t cc;
    for (cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        CHECK_PRINT_EVAL(cc >= PAPIROCM_MAX_COUNTERS, "Exceeded hardcoded "
                         "maximum number of contexts (PAPIROCM_MAX_COUNTERS)",
                         return PAPI_EMISC);
        if (gctrl->arrayOfActiveContexts[cc]->deviceNum == eventDeviceNum) {
            break;
        }
    }

    if (cc == gctrl->countOfActiveContexts) {
        ROCMDBG("Event %s device %d does not have a ctx registered yet...\n",
                eventName, eventDeviceNum);

        gctrl->arrayOfActiveContexts[cc] =
            papi_calloc(1, sizeof(_rocm_active_context_t));

        CHECK_PRINT_EVAL(gctrl->arrayOfActiveContexts[cc] == NULL,
                         "Memory allocation for new active context failed",
                         return PAPI_ENOMEM);

        gctrl->arrayOfActiveContexts[cc]->deviceNum = eventDeviceNum;
        gctrl->arrayOfActiveContexts[cc]->ctx = NULL;
        gctrl->arrayOfActiveContexts[cc]->conEventsCount = 0;
        gctrl->countOfActiveContexts++;

        ROCMDBG("Added a new context deviceNum %d ... now "
                "countOfActiveContexts is %d\n", eventDeviceNum,
                gctrl->countOfActiveContexts);
    }

    return cc;
}

int
get_event_control( int ntv_event_idx, int eventContextIdx,
                   _rocm_active_context_t **eventctrl )
{
    _rocm_context_t *gctxt = global__rocm_context;
    _rocm_control_t *gctrl = global__rocm_control;

    *eventctrl = gctrl->arrayOfActiveContexts[eventContextIdx];

    if ((*eventctrl)->conEventsCount >= PAPIROCM_MAX_COUNTERS) {
        ROCMDBG("Num events exceeded PAPIROCM_MAX_COUNTERS\n");
        return PAPI_EINVAL;
    }

    /* lookup eventid for this event index */
    EventID eventId = gctxt->availEventIDArray[ntv_event_idx];
    (*eventctrl)->conEvents[(*eventctrl)->conEventsCount] = eventId;
    (*eventctrl)->conEventIndex[(*eventctrl)->conEventsCount] = ntv_event_idx;
    (*eventctrl)->conEventsCount++;

    return PAPI_OK;
}

int
open_rocm_event( int eventDeviceNum, _rocm_active_context_t *eventctrl,
                 hwd_control_state_t *ctrl )
{
    _rocm_context_t *gctxt = global__rocm_context;

    ROCMDBG("Create eventGroupPasses for context (destroy pre-existing) "
            "(nativeCount %d, conEventsCount %d) \n",
            global__rocm_control->activeEventCount, eventctrl->conEventsCount);

    if (eventctrl->conEventsCount > 0) {
        if (eventctrl->ctx != NULL) {
            ROCP_CALL_CK(rocprofiler_close, (eventctrl->ctx),
                         return PAPI_EMISC);
        }

        int openFailed = 0;
        const uint32_t mode = (global__ctx_properties.queue != NULL) ?
                ROCPROFILER_MODE_STANDALONE :
                ROCPROFILER_MODE_STANDALONE | ROCPROFILER_MODE_CREATEQUEUE;

        ROCP_CALL_CK(rocprofiler_open,
                     (gctxt->availAgentArray[eventDeviceNum],
                      eventctrl->conEvents, eventctrl->conEventsCount,
                      &(eventctrl->ctx), mode, &global__ctx_properties),
                     openFailed = 1);

        if (openFailed) {
            ROCMDBG("Error occurred: The ROCM event was not accepted by the "
                    "ROCPROFILER.\n");
            _rocm_cleanup_eventset(ctrl);
            return PAPI_ECOMBO;
        }

        uint32_t numPasses;
        ROCP_CALL_CK(rocprofiler_group_count, (eventctrl->ctx, &numPasses),
                     return PAPI_EMISC);

        if (numPasses > 1) {
            ROCMDBG("Error occurred: The combined ROCM events require more "
                    "than 1 pass... try different events\n");
            _rocm_cleanup_eventset(ctrl);
            return PAPI_ECOMBO;
        } else  {
            ROCMDBG("Created eventGroupPasses for context total-events %d "
                    "in-this-context %d passes-required %d) \n",
                    global__rocm_control->activeEventCount,
                    eventctrl->conEventsCount,
                    numPasses);
        }
    }

    return PAPI_OK;
}

int
get_event_interval_length( uint64_t *durationNs )
{
    _rocm_control_t *gctrl = global__rocm_control;

    ROCM_CALL_CK(hsa_system_get_info,
                 (HSA_SYSTEM_INFO_TIMESTAMP, &gctrl->readTimestampNs),
                 return PAPI_EMISC);

    *durationNs = gctrl->readTimestampNs - gctrl->startTimestampNs;
    gctrl->startTimestampNs = gctrl->readTimestampNs;

    return PAPI_OK;
}

int
foreach_context_do_read( uint64_t durationNs, long long **values )
{
    _rocm_control_t *gctrl = global__rocm_control;

    uint32_t cc;
    for (cc = 0; cc < gctrl->countOfActiveContexts; cc++) {
        int eventDeviceNum = gctrl->arrayOfActiveContexts[cc]->deviceNum;
        Context eventCtx = gctrl->arrayOfActiveContexts[cc]->ctx;

        ROCMDBG("Read device %d ctx %p(%u) ts %lu\n", eventDeviceNum, eventCtx,
                cc, gctrl->readTimestampNs);

        ROCP_CALL_CK(rocprofiler_read, (eventCtx, 0), return PAPI_EMISC);

        ROCMDBG("waiting for data\n");

        ROCP_CALL_CK(rocprofiler_get_data, (eventCtx, 0), return PAPI_EMISC);
        ROCP_CALL_CK(rocprofiler_get_metrics, (eventCtx), return PAPI_EMISC);

        ROCMDBG("done\n");

        foreach_event_do_read(eventDeviceNum, cc, durationNs);
    }

    *values = gctrl->activeEventValues;
    return PAPI_OK;
}

void
foreach_event_do_read( int eventDeviceNum, int ctx_idx,
                       uint64_t durationNs __attribute__((unused)) )
{
    _rocm_context_t *gctxt = global__rocm_context;
    _rocm_control_t *gctrl = global__rocm_control;

    uint32_t jj;
    for (jj = 0; jj < gctrl->activeEventCount; jj++) {
        int index = gctrl->activeEventIndex[jj];

        ROCMDBG("jj=%i of %i, index=%i, device#=%i.\n", jj,
                gctrl->activeEventCount, index,
                gctxt->availEventDeviceNum[index]);

        if(gctxt->availEventDeviceNum[index] != eventDeviceNum) {
            continue;
        }

        uint32_t ee;
        for (ee = 0; ee < gctrl->arrayOfActiveContexts[ctx_idx]->conEventsCount; ee++) {
            ROCMDBG("Searching for activeEvent %s in Activecontext %u "
                    "eventIndex %d duration %lu\n", gctxt->availEventIDArray[index].name,
                    ee, index, durationNs);

            if (gctrl->arrayOfActiveContexts[ctx_idx]->conEventIndex[ee] == index) {
                gctrl->activeEventValues[jj] =
                    gctrl->arrayOfActiveContexts[ctx_idx]->conEvents[ee].data.
                        result_int64;

                ROCMDBG("Matched event %d:%d eventName %s value %lld\n", jj,
                        index, gctxt->availEventIDArray[index].name,
                        gctrl->activeEventValues[jj]);

                break;
            }
        }
    }
}
