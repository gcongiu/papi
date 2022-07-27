/**
 * @file    cpu_prof.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */
#ifndef __CPU_PROF_H__
#define __CPU_PROF_H__

#include "common.h"

typedef struct prof_ctx *prof_ctx_t;

int prof_init(ntv_event_table_t **, const char **);
int prof_shutdown(void);
int prof_ctx_open(prof_info_t *, prof_ctx_t *);
int prof_ctx_close(prof_ctx_t);
int prof_ctx_read(prof_ctx_t, long long **);
int prof_ctx_write(prof_ctx_t, long long *);
int prof_ctx_reset(prof_ctx_t);
int prof_ctx_ctl(int, prof_ctx_t);

#endif /* End of __CPU_PROF_H__ */
