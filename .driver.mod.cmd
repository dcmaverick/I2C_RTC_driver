cmd_/home/firm/workspace/I2C_RTC_driver/driver.mod := printf '%s\n'   driver.o | awk '!x[$$0]++ { print("/home/firm/workspace/I2C_RTC_driver/"$$0) }' > /home/firm/workspace/I2C_RTC_driver/driver.mod
