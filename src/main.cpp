#include "log.h"
#include "Memory.h"
#include "Tomasulo.h"
#include "Exceptions.h"
#include "log/FileLogWriter.h"
#include "log/StreamLogWriter.h"
#include "log/ILogFormatter.h"
#include "utility/stream_manip.h"
#include "tclap/CmdLine.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <sstream>

using namespace util;

static const std::string TAG = "main";
static const std::size_t TOMASULO_MEMORY_SIZE = 4 * 1024;
static const std::string FILE_EXT = ".hex";
static const std::string HEX_DIGIT = "0123456789abcdefABCDEF";

const StrongLogPtr logger(new Log("tomasulo log"));

/**
 * Formats log messages in the form:
 * [elapsed millis] level-initial tag: message
 */
class Formatter
  : public ILogFormatter
{
private:
  const std::chrono::system_clock::time_point start;

public:
  Formatter();
  Formatter(const Formatter&) = default;
  Formatter& operator=(const Formatter&) = delete;

  virtual std::string format(const LogMessage& msg) override;
};

/**
 * Encapsulates the command line options.
 */
struct ArgPack
{
  bool verbose;
  std::string fileName;
  LogLevel logLevel;
  bool logConsole;
  std::string logFileName;
};

/**
 * Parses the command line, storing the results in an ArgPack.
 */
static bool parseArgs(int argc, char* argv[], ArgPack& out);

/**
 * Populates memory with the contents of a .hex file.
 */
static bool loadFromFile(Memory& mem, const std::string& filename);

int main(int argc, char* argv[])
{
  // parameter parsing
  ArgPack args;
  if (!parseArgs(argc, argv, args))
  {
    return 1;
  }

  // logging initialization
  logger->setLevel(args.logLevel);
  StrongLogFormatterPtr formatter(new Formatter);
  if (args.logConsole)
  {
    StrongLogWriterPtr console(new StreamLogWriter(std::cerr));
    console->setFormatter(formatter);
    logger->addWriter("console", console);
  }
  if (!args.logFileName.empty())
  {
    StrongPtr<FileLogWriter> file(new FileLogWriter(args.logFileName));
    if (!file->isOpen())
    {
      std::cerr << "Unable to open log file: " << args.logFileName << std::endl;
      return 1;
    }
    file->setFormatter(formatter);
    logger->addWriter("file", file);
  }

  try
  {
    MemoryPtr memory(new Memory(TOMASULO_MEMORY_SIZE));
    if (!loadFromFile(*memory, args.fileName))
    {
      std::cerr << "Error reading file " << args.fileName << std::endl;
      return 1;
    }

    Tomasulo tomasulo(memory, args.verbose);
    tomasulo.run();
    logger->info(TAG) << "Execution finished in " << tomasulo.clocks()
      << " cycles";
  }
  catch (Exception& e)
  {
    logger->error(TAG) << "Aborted with exception: " << e;
    std::cerr << "Aborted with exception: " << e << std::endl;
  }

  return 0;
}

Formatter::Formatter()
  : start(std::chrono::system_clock::now())
{
}

std::string Formatter::format(const LogMessage& msg)
{
  auto elapsed = msg.timeStamp - start;
  auto elapsedMillis =
    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  std::ostringstream ss;
  ss << "[" << elapsedMillis << "] " << toString(msg.level).at(0) << " "
    << msg.tag << " " << msg.message;
  return ss.str();
}

bool parseArgs(int argc, char* argv[], ArgPack& out)
{
  using namespace TCLAP;
  try
  {
    CmdLine cmd(
      "Implements a Tomasulo processor for the DLX assembly language."
      );
    SwitchArg verbose("v", "verbose",
      "Enable extra output about the processor state", cmd, false
      );
    ValueArg<std::string> fileName("f", "file", "The input program file",
      true, "", "string", cmd
      );
    std::vector<std::string> logLevels{ "verbose", "debug", "info", "warning",
      "error"
    };
    ValuesConstraint<std::string> logLevelConstraint(logLevels);
    ValueArg<std::string> logLevel("", "loglevel", "The logging output level",
      false, "warning", &logLevelConstraint, cmd
      );
    SwitchArg logConsole("c", "console", "Echoes the log to stdout", cmd, 
      false
      );
    ValueArg<std::string> logFileName("", "logfile", 
      "The output file for logging information", false, "tomasulo.log", "path", 
      cmd
      );    

    cmd.parse(argc, argv);

    out.verbose = verbose.getValue();
    out.fileName = fileName.getValue();
    out.logConsole = logConsole.getValue();
    out.logFileName = logFileName.getValue();
    
    std::string level = logLevel.getValue();
    if (level == "verbose")
    {
      out.logLevel = LogLevel::Verbose;
    }
    else if (level == "debug")
    {
      out.logLevel = LogLevel::Debug;
    }
    else if (level == "info")
    {
      out.logLevel = LogLevel::Info;
    }
    else if (level == "warning")
    {
      out.logLevel = LogLevel::Warning;
    }
    else if (level == "error")
    {
      out.logLevel = LogLevel::Error;
    }
  }
  catch (ArgException& e)
  {
    std::cerr << "Error: " << e.error() << " for arg " << e.argId() 
      << std::endl;
    return false;
  }

  return true;
}

bool loadFromFile(Memory& mem, const std::string& filename)
{
  bool hasFileExt = filename.compare(
    filename.length() - FILE_EXT.length(),
    FILE_EXT.length(), FILE_EXT
    ) == 0;
  if (!hasFileExt)
  {
    logger->error(TAG) << "Invalid file type " << filename;
    return false;
  }

  std::ifstream file(filename.c_str(), std::ios::in);
  if (!file)
  {
    logger->error(TAG, "Unable to open file " + filename);
    return false;
  }

  std::size_t count = 0;
  logger->debug(TAG, "Loading from file " + filename);
  while (!file.eof())
  {
    std::string line;
    std::getline(file, line);
    //logger->verbose(TAG, "Read line \"" + line + "\"");

    // strip comments
    auto commentIdx = line.find_first_of('#');
    if (commentIdx != std::string::npos)
    {
      line.resize(commentIdx);
    }

    // relevant data in the line is
    // [32 bit address]: [bytes...]
    // A line without a colon is skipped
    auto colon = line.find(':');
    if (colon == std::string::npos)
    {
      continue;
    }

    std::istringstream is;

    // pull the address
    Address addr;
    is.str(line.substr(0, colon));
    is >> std::hex >> addr;
    //logger->verbose(TAG) << "Address: " << util::hex<Address> << addr;

    // find the hex data string after the colon
    auto start = line.find_first_of(HEX_DIGIT, colon + 1);
    auto end = line.find_last_of(HEX_DIGIT);
    is = std::istringstream(line.substr(start, end - start + 1));
    //logger->verbose(TAG, is.str());

    ByteBuffer buffer;
    buffer.reserve((end - start) / 2);
    while (is.rdbuf()->in_avail() > 0)
    {
      std::string str;
      std::stringstream temp;
      // must use an integer type because streams will treat *any* char type 
      // as an ascii character rather than an integer
      UWord byte;

      // pull 2 characters from the stream and convert them through a 
      // temporary stream buffer
      // why can't you pull them directly from the stream?  no idea, but if you 
      // try to do that it ignores the width directive
      is.width(2);
      is >> str;
      temp << str;
      temp >> std::hex >> byte;
      buffer.push_back(static_cast<Byte>(byte));
      //logger->verbose(TAG) << "Byte: " << util::hex<Byte> << byte;
    }

    count += buffer.size();
    mem.write(addr, buffer);
    logger->debug(TAG) << "Writing " << buffer.size() << " bytes to "
      << util::hex<Address> << addr;
  }

  logger->debug(TAG) << "Read in " << count << " bytes";
  return true;
}
