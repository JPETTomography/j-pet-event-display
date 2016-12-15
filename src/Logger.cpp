/** 
 *   @copyright Copyright (c) 2012, Wojciech Krzemien
 *   @file Logger.cc
 *   @version 1.0
 *   @author Wojciech Krzemien
 */

#include "./Logger.h"

const char* Logger::fOutputFile = "EventDisplay.log";

void Logger::logMessage(const char* func, const char* msg, MessageType type) {
  std::ofstream out(fOutputFile, std::ios_base::app);
  if (out.fail()) {
    std::cerr << "Unable to open log file!" << std::endl;
    return;
  }
  switch (type) {
    case kInfo:
      out << "Info ";
      break;
    case kWarning:
      out << "Warning ";
      break;
    case kError:
      out << "Error ";
      break;
  }
  out << func << "():"
  << msg
  << std::endl;
  out.close();
}
