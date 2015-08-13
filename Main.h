

#ifndef MAIN_H
#define MAIN_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>

class CRingBuffer;

class Main 
{

  private:
    boost::program_options::options_description desc;
    boost::program_options::variables_map       vm;

    std::string   output_ring;
    std::string   input_file_path;
    std::string   libname;
    int           sourceId;
    size_t        nToProcess;
    size_t        nToSkip;
    bool          verboseOutput;
    size_t        index;


  public:
    Main();

    int run(int argc, char** argv);

    CRingBuffer* attachRing(std::string ringName);

    void setUpCommandLineOptions();
    void parseCommandLine(int argc, char** argv);
    void validateCommandLine();
    int  mainLoop();

    void setVerbose(bool onoff) { verboseOutput = onoff; }

};

#endif
