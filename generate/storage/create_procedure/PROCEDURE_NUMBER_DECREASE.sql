CREATE DEFINER=`root`@`%` PROCEDURE `PROCEDURE_NUMBER_DECREASE`(IN `key_` VARCHAR(255), IN `key_hash_` BIGINT, IN `decrease_` VARCHAR(32))
BEGIN
	DECLARE number_table_number INT DEFAULT 2;
	DECLARE key_exist INT DEFAULT 0;
	DECLARE value_number BIGINT DEFAULT 0;
	
	SET @param_key = key_;
	SET @param_decrease = decrease_;
	SET @value_ret = 0;
	
	SET @retcode_ = 0;
	SET @key_hash = key_hash_ % number_table_number;
	SET @table_name = concat('number-', @key_hash);
	
	START TRANSACTION;
	
	SET @select_sql = CONCAT('select count(*), value into @key_exist, @value_ret from `', @table_name, '` where `key`= ? LIMIT 1');
	PREPARE stmt1 FROM @select_sql;
	EXECUTE stmt1 USING @param_key;
	
	IF @key_exist = 1
	THEN
		SET @value_ret = @value_ret - @param_decrease;
		SET @update_value = CONCAT('UPDATE `', @table_name, '` SET `value`= ? WHERE `key`= ?');
		PREPARE update_stmt FROM @update_value;
		EXECUTE update_stmt USING @value_ret, @param_key;
		SET @retcode_ = 0;
	ELSE
		SET @value_ret = 0 - @param_increase;
		SET @insert_value = CONCAT('INSERT INTO `', @table_name, '` (`key`, `value`) VALUES(?,?)');
		PREPARE insert_stmt FROM @insert_value;
		EXECUTE insert_stmt USING @param_key, @value_ret;
		SET @retcode_ = 0;
	END IF;
	
	COMMIT;

	SELECT @retcode_, @value_ret;
END