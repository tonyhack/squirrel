<?php
/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
include_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';


$GLOBALS['squirrel_protocol_E_MapRequestType'] = array(
  'MAP_REQUEST_TYPE_INSERT' => 0,
  'MAP_REQUEST_TYPE_REMOVE' => 1,
  'MAP_REQUEST_TYPE_QUERY_RANKING' => 2,
  'MAP_REQUEST_TYPE_QUERY_RANKING_RANGE' => 3,
  'MAP_REQUEST_TYPE_RESET_RANKING_KEY' => 4,
  'MAP_REQUEST_TYPE_CLEAR' => 5,
  'MAP_REQUEST_TYPE_DELETE' => 6,
  'MAP_REQUEST_TYPE_MAX' => 7,
);

final class MapRequestType {
  const MAP_REQUEST_TYPE_INSERT = 0;
  const MAP_REQUEST_TYPE_REMOVE = 1;
  const MAP_REQUEST_TYPE_QUERY_RANKING = 2;
  const MAP_REQUEST_TYPE_QUERY_RANKING_RANGE = 3;
  const MAP_REQUEST_TYPE_RESET_RANKING_KEY = 4;
  const MAP_REQUEST_TYPE_CLEAR = 5;
  const MAP_REQUEST_TYPE_DELETE = 6;
  const MAP_REQUEST_TYPE_MAX = 7;
  static public $__names = array(
    0 => 'MAP_REQUEST_TYPE_INSERT',
    1 => 'MAP_REQUEST_TYPE_REMOVE',
    2 => 'MAP_REQUEST_TYPE_QUERY_RANKING',
    3 => 'MAP_REQUEST_TYPE_QUERY_RANKING_RANGE',
    4 => 'MAP_REQUEST_TYPE_RESET_RANKING_KEY',
    5 => 'MAP_REQUEST_TYPE_CLEAR',
    6 => 'MAP_REQUEST_TYPE_DELETE',
    7 => 'MAP_REQUEST_TYPE_MAX',
  );
}

$GLOBALS['squirrel_protocol_E_MapResponseType'] = array(
  'MAP_RESPONSE_TYPE_SUCCESS' => 0,
  'MAP_RESPONSE_TYPE_KEY_NOT_EXIST' => 1,
  'MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST' => 2,
  'MAP_RESPONSE_TYPE_ELEM_NOT_EXIST' => 3,
  'MAP_RESPONSE_TYPE_DISCONNECTED' => 4,
  'MAP_RESPONSE_TYPE_UNKNOWN' => 5,
  'MAP_RESPONSE_TYPE_MAX' => 6,
);

final class MapResponseType {
  const MAP_RESPONSE_TYPE_SUCCESS = 0;
  const MAP_RESPONSE_TYPE_KEY_NOT_EXIST = 1;
  const MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST = 2;
  const MAP_RESPONSE_TYPE_ELEM_NOT_EXIST = 3;
  const MAP_RESPONSE_TYPE_DISCONNECTED = 4;
  const MAP_RESPONSE_TYPE_UNKNOWN = 5;
  const MAP_RESPONSE_TYPE_MAX = 6;
  static public $__names = array(
    0 => 'MAP_RESPONSE_TYPE_SUCCESS',
    1 => 'MAP_RESPONSE_TYPE_KEY_NOT_EXIST',
    2 => 'MAP_RESPONSE_TYPE_ELEM_ALREDY_EXIST',
    3 => 'MAP_RESPONSE_TYPE_ELEM_NOT_EXIST',
    4 => 'MAP_RESPONSE_TYPE_DISCONNECTED',
    5 => 'MAP_RESPONSE_TYPE_UNKNOWN',
    6 => 'MAP_RESPONSE_TYPE_MAX',
  );
}

$GLOBALS['squirrel_protocol_E_MapSortType'] = array(
  'MAP_SORT_TYPE_ASC' => 0,
  'MAP_SORT_TYPE_DESC' => 1,
  'MAP_SORT_TYPE_MAX' => 2,
);

final class MapSortType {
  const MAP_SORT_TYPE_ASC = 0;
  const MAP_SORT_TYPE_DESC = 1;
  const MAP_SORT_TYPE_MAX = 2;
  static public $__names = array(
    0 => 'MAP_SORT_TYPE_ASC',
    1 => 'MAP_SORT_TYPE_DESC',
    2 => 'MAP_SORT_TYPE_MAX',
  );
}

class squirrel_protocol_MapRequest {
  static $_TSPEC;

