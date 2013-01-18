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
END