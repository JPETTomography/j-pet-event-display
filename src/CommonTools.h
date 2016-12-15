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
 *  @file CommonTools.h
 */

#ifndef COMMONTOOLS_H_
#define COMMONTOOLS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <cassert>
#include <algorithm>

class CommonTools {
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
  CommonTools();
  CommonTools(const CommonTools& source);
  CommonTools& operator=(const CommonTools&);

  static bool isStringOnlyPath(const std::string &str);
  static bool isNotForrbidenSyntax(char c);
};

#endif  // COMMONTOOLS_H_
