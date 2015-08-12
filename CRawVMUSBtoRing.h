
#include "IOU16.h"

#include <vector>
#include <cstdint>
#include <ctime>

class CRingBuffer;

class CRawVMUSBtoRing
{
  // Private data type:
  typedef uint64_t (*TimestampExtractor)(void*);

  uint32_t    m_runNumber;	// Run number;
  std::string m_title;          // Run title

  // other data:
  private:
  int         m_elapsedSeconds;	   /* Seconds into the run. */
  timespec    m_startTimestamp;    //!< Run start time.
  timespec    m_lastStampedBuffer; //!< Seconds into run of last stamped buffer.
  size_t      m_nOutputBufferSize;       //!< size of output buffer in bytes.
  //!< determined at the start of a run.
  uint8_t*    m_pBuffer;	         //!< Pointer to the current buffer.
  uint8_t*    m_pCursor;           //!< Where next event goes in buffer.
  size_t      m_nWordsInBuffer;    //!< Number of words already in the buffer.
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
  CRawVMUSBtoRing(CRingBuffer* pRingBuffer);

  void processBuffer(std::vector<IOU16>& buffer);     // 
  void processEvents(std::vector<IOU16>& buffer);     //
  uint8_t* newOutputBuffer();                //
  void event(IOU16* pData);      //
  void scaler(IOU16* pData);	//
  void outputTriggerCount(uint32_t runOffset);
  void getTimestampExtractor();

  void setOptHeader(bool onoff) { m_optionalHeaderExists = onoff; }
  bool getOptHeader() const { return m_optionalHeaderExists; }
  bool hasOptionalHeader();

  void setTimestampExtractorLib(std::string path) { m_tstampExtractorLib = path; }
  void setScalerPeriod(int nsecs) { m_scalerPeriod = nsecs; }
  void setSourceId(int id) { m_sourceId = id; }

};
