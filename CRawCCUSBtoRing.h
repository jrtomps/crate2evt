
#ifndef CRAWCCUSBTORING_H
#define CRAWCCUSBTORING_H

#include "CRawXXUSBtoRing.h"

class CRawCCUSBtoRing : public CRawXXUSBtoRing
{
  public:
    CRawCCUSBtoRing(CRingBuffer* pRing);

    virtual void 
      validateEndOfBuffer(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer);

    virtual std::vector<uint32_t> 
      extractScalerData(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer,
          size_t nScalers);

    virtual void 
      formAndOutputScalerItem(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer, 
        const std::vector<uint32_t>& scalers,
        uint32_t endTime);

    virtual void formAndOutputPhysicsEventItem();
};
#endif
