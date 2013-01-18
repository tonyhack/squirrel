set(KEYVALUE_TEST_SOURCES
    http_client.cc
    keyvalue_test.cc)

set(LIST_TEST_SOURCES
    http_client.cc
    list_test.cc)

set(MAP_TEST_SOURCES
    http_client.cc
    map_test.cc)

set(NUMBER_TEST_SOURCES
    http_client.cc
    number_test.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/test/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test)

#Build keyvalue_test
add_executable(keyvalue_test ${KEYVALUE_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${PROTOCOL_SOURCE})
target_link_libraries(keyvalue_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build list_test
add_executable(list_test ${LIST_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${PROTOCOL_SOURCE})
target_link_libraries(list_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build map_test
add_executable(map_test ${MAP_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${PROTOCOL_SOURCE})
target_link_libraries(map_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build number_test
add_executable(number_test ${NUMBER_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${PROTOCOL_SOURCE})
target_link_libraries(number_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build signal_test
add_executable(signal_test signal_test.cc)
target_link_libraries(signal_test pthread rt ${PROJECT_BINARY_DIR}/lib/libcore.a)
