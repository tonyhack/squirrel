
set(DATA_PROXY_SERVER_SOURCES
    async_request.cc
    async_request_loop.cc
    async_request_pool.cc
    data_proxy_server.cc
    default_request_handler.cc
    keyvalue_request_handler.cc
    list_request_handler.cc
    main.cc
    map_request_handler.cc
    number_request_handler.cc
    request_service.cc
    request_terminal.cc
    request_terminal_set.cc
    server_configure.cc)

set(REDIS_PROXY_SOURCES
    ${REDIS_PROXY_PATH}/redis.cc
    ${REDIS_PROXY_PATH}/redis_set.cc)

set(STORAGE_PROXY_SOURCES
    ${STORAGE_PROXY_PATH}/storage.cc
    ${STORAGE_PROXY_PATH}/storage_set.cc)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/data_proxy_server)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/data_proxy_server)

#Build data_proxy_server
add_executable(data_proxy_server ${DATA_PROXY_SERVER_SOURCES} ${GLOBAL_SOURCES} ${COMMON_SOURCE} ${SCHEDULE_CLIENT_SOURCE} ${REDIS_PROXY_SOURCES} ${STORAGE_PROXY_SOURCES})
target_link_libraries(data_proxy_server pthread rt thrift hiredis mysqlpp ${PROJECT_BINARY_DIR}/lib/libcore.a)

