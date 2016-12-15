/** 
 *   @copyright Copyright (c) 2012, Wojciech Krzemien
 *   @file EDCommonTools.h
 *   @version 1.0
 *   @author Adrian Dybczak, Wojciech Krzemien 
 *   @brief Set of tools used in different classes 
 */

#ifndef EDCOMMONTOOLS_H_
#define EDCOMMONTOOLS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <cassert>
#include <algorithm>

class EDCommonTools {
 public :
  static bool isValidString(const std::string &str);
  static bool fileExists(const std::string &str);
  static bool isEqual(double x, double y, double epsilon = 1.0E-20);
  static const char* itoa(int x) { return intToString(x).c_str();}
  static std::string intToString(int x);
  // Convert double without floating point part (mantissa) to string
  static std::string doubleToString(double x);  
  static int stringToInt(const std::string& str);

 private:
  EDCommonTools();
  EDCommonTools(const EDCommonTools& source);
  EDCommonTools& operator=(const EDCommonTools&);

  static bool isStringOnlyPath(const std::string &str);
  static bool isNotForrbidenSyntax(char c);
};

#endif  // EDCOMMONTOOLS_H_
