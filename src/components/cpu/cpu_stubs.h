/**
 * @file    cpu_stubs.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */
#ifndef __CPU_STUBS_H__
#define __CPU_STUBS_H__

#include "common.h"

int stub_init(ntv_event_table_t **, const char **);
int stub_shutdown(void);
int stub_ctx_open(prof_info_t *, prof_ctx_t *);
int stub_ctx_close(prof_ctx_t);
int stub_ctx_read(prof_ctx_t, long long **);
int stub_ctx_write(prof_ctx_t, long long *);
int stub_ctx_reset(prof_ctx_t);
int stub_ctx_ctl(int, prof_ctx_t);

#endif /* End of __CPU_STUBS_H__ */
