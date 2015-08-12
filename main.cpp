#include <CRingBuffer.h>
#include <CRawVMUSBtoRing.h>
#include <IOU16.h>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <iterator>
#include <algorithm>

using namespace std;

string output_ring = "rawvmusb";
string input_file_path =  "run59-VM0079.cra";

/**
 * Attach to the ring buffer specified in m_ringName.  If that ring does not
 * yet exist, it is created.  A pointer to the created CRingBuffer object is
 * put in m_pRing
 *
 * @throws std::string if we can't attach to the ring or creation fails.
 *                     The actual exceptions are tossed from the bowels of
 *                     the CRingBuffer class.
 */
CRingBuffer* attachRing(std::string ringName)
{
  return CRingBuffer::createAndProduce(ringName);
}





int main(int argc, char* argv[])
{
  if (argc!=5) {
    cout << "Usage: crate2ring input_fileringname tstamplib id" << endl; 
    return 1;
  }

  input_file_path = argv[1];
  output_ring = argv[2];
  std::string libname = argv[3];
  int sourceId = std::atoi(argv[4]);
  
  CRingBuffer* pRing = attachRing(output_ring);
  if (!pRing) {
    cout << "Failed to open ring " << output_ring << endl;
    return 1;
  }

  std::ifstream stream(input_file_path.c_str(), ios::binary | ios::in);

  CRawVMUSBtoRing convertAndOutput(pRing);
  convertAndOutput.setSourceId(sourceId);
  convertAndOutput.setTimestampExtractorLib(libname);


  convertAndOutput.getTimestampExtractor();

  while (1) {

    std::vector<IOU16> buffer(2);
    stream >> buffer[0] >> buffer[1];
     
    if (stream.rdstate() != 0) {
      if (stream.eof()) {
        cout << "Found end of file" << endl;
      } else {
        cout << "Error state in file stream! " << endl;
        cout << "Bad bit = " << stream.bad() << endl;
        cout << "Fail bit = " << stream.fail() << endl;
      }
      break;
    }

//    cout << buffer[0].value << endl;
//    cout << buffer[1].value << endl;

    uint16_t nLeftToRead = buffer[1].value;
    copy_n( istream_iterator<IOU16>(stream), nLeftToRead, back_inserter(buffer) );

    convertAndOutput.processBuffer( buffer );
  }

  return 0;
}
