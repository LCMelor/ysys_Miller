#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  // return the number of characters printed
  int count = 0;
  while(*fmt) {
    // if it is a conversion specifier
    if(*fmt == '%') {
      // handle %d and %s
      fmt++;
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
