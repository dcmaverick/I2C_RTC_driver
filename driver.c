#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/rtc.h>




static const struct i2c_device_id rtc_i2c_id_table[] = {
  { "rtc-ds1307", 0 },
  { }
};

MODULE_DEVICE_TABLE(i2c, rtc_i2c_id_table);

int rtc_i2c_set_time(struct device *dev, struct rtc_time *tm)
{
  // Implement code to set the time on the RTC device
  return 0;
}

static int rtc_i2c_get_time(struct device *dev,
                             struct rtc_time *tm)
{
  // Implement code to get the time from the RTC device
  return 0;
}

static struct rtc_class_ops rtc_i2c_ops = {
  .read_time = rtc_i2c_get_time,
  .set_time = rtc_i2c_set_time,
//  .set_alarm = rtc_i2c_set_alarm,
 // .read_alarm = rtc_i2c_read_alarm,
};


static int rtc_i2c_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
  struct rtc_device *rtc_dev;
  int ret;

  /*if (!i2c_check_functionality(client, I2C_FUNC_SMBUS_BYTE_DATA)) {
    dev_err(&client->dev, "SMBUS byte data not supported\n");
    return -EOPNOTSUPP;
  }*/

  rtc_dev = kzalloc(sizeof(struct rtc_device), GFP_KERNEL);
  if (!rtc_dev) {
    dev_err(&client->dev, "Failed to allocate memory for RTC device\n");
    return -ENOMEM;
  }
  // added back in and removed &
  rtc_dev->dev = client->dev;
  rtc_dev->ops = &rtc_i2c_ops;
  // added int
  ret = (int)devm_rtc_device_register(&client->dev, client->name,
        &rtc_i2c_ops, THIS_MODULE);


  if (ret) {
    dev_err(&client->dev, "Failed to register RTC device\n");
    goto err_free_rtc_dev;
  }

  dev_info(&client->dev, "RTC device registered successfully\n");

  return 0;

err_free_rtc_dev:
  kfree(rtc_dev);
  return ret;
}

static void rtc_i2c_remove(struct i2c_client *client)
{
  struct rtc_device *rtc_dev = i2c_get_clientdata(client);

  rtc_device_unregister(rtc_dev);
  kfree(rtc_dev);

 // return 0;
}




/*static int rtc_i2c_set_alarm(struct rtc_device *rtc_dev,
                               struct rtc_wkalarm *alarm)
{
  // Implement code to set the alarm on the RTC device
  return 0;
}

static int rtc_i2c_read_alarm(struct rtc_device *rtc_dev,
                               struct rtc_wkalarm *alarm)
{
  // Implement code to read the alarm from the RTC device
  return 0;
}*/



static struct i2c_driver rtc_i2c_driver = {
  .driver = {
    .name = "rtc-i2c",
    .owner = THIS_MODULE,
  },
  .id_table = rtc_i2c_id_table,
  .probe = rtc_i2c_probe,
  .remove = rtc_i2c_remove,
};

module_i2c_driver(rtc_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bard");
MODULE_DESCRIPTION("Linux driver for I2C RTC");
