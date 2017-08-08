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
 */

#include "src/EventDisplay.h"
#include <TRint.h>
#include <iostream>

int main(int argc, char **argv)
{
  using namespace jpet_event_display;
  EventDisplay myDisplay("large_barrel.json", 44, 3, 50,
                         std::vector< std::pair< int, double > >(
                             {std::pair< int, double >(48, 42.5),
                              std::pair< int, double >(48, 46.75),
                              std::pair< int, double >(96, 57.5)}));
  return 0;
}
