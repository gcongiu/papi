/**
 * @file    pet.h
 * @author  Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 */
#ifndef __PET_H__
#define __PET_H__

int PET_get_events(int cmpid, int dev_id, int event_count, int *events);
int PET_init_eventset(int event_count, int *events, int *eventset);
int PET_run_tests(int eventset, int iterations, int deviation,
                  void (*kernel)(void *args), void *args);
int PET_shutdown_eventset(int *eventset);

#endif /* End of __PET_H__ */
