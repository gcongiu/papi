/**
 * @file    pet.c
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 *
 * @brief
 *  PAPI event test interfaces.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "papi.h"
#include "pet.h"

int
PET_get_events(int cmpid, int dev_id, int event_count, int *events)
{
    int pet_errno = PAPI_OK;
    const PAPI_component_info_t *cmpinfo = PAPI_get_component_info(cmpid);
    int num_native_events = cmpinfo->num_native_events;

    int *native_events = calloc(num_native_events, sizeof(int));
    if (native_events == NULL) {
        pet_errno = PAPI_ENOMEM;
        goto fn_fail;
    }

    int i = 0;
    int mode = PAPI_ENUM_FIRST;
    int first_ntv_id = PAPI_NATIVE_MASK;
    pet_errno = PAPI_enum_cmp_event(&first_ntv_id, mode, cmpid);
    if (pet_errno != PAPI_OK) {
        goto fn_fail;
    }

    char search_string[32] = { 0 };
    sprintf(search_string, "device=%d", dev_id);
    PAPI_event_info_t ntv_info;

    while (i < event_count) {
        int ntv_id = first_ntv_id + (rand() % num_native_events);
        pet_errno = PAPI_get_event_info(ntv_id, &ntv_info);
        if (pet_errno != PAPI_OK) {
            goto fn_fail;
        }

        if (dev_id >= 0 && strstr(ntv_info.symbol, search_string) == NULL) {
            continue;
        }

        if (native_events[ntv_id] == 0) {
            native_events[ntv_id] = 1;
            events[i++] = ntv_id;
        }
    }

  fn_exit:
    free(native_events);
    return pet_errno;
  fn_fail:
    goto fn_exit;
}

int
PET_init_eventset(int event_count, int *events, int *eventset)
{
    int pet_errno = PAPI_OK;

    *eventset = PAPI_NULL;
    pet_errno = PAPI_create_eventset(eventset);
    if (pet_errno != PAPI_OK) {
        goto fn_fail;
    }

    int i = 0;
    while (i < event_count) {
        pet_errno = PAPI_add_event(*eventset, events[i++]);
        if (pet_errno != PAPI_OK) {
            goto fn_fail;
        }
    }

  fn_exit:
    return pet_errno;
  fn_fail:
    if (*eventset != PAPI_NULL) {
        PAPI_cleanup_eventset(*eventset);
        PAPI_destroy_eventset(eventset);
    }
    goto fn_exit;
}

int
PET_run_tests(int eventset, int iter, int deviation, void (*kernel)(void *),
              void *args)
{
    int pet_errno = PAPI_OK;
    long long **counters = NULL;
    double *mean = NULL;
    double *stdev = NULL;

    counters = calloc(iter, sizeof(long long));
    if (counters == NULL) {
        pet_errno = PAPI_ENOMEM;
        goto fn_fail;
    }

    int num_events = PAPI_num_events(eventset);
    int i;
    for (i = 0; i < iter; ++i) {
        counters[i] = calloc(num_events, sizeof(long long));
        if (counters[i] == NULL) {
            pet_errno = PAPI_ENOMEM;
            goto fn_fail;
        }
    }

    int it;
    for (it = 0; it < iter; ++it) {
        pet_errno = PAPI_start(eventset);
        if (pet_errno != PAPI_OK) {
            goto fn_fail;
        }

        kernel(args);

        pet_errno = PAPI_stop(eventset, counters[it]);
        if (pet_errno != PAPI_OK) {
            goto fn_fail;
        }
    }

    stdev = calloc(num_events, sizeof(double));
    if (stdev == NULL) {
        pet_errno = PAPI_ENOMEM;
        goto fn_fail;
    }

    mean = calloc(num_events, sizeof(double));
    if (mean == NULL) {
        pet_errno = PAPI_ENOMEM;
        goto fn_fail;
    }

    int evt;
    for (evt = 0; evt < num_events; ++evt) {
        for (it = 0; it < iter; ++it) {
            mean[evt] += (double) counters[it][evt];
        }
        mean[evt] /= num_events;
    }

    for (evt = 0; evt < num_events; ++evt) {
        for (it = 0; it < iter; ++it) {
            stdev[evt] += pow((double) counters[it][evt] - mean[evt], 2);
        }
        stdev[evt] = sqrt(stdev[evt] / iter);
    }

    for (evt = 0; evt < num_events; ++evt) {
        if (stdev[evt] > ((double) deviation / 100.0)) {
            pet_errno = PAPI_EMISC;
        }
    }

  fn_exit:
    if (counters) {
        int j;
        for (j = 0; j < i; ++j) {
            free(counters[j]);
        }
        free(counters);
    }
    if (mean) {
        free(mean);
    }
    if (stdev) {
        free(stdev);
    }
    return pet_errno;
  fn_fail:
    goto fn_exit;
}

int
PET_shutdown_eventset(int *eventset)
{
    int pet_errno = PAPI_OK;

    pet_errno = PAPI_cleanup_eventset(*eventset);
    if (pet_errno != PAPI_OK) {
        goto fn_fail;
    }

    pet_errno = PAPI_destroy_eventset(eventset);

  fn_exit:
    *eventset = PAPI_NULL;
    return pet_errno;
  fn_fail:
    goto fn_exit;
}
