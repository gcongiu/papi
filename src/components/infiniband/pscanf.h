/* This file was taken from the tacc_stats utility, which is distributed 
 * under a GPL license.
 */
#ifndef _PSCANF_H_
#define _PSCANF_H_
#include <stdio.h>
#include <stdarg.h>

__attribute__((format(scanf, 2, 3)))
  static inline int pscanf(FILE *fh, const char *fmt, ...)
{
  int rc = -1;
  char file_buf[4096];
  va_list arg_list;
  va_start(arg_list, fmt);

  if (fh == NULL)
    goto out;
  setvbuf(fh, file_buf, _IOFBF, sizeof(file_buf));

  rc = vfscanf(fh, fmt, arg_list);
  fseek(fh, 0, SEEK_SET);

 out:
  va_end(arg_list);
  return rc;
}

#endif
