set(GLOBAL_SOURCES
    ${GLOBAL_PATH}/async_logging.cc
    ${GLOBAL_PATH}/async_logging_loop.cc
    ${GLOBAL_PATH}/async_logging_singleton.cc
    )

set(GATEWAY_SERVER_SOURCES
    data_request_handler.cc
    gateway_server.cc
    http_response.cc
    http_service_handler.cc
    main.cc
    map_request_handler.cc
    message_thread_pool.cc
    server_configure.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/gateway_server)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/gateway_server)

#Build gateway_server
add_executable(gateway_server ${GATEWAY_SERVER_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${SCHEDULE_CLIENT_SOURCE} ${DATAPROXY_CLIENT_SOURCE} ${PROTOCOL_SOURCE})
target_link_libraries(gateway_server pthread rt thrift ${PROJECT_BINARY_DIR}/lib/libcore.a)

