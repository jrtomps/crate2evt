


#include <Main.h>
#include <IOU16.h>
#include <CRawVMUSBtoRing.h>
#include <ByteBuffer.h>
#include <CRingBuffer.h>
#include <CRingStateChangeItem.h>
#include <CDataFormatItem.h>

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <chrono>

namespace po = boost::program_options;

using namespace std;

Main::Main()
  : desc(),
  vm(),
  output_ring("out"),
  input_file_path(),
  libname(),
  sourceId(0),
  runNo(0),
  nToProcess(std::numeric_limits<size_t>::max()),
  nToSkip(0),
  verboseOutput(false),
  index(std::numeric_limits<size_t>::max())
{
  setUpCommandLineOptions();
}

void Main::setUpCommandLineOptions() {
  desc.add_options()
    ("help", "display this help message")
    ("input_file", po::value<string>(), "crate file to read data from")
    ("ring_name", po::value<string>()->default_value("out"), "name of ring buffer to output to")
    ("tstamplib", "path to timestamp extractor library")
    ("id", po::value<int>(), "source id")
    ("run", po::value<int>(), "run")
    ("count", po::value<size_t>(), "number of buffers to process")
    ("skip", po::value<size_t>(), "number of buffers to skip")
    ("verbose", po::value<bool>(), "output debugging messages")
    ("dumpindex", po::value<size_t>()->default_value(0), "index of buffer to dump to file");
}

void Main::parseCommandLine(int argc, char** argv)
{
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
}


void Main::validateCommandLine()
{
  if (vm.count("help")) {
    cout << desc << endl;
    exit(0);
  }

  if (vm.count("input_file")) {
    input_file_path = vm["input_file"].as<string>();
  } else {
    throw runtime_error("User must specify an input file");
  }

  if (vm.count("ring_name")) {
    output_ring = vm["ring_name"].as<string>();
  }

  if (vm.count("tstamplib")) {
    libname = vm["tstamplib"].as<string>();
  } else {
    throw runtime_error("User must specify a tstamp lib file");
  }

  if (vm.count("id")) {
    sourceId = vm["id"].as<int>();
  }
  if (vm.count("run")) {
    runNo = vm["run"].as<int>();
  }
  
  if (vm.count("dumpindex")) {
    index = vm["dumpindex"].as<size_t>();
  }

  if (vm.count("count")) {
    nToProcess = vm["count"].as<size_t>();
  }

  if (vm.count("skip")) {
    nToSkip = vm["skip"].as<size_t>();
  }
}


int Main::run(int argc, char** argv)
{
  parseCommandLine(argc, argv);
  validateCommandLine();

  mainLoop();
}

/**
 * Attach to the ring buffer specified in m_ringName.  If that ring does not
 * yet exist, it is created.  A pointer to the created CRingBuffer object is
 * put in m_pRing
 *
 * @throws std::string if we can't attach to the ring or creation fails.
 *                     The actual exceptions are tossed from the bowels of
 *                     the CRingBuffer class.
 */
CRingBuffer* Main::attachRing(std::string ringName)
{
  return CRingBuffer::createAndProduce(ringName);
}

int Main::mainLoop()
{
  CRingBuffer* pRing = attachRing(output_ring);
  if (!pRing) {
    cout << "Failed to open ring " << output_ring << endl;
    return 1;
  }


  std::ifstream stream(input_file_path.c_str(), ios::binary | ios::in);
  if (!stream.is_open()) {
    cout << "Failed to open " << input_file_path << endl;
    return 1;
  }

  CRawVMUSBtoRing convertAndOutput(pRing);
  convertAndOutput.setSourceId(sourceId);
  convertAndOutput.setTimestampExtractorLib(libname);
  convertAndOutput.getTimestampExtractor();
  convertAndOutput.setOptionalHeader(true);

  emitFormat(pRing);
  emitBeginRun(pRing);

  size_t bufferCount = 0;
  while (bufferCount < nToSkip+nToProcess) {

    IOU16 bufHeader, optHeader;
    auto bufStartPos = stream.tellg();

    stream.read(bufHeader.bytes, sizeof(bufHeader));
    stream.read(optHeader.bytes, sizeof(optHeader));
     
    if (stream.rdstate() != 0) {
      if (stream.eof()) {
        cout << "Found end of file" << endl;
      } else {
        cout << "Error state in file stream! " << endl;
        cout << "Bad bit = " << stream.bad() << endl;
        cout << "Fail bit = " << stream.fail() << endl;
        cout << "Current pos = " << stream.tellg() << endl;
      }
      break;
    }

    // header specifies size of buffer, there are two more 
    // EOB words
    size_t nEOBWords = 2;
    size_t bytesInBuffer = (optHeader.value+nEOBWords)*sizeof(uint16_t);
    DAQ::Buffer::ByteBuffer buffer;
    buffer.reserve(bytesInBuffer);
    buffer << bufHeader.value;
    buffer << optHeader.value;
    buffer.resize(bytesInBuffer);
    
    stream.read(reinterpret_cast<char*>(buffer.data()+2*sizeof(uint16_t)),
                buffer.size()-nEOBWords*sizeof(uint16_t));

    if (bufferCount == index) {
      std::ofstream dbgFile("debug", ios::binary);
      dbgFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    if (bufferCount >= nToSkip) {
      if (verboseOutput) {
        cout << "Buffer information" << endl;
        cout << "Header  : 0x" << hex << bufHeader.value << dec << endl;
        cout << "OptHdr  : "   << optHeader.value << endl;
        cout << "Begin @ : "   << bufStartPos << endl;
        cout << "Nbytes  : "   << bytesInBuffer + nEOBWords*sizeof(uint16_t) << endl;
      }
      convertAndOutput.processBuffer( buffer );
    }

    ++bufferCount;
  }

  emitEndRun(pRing);

  return 0;
}


void Main::emitFormat(CRingBuffer* pRing)
{
  CDataFormatItem format;
  format.commitToRing(*pRing);
}

void Main::emitBeginRun(CRingBuffer* pRing)
{
  using namespace std::chrono;
  CRingStateChangeItem begin(0, sourceId, BEGIN_RUN, BEGIN_RUN,
      runNo,
      0,
      system_clock::to_time_t(system_clock::now()),
      "this is a fabricated begin", 1);

  begin.commitToRing(*pRing);
}
void Main::emitEndRun(CRingBuffer* pRing)
{
  using namespace std::chrono;
  CRingStateChangeItem end(0, sourceId, END_RUN, END_RUN,
      runNo,
      1,
      system_clock::to_time_t(system_clock::now()),
      "this is a fabricated end", 1);
  end.commitToRing(*pRing);
}
