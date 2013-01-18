set(REDIS_KEYVALUE_UNIT_TEST_SOURCES
    redis_keyvalue_unit_test.cc)

set(REDIS_NUMBER_UNIT_TEST_SOURCES
    redis_number_unit_test.cc)

set(REDIS_LIST_UNIT_TEST_SOURCES
    redis_list_unit_test.cc)

set(REDIS_MAP_UNIT_TEST_SOURCES
    redis_map_unit_test.cc)

set(REDIS_SET_UNIT_TEST_SOURCES
    redis_set_unit_test.cc)

set(REDIS_RECONNECTOR_UNIT_TEST_SOURCES
    redis_reconnector_unit_test.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/test/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test)

#Build redis_keyvalue_unit_test
add_executable(redis_keyvalue_unit_test ${REDIS_KEYVALUE_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_keyvalue_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build redis_number_unit_test
add_executable(redis_number_unit_test ${REDIS_NUMBER_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_number_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build redis_list_unit_test
add_executable(redis_list_unit_test ${REDIS_LIST_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_list_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build redis_map_unit_test
add_executable(redis_map_unit_test ${REDIS_MAP_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_map_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build redis_set_unit_test
add_executable(redis_set_unit_test ${REDIS_SET_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_set_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build redis_reconnector_unit_test
add_executable(redis_reconnector_unit_test ${REDIS_RECONNECTOR_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${REDIS_PROXY_SOURCES})
target_link_libraries(redis_reconnector_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)
