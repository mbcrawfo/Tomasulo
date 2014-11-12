#include "tclap/CmdLine.h"
#include "log/Log.h"
#include "log/FileLogWriter.h"
#include "log/StreamLogWriter.h"
#include "log/ILogFormatter.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>

using namespace util;

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

int main(int argc, char* argv[])
{
  // parameter parsing
  ArgPack args;
  if (!parseArgs(argc, argv, args))
  {
    return 1;
  }

  // logging initialization
  StrongLogPtr log(new Log("tomasulo log", args.logLevel));
  StrongLogFormatterPtr formatter(new Formatter);
  if (args.logConsole)
  {
    StrongLogWriterPtr console(new StreamLogWriter(std::cout));
    console->setFormatter(formatter);
    log->addWriter("console", console);
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
    log->addWriter("file", file);
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
