/**
 * @file    cpu_prof.c
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */

#include "papi.h"
#include "cpu_prof.h"
#include "common.h"

#ifdef HAVE_PERF_EVENT
#include "cpu_pe.h"
#endif

#ifdef HAVE_PCP
#include "cpu_pcp.h"
#endif

int
prof_init(ntv_event_table_t **ntv_table, const char **err_string)
{
    int papi_errno;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_init(ntv_table, err_string);
#elif defined(HAVE_PCP)
    papi_errno = pcp_init(ntv_table, err_string);
#else
    papi_errno = stub_init(ntv_table, err_string);
#endif

    return papi_errno;
}

int
prof_shutdown(void)
{
    int papi_errno;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_shutdown();
#elif defined(HAVE_PCP)
    papi_errno = pcp_shutdown();
#else
    papi_errno = stub_shutdown();
#endif

    return papi_errno;
}

int
prof_ctx_open(prof_info_t *info, prof_ctx_t *ctx)
{
    int papi_errno;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_open(info, ctx);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_open(info, ctx);
#else
    papi_errno = stub_ctx_open(info, ctx);
#endif

    return papi_errno;
}

int
prof_ctx_close(prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_close(ctx);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_close(ctx);
#else
    papi_errno = stub_ctx_close(ctx);
#endif

    return papi_errno;
}

int
prof_ctx_read(prof_ctx_t ctx, long long **counters)
{
    int papi_errno = PAPI_OK;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_read(ctx, counters);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_read(ctx, counters);
#else
    papi_errno = stub_ctx_read(ctx, counters);
#endif

    return papi_errno;
}

int
prof_ctx_write(prof_ctx_t ctx, long long *values)
{
    int papi_errno = PAPI_OK;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_write(ctx, values);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_write(ctx, values);
#else
    papi_errno = stub_ctx_write(ctx, values);
#endif

    return papi_errno;
}

int
prof_ctx_reset(prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_reset(ctx);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_reset(ctx);
#else
    papi_errno = stub_ctx_reset(ctx);
#endif

    return papi_errno;
}

int
prof_ctx_ctl(int code, prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;

#if defined(HAVE_PERF_EVENT)
    papi_errno = pe_ctx_ctl(code, ctx);
#elif defined(HAVE_PCP)
    papi_errno = pcp_ctx_ctl(code, ctx);
#else
    papi_errno = stub_ctx_ctl(code, ctx);
#endif

    return papi_errno;
}
