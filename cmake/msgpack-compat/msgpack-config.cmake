# Compatibility shim: msgpack-c 5+ renamed its CMake package from "msgpack" to
# "msgpack-c". hackrf_sweeper still asks for "msgpack", so forward the lookup.
include(CMakeFindDependencyMacro)
find_dependency(msgpack-c CONFIG)
