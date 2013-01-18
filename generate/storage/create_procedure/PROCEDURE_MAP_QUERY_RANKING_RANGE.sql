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
END