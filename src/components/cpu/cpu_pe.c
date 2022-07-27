/**
 * @file    cpu_pe.c
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */

#include "papi.h"
#include "cpu_pe.h"

int
pe_init(ntv_event_table_t **ntv_table, const char **err_string)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_shutdown(void)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_open(prof_info_t *info, prof_ctx_t *ctx)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_close(prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_read(prof_ctx_t ctx, long long **counters)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_write(prof_ctx_t ctx, long long *values)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_reset(prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}

int
pe_ctx_ctl(int code, prof_ctx_t ctx)
{
    int papi_errno = PAPI_OK;
    return papi_errno;
}
