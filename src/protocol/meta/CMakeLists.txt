set(THRIFT_SCRIPT
    message.thrift
    protocol_keyvalue_message.thrift
    protocol_list_message.thrift
    protocol_map_message.thrift
    protocol_number_message.thrift)

if(UNIX)

foreach(script ${THRIFT_SCRIPT})
  exec_program(thrift ${PROJECT_SOURCE_DIR}/src/protocol/meta/
      ARGS "-r -out ../ --gen cpp ${script}")
endforeach(script)

endif(UNIX)

