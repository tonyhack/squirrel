set(TEST_DIR ${PROJECT_SOURCE_DIR}/src/test)

set(ASYNC_LOGGING_SOURCES
    async_logging.cc
    ../async_logging.cc
    ../async_logging_loop.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/test)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test)

#Build async_logging
add_executable(async_logging ${ASYNC_LOGGING_SOURCES})
target_link_libraries(async_logging pthread rt ${PROJECT_BINARY_DIR}/lib/libcore.a)
