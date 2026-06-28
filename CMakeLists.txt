cmake_minimum_required(VERSION 3.15)
project(NexusRPC CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable all warnings
add_compile_options(-Wall -Wextra -Werror)

# Source files
file(GLOB_RECURSE SRC_FILES src/*.cc)

# Core library
add_library(nexus_rpc_static STATIC ${SRC_FILES})
target_include_directories(nexus_rpc_static PUBLIC src)

# Fuzzing build setup
if(DEFINED ENV{LIB_FUZZING_ENGINE})
    # Under ClusterFuzzLite, the engine flags are provided by the environment
    set(FUZZER_LINK_LIBS $ENV{LIB_FUZZING_ENGINE})
else()
    # Local fallback for compilation tests
    set(FUZZER_LINK_LIBS "-fsanitize=fuzzer")
    add_compile_options(-fsanitize=fuzzer-no-link,address,undefined)
    link_libraries(-fsanitize=address,undefined)
endif()

# Fuzzing targets
add_executable(fuzz_nexus_broker fuzz/fuzz_nexus_broker.cc)
target_link_libraries(fuzz_nexus_broker PRIVATE nexus_rpc_static ${FUZZER_LINK_LIBS})

add_executable(fuzz_packet_parser fuzz/fuzz_packet_parser.cc)
target_link_libraries(fuzz_packet_parser PRIVATE nexus_rpc_static ${FUZZER_LINK_LIBS})
