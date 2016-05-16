project(audiodecoder.modplug)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(p8-platform REQUIRED)
find_package(Modplug REQUIRED)

include_directories(${kodiplatform_INCLUDE_DIRS}
                    ${p8-platform_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}
                    ${MODPLUG_INCLUDE_DIRS})

set(MODPLUG_SOURCES src/ModplugCodec.cpp)

set(DEPLIBS ${p8-platform_LIBRARIES}
            ${MODPLUG_LIBRARIES})

build_addon(audiodecoder.modplug MODPLUG DEPLIBS)

include(CPack)
