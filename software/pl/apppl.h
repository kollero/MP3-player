#include <vstypes.h>

extern const u_int16 apppl[];

const struct {
  char device;
  char colon;
  void *address;
  char endzero;
} libpl = {'X', ':', apppl, 0};

#define pl (char*)(&libpl)
