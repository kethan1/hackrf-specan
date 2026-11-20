cmake_minimum_required(VERSION 3.21)
project(hackrf-specan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

set(QT_COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Concurrent Svg PrintSupport)
find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
list(TRANSFORM QT_COMPONENTS PREPEND Qt6:: OUTPUT_VARIABLE QT_LIBRARIES)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(FindLibHackRF)
include(FindLibUSB)

# Check for libraries
if (NOT LIBHACKRF_INCLUDE_DIR OR NOT LIBHACKRF_LIBRARIES)
  message(FATAL_ERROR "Could not find HackRF library.")
endif()

if (NOT LIBUSB_INCLUDE_DIR OR NOT LIBUSB_LIBRARIES)
  message(FATAL_ERROR "Could not find libusb library.")
endif()

# Vendored dependencies: defines the hackrf_sweeper and qwt targets
add_subdirectory(libs)

file(GLOB source_list "src/*.cpp" "include/*.hpp")

add_executable(${PROJECT_NAME} ${source_list})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${LIBUSB_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE hackrf_sweeper qwt Qt6::Core Qt6::Gui Qt6::Widgets ${LIBHACKRF_LIBRARIES} ${LIBUSB_LIBRARIES})
