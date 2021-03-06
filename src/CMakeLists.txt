include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/redis/src/)
include_directories(${CMAKE_SOURCE_DIR}/redis/deps/)

find_package(glog 0.3.5 REQUIRED)
find_package(LevelDB REQUIRED)
find_package(Tcmalloc)
include_directories(${LevelDB_INCLUDES})

# For redis, build with either of
#   make MALLOC=tcmalloc
# or
#   make MALLOC=jemalloc
if (Tcmalloc_FOUND)
  set(MALLOC_LIBRARY "${Tcmalloc_LIBRARY}")
else()
  set(MALLOC_LIBRARY "${CMAKE_SOURCE_DIR}/redis/deps/jemalloc/lib/libjemalloc.a")
endif()

set(REDIS_MODULE_CFLAGS -W -Wall -fno-common -g -ggdb -std=c++11 -O2)
if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
  set(REDIS_MODULE_LDFLAGS "-undefined dynamic_lookup")
else()
  set(REDIS_MODULE_LDFLAGS -shared)
endif()

# timer
add_library(timer STATIC timer.cc)
target_compile_options(timer PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)
target_link_libraries(timer glog::glog)

# utils
add_library(utils STATIC utils.cc)
target_compile_options(utils PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)

# master_client
add_library(master_client STATIC master_client.cc)
target_compile_options(master_client PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)
target_link_libraries(master_client ${LevelDB_LIBRARIES})
target_link_libraries(master_client glog::glog)

# client
add_library(client STATIC client.cc)
target_compile_options(client PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)
target_link_libraries(client ${LevelDB_LIBRARIES})
target_link_libraries(client glog::glog)

# master
add_library(master SHARED master.cc)
target_compile_options(master PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)
target_link_libraries(master ${REDIS_MODULE_LDFLAGS})
target_link_libraries(master utils)
target_link_libraries(master glog::glog)
target_link_libraries(master ${LevelDB_LIBRARIES})
target_link_libraries(master master_client)

# member
add_library(member SHARED member.cc)
target_compile_options(member PUBLIC ${REDIS_MODULE_CFLAGS} -fPIC)
target_link_libraries(member ${REDIS_MODULE_LDFLAGS})
target_link_libraries(member utils)
target_link_libraries(member glog::glog)
target_link_libraries(member ${LevelDB_LIBRARIES})
target_link_libraries(member master_client)
target_link_libraries(member ${MALLOC_LIBRARY} timer)

# Benchmarks.
add_executable(credis_parput_bench credis_parput_bench.cc)
target_link_libraries(credis_parput_bench glog::glog client
  ${CMAKE_SOURCE_DIR}/redis/deps/hiredis/libhiredis.a
  ${CMAKE_SOURCE_DIR}/redis/src/ae.o
  ${CMAKE_SOURCE_DIR}/redis/src/zmalloc.o
  ${MALLOC_LIBRARY} timer)

add_executable(credis_seqput_bench credis_seqput_bench.cc)
target_link_libraries(credis_seqput_bench glog::glog client
  ${CMAKE_SOURCE_DIR}/redis/deps/hiredis/libhiredis.a
  ${CMAKE_SOURCE_DIR}/redis/src/ae.o
  ${CMAKE_SOURCE_DIR}/redis/src/zmalloc.o
  ${MALLOC_LIBRARY} timer master_client)

add_executable(redis_parput_bench redis_parput_bench.cc)
target_link_libraries(redis_parput_bench glog::glog client
  ${CMAKE_SOURCE_DIR}/redis/deps/hiredis/libhiredis.a
  ${CMAKE_SOURCE_DIR}/redis/src/ae.o
  ${CMAKE_SOURCE_DIR}/redis/src/zmalloc.o
  ${MALLOC_LIBRARY} timer)

add_executable(redis_seqput_bench redis_seqput_bench.cc)
target_link_libraries(redis_seqput_bench glog::glog client
  ${CMAKE_SOURCE_DIR}/redis/deps/hiredis/libhiredis.a
  ${CMAKE_SOURCE_DIR}/redis/src/ae.o
  ${CMAKE_SOURCE_DIR}/redis/src/zmalloc.o
  ${MALLOC_LIBRARY} timer)
