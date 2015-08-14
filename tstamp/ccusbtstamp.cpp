
#include <cstdint>
#include <algorithm>

union Tstamp {
  uint64_t value;
  uint16_t shorts[4]; 
};

extern "C" {


  std::uint64_t getEventTimestamp(void* pBody) {
    uint16_t* pData = reinterpret_cast<uint16_t*>(pBody);
    pData += 7; // get to the ulm output
    pData++; // skip over trigger register
    Tstamp tstamp;
    std::copy(pData, pData+4, tstamp.shorts);

    return tstamp.value;
  }

}
