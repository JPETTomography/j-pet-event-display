/** 
 *   @copyright Copyright (c) 2012, Wojciech Krzemien
 *   @file LoggerInclude.h
 *   @version 1.0
 *   @author Wojciech Krzemien
 *   @brief Configuration file for the Logger class
 *   Three independent level of logging are defined: LEVEL_INFO, LEVEL_WARNING and LEVEL_ERROR
 *   Levels can be switched on/off separately, by assignin the value different than 1.
 *   Also the whole Logger can be switched off/on by setting the LOGGER_ON flag to 1.
 */

// current settings
// 1 - switched on
#define LOGGER_ON 1
#define _LOGGER_LEVEL_INFO 1
#define _LOGGER_LEVEL_WARNING 1
#define _LOGGER_LEVEL_ERROR 1


// don't touch this part
#if LOGGER_ON == 1
  #include "./Logger.h"
  #define DATE_AND_TIME()   Logger::dateAndTime()
  #if _LOGGER_LEVEL_INFO == 1
    #define INFO(X)   Logger::info(__func__, X)
  #else
    #define INFO(X)
  #endif
  #if _LOGGER_LEVEL_WARNING == 1
    #define WARNING(X) Logger::warning(__func__, X)
  #else
    #define WARNING(X)
  #endif
  #if _LOGGER_LEVEL_ERROR == 1
    #define ERROR(X)   Logger::error(__func__, X)
  #else
    #define ERROR(X)
  #endif
#else
  #define WARNING(X)
  #define ERROR(X)
  #define INFO(X)
  #define DATE_AND_TIME()
#endif
