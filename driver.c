#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/rtc.h>

/* Define the I2C address of the RTC device */
#define RTC_I2C_ADDR 0x50

/* Define the RTC registers */
#define RTC_REG_SEC 0x00
#define RTC_REG_MIN 0x01
#define RTC_REG_HOUR 0x02
#define RTC_REG_DAY 0x03
#define RTC_REG_MONTH 0x04
#define RTC_REG_YEAR 0x05

/* Define the driver structure */
struct rtc_i2c_drv {
  struct i2c_client *client;
};

/* Read a byte from the RTC register */
static int rtc_i2c_read(struct rtc_i2c_drv *drv, uint8_t reg) {
  uint8_t data;
  int ret;

  ret = i2c_smbus_read_byte_data(drv->client, reg);
  if (ret < 0) {
    return ret;
  }

  data = ret & 0xFF;
  return data;
}

/* Write a byte to the RTC register */
static int rtc_i2c_write(struct rtc_i2c_drv *drv, uint8_t reg, uint8_t data) {
  int ret;

  ret = i2c_smbus_write_byte_data(drv->client, reg, data);
  if (ret < 0) {
    return ret;
  }

  return 0;
}

/* Get the time from the RTC */
static int rtc_i2c_get_time(struct device *dev, struct rtc_time *tm) {
  struct rtc_i2c_drv *drv = dev_get_drvdata(dev);

  tm->tm_sec = rtc_i2c_read(drv, RTC_REG_SEC);
  tm->tm_min = rtc_i2c_read(drv, RTC_REG_MIN);
  tm->tm_hour = rtc_i2c_read(drv, RTC_REG_HOUR);
  tm->tm_mday = rtc_i2c_read(drv, RTC_REG_DAY);
  tm->tm_mon = rtc_i2c_read(drv, RTC_REG_MONTH) - 1;
  tm->tm_year = rtc_i2c_read(drv, RTC_REG_YEAR) - 1900;

  return 0;
}

/* Set the time on the RTC */
static int rtc_i2c_set_time(struct device *dev, struct rtc_time *tm) {
  struct rtc_i2c_drv *drv = dev_get_drvdata(dev);

  rtc_i2c_write(drv, RTC_REG_SEC, tm->tm_sec);
  rtc_i2c_write(drv, RTC_REG_MIN, tm->tm_min);
  rtc_i2c_write(drv, RTC_REG_HOUR, tm->tm_hour);
  rtc_i2c_write(drv, RTC_REG_DAY, tm->tm_mday);
  rtc_i2c_write(drv, RTC_REG_MONTH, tm->tm_mon + 1);
  rtc_i2c_write(drv, RTC_REG_YEAR, tm->tm_year + 1900);

  return 0;
}

/* Define the RTC device operations */
static const struct rtc_device_ops rtc_i2c_ops = {
  .get_time = rtc_i2c_get_time,
  .set_time = rtc_i2c_set_time,
};

static int rtc_i2c_probe(struct i2c_client *client) {
  struct rtc_i2c_drv *drv;
  int ret;

  drv = devm_kzalloc(&client->dev, sizeof(struct rtc_i2c_drv), GFP_KERNEL);
  if (drv == NULL) {
    return -ENOMEM;
  }

  drv->client = client;

  /* Check the RTC device ID */
  if (i2c_smbus_read_byte_data(client, 0x00) != 0x50) {
    dev_err(&client->dev, "RTC device ID does not match\n");
    return -ENODEV;
  }

  /* Register the RTC device */
  dev_set_drvdata(&client->dev, drv);

  ret = rtc_register(&dev, client->name);
  if (ret < 0) {
    dev_err(&client->dev, "Failed to register RTC device: %d\n", ret);
    return ret;
  }

  dev_info(&client->dev, "I2C RTC driver registered\n");

  return 0;
}

/* Remove the I2C RTC device */
static int rtc_i2c_remove(struct i2c_client *client) {
  struct rtc_i2c_drv *drv = dev_get_drvdata(&client->dev);

  /* Unregister the RTC device */
  rtc_unregister(&dev);

  dev_info(&client->dev, "I2C RTC driver removed\n");

  return 0;
}

/* Driver definition */
static const struct i2c_device_id rtc_i2c_id[] = {
  { "rtc-i2c", 0 },
  { }
};

static struct i2c_driver rtc_i2c_driver = {
  .driver = {
    .name = "rtc-i2c",
  },
  .probe = rtc_i2c_probe,
  .remove = rtc_i2c_remove,
  .id_table = rtc_i2c_id,
};

/* Module initialization and cleanup */
module_init(rtc_i2c_driver_init);
module_exit(rtc_i2c_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("I2C RTC driver");
MODULE_AUTHOR("Bard");
