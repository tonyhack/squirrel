#Declare project
project("squirrel")

#Declare version.
cmake_minimum_required(VERSION 0.1)

#Compile options:
# Take almost all warnining;
# Take warnings as errors;
# Optimization level 2;
#add_definitions("-Wall -Wno-deprecated -Werror -O2")
#add_definitions("-g -Wall -Werror -O2")
#add_definitions("-g -Wall -Werror")
#add_definitions("-g -Wall -Wno-unused-variable -Wno-deprecated -Werror -rdynamic -D_CORE_DEBUG -D_LOGIC_DEBUG")
#add_definitions("-g -Wall -Wno-unused-variable -Wno-deprecated -Werror-rdynamic -D_CORE_DEBUG -DPERFORMANCE_TEST -D_CORE_PERFORM_TEST -fno-inline -fno-strict-aliasing")
#add_definitions("-g -Wall -Wno-unused-variable -Wno-deprecated -Werror -rdynamic")

#Default
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -g -Wall -Wno-unused-variable -Wno-deprecated -Werror -rdynamic -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H -D_CORE_DEBUG -D_SQUIRREL_TEST -D_REDIS_MONITOR_ -fno-inline -fno-strict-aliasing")
#Debug
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -Wall -Wno-unused-variable -Wno-deprecated -Werror -rdynamic -D_CORE_DEBUG -DPERFORMANCE_TEST -D_CORE_PERFORM_TEST -fno-inline -fno-strict-aliasing")
#Release
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -g -Wall -O3 -Wno-unused-variable -Wno-deprecated -Werror -rdynamic -DNDEBUG -fno-strict-aliasing")


#Declare project binary/library directory.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(GLOBAL_PATH ${PROJECT_SOURCE_DIR}/src/global)

set(GLOBAL_SOURCES
    ${GLOBAL_PATH}/async_logging.cc
    ${GLOBAL_PATH}/async_logging_loop.cc
    ${GLOBAL_PATH}/async_logging_singleton.cc)

set(COMMON_SOURCE
  ${PROJECT_SOURCE_DIR}/src/common/async_request_object_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/list_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_schedule_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/request_type_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/async_request_object_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/list_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_data_proxy_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/request_type_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/keyvalue_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/map_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_data_proxy_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/number_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/server_type_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/keyvalue_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/map_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/message_schedule_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/common/number_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/common/server_type_types.cpp
  )

set(PROTOCOL_SOURCE
  ${PROJECT_SOURCE_DIR}/src/protocol/message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_keyvalue_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_keyvalue_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_list_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_list_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_number_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_number_message_types.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_map_message_constants.cpp
  ${PROJECT_SOURCE_DIR}/src/protocol/protocol_map_message_types.cpp)

set(SCHEDULE_CLIENT_SOURCE
  ${PROJECT_SOURCE_DIR}/src/schedule_server/client/schedule_client.cc
  ${PROJECT_SOURCE_DIR}/src/schedule_server/client/schedule_client_session.cc)

set(DATAPROXY_CLIENT_SOURCE
  ${PROJECT_SOURCE_DIR}/src/data_proxy_server/client/data_proxy_client.cc
  ${PROJECT_SOURCE_DIR}/src/data_proxy_server/client/data_proxy_client_session.cc
  ${PROJECT_SOURCE_DIR}/src/data_proxy_server/client/default_configure.cc)

set(REDIS_PROXY_PATH ${PROJECT_SOURCE_DIR}/src/data_proxy_server/redis_proxy)
set(STORAGE_PROXY_PATH ${PROJECT_SOURCE_DIR}/src/data_proxy_server/storage_proxy)

message(STATUS "This is SOURCE dir "${PROJECT_SOURCE_DIR})
message(STATUS "This is SOURCE dir "${PROJECT_BINARY_DIR})

#Set include paths.
include_directories(
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_BINARY_DIR}"
  "${PROJECT_SOURCE_DIR}/src"
  "/usr/local/include/thrift"
  "/usr/include/mysql/"
  )

#Add subdirectory.
add_subdirectory(src/core)
#add_subdirectory(src/core/test)

add_subdirectory(src/protocol/meta)
add_subdirectory(src/common/meta)

add_subdirectory(src/global/tests)

add_subdirectory(src/gateway_server)
add_subdirectory(src/schedule_server/server)
add_subdirectory(src/data_proxy_server/server)

add_subdirectory(src/data_proxy_server/redis_proxy)
add_subdirectory(src/data_proxy_server/storage_proxy)

add_subdirectory(src/test)

