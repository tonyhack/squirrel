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
require_once realpath(dirname(__FILE__)).'/protocol_map_message/protocol_map_message_types.php';
error_reporting(E_ALL);

try {

  if(count($argv) == 2) {

    // Map request.
    $request = new squirrel_protocol_MapRequest();
    $request->type_ = MapRequestType::MAP_REQUEST_TYPE_CLEAR;
    $request->key_ = $argv[1];

    // Serialize.
    $buffer = new TMemoryBuffer();
    $buffer_protocol = new TBinaryProtocol($buffer);
    $request->write($buffer_protocol);

    // Request.
    $message = new squirrel_protocol_Request();
    $message->data_type_ = DataType::DATA_TYPE_MAP;
    $message->storage_type_ = StorageType::STORAGE_TYPE_DISK;
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

    if($response->data_type_ == DataType::DATA_TYPE_MAP) {

      $buffer->__construct();
      $buffer->write($response->response_);
      $map_response = new squirrel_protocol_MapResponse();
      $map_response->read($buffer_protocol);

      if($map_response->type_ == MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {

        echo "Clear ok\n";

      } else {

        print "Failure response[".$map_response->result_."].\n";

      }

    } else {

      print "Error data type.\n";

    }

  }

} catch (TException $tx) {

  print 'TException: '.$tx->getMessage()."\n";

}

?>

