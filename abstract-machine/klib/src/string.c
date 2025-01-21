#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(s != NULL && s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  char * p = dst;
  while(*src != '\0') {
    *p = *src;
    p++;
    src++;
  }
  *p = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char * p = dst;
  size_t i = 0;

  for(i = 0; i < n && src[i] != '\0'; i++) {
    p[i] = src[i];
  }
  for(; i < n; i++) {
    p[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *p = dst;
  while(*p != '\0') {
    p++;
  }

  while(*src != '\0') {
    *p = *src;
    p++;
    src++;
  }
  *p = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1 != '\0' && *s2 != '\0') {
    if(*s1 != *s2) {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
  while(i < n && s1[i] != '\0' && s2[i] != '\0') {
    if(s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
    i++;
  }
  if(i == n) {
    return 0;
  }
  return s1[i] - s2[i];
}

void *memset(void *s, int c, size_t n) {
  char *p = s;
  for(size_t i = 0; i < n; i++) {
    p[i] = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = dst;
  char tmp[n];
  for(size_t i = 0; i < n; i++) {
    tmp[i] = ((char *)src)[i];
  }
  for(size_t i = 0; i < n; i++) {
    d[i] = tmp[i];
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *o = out;
  char *i = (char *)in;
  for(size_t j = 0; j < n; j++) {
    o[j] = i[j];
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  char *p1 = (char *)s1;
  char *p2 = (char *)s2;
  for(size_t i = 0; i < n; i++) {
    if(p1[i] != p2[i]) {
      return p1[i] - p2[i];
    }
  }
  return 0;
}

#endif
