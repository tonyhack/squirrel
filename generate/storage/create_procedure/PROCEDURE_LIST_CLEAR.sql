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
END