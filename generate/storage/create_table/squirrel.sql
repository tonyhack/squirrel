/*
Navicat MySQL Data Transfer

Source Server         : 10.10.12.6
Source Server Version : 50520
Source Host           : 10.10.12.6:3306
Source Database       : squirrel001

Target Server Type    : MYSQL
Target Server Version : 50520
File Encoding         : 65001

Date: 2012-09-27 18:08:39
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `keyvalue-0`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-0`;
CREATE TABLE `keyvalue-0` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-0
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-1`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-1`;
CREATE TABLE `keyvalue-1` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-1
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-2`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-2`;
CREATE TABLE `keyvalue-2` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-2
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-3`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-3`;
CREATE TABLE `keyvalue-3` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-3
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-4`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-4`;
CREATE TABLE `keyvalue-4` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-4
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-5`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-5`;
CREATE TABLE `keyvalue-5` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-5
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-6`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-6`;
CREATE TABLE `keyvalue-6` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-6
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-7`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-7`;
CREATE TABLE `keyvalue-7` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-7
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-8`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-8`;
CREATE TABLE `keyvalue-8` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-8
-- ----------------------------

-- ----------------------------
-- Table structure for `keyvalue-9`
-- ----------------------------
DROP TABLE IF EXISTS `keyvalue-9`;
CREATE TABLE `keyvalue-9` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` blob NOT NULL,
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of keyvalue-9
-- ----------------------------

-- ----------------------------
-- Table structure for `map_example`
-- ----------------------------
DROP TABLE IF EXISTS `map_example`;
CREATE TABLE `map_example` (
  `ranking_key` bigint(20) NOT NULL,
  `element` blob NOT NULL,
  KEY `ranking_key` (`ranking_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of map_example
-- ----------------------------

-- ----------------------------
-- Table structure for `number-0`
-- ----------------------------
DROP TABLE IF EXISTS `number-0`;
CREATE TABLE `number-0` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of number-0
-- ----------------------------

-- ----------------------------
-- Table structure for `number-1`
-- ----------------------------
DROP TABLE IF EXISTS `number-1`;
CREATE TABLE `number-1` (
  `key` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `value` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of number-1
-- ----------------------------

-- ----------------------------
-- Table structure for `summary-list-0`
-- ----------------------------
DROP TABLE IF EXISTS `summary-list-0`;
CREATE TABLE `summary-list-0` (
  `tablename` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  PRIMARY KEY (`tablename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of summary-list-0
-- ----------------------------

-- ----------------------------
-- Table structure for `summary-list-1`
-- ----------------------------
DROP TABLE IF EXISTS `summary-list-1`;
CREATE TABLE `summary-list-1` (
  `tablename` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  PRIMARY KEY (`tablename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of summary-list-1
-- ----------------------------

-- ----------------------------
-- Table structure for `summary-map-0`
-- ----------------------------
DROP TABLE IF EXISTS `summary-map-0`;
CREATE TABLE `summary-map-0` (
  `tablename` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  PRIMARY KEY (`tablename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of summary-map-0
-- ----------------------------

-- ----------------------------
-- Table structure for `summary-map-1`
-- ----------------------------
DROP TABLE IF EXISTS `summary-map-1`;
CREATE TABLE `summary-map-1` (
  `tablename` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  PRIMARY KEY (`tablename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of summary-map-1
-- ----------------------------

-- ----------------------------
-- Procedure structure for `PROCEDURE_KEYVEALUE_DELETE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_KEYVEALUE_DELETE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_KEYVEALUE_DELETE`(IN `key_` varchar(255), IN `key_hash_` bigint)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;
	
	SET @param_key = key_;
	SET @retcode_ = 0;
	SET @value_ = NULL;

	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('keyvalue-', @key_hash_);
	
	START TRANSACTION;

	SET @delete_sql = CONCAT('DELETE FROM `', @table_name, '` WHERE `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @delete_sql;
	EXECUTE stmt1 USING @param_key;
	
	SET @retcode_ = 0;
	
	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_KEYVEALUE_GET`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_KEYVEALUE_GET`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_KEYVEALUE_GET`(IN `key_` varchar(255), IN `key_hash_` bigint)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;
	
	SET @param_key = key_;
	SET @retcode_ = 0;
	SET @value_ = NULL;

	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('keyvalue-', @key_hash_);
	
	START TRANSACTION;

	SET @select_sql = CONCAT('select value into @value_ from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @select_sql;
	EXECUTE stmt1 USING @param_key;
	
	IF @value_ is null
	THEN
		SET @retcode_ = -1;
	ELSE
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;

	SELECT @retcode_, @value_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_KEYVEALUE_SET`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_KEYVEALUE_SET`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_KEYVEALUE_SET`(IN `key_` varchar(255), IN `key_hash_` bigint, IN `value_` blob)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;

	SET @param_key = key_;
	SET @param_value = value_;

	SET @retcode_ = 0;
	SET @value = 0;
	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('keyvalue-', @key_hash_);

	START TRANSACTION;

	SET @execute_exist = CONCAT('select count(*) into @key_exist  from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE select_stmt FROM @execute_exist;
	EXECUTE select_stmt USING @param_key;

	#SELECT @key_exist;

	IF @key_exist <= 0
	THEN
		SET @insert_value = CONCAT('INSERT INTO `', @table_name, '` (`key`, `value`) VALUES(?,?)');
		PREPARE insert_stmt FROM @insert_value;
		EXECUTE insert_stmt USING @param_key, @param_value;
		SET @retcode_ = 0;
	ELSE	
		SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= ? WHERE `key`= ?');
		PREPARE update_stmt FROM @update_value;
		EXECUTE update_stmt USING @param_key, @param_value;
		SET @retcode_ = 0;
	END IF;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_LIST_CLEAR`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_LIST_CLEAR`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_LIST_CLEAR`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE summary_list_number INT DEFAULT 2;
	
	SET @param_key = key_;

	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % summary_list_number;
	SET @summary_table_name = CONCAT('summary-list-', @key_hash);
	
	SET @list_table_name = CONCAT('list-', @param_key);

	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @list_table_name;
	
	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @list_table_name, '`');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt;
		SET @retcode_ = 0;
	END IF;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_LIST_DELETE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_LIST_DELETE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_LIST_DELETE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE summary_list_number INT DEFAULT 2;

	SET @param_key = key_;

	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % summary_list_number;
	SET @summary_table_name = CONCAT('summary-list-', @key_hash);
	
	SET @list_table_name = CONCAT('list-', @param_key);

	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @list_table_name;
	
	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @summary_table_name, '` WHERE tablename= ?');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt USING @list_table_name;
	
		SET @drop_sql = CONCAT('DROP TABLE `', @list_table_name, '`');
		PREPARE stmt FROM @drop_sql;
		EXECUTE stmt;
		SET @retcode_ = 0;
	END IF;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_LIST_POP`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_LIST_POP`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_LIST_POP`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE summary_list_number INT DEFAULT 2;

	SET @param_key = key_;

	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % summary_list_number;
	SET @summary_table_name = CONCAT('summary-list-', @key_hash);
	
	SET @list_table_name = CONCAT('list-', @param_key);

	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1), `element` into @count, @element_ FROM `', @list_table_name, '` ORDER BY RAND() LIMIT 1');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt;
	
	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @list_table_name, '` WHERE `element` = ?');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt USING @element_;
		SET @retcode_ = 0;
	END IF;

	COMMIT;

	SELECT @retcode_, @element_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_LIST_PUSH`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_LIST_PUSH`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_LIST_PUSH`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `element_` BLOB)
BEGIN
	DECLARE summary_list_number INT DEFAULT 2;

	SET @param_key = key_;
	SET @param_element = element_;

	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % summary_list_number;
	SET @summary_table_name = CONCAT('summary-list-', @key_hash);
	
	SET @list_table_name = CONCAT('list-', @param_key);

	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @list_table_name;
	IF @count = 0 THEN
		SET @create_sql = CONCAT('CREATE TABLE `', @list_table_name, '` (
			`element` blob NOT NULL
		) ENGINE=InnoDB DEFAULT CHARSET=latin1');
		PREPARE stmt FROM @create_sql;
		EXECUTE stmt;

		SET @insert_sql = CONCAT('INSERT INTO `', @summary_table_name, '` (`tablename`) VALUES (?)');
		PREPARE stmt FROM @insert_sql;
		EXECUTE stmt USING @list_table_name;
	ELSE
		SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @list_table_name, '` WHERE `element` = ?');
		PREPARE stmt FROM @select_sql;
		EXECUTE stmt USING @param_element;
	END IF;

	IF @count = 0 THEN
		SET @insert_sql = CONCAT('INSERT INTO `', @list_table_name, '` (`element`) VALUES (?)');
		PREPARE stmt FROM @insert_sql;
		EXECUTE stmt USING @param_element;
	ELSE
		SET @retcode_ = -1;
	END IF;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_LIST_REMOVE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_LIST_REMOVE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_LIST_REMOVE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `element_` BLOB)
BEGIN
	SET @param_key = key_;
	SET @param_element = element_;

	SET @retcode_ = 0;
	
	SET @list_table_name = CONCAT('list-', @param_key);

	START TRANSACTION;
	
	SET @delete_sql = CONCAT('DELETE FROM `', @list_table_name, '` WHERE `element` = ?');
	PREPARE stmt FROM @delete_sql;
	EXECUTE stmt USING @param_element;
	SET @retcode_ = 0;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_CLEAR`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_CLEAR`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_CLEAR`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @map_table_name;

	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @map_table_name, '`');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt;
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;
	
	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_DELETE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_DELETE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_DELETE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @map_table_name;

	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @summary_table_name, '` WHERE tablename= ?');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt USING @map_table_name;
	
		SET @drop_sql = CONCAT('DROP TABLE `', @map_table_name, '`');
		PREPARE stmt FROM @drop_sql;
		EXECUTE stmt;
		
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;
	
	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_INSERT`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_INSERT`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_INSERT`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `ranking_key_` BIGINT, IN `element_` BLOB)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	SET @param_ranking_key = ranking_key_;
	SET @param_element = element_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @map_table_name;

	IF @count = 0 THEN
		SET @create_sql = CONCAT('CREATE TABLE `', @map_table_name, '` (
			`ranking_key` BIGINT(20) NOT NULL,
			`element` BLOB NOT NULL,
			KEY `ranking_key` (`ranking_key`)
		) ENGINE=InnoDB DEFAULT CHARSET=latin1');
		PREPARE stmt FROM @create_sql;
		EXECUTE stmt;
		
		SET @insert_sql = CONCAT('INSERT INTO `', @summary_table_name, '` (`tablename`) VALUE (?)');
		PREPARE stmt FROM @insert_sql;
		EXECUTE stmt USING @map_table_name;
	ELSE
		SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @map_table_name, '` WHERE `element` = ?');
		PREPARE stmt FROM @select_sql;
		EXECUTE stmt USING @param_element;
	END IF;

	IF @count = 0 THEN
		SET @insert_sql = CONCAT('INSERT INTO `', @map_table_name, '` (`ranking_key`, `element`) VALUES (?, ?)');
		PREPARE stmt FROM @insert_sql;
		EXECUTE stmt USING @param_ranking_key, @param_element;
	ELSE
		SET @update_sql = CONCAT('UPDATE `', @map_table_name, '` SET `ranking_key`= ? WHERE `element`= ?');
		PREPARE stmt FROM @update_sql;
		EXECUTE stmt USING @param_ranking_key, @param_element;
	END IF;
	
	COMMIT;
	
	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_QUERY_RANKING`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_QUERY_RANKING`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_QUERY_RANKING`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `element_` BLOB, IN `asc_` TINYINT)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	SET @param_element = element_;
	SET @param_asc = asc_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	SET @first_ranking = 0;
	SET @second_ranking = 0;
	
	IF @param_asc = 1 THEN
	
		START TRANSACTION;
	
		SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
		PREPARE stmt FROM @select_sql;
		EXECUTE stmt USING @map_table_name;

		IF @count = 0 THEN
			SET @retcode_ = -1;
		ELSE
			SET @select_sql = CONCAT('SELECT count(1), `ranking_key` into @count, @ranking_key FROM `', @map_table_name,
				'` WHERE `element` = ?');
			PREPARE stmt FROM @select_sql;
			EXECUTE stmt USING @param_element;
		
			IF @count = 0 THEN
				SET @retcode_ = -2;
			ELSE
				SET @select_sql = CONCAT('SELECT count(*) into @first_ranking FROM `', @map_table_name,
					'` WHERE `ranking_key` <= ? ');
				PREPARE stmt FROM @select_sql;
				EXECUTE stmt USING @ranking_key;
			
				SET @select_sql = CONCAT('SELECT count(*) into @second_ranking FROM `', @map_table_name,
					'` WHERE `ranking_key` = ?  AND `element` > ?');
				PREPARE stmt FROM @select_sql;
				EXECUTE stmt USING @ranking_key, @param_element;
			END IF;
		END IF;
	
		COMMIT;
	
	ELSE
	
		START TRANSACTION;
	
		SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
		PREPARE stmt FROM @select_sql;
		EXECUTE stmt USING @map_table_name;

		IF @count = 0 THEN
			SET @retcode_ = -1;
		ELSE
			SET @select_sql = CONCAT('SELECT count(1), `ranking_key` into @count, @ranking_key FROM `', @map_table_name,
				'` WHERE `element` = ?');
			PREPARE stmt FROM @select_sql;
			EXECUTE stmt USING @param_element;
		
			IF @count = 0 THEN
				SET @retcode_ = -2;
			ELSE
				SET @select_sql = CONCAT('SELECT count(*) into @first_ranking FROM `', @map_table_name,
					'` WHERE `ranking_key` >= ? ');
				PREPARE stmt FROM @select_sql;
				EXECUTE stmt USING @ranking_key;
			
				SET @select_sql = CONCAT('SELECT count(*) into @second_ranking FROM `', @map_table_name,
					'` WHERE `ranking_key` = ?  AND `element` < ?');
				PREPARE stmt FROM @select_sql;
				EXECUTE stmt USING @ranking_key, @param_element;
			END IF;
		END IF;
	
		COMMIT;
	
	END IF;

	SET @ranking_ = @first_ranking - @second_ranking - 1;
	
	SELECT @retcode_, @ranking_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_QUERY_RANKING_RANGE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_QUERY_RANKING_RANGE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_QUERY_RANKING_RANGE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `start_` VARCHAR(32), IN `end_` VARCHAR(32), IN `asc_` TINYINT)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	SET @param_start = start_;
	SET @param_end = end_;
	SET @param_asc = asc_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	SET @first_ranking = 0;
	SET @second_ranking = 0;
	
	IF @param_start > @param_end AND @param_end != -1 THEN
		SET @retcode_ = -2;
		SELECT @retcode_;
	ELSEIF @param_end < -1 OR @param_start < 0 THEN
		SET @retcode_ = -2;
		SELECT @retcode_;
	ELSE
		START TRANSACTION;
	
		SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
		PREPARE stmt FROM @select_sql;
		EXECUTE stmt USING @map_table_name;

		IF @count = 0 THEN
			SET @retcode_ = -1;
		ELSE
			SET @retcode_ = 0;
			IF @param_start = @param_end THEN
				SET @param_end = 1;
			ELSEIF @param_end > 0 THEN
				SET @param_end = @param_end - @param_start + 1;
			ELSE
				SET @select_sql = CONCAT('SELECT count(*) into @count FROM `', @map_table_name, '`');
				PREPARE stmt FROM @select_sql;
				EXECUTE stmt;
				IF @param_start > @count THEN
					SET @param_end = 0;
				ELSE
					SET @param_end = @count - @param_start;
				END IF;
			END IF;

			IF @param_asc = 1 THEN
				SET @select_sql = CONCAT('SELECT * FROM `', @map_table_name, '` ORDER BY `ranking_key` ASC, `element` ASC limit ?, ?');
			ELSE
				SET @select_sql = CONCAT('SELECT * FROM `', @map_table_name, '` ORDER BY `ranking_key` DESC, `element` DESC limit ?, ?');
			END IF;

			PREPARE stmt FROM @select_sql;
			EXECUTE stmt USING @param_start, @param_end;
		END IF;
	
		COMMIT;

	END IF;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_MAP_REMOVE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_MAP_REMOVE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_MAP_REMOVE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `element_` BLOB)
BEGIN
	DECLARE summary_map_number INT DEFAULT 2;
	
	SET @param_key = key_;
	SET @param_key_hash = key_hash_;
	SET @param_element = element_;
	
	SET @retcode_ = 0;
	SET @key_hash = @param_key_hash % summary_map_number;
	SET @summary_table_name = CONCAT('summary-map-', @key_hash);
	
	SET @map_table_name = CONCAT('map-', @param_key);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('SELECT count(1) into @count FROM `', @summary_table_name, '` WHERE tablename = ?');
	PREPARE stmt FROM @select_sql;
	EXECUTE stmt USING @map_table_name;

	IF @count = 0 THEN
		SET @retcode_ = -1;
	ELSE
		SET @delete_sql = CONCAT('DELETE FROM `', @map_table_name, '` WHERE  `element` = ?');
		PREPARE stmt FROM @delete_sql;
		EXECUTE stmt USING @param_element;
	END IF;
	
	COMMIT;
	
	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_NUMBER_DECREASE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_NUMBER_DECREASE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_NUMBER_DECREASE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `decrease_` VARCHAR(32))
BEGIN
	DECLARE number_table_number INT DEFAULT 2;
	DECLARE key_exist INT DEFAULT 0;
	DECLARE value_number BIGINT DEFAULT 0;
	
	SET @param_key = key_;
	SET @param_decrease = decrease_;
	SET @value_ret = 0;
	
	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % number_table_number;
	SET @table_name = concat('number-', @key_hash);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('select count(*), value into @key_exist, @value_ret from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @select_sql;
	EXECUTE stmt1 USING @param_key;
	
	IF @key_exist = 1
	THEN
		SET @value_ret = @value_ret - @param_decrease;
		SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= ? WHERE `key`= ?');
		PREPARE update_stmt FROM @update_value;
		EXECUTE update_stmt USING @value_ret, @param_key;
		SET @retcode_ = 0;
	ELSE
		SET @value_ret = 0 - @param_increase;
		SET @insert_value = CONCAT('INSERT INTO `', @table_name, '` (`key`, `value`) VALUES(?,?)');
		PREPARE insert_stmt FROM @insert_value;
		EXECUTE insert_stmt USING @param_key, @value_ret;
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;

	SELECT @retcode_, @value_ret;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_NUMBER_DELETE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_NUMBER_DELETE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_NUMBER_DELETE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE number_table_number INT DEFAULT 2;
	DECLARE key_exist INT DEFAULT 0;
	
	SET @param_key = key_;
	SET @value_ret = 0;
	
	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % number_table_number;
	SET @table_name = concat('number-', @key_hash);
	
	START TRANSACTION;
	
	SET @delete_value = CONCAT('DELETE FROM `', @table_name, '` WHERE `key`= ?');
	PREPARE delete_stmt FROM @delete_value;
	EXECUTE delete_stmt USING @param_key;
	SET @retcode_ = 0;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_NUMBER_INCREASE`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_NUMBER_INCREASE`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_NUMBER_INCREASE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `increase_` VARCHAR(32))
BEGIN
	DECLARE number_table_number INT DEFAULT 2;
	DECLARE key_exist INT DEFAULT 0;
	DECLARE value_number BIGINT DEFAULT 0;
	
	SET @param_key = key_;
	SET @param_increase = increase_;
	SET @value_ret = 0;
	
	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % number_table_number;
	SET @table_name = concat('number-', @key_hash);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('select count(*), value into @key_exist, @value_ret from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @select_sql;
	EXECUTE stmt1 USING @param_key;
	
	IF @key_exist = 1
	THEN
		SET @value_ret = @value_ret + @param_increase;
		SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= ? WHERE `key`= ?');
		PREPARE update_stmt FROM @update_value;
		EXECUTE update_stmt USING @value_ret, @param_key;
		SET @retcode_ = 0;
	ELSE
		SET @insert_value = CONCAT('INSERT INTO `', @table_name, '` (`key`, `value`) VALUES(?,?)');
		PREPARE insert_stmt FROM @insert_value;
		EXECUTE insert_stmt USING @param_key, @param_increase;
		SET @retcode_ = 0;
		SET @value_ret = @param_increase;
	END IF;
	
	COMMIT;

	SELECT @retcode_, @value_ret;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_NUMBER_RESET`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_NUMBER_RESET`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_NUMBER_RESET`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT)
BEGIN
	DECLARE number_table_number INT DEFAULT 2;
	DECLARE key_exist INT DEFAULT 0;
	DECLARE value_number BIGINT DEFAULT 0;
	
	SET @param_key = key_;
	SET @value_ret = 0;
	
	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % number_table_number;
	SET @table_name = concat('number-', @key_hash);
	
	START TRANSACTION;
	
	SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= 0 WHERE `key`= ?');
	PREPARE update_stmt FROM @update_value;
	EXECUTE update_stmt USING @param_key;
	SET @retcode_ = 0;

	COMMIT;

	SELECT @retcode_;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `PROCEDURE_TEST`
-- ----------------------------
DROP PROCEDURE IF EXISTS `PROCEDURE_TEST`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_TEST`(IN `key_` varchar(255), IN `key_hash_` bigint)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;
	DECLARE my_value BIGINT DEFAULT 0;

	SET @my_value = my_value;
	SET @param_key = key_;

	SET @retcode_ = 0;
	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('number-', @key_hash_);

	START TRANSACTION;

	
	#SET @select_sql = CONCAT('select count(*) into @key_exist from `', @table_name, '` where `key`= ? LIMIT 1');
	SET @select_sql = CONCAT('select count(*), value into @key_exist, @my_value from `', @table_name, '` where `key`= ? LIMIT 1');
	SELECT @select_sql;
	PREPARE select_stmt FROM @select_sql;
	EXECUTE select_stmt USING @param_key;

	#SELECT @key_exist;

	IF @key_exist = 1
	THEN
		SELECT 1, @my_value;
	END IF;

	COMMIT;
END;;
DELIMITER ;
