set(THRIFT_SCRIPT
    async_request_object.thrift
    server_type.thrift
    message.thrift
    message_schedule.thrift
    message_data_proxy.thrift
    keyvalue_message.thrift
    list_message.thrift
    map_message.thrift
    number_message.thrift
    )

if(UNIX)

foreach(script ${THRIFT_SCRIPT})
  exec_program(thrift ${PROJECT_SOURCE_DIR}/src/common/meta/
      ARGS "-r -out ../ --gen cpp ${script}")
endforeach(script)

endif(UNIX)

