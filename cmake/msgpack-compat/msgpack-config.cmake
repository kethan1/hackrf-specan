# msgpack-c 5+ renamed its CMake package from "msgpack" to "msgpack-c".
# hackrf_sweeper still uses for "msgpack", so forward the lookup.
find_package(msgpack-c CONFIG QUIET)
