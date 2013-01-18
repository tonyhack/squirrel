#CMakeLists for src directory.

set(CORE_BASE_SOURCES
  base/bound_queue.cc
  base/byte_buffer_dynamic.cc
  base/common.cc
  base/logging.cc
  base/thread.cc)

set(CORE_TINYXML_SOURCES
  tinyxml/tinystr.cpp
  tinyxml/tinyxml.cpp
  tinyxml/tinyxmlerror.cpp
  tinyxml/tinyxmlparser.cpp)

set(CORE_SOURCES
  acceptor.cc
  channel.cc
  configure.cc
  connector.cc
  epoller.cc
  event_control.cc
  event_loop.cc
  event_loop_thread_group.cc
  inet_address.cc
  loop_thread.cc
  signal_set.cc
  tcp_client.cc
  tcp_connection.cc
  tcp_server.cc
  tcp_socket.cc
  timing_base.cc
  timing_group.cc
  timing_wheel.cc)

#Shared library
#add_library(core SHARED
#  ${CORE_BASE_SOURCES} ${CORE_TINYXML_SOURCES} ${CORE_SOURCES})

#Static library
add_library(core_static STATIC
  ${CORE_BASE_SOURCES} ${CORE_TINYXML_SOURCES} ${CORE_SOURCES})

#Ensure build both share and static library using same name.
set_target_properties(core_static PROPERTIES OUTPUT_NAME "core")
#set_target_properties(core PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(core_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

