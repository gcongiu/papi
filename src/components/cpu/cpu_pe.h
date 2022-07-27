/**
 * @file    cpu_pe.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */
#ifndef __CPU_PE_H__
#define __CPU_PE_H__

#include "common.h"

int pe_init(ntv_event_table_t **, const char **);
int pe_shutdown(void);
int pe_ctx_open(prof_info_t *, prof_ctx_t *);
int pe_ctx_close(prof_ctx_t);
int pe_ctx_read(prof_ctx_t, long long **);
int pe_ctx_write(prof_ctx_t, long long *);
int pe_ctx_reset(prof_ctx_t);
int pe_ctx_ctl(int, prof_ctx_t);

#endif /* End of __CPU_PE_H__ */
