#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char out_buf[5000];
  int count = vsprintf(out_buf, fmt, args);
  for(int i = 0; i < count; i++) {
    putch(out_buf[i]);
  }
  va_end(args);

  return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int count = 0;
  bool zero_flag = false;
  int width = 0;
  while(*fmt) {
    // if it is a conversion specifier
    if(*fmt == '%') {
      // handle %d and %s
      fmt++;
      // flag
      if(*fmt == '0') {
        zero_flag = true;
        fmt++;
      }
      // width
      if(*fmt >= '1' && *fmt <= '9') {
        width = *fmt - '0';
        fmt++;
      }

      switch(*fmt) {
        case 'd': {
          int num = va_arg(ap, int);
          char buf[32];
          int i = 0;
          for(; num; num /= 10) {
            buf[i++] = num % 10 + '0';
          }
          if(i == 0) {
            buf[i++] = '0';
          }
          if(zero_flag) {
            while(i < width) {
              buf[i++] = '0';
            }
          }
          for(i--; i >= 0; i--) {
            *out = buf[i];
            out++;
            count++;
          }
          break;
        }
        case 's' : {
          const char *str = va_arg(ap, const char *);
          while(*str) {
            *out = *str;
            out++;
            str++;
            count++;
          }
        }
        break;
      }
    }
    else {
      *out = *fmt;
      out++;
      count++;
    }
    fmt++;
  }
  *out = '\0';
  
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int count = vsprintf(out, fmt, ap);
  va_end(ap);
  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
