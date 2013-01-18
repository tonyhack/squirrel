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

  if(count($argv) == 4) {

    // Map query request.
    $query = new squirrel_protocol_MapQueryRankingRangeRequest();
    $query->start_ = $argv[2];
    $query->end_ = $argv[3];
    $query->sort_type_ = MapSortType::MAP_SORT_TYPE_ASC;

    // Serialize.
    $buffer = new TMemoryBuffer();
    $buffer_protocol = new TBinaryProtocol($buffer);
    $query->write($buffer_protocol);

    // Map request.
    $request = new squirrel_protocol_MapRequest();
    $request->type_ = MapRequestType::MAP_REQUEST_TYPE_QUERY_RANKING_RANGE;
    $request->key_ = $argv[1];
    $request->data_ = $buffer->getBuffer();

    // Serialize.
    $buffer->__construct();
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

    print "\n";

    // Response.
    $response = new squirrel_protocol_Response();
    $response->read($buffer_protocol);

    if($response->data_type_ == DataType::DATA_TYPE_MAP) {

      $buffer->__construct();
      $buffer->write($response->response_);
      $map_response = new squirrel_protocol_MapResponse();
      $map_response->read($buffer_protocol);

      if($map_response->type_ == MapResponseType::MAP_RESPONSE_TYPE_SUCCESS) {

        // Deserialize.
        $buffer->__construct();
        $buffer->write($map_response->data_);

        // Deserialize ranking.
        $ranking = new squirrel_protocol_MapQueryRankingRangeResponse();
        $ranking->read($buffer_protocol);

        print_r($ranking->elements_);

      } else {

        print "Failure response[".$map_response->type_."].\n";

      }

    } else {

      print "Error data type.\n";

    }

  } else {

    print "Error argument.\n";

  }

} catch (TException $tx) {

  print 'TException: '.$tx->getMessage()."\n";

}

?>

