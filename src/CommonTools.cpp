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
 *  @file CommonTools.cpp
 */

#include <sstream>
#include <cmath>
#include <JPetLoggerInclude.h>
#include "./CommonTools.h"

CommonTools::CommonTools() {
}
// __________________________________________________________________________
bool CommonTools::fileExists(const std::string& str) {
  if (isStringOnlyPath(str)) {
    return false;
  } else {
    std::fstream fs(str.c_str(), std::ios_base::in);  // open to read only
    if (!fs) {  // file don`t exists
      return false;
    } else {  // ok, file exists
      fs.close();
      return true;
    }
  }
}
// __________________________________________________________________________
bool CommonTools::isValidString(const std::string &str) {
  if (str.length() == 0) return false;
  return find_if(str.begin(), str.end(), isNotForrbidenSyntax) == str.end();
}
// __________________________________________________________________________
bool CommonTools::isEqual(double x, double y, double epsilon) {
  assert(epsilon > 0);
  return std::abs(x-y) < epsilon ? true : false;
}
// __________________________________________________________________________
std::string CommonTools::intToString(int x) {
  std::ostringstream out;
  out << x;
  return out.str();
}
// __________________________________________________________________________
std::string CommonTools::doubleToString(double x) {
  std::ostringstream out;
  out << x;
  return out.str();
}
// __________________________________________________________________________
int CommonTools::stringToInt(const std::string& str) {
  std::istringstream in(str);
  int num;
  in >> num;
  return num;
}
// __________________________________________________________________________
bool CommonTools::isStringOnlyPath(const std::string& str) {
  std::string OutFileName;
  size_t i = str.find_last_of("\\/:");
  if (i == std::string::npos)
    OutFileName = str;
  else
    OutFileName = str.substr(i + 1);
  if (OutFileName.length() == 0 || str == ".") {
    WARNING(std::string("There is only path without file name!!!"));
    return true;
  } else {
    return false;
  }
}
// __________________________________________________________________________
bool CommonTools::isNotForrbidenSyntax(char c) {
  return !(isalnum(c) || (c == '.') || (c == '_') || (c == '/'));
}
