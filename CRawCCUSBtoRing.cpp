
#include <CRawCCUSBtoRing.h>
#include <CRingItem.h>
#include <CRingScalerItem.h>
#include <CRingPhysicsEventCountItem.h>
#include <CPhysicsEventItem.h>
#include <ErrnoException.h>
#include <memory>

using namespace std;
using namespace DAQ::Buffer;

CRawCCUSBtoRing::CRawCCUSBtoRing(CRingBuffer* pRing)
  : CRawXXUSBtoRing(pRing) {}


  void 
CRawCCUSBtoRing::validateEndOfBuffer(Deserializer<ByteBuffer>& buffer)
{
  // Next long should be 0xffffffff buffer terminator:
  // I've seen this happen but it's not fatal...just go on to the next buffer.

  //  std::cout << "Checking EOB @ " << distance(buffer.begin(), buffer.pos()) << endl;
  uint32_t nextLong;
  buffer >> nextLong;
  if (nextLong != 0xffffffff) {
    cerr << "Ran out of events but did not see buffer terminator\n";
    cerr << distance(buffer.pos(), buffer.end()) << " bytes remaining unprocessed\n";
    cerr <<  "Observed instead 0x" << hex << nextLong << dec << endl;
  } else {
    //  cout << "FOUND" << endl;
  }
}

  std::vector<uint32_t> 
  CRawCCUSBtoRing::extractScalerData(Deserializer<ByteBuffer>& buffer,
      size_t nScalers)
{
  vector<uint32_t> counters;
  counters.reserve(nScalers);

  for (size_t i = 0; i < nScalers; i++) {
    uint32_t value;
    buffer >> value;
    counters.push_back(value);
  }

  return counters;
}

void CRawCCUSBtoRing::formAndOutputScalerItem(Deserializer<ByteBuffer>& buffer, 
    const std::vector<uint32_t>& scalers,
    uint32_t endTime)
{
  time_t timestamp;
  if (time(&timestamp) == -1) {
    throw CErrnoException("CRawXXUSBtoRing::scaler unable to get the absolute timestamp");
  }

  unique_ptr<CRingItem> pEvent;
  if (m_pSclrTimestampExtractor) {
    void* pData = const_cast<void*>(
                                    static_cast<const void*>(buffer.getContainer().data()
                                    +std::distance(buffer.begin(),buffer.pos())
                                              )
                  );
    pEvent.reset(new CRingScalerItem(m_pSclrTimestampExtractor(pData), 
                                 m_sourceId,
                                 0,
                                 m_elapsedSeconds, 
                                 endTime, 
                                 timestamp, 
                                 scalers,
				 1, false));
  } else {
    pEvent.reset(new CRingScalerItem(m_elapsedSeconds, 
                                 endTime, 
                                 timestamp, 
                                 scalers,
				 false));
  }

  pEvent->commitToRing(*m_pRing);
}


void CRawCCUSBtoRing::formAndOutputPhysicsEventItem()
{
    unique_ptr<CRingItem> pEvent;

    if (m_pEvtTimestampExtractor) {
      pEvent.reset(new CRingItem(PHYSICS_EVENT, 
                                 m_pEvtTimestampExtractor(m_buffer.data()), m_sourceId,
                                 0, m_buffer.size() + 100));        
    } else {
      pEvent.reset(new CRingItem(PHYSICS_EVENT, m_buffer.size() + 100)); 
    }

    CRingItem& event(*pEvent);
    fillBodyWithData(event, m_buffer);
}

