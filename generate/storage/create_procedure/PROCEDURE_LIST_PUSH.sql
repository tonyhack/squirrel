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
END