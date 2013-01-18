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
END