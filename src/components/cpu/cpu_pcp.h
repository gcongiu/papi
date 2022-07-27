/**
 * @file    cpu_pcp.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */
#ifndef __CPU_PCP_H__
#define __CPU_PCP_H__

#include "common.h"

int pcp_init(ntv_event_table_t **, const char **);
int pcp_shutdown(void);
int pcp_ctx_open(prof_info_t *, prof_ctx_t *);
int pcp_ctx_close(prof_ctx_t);
int pcp_ctx_read(prof_ctx_t, long long **);
int pcp_ctx_write(prof_ctx_t, long long *);
int pcp_ctx_reset(prof_ctx_t);
int pcp_ctx_ctl(int, prof_ctx_t);

#endif /* End of __CPU_PCP_H__ */
