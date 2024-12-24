#ifndef AD1DEBE_PAGER_H
#define AD1DEBE_PAGER_H

#include <stdio.h>
#include <stdint.h>
typedef struct {
  FILE* file_descriptor;
  uint32_t file_length;
} debe_pager;

#endif
