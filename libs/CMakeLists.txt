# hackrf_sweeper requires a CMake package named "msgpack", but msgpack-c 5+
# installs its config as "msgpack-c". Fall back to a shim that forwards to it.
find_package(msgpack QUIET CONFIG)
if (NOT msgpack_FOUND)
  set(msgpack_DIR ${PROJECT_SOURCE_DIR}/cmake/msgpack-compat CACHE PATH "Directory containing the msgpack CMake config" FORCE)
endif()

add_subdirectory(hackrf_sweeper EXCLUDE_FROM_ALL)

file(GLOB qwt_sources qwt/src/*.cpp)

# QwtPlotGLCanvas is built on QGLWidget, which no longer exists in Qt 6
list(FILTER qwt_sources EXCLUDE REGEX "/qwt_plot_glcanvas\\.cpp$")

add_library(qwt ${qwt_sources})

target_include_directories(qwt PUBLIC qwt/src qwt/classincludes)
target_link_libraries(qwt PUBLIC ${QT_LIBRARIES})
