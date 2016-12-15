/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @brief Simple logger class. Don't use directly. Macros from LoggerInclude.h should be used instead. 
 *  Logger class implements a simple logging functionality.
 *  For every message the logfile stream is opened, the message is written and the stream is closed again. This assures that no messages are lost in case of errrors.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
class Logger {
 public:
  static void dateAndTime() {
    std::ofstream out(fOutputFile, std::ios_base::app);
    if (out.fail()) {
      std::cerr << "Unable to open log file!" << std::endl;
      return;
    }
    time_t t = time(0);   /// current time
    struct tm * now = localtime(&t);
    out << (now->tm_year + 1900) << '-'
    << (now->tm_mon + 1) << '-'
    <<  now->tm_mday << " "
    <<  now->tm_hour << ":"
    <<  now->tm_min << ":"
    <<  now->tm_sec << std::endl;
    out.close();
  }
  inline static void warning(const char* func, const char* msg) {
    logMessage(func, msg, kWarning);
  }
  inline static void error(const char* func, const char* msg) {
    logMessage(func, msg, kError);
  }
  inline static void info(const char* func, const char* msg) {
    logMessage(func, msg, kInfo);
  }

  inline static void warning(const char* func, std::string msg) {
    logMessage(func , msg.c_str(), kWarning);
  }
  inline static void error(const char* func, std::string msg) {
    logMessage(func, msg.c_str(), kError);
  }
  inline static void info(const char* func, std::string msg) {
    logMessage(func, msg.c_str(), kInfo);
  }

 private:
  enum MessageType {kInfo, kWarning, kError};

  Logger();
  Logger(const Logger&);
  Logger& operator=(const Logger&);

  static void logMessage(const char* func, const char* msg, MessageType type);
  static const char* fOutputFile;
};

#endif /*  !LOGGER_H */

