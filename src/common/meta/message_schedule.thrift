include "server_type.thrift"

namespace cpp squirrel.common

enum MessageScheduleType {
  MESSAGE_SCHEDULE_TYPE_LOGIN_REQ,
  MESSAGE_SCHEDULE_TYPE_LOGIN_RES,
  MESSAGE_SCHEDULE_TYPE_LOGOUT_REQ,
  MESSAGE_SCHEDULE_TYPE_LOGOUT_RES,
  MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC,
  MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC,
  MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC_TRANSFER,
  MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC_TRANSFER,
  MESSAGE_SCHEDULE_TYPE_MAX,
}

struct MessageSchedule {
  1 : required MessageScheduleType type_,
  2 : optional binary message_,
}


// type: MESSAGE_SCHEDULE_TYPE_LOGIN
// meaning: 请求登录schedule服务器
// direction: other->schedule
struct MessageScheduleLoginReq {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
  3 : required string host_,
  4 : required i16 port_,
}


// type: MESSAGE_SCHEDULE_TYPE_LOGIN_RES
// meaning: 返回登录schedule服务器结果
// direction: schedule->other
struct MessageScheduleLoginRes {
  1 : required bool result_,
  2 : optional list<server_type.ServerSignature> server_signatures_;
}


// type: MESSAGE_SCHEDULE_TYPE_LOGOUT_REQ
// meaning: 请求退出schedule服务器
// direction: other->schedule


// type: MESSAGE_SCHEDULE_TYPE_LOGOUT_RES
// meaning: 返回退出schedule服务器结果
// direction: schedule->other
struct MessageScheduleLogoutRes {
  1 : required bool result_,
}


// type: MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC
// meaning: 同步登录schedule到其它服务器
// direction: schedule->other
struct MessageScheduleLoginSync {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
  3 : required string host_,
  4 : required i16 port_,
}

// type: MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC
// meaning: 同步登出schedule到其它服务器
// direction: schedule->other
struct MessageScheduleLogoutSync {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
}

// type: MESSAGE_SCHEDULE_TYPE_LOGIN_SYNC_TRANSFER
// meaning: 在schedule之间对应用服务器的登录同步进行环形传递
// direction: schedule(client)->schedule(server)
struct MessageScheduleLoginSyncTransfer {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
  3 : required string host_,
  4 : required i16 port_,
  5 : required byte transfer_node_count_,
  6 : required i32 transfer_schedule_id_,
}

// type: MESSAGE_SCHEDULE_TYPE_LOGOUT_SYNC_TRANSFER
// meaning: 在schedule之间对应用服务器的登出同步进行环形传递
// direction: schedule(client)->schedule(server)
struct MessageScheduleLogoutSyncTransfer {
  1 : required server_type.ServerType type_,
  2 : required i32 id_,
  3 : required byte transfer_node_count_,
  4 : required i32 transfer_schedule_id_,
}

