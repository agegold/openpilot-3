#include "i2c.h"

#include <cassert>
#include <unistd.h>
#include <stdexcept>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "common/swaglog.h"

#define UNUSED(x) (void)(x)

#ifdef QCOM2
// TODO: decide if we want to isntall libi2c-dev everywhere
#include <linux/i2c-dev.h>

I2CBus::I2CBus(uint8_t bus_id){
  char bus_name[20];
  snprintf(bus_name, 20, "/dev/i2c-%d", bus_id);

  i2c_fd = open(bus_name, O_RDWR);
  if(i2c_fd < 0){
    throw std::runtime_error("Failed to open I2C bus");
  }
}

I2CBus::~I2CBus(){
  if(i2c_fd >= 0){ close(i2c_fd); }
}

int I2CBus::read_register(uint8_t device_address, uint register_address, uint8_t *buffer, uint8_t len){
  int ret = 0;

  ret = ioctl(i2c_fd, I2C_SLAVE, device_address);
  if(ret < 0) { goto fail; }

  ret = i2c_smbus_read_i2c_block_data(i2c_fd, register_address, len, buffer);
  if((ret < 0) || (ret != len)) { goto fail; }

fail:
  return ret;
}

int I2CBus::set_register(uint8_t device_address, uint register_address, uint8_t data){
  int ret = 0;

  ret = ioctl(i2c_fd, I2C_SLAVE, device_address);
  if(ret < 0) { goto fail; }

  ret = i2c_smbus_write_byte_data(i2c_fd, register_address, data);
  if(ret < 0) { goto fail; }

fail:
  return ret;
}

#else

I2CBus::I2CBus(uint8_t bus_id){
  UNUSED(bus_id);
  i2c_fd = -1;
}

I2CBus::~I2CBus(){}

int I2CBus::read_register(uint8_t device_address, uint register_address, uint8_t *buffer, uint8_t len){
  UNUSED(device_address);
  UNUSED(register_address);
  UNUSED(buffer);
  UNUSED(len);
  return -1;
}

int I2CBus::set_register(uint8_t device_address, uint register_address, uint8_t data){
  UNUSED(device_address);
  UNUSED(register_address);
  UNUSED(data);
  return -1;
}
#endif
