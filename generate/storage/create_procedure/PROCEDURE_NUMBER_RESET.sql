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
END