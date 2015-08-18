
#ifndef CRAWVMUSBTORING_H
#define CRAWVMUSBTORING_H

#include "CRawXXUSBtoRing.h"

class CRawVMUSBtoRing : public CRawXXUSBtoRing
{
  public:
    CRawVMUSBtoRing(CRingBuffer* pRing);

    virtual void 
      validateEndOfBuffer(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer);

    virtual bool isScalerData(uint16_t bufHdr, uint16_t evtHdr);
//    virtual std::vector<uint32_t> 
//      extractScalerData(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer,
//          size_t nScalers);
//
//    virtual void 
//      formAndOutputScalerItem(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer, 
//        const std::vector<uint32_t>& scalers,
//        uint32_t endTime);
//
//    virtual void formAndOutputPhysicsEventItem();
};
#endif
