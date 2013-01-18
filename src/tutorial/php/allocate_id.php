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
require_once realpath(dirname(__FILE__)).'/protocol_number_message/protocol_number_message_types.php';
error_reporting(E_ALL);

try {

$socket = new THttpClient('10.10.10.99', 8081);
$transport = new TBufferedTransport($socket, 1024, 1024);
$protocol = new TBinaryProtocol($transport);

$transport->open();

// Number request.
$request = new squirrel_protocol_NumberRequest();
$request->type_ = NumberRequestType::NUMBER_REQUEST_TYPE_INC;
$request->key_ = "TEST-NUMBER-001";
$request->increment_ = "1";

// Serialize.
$buffer = new TMemoryBuffer();
$buffer_protocol = new TBinaryProtocol($buffer);
$request->write($buffer_protocol);

// Request.
$message = new squirrel_protocol_Request();
$message->data_type_ = DataType::DATA_TYPE_NUMBER;
$message->storage_type_ = StorageType::STORAGE_TYPE_DISK;
$message->request_ = $buffer->getBuffer();

// Serialize.
$message_buffer = new TMemoryBuffer();
$message_buffer_protocol = new TBinaryProtocol($message_buffer);
$message->write($message_buffer_protocol);

$_post_url = 'http://10.10.10.99:8081/';

$curl = curl_init($_post_url);
curl_setopt($curl, CURLOPT_HEADER, 0);
$header = array();
$header [] = 'Host: 10.10.10.99';
$header [] = 'Connection: Keep-Alive';
$header [] = 'User-Agent: Squirrel tutorial';
$header [] = 'Accept: application/thrift-x';
$header [] = 'Content-Type: application/thrift-x';
curl_setopt($curl, CURLOPT_HTTPHEADER, $header);
curl_setopt($curl, CURLOPT_POST, 1);
curl_setopt($curl, CURLOPT_POSTFIELDS, $message_buffer->getBuffer());  
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($curl); 

// Receive response and deserialize.
$buffer->__construct();
// $buffer->write($socket->read(1024));
$buffer->write($result);

// Response.
$response = new squirrel_protocol_Response();
$response->read($buffer_protocol);

if ($response->data_type_ == DataType::DATA_TYPE_NUMBER) {

  $buffer->__construct();
  $buffer->write($response->response_);
  $number_response = new squirrel_protocol_NumberResponse();
  $number_response->read($buffer_protocol);

  if ($number_response->result_ == NumberResponseType::NUMBER_RESPONSE_TYPE_SUCCESS) {

    print "value is ".$number_response->value_."\n";

  } else {

    print "Failure response.\n";

  }

} else {

  print "Error data type.\n";

}

curl_setopt ( $curl, CURLOPT_POSTFIELDS, $message_buffer->getBuffer() );  
$result = curl_exec ( $curl );  
curl_close ( $curl );

$transport->close();

} catch (TException $tx) {
  print 'TException: '.$tx->getMessage()."\n";
}

?>

