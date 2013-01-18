CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_KEYVEALUE_GET`(IN `key_` varchar(255), IN `key_hash_` bigint)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;
	
	SET @param_key = key_;
	SET @retcode_ = 0;
	SET @value_ = NULL;

	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('keyvalue-', @key_hash_);
	
	START TRANSACTION;

	SET @select_sql = CONCAT('select value into @value_ from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @select_sql;
	EXECUTE stmt1 USING @param_key;
	
	IF @value_ is null
	THEN
		SET @retcode_ = -1;
	ELSE
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;

	SELECT @retcode_, @value_;
END