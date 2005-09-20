/*
 * $Id$
 * $Author$
 */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "regex.h"
#include "upcase.h"

#define BYTEWIDTH 8

int match_count (char *needle, char *haystack)
{
  int i, count, pos;
  struct re_pattern_buffer buf;
  char fastmap[(1 << BYTEWIDTH)];

  /* Case insensitive search options, among other things */
  buf.allocated = 0;
  buf.buffer = NULL;
  buf.fastmap = fastmap;
  buf.translate = upcase;

  struct re_registers regs;

  re_compile_pattern (needle, strlen (needle), &buf);
  re_compile_fastmap (&buf);

  count = 0;
  pos = 0;
  while ((i = re_search (&buf, haystack, strlen (haystack), pos, strlen (haystack), &regs)) != -1) {
    pos = i + 1;
#ifdef DEBUG 
    printf("found instance at %d, pos = %d, count = %d\n", i, pos, count);
#endif
    count = count + 1;
  }

#ifdef DEBUG
  printf("found %d total instances\n-------\n\n", count);
#endif
  return count;
}

