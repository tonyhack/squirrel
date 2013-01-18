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
END