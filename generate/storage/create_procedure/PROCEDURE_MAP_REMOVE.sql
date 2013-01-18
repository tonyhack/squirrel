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
END