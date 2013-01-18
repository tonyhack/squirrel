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
END