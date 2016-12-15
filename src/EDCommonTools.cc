/** 
 *   @copyright Copyright (c) 2012, Wojciech Krzemien
 *   @file EDCommonTools.cc
 *   @version 1.0
 *   @author Adrian Dybczak, Wojciech Krzemien 
 */

#include <sstream>
#include <cmath>
#include "./EDLoggerInclude.h"
#include "./EDCommonTools.h"

EDCommonTools::EDCommonTools() {
}
// __________________________________________________________________________
bool EDCommonTools::fileExists(const std::string& str) {
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
bool EDCommonTools::isValidString(const std::string &str) {
  if (str.length() == 0) return false;
  return find_if(str.begin(), str.end(), isNotForrbidenSyntax) == str.end();
}
// __________________________________________________________________________
bool EDCommonTools::isEqual(double x, double y, double epsilon) {
  assert(epsilon > 0);
  return std::abs(x-y) < epsilon ? true : false;
}
// __________________________________________________________________________
std::string EDCommonTools::intToString(int x) {
  std::ostringstream out;
  out << x;
  return out.str();
}
// __________________________________________________________________________
std::string EDCommonTools::doubleToString(double x) {
  std::ostringstream out;
  out << x;
  return out.str();
}
// __________________________________________________________________________
int EDCommonTools::stringToInt(const std::string& str) {
  std::istringstream in(str);
  int num;
  in >> num;
  return num;
}
// __________________________________________________________________________
bool EDCommonTools::isStringOnlyPath(const std::string& str) {
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
bool EDCommonTools::isNotForrbidenSyntax(char c) {
  return !(isalnum(c) || (c == '.') || (c == '_') || (c == '/'));
}
