CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_KEYVEALUE_SET`(IN `key_` varchar(255), IN `key_hash_` bigint, IN `value_` blob)
BEGIN
	DECLARE keyvalue_table_number INT DEFAULT 10;
	DECLARE key_exist INT DEFAULT 0;

	SET @param_key = key_;
	SET @param_value = value_;

	SET @retcode_ = 0;
	SET @value = 0;
	SET @key_hash_ = key_hash_ % keyvalue_table_number;
	SET @table_name = concat('keyvalue-', @key_hash_);

	START TRANSACTION;

	SET @execute_exist = CONCAT('select count(*) into @key_exist  from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE select_stmt FROM @execute_exist;
	EXECUTE select_stmt USING @param_key;

	SELECT @key_exist;

	IF @key_exist <= 0
	THEN
		SET @insert_value = CONCAT('INSERT INTO `', @table_name, '` (`key`, `value`) VALUES(?,?)');
		PREPARE insert_stmt FROM @insert_value;
		EXECUTE insert_stmt USING @param_key, @param_value;
		SET @retcode_ = 0;
	ELSE	
		SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= ? WHERE `key`= ?');
		PREPARE update_stmt FROM @update_value;
		EXECUTE update_stmt USING @param_key, @param_value;
		SET @retcode_ = 0;
	END IF;

	COMMIT;

	SELECT @retcode_;
END