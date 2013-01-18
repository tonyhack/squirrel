set(STORAGE_SET_UNIT_TEST_SOURCES
    storage_set_unit_test.cc
    ../../server/server_configure.cc)

set(STORAGE_NUMBER_UNIT_TEST_SOURCES
    storage_number_unit_test.cc
    ../../server/server_configure.cc)

set(STORAGE_LIST_UNIT_TEST_SOURCES
    storage_list_unit_test.cc
    ../../server/server_configure.cc)

set(STORAGE_MAP_UNIT_TEST_SOURCES
    storage_map_unit_test.cc
    ../../server/server_configure.cc)

set(STORAGE_RECONNECTOR_UNIT_TEST_SOURCES
    storage_reconnector_unit_test.cc
    ../../server/server_configure.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/test/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test)

#Build storage_set_unit_test
add_executable(storage_set_unit_test ${STORAGE_SET_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${STORAGE_PROXY_SOURCES})
target_link_libraries(storage_set_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build storage_number_unit_test
add_executable(storage_number_unit_test ${STORAGE_NUMBER_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${STORAGE_PROXY_SOURCES})
target_link_libraries(storage_number_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build storage_list_unit_test
add_executable(storage_list_unit_test ${STORAGE_LIST_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${STORAGE_PROXY_SOURCES})
target_link_libraries(storage_list_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build storage_map_unit_test
add_executable(storage_map_unit_test ${STORAGE_MAP_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${STORAGE_PROXY_SOURCES})
target_link_libraries(storage_map_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

#Build storage_reconnector_unit_test
add_executable(storage_reconnector_unit_test ${STORAGE_RECONNECTOR_UNIT_TEST_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${STORAGE_PROXY_SOURCES})
target_link_libraries(storage_reconnector_unit_test pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

