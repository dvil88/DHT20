/*
 * MIT License
 * 
 * Copyright (c) 2022 Diego Villar - dvil88
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* https://cdn.sparkfun.com/assets/8/a/1/5/0/DHT20.pdf */

#include "mgos.h"
#include "dht20.h"
#include "mgos_debug.h"

float _temperature = 0;
float _humidity    = 0;
float _humOffset   = 0;
float _tempOffset  = 0;
uint8_t _status      = 0;
int64_t _lastRequest = 0;
double _lastRead    = 0;
uint8_t  _bits[7];

bool mgos_dht20_init(void) {
  return true;
}

bool dht20_ready_data()
{
  return ((mgos_uptime_micros() - _lastRequest) > 85);
}

int dht20_read()
{
  struct mgos_i2c *i2c = mgos_i2c_get_global();

  // READ SENSOR ==> uses the async interface!
  int status = dht20_request_data(i2c);
  if (status < 0) {
    return status;
  }
  while (!dht20_ready_data()) {
  }
  dht20_read_data(i2c);
  
  // CONVERT AND STORE
  _status      = _bits[0];
  uint32_t tmp = _bits[1];
  tmp <<= 8;
  tmp += _bits[2];
  tmp <<= 4;
  tmp += (_bits[3] >> 4);
  _humidity = tmp * 9.5367431640625e-5;   // ==> / 1048576.0 * 100%;

  tmp = (_bits[3] & 0x0F);
  tmp <<= 8;
  tmp += _bits[4];
  tmp <<= 8;
  tmp += _bits[5];
  _temperature = tmp * 1.9073486328125e-4 - 50;  //  ==> / 1048576.0 * 200 - 50;

  // TEST CHECKSUM
  uint8_t _crc = crc8(_bits, 6);

  if (_crc != _bits[6]) return DHT20_ERROR_CHECKSUM;

  return DHT20_OK;
}

bool dht20_request_data(struct mgos_i2c *i2c)
{
  uint8_t data[4];
  data[0] = 0xAC;
  data[1] = 0x33;
  data[2] = 0x00;

  bool rv = mgos_i2c_write(i2c, DHT20_ADDRESS, data, 3, true);
  if (!rv) {
    LOG(LL_ERROR, ("I2C=0x%02x cmd=%u (0x%04x) write error", DHT20_ADDRESS, DHT20_REQUEST_DATA_CMD, DHT20_REQUEST_DATA_CMD));
    return false;
  }
  LOG(LL_DEBUG, ("I2C=0x%02x cmd=%u (0x%04x) write success", DHT20_ADDRESS, DHT20_REQUEST_DATA_CMD, DHT20_REQUEST_DATA_CMD));


  _lastRequest = mgos_uptime_micros();
  return rv;
}

int dht20_read_data(struct mgos_i2c *i2c)
{
  // GET DATA
  const uint8_t length = 7;
  uint8_t  data[length];
  mgos_i2c_read(i2c, DHT20_ADDRESS, data, length, true);
  for( int i =0; i < length; i++) {
    _bits[i] = data[i];
  }

  _lastRead = mgos_uptime_micros();
  
  return length;
}

bool dht20_read_status(struct mgos_i2c *i2c)
{
  uint8_t data[4];
  data[0] = 0xAC;
  data[1] = 0x71;
  data[2] = 0x00;

  bool rv = mgos_i2c_write(i2c, DHT20_ADDRESS, data, 3, true);
  if (!rv) {
    LOG(LL_ERROR, ("I2C=0x%02x cmd=%u (0x%04x) write error", DHT20_ADDRESS, DHT20_REQUEST_DATA_CMD, DHT20_REQUEST_DATA_CMD));
    return false;
  }
  LOG(LL_DEBUG, ("I2C=0x%02x cmd=%u (0x%04x) write success", DHT20_ADDRESS, DHT20_REQUEST_DATA_CMD, DHT20_REQUEST_DATA_CMD));


  _lastRequest = mgos_uptime_micros();
  return rv;
}

// SYNCHRONUOUS CALL
float    dht20_get_humidity()    { return _humidity    + _humOffset; };
float    dht20_get_temperature() { return _temperature + _tempOffset; };

// allows 1st order calibration
void     dht20_set_hum_offset(float offset)  { _humOffset  = offset; };
void     dht20_set_temp_offset(float offset) { _tempOffset = offset; };
float    dht20_get_hum_offset()              { return _humOffset; };
float    dht20_get_temp_offset()             { return _tempOffset; };

// OTHER
uint32_t dht20_last_read()       { return _lastRead; };
uint32_t dht20_last_request()    { return _lastRequest; };
int      dht20_internal_status() { return _status; };


uint8_t crc8(const uint8_t *data, int len) {
  const uint8_t poly = 0x31;
  uint8_t       crc  = 0xFF;

  for (int j = len; j; --j) {
    crc ^= *data++;
    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ poly : (crc << 1);
    }
  }
  return crc;
}
