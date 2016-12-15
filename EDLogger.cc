/** 
 *   @copyright Copyright (c) 2012, Wojciech Krzemien
 *   @file EDLogger.cc
 *   @version 1.0
 *   @author Wojciech Krzemien
 */

#include "./EDLogger.h"

const char* EDLogger::fOutputFile = "EventDisplay.log";

void EDLogger::logMessage(const char* func, const char* msg, MessageType type) {
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
