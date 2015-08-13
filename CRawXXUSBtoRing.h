#ifndef CRAWXXUSBTORING_H
#define CRAWXXUSBTORING_H

#include <ByteBuffer.h>
#include <Deserializer.h>
#include "IOU16.h"

#include <vector>
#include <cstdint>
#include <ctime>

class CRingItem;
class CRingBuffer;

class CRawXXUSBtoRing
{
  // Private data type:
  typedef uint64_t (*TimestampExtractor)(void*);

  // other data:
  protected:
  int         m_elapsedSeconds;	   /* Seconds into the run. */
  timespec    m_startTimestamp;    //!< Run start time.
  timespec    m_lastStampedBuffer; //!< Seconds into run of last stamped buffer.
  size_t      m_nOutputBufferSize;       //!< size of output buffer in bytes.
  //!< determined at the start of a run.
  std::vector<uint8_t>   m_buffer;
  CRingBuffer* m_pRing;		    //!< The actual ring in which we put data.
  uint64_t    m_nEventsSeen;        //!< Events processed so far for the physics trigger item.
  unsigned    m_nBuffersBeforeEventCount; //!< Buffers to go before an event count item.
  bool        m_optionalHeaderExists;
  int         m_scalerPeriod;
  int         m_sourceId;
  std::string m_tstampExtractorLib;
  TimestampExtractor m_pEvtTimestampExtractor;
  TimestampExtractor m_pSclrTimestampExtractor;

  public:
  CRawXXUSBtoRing(CRingBuffer* pRingBuffer);
  void processBuffer(DAQ::Buffer::ByteBuffer& buffer);     // 
  void processEvents(DAQ::Buffer::ByteBuffer& buffer);     //
  uint8_t* newOutputBuffer();                //
  void event(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer);      //
  void scaler(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer);	//
  void outputTriggerCount(uint32_t runOffset);
  void getTimestampExtractor();

  void setOptionalHeader(bool onoff) { m_optionalHeaderExists = onoff; }
  bool getOptionalHeader() const { return m_optionalHeaderExists; }
  bool hasOptionalHeader();

  void setTimestampExtractorLib(std::string path) { m_tstampExtractorLib = path; }
  void setScalerPeriod(int nsecs) { m_scalerPeriod = nsecs; }
  void setSourceId(int id) { m_sourceId = id; }

  void validateOptionalHeader(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer,
                              size_t nWords);
  virtual void 
    validateEndOfBuffer(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer) = 0;

  virtual std::vector<uint32_t> 
    extractScalerData(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer,
                      size_t nScalers) = 0;
  virtual void 
    formAndOutputScalerItem(DAQ::Buffer::Deserializer<DAQ::Buffer::ByteBuffer>& buffer, 
                               const std::vector<uint32_t>& scalers,
                               uint32_t endTime) = 0;
  virtual void 
    formAndOutputPhysicsEventItem() = 0;
  void fillBodyWithData(CRingItem& event, const std::vector<uint8_t>& data);

  bool eventComplete(uint16_t header);
};

#endif