  public $type_ = null;
  public $key_ = null;
  public $data_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'type_',
          'type' => TType::I32,
          ),
        2 => array(
          'var' => 'key_',
          'type' => TType::STRING,
          ),
        3 => array(
          'var' => 'data_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['type_'])) {
        $this->type_ = $vals['type_'];
      }
      if (isset($vals['key_'])) {
        $this->key_ = $vals['key_'];
      }
      if (isset($vals['data_'])) {
        $this->data_ = $vals['data_'];
      }
    }
  }

  public function getName() {
    return 'MapRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->type_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->key_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->data_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapRequest');
    if ($this->type_ !== null) {
      $xfer += $output->writeFieldBegin('type_', TType::I32, 1);
      $xfer += $output->writeI32($this->type_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->key_ !== null) {
      $xfer += $output->writeFieldBegin('key_', TType::STRING, 2);
      $xfer += $output->writeString($this->key_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->data_ !== null) {
      $xfer += $output->writeFieldBegin('data_', TType::STRING, 3);
      $xfer += $output->writeString($this->data_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapResponse {
  static $_TSPEC;

  public $type_ = null;
  public $data_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'type_',
          'type' => TType::I32,
          ),
        2 => array(
          'var' => 'data_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['type_'])) {
        $this->type_ = $vals['type_'];
      }
      if (isset($vals['data_'])) {
        $this->data_ = $vals['data_'];
      }
    }
  }

  public function getName() {
    return 'MapResponse';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->type_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->data_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapResponse');
    if ($this->type_ !== null) {
      $xfer += $output->writeFieldBegin('type_', TType::I32, 1);
      $xfer += $output->writeI32($this->type_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->data_ !== null) {
      $xfer += $output->writeFieldBegin('data_', TType::STRING, 2);
      $xfer += $output->writeString($this->data_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapInsertRequest {
  static $_TSPEC;

  public $ranking_key_ = null;
  public $element_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'ranking_key_',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'element_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['ranking_key_'])) {
        $this->ranking_key_ = $vals['ranking_key_'];
      }
      if (isset($vals['element_'])) {
        $this->element_ = $vals['element_'];
      }
    }
  }

  public function getName() {
    return 'MapInsertRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->ranking_key_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->element_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapInsertRequest');
    if ($this->ranking_key_ !== null) {
      $xfer += $output->writeFieldBegin('ranking_key_', TType::STRING, 1);
      $xfer += $output->writeString($this->ranking_key_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->element_ !== null) {
      $xfer += $output->writeFieldBegin('element_', TType::STRING, 2);
      $xfer += $output->writeString($this->element_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapResetRankingKeyRequest {
  static $_TSPEC;

  public $ranking_key_ = null;
  public $element_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'ranking_key_',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'element_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['ranking_key_'])) {
        $this->ranking_key_ = $vals['ranking_key_'];
      }
      if (isset($vals['element_'])) {
        $this->element_ = $vals['element_'];
      }
    }
  }

  public function getName() {
    return 'MapResetRankingKeyRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->ranking_key_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->element_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapResetRankingKeyRequest');
    if ($this->ranking_key_ !== null) {
      $xfer += $output->writeFieldBegin('ranking_key_', TType::STRING, 1);
      $xfer += $output->writeString($this->ranking_key_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->element_ !== null) {
      $xfer += $output->writeFieldBegin('element_', TType::STRING, 2);
      $xfer += $output->writeString($this->element_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapRemoveRequest {
  static $_TSPEC;

  public $element_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'element_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['element_'])) {
        $this->element_ = $vals['element_'];
      }
    }
  }

  public function getName() {
    return 'MapRemoveRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->element_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapRemoveRequest');
    if ($this->element_ !== null) {
      $xfer += $output->writeFieldBegin('element_', TType::STRING, 1);
      $xfer += $output->writeString($this->element_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapQueryRankingRequest {
  static $_TSPEC;

  public $element_ = null;
  public $sort_type_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'element_',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'sort_type_',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['element_'])) {
        $this->element_ = $vals['element_'];
      }
      if (isset($vals['sort_type_'])) {
        $this->sort_type_ = $vals['sort_type_'];
      }
    }
  }

  public function getName() {
    return 'MapQueryRankingRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->element_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->sort_type_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapQueryRankingRequest');
    if ($this->element_ !== null) {
      $xfer += $output->writeFieldBegin('element_', TType::STRING, 1);
      $xfer += $output->writeString($this->element_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->sort_type_ !== null) {
      $xfer += $output->writeFieldBegin('sort_type_', TType::I32, 2);
      $xfer += $output->writeI32($this->sort_type_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapQueryRankingResponse {
  static $_TSPEC;

  public $ranking_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'ranking_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['ranking_'])) {
        $this->ranking_ = $vals['ranking_'];
      }
    }
  }

  public function getName() {
    return 'MapQueryRankingResponse';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->ranking_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapQueryRankingResponse');
    if ($this->ranking_ !== null) {
      $xfer += $output->writeFieldBegin('ranking_', TType::STRING, 1);
      $xfer += $output->writeString($this->ranking_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapQueryRankingRangeRequest {
  static $_TSPEC;

  public $start_ = null;
  public $end_ = null;
  public $sort_type_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'start_',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'end_',
          'type' => TType::STRING,
          ),
        3 => array(
          'var' => 'sort_type_',
          'type' => TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['start_'])) {
        $this->start_ = $vals['start_'];
      }
      if (isset($vals['end_'])) {
        $this->end_ = $vals['end_'];
      }
      if (isset($vals['sort_type_'])) {
        $this->sort_type_ = $vals['sort_type_'];
      }
    }
  }

  public function getName() {
    return 'MapQueryRankingRangeRequest';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->start_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->end_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::I32) {
            $xfer += $input->readI32($this->sort_type_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapQueryRankingRangeRequest');
    if ($this->start_ !== null) {
      $xfer += $output->writeFieldBegin('start_', TType::STRING, 1);
      $xfer += $output->writeString($this->start_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->end_ !== null) {
      $xfer += $output->writeFieldBegin('end_', TType::STRING, 2);
      $xfer += $output->writeString($this->end_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->sort_type_ !== null) {
      $xfer += $output->writeFieldBegin('sort_type_', TType::I32, 3);
      $xfer += $output->writeI32($this->sort_type_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapQueryRankingRangeInfo {
  static $_TSPEC;

  public $ranking_ = null;
  public $ranking_key_ = null;
  public $elements_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'ranking_',
          'type' => TType::STRING,
          ),
        2 => array(
          'var' => 'ranking_key_',
          'type' => TType::STRING,
          ),
        3 => array(
          'var' => 'elements_',
          'type' => TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['ranking_'])) {
        $this->ranking_ = $vals['ranking_'];
      }
      if (isset($vals['ranking_key_'])) {
        $this->ranking_key_ = $vals['ranking_key_'];
      }
      if (isset($vals['elements_'])) {
        $this->elements_ = $vals['elements_'];
      }
    }
  }

  public function getName() {
    return 'MapQueryRankingRangeInfo';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->ranking_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 2:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->ranking_key_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        case 3:
          if ($ftype == TType::STRING) {
            $xfer += $input->readString($this->elements_);
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapQueryRankingRangeInfo');
    if ($this->ranking_ !== null) {
      $xfer += $output->writeFieldBegin('ranking_', TType::STRING, 1);
      $xfer += $output->writeString($this->ranking_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->ranking_key_ !== null) {
      $xfer += $output->writeFieldBegin('ranking_key_', TType::STRING, 2);
      $xfer += $output->writeString($this->ranking_key_);
      $xfer += $output->writeFieldEnd();
    }
    if ($this->elements_ !== null) {
      $xfer += $output->writeFieldBegin('elements_', TType::STRING, 3);
      $xfer += $output->writeString($this->elements_);
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

class squirrel_protocol_MapQueryRankingRangeResponse {
  static $_TSPEC;

  public $elements_ = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'elements_',
          'type' => TType::LST,
          'etype' => TType::STRUCT,
          'elem' => array(
            'type' => TType::STRUCT,
            'class' => 'squirrel_protocol_MapQueryRankingRangeInfo',
            ),
          ),
        );
    }
    if (is_array($vals)) {
      if (isset($vals['elements_'])) {
        $this->elements_ = $vals['elements_'];
      }
    }
  }

  public function getName() {
    return 'MapQueryRankingRangeResponse';
  }

  public function read($input)
  {
    $xfer = 0;
    $fname = null;
    $ftype = 0;
    $fid = 0;
    $xfer += $input->readStructBegin($fname);
    while (true)
    {
      $xfer += $input->readFieldBegin($fname, $ftype, $fid);
      if ($ftype == TType::STOP) {
        break;
      }
      switch ($fid)
      {
        case 1:
          if ($ftype == TType::LST) {
            $this->elements_ = array();
            $_size0 = 0;
            $_etype3 = 0;
            $xfer += $input->readListBegin($_etype3, $_size0);
            for ($_i4 = 0; $_i4 < $_size0; ++$_i4)
            {
              $elem5 = null;
              $elem5 = new squirrel_protocol_MapQueryRankingRangeInfo();
              $xfer += $elem5->read($input);
              $this->elements_ []= $elem5;
            }
            $xfer += $input->readListEnd();
          } else {
            $xfer += $input->skip($ftype);
          }
          break;
        default:
          $xfer += $input->skip($ftype);
          break;
      }
      $xfer += $input->readFieldEnd();
    }
    $xfer += $input->readStructEnd();
    return $xfer;
  }

  public function write($output) {
    $xfer = 0;
    $xfer += $output->writeStructBegin('MapQueryRankingRangeResponse');
    if ($this->elements_ !== null) {
      if (!is_array($this->elements_)) {
        throw new TProtocolException('Bad type in structure.', TProtocolException::INVALID_DATA);
      }
      $xfer += $output->writeFieldBegin('elements_', TType::LST, 1);
      {
        $output->writeListBegin(TType::STRUCT, count($this->elements_));
        {
          foreach ($this->elements_ as $iter6)
          {
            $xfer += $iter6->write($output);
          }
        }
        $output->writeListEnd();
      }
      $xfer += $output->writeFieldEnd();
    }
    $xfer += $output->writeFieldStop();
    $xfer += $output->writeStructEnd();
    return $xfer;
  }

}

?>
