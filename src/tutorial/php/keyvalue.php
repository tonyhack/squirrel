#!/usr/bin/env php

<?php

$GLOBALS['THRIFT_ROOT'] = realpath(dirname(__FILE__)).'/thrift/src';
require_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';
require_once $GLOBALS['THRIFT_ROOT'].'/protocol/TBinaryProtocol.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TSocket.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/THttpClient.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TBufferedTransport.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TMemoryBuffer.php';

error_reporting(E_ALL);
require_once realpath(dirname(__FILE__)).'/message/message_types.php';
require_once realpath(dirname(__FILE__)).'/protocol_keyvalue_message/protocol_keyvalue_message_types.php';
error_reporting(E_ALL);

try {

// Keyvalue request.
$request = new squirrel_protocol_KeyvalueRequest();
$request->type_ = KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_SET;
$request->key_list_[] = 'KEYVALUE-KEY-1';
$request->value_list_[] = 'KEYVALUE-VALUE-1';

// Serialize.
$buffer = new TMemoryBuffer();
$buffer_protocol = new TBinaryProtocol($buffer);
$request->write($buffer_protocol);

// Request.
$message = new squirrel_protocol_Request();
$message->data_type_ = DataType::DATA_TYPE_KEYVALUE;
$message->storage_type_ = StorageType::STORAGE_TYPE_DISK | StorageType::STORAGE_TYPE_CACHE;
$message->request_ = $buffer->getBuffer();

// Serialize.
$buffer->__construct();
$message->write($buffer_protocol);

$post_url = 'http://10.10.10.99:8081/';

$curl = curl_init($post_url);
curl_setopt($curl, CURLOPT_HEADER, 0);
$header = array();
$header[] = 'Host: 10.10.10.99';
$header[] = 'Connection: Keep-Alive';
$header[] = 'User-Agent: Squirrel tutorial';
$header[] = 'Accept: application/thrift-x';
$header[] = 'Content-Type: application/thrift-x';
curl_setopt($curl, CURLOPT_HTTPHEADER, $header);
curl_setopt($curl, CURLOPT_POST, 1);
curl_setopt($curl, CURLOPT_POSTFIELDS, $buffer->getBuffer());  
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($curl); 

// Receive response and deserialize.
$buffer->__construct();
$buffer->write($result);

// Response.
$response = new squirrel_protocol_Response();
$response->read($buffer_protocol);

if($response->data_type_ == DataType::DATA_TYPE_KEYVALUE) {

  $buffer->__construct();
  $buffer->write($response->response_);
  $keyvalue_response = new squirrel_protocol_KeyvalueResponse();
  $keyvalue_response->read($buffer_protocol);

  if($keyvalue_response->result_ == KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {

    echo "Set ok\n";

  } else {

    print "Failure response[".$keyvalue_response->result_."].\n";

  }

} else {

  print "Error data type.\n";

}

// Keyvalue request.
$request = new squirrel_protocol_KeyvalueRequest();
$request->type_ = KeyvalueRequestType::KEYVALUE_REQUEST_TYPE_GET;
$request->key_list_[] = 'KEYVALUE-KEY-1';

// Serialize.
$buffer->__construct();
$request->write($buffer_protocol);

// Request.
$message = new squirrel_protocol_Request();
$message->data_type_ = DataType::DATA_TYPE_KEYVALUE;
$message->storage_type_ = StorageType::STORAGE_TYPE_CACHE;
$message->request_ = $buffer->getBuffer();

// Serialize.
$buffer->__construct();
$message->write($buffer_protocol);

curl_setopt($curl, CURLOPT_POSTFIELDS, $buffer->getBuffer());  
$result = curl_exec($curl); 

// Receive response and deserialize.
$buffer->__construct();
$buffer->write($result);

// Response.
$response = new squirrel_protocol_Response();
$response->read($buffer_protocol);

if($response->data_type_ == DataType::DATA_TYPE_KEYVALUE) {

  $buffer->__construct();
  $buffer->write($response->response_);
  $keyvalue_response = new squirrel_protocol_KeyvalueResponse();
  $keyvalue_response->read($buffer_protocol);

  if($keyvalue_response->result_ == KeyvalueResponseType::KEYVALUE_RESPONSE_TYPE_SUCCESS) {

    print "Value is ".$keyvalue_response->value_list_[0]."\n";

  } else {

    print "Failure response[".$keyvalue_response->result_."].\n";

  }

} else {

  print "Error data type.\n";

}


curl_close ( $curl );

} catch (TException $tx) {
  print 'TException: '.$tx->getMessage()."\n";
}

?>

