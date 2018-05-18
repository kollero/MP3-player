#include <vstypes.h>

extern const u_int16 app123[];

const struct {
  char device;
  char colon;
  void *address;
  char endzero;
} lib123 = {'X', ':', app123, 0};

#define 123 (char*)(&lib123)
