#ifndef __STRLIB_H__
#define __STRLIB_H__

#include <stdint.h>
#ifndef NULL
  #define NULL ((void*)0)
#endif

//if buf == NULL then uses internal global buffer

// uint8_t to binary (0b...). bits - number of bits printed
// return value - pointer to first char of string (buf[0])
char* utobin(char *buf, uint32_t val, uint8_t bits);

// int32_t to fixed-point; dot - number of digits after the decimal point; field - minimal field size
// return value - pointer to first char of string (not necessarily buf[0])
char* fpi32tos(char *buf, int32_t val, uint8_t dot, int8_t field);

// int32_t to fixed-point; dot - number of digits after the decimal point; field - minimal field size
// changes only [field] or [len(val)] chars. Does not add '\0'
// if field is less than the required space, fills all [field] by "+++" or "---" depends on sign
// return value - pointer to LAST char of string
char* fpi32tos_inplace(char *buf, int32_t val, uint8_t dot, int8_t field);

// uint32_t to hexadecimal
// digs - number of digits to print
// return value - pointer to first char of string (buf[0])
char* u32tohex(char *buf, uint32_t val, uint32_t digs);


// -DMEMFUNC or -DMEMFUNC_DMA=2,3
#if defined(MEMFUNC) || defined(MEMFUNC_DMA)
void* memcpy(void *dst, void *src, uint32_t len);
void *memset(void *s, int c, uint32_t n);
#endif

#ifdef STRMATCH_FUNC
char* strstr(char *str, char *substr);
#endif

#endif