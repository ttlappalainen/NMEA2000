#  The MIT License
#
#  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.

cmake_minimum_required(VERSION 3.0)

if(ESP_PLATFORM)
  # using this library as a component in an ESP-IDF project
  idf_component_register(SRC_DIRS src INCLUDE_DIRS src REQUIRES esp_timer driver)
  return() # ignore the rest of this file
endif()

# else probably running unit tests with catch on a host
project(NMEA2000)
add_compile_options(
  -Wall
  -Werror
  -std=c++11
  -g
)

enable_testing()
add_subdirectory(src)
add_subdirectory(third-party/catch)
add_subdirectory(test)
