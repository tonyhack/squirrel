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
END