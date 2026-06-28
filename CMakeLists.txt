cmake_minimum_required(VERSION 3.15)
project(NexusRPC CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable all warnings
add_compile_options(-Wall -Wextra -Werror)

# Source files
set(SRC_FILES
    src/broker.cc
    src/routing_engine.cc
    src/storage_engine.cc
    src/utils.cc
    src/protocol_suite.cc
    src/packet_assembler.cc
    src/storage.cc
    src/cache.cc
    src/index.cc
    src/query.cc
    src/database.cc
    src/wal.cc
    src/lock_manager.cc
    src/sql_parser.cc
    src/json_parser.cc
    src/query_planner.cc
    src/transaction_manager.cc
    src/optimizer.cc
    src/checkpoint.cc
    src/wal_buffer.cc
    src/query_engine_compiler.cc
    src/query_engine_compiler_pass.cc
    src/query_engine_compiler_window.cc
    src/query_engine_compiler_window_agg.cc
    src/query_engine_compiler_window_ranking.cc
    src/query_engine_compiler_recursive.cc
    src/query_engine_compiler_pivot.cc
    src/query_engine_compiler_partition.cc
    src/query_engine_compiler_partition_hash.cc
    src/query_engine_compiler_partition_list.cc
    src/query_engine_compiler_pivot_unpivot.cc
)

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
