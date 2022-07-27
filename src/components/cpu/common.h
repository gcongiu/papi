/**
 * @file    common.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#define CPU_MAX_MPX_COUNTERS (384)

typedef struct perf_ctx *perf_ctx_t;

typedef struct {
    char *name;
    char *descr;
    unsigned int id;
} ntv_event_t;

typedef struct {
    ntv_event_t *events;
    unsigned int count;
} cpu_event_table_t;

typedef struct {
    int num_events;
    unsigned int domain;
    unsigned int granularity;
    unsigned int multiplexed;
    unsigned int overflow;
    unsigned int inherit;
    unsigned int overflow_signal;
    unsigned int attached;
    int cidx;
    int cpu;
    pid_t tid;
    int events_id[CPU_MAX_MPX_COUNTERS];
    cpu_event_table_t *ntv_table;
} prof_info_t;

#endif /* End of __COMMON_H__ */
