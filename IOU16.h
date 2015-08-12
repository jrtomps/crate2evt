
#ifndef IOU16_H
#define IOU16_H 1

#include <iostream>
#include <cstdint>

union IOU16
{
  uint16_t value;
  char     bytes[sizeof(uint16_t)];
};

inline std::istream& operator>>(std::istream& stream, IOU16& val)
{
  stream.read(val.bytes, sizeof(val));
  return stream;
}


#endif
