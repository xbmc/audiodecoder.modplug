project(audiodecoder.modplug)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(xbmc REQUIRED)
find_package(Modplug REQUIRED)

include_directories(${XBMC_INCLUDE_DIR}
                    ${MODPLUG_INCLUDE_DIRS})

set(MODPLUG_SOURCES src/ModplugCodec.cpp)

set(DEPLIBS ${MODPLUG_LIBRARIES})

build_addon(audiodecoder.modplug MODPLUG DEPLIBS)
