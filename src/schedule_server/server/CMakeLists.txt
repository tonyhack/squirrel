set(GLOBAL_SOURCES
    ${GLOBAL_PATH}/async_logging.cc
    ${GLOBAL_PATH}/async_logging_loop.cc
    ${GLOBAL_PATH}/async_logging_singleton.cc
    )

set(SCHEDULE_SERVER_SOURCES
    main.cc
    server_configure.cc
    schedule_circle.cc
    schedule_client.cc
    schedule_server.cc
    schedule_terminal.cc
    schedule_terminal_set.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/schedule_server)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/schedule_server)

#Build schedule_server
add_executable(schedule_server ${SCHEDULE_SERVER_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE})
target_link_libraries(schedule_server pthread rt thrift ${PROJECT_BINARY_DIR}/lib/libcore.a)

