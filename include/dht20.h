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

#pragma once

#include <stdbool.h>
#include "mgos_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DHT20_LIB_VERSION    (F("0.1.0"))

#define DHT20_ADDRESS 0x38
#define DHT20_REQUEST_DATA_CMD 0xAC3300
#define DHT20_READ_STATUS_CMD  0xAC7100
    
#define DHT20_OK             0
#define DHT20_ERROR_CHECKSUM -10
#define DHT20_ERROR_CONNECT  -11
#define DHT20_MISSING_BYTES  -12


// SYNCHRONUOUS CALL
int      dht20_read(void);
float    dht20_get_temperature(void);
float    dht20_get_humidity(void);

// ASYNCHRONUOUS CALL
bool     dht20_request_data(struct mgos_i2c *i2c);
bool     dht20_ready_data(void);
int      dht20_read_data(struct mgos_i2c *i2c);

bool     dht20_read_status(struct mgos_i2c *i2c);

// allows 1st order calibration
void     dht20_set_hum_offset(float offset);
void     dht20_set_temp_offset(float offset);
float    dht20_get_hum_offset(void);
float    dht20_get_temp_offset(void);

// OTHER
uint32_t dht20_last_read(void);
uint32_t dht20_last_request(void);
int      dht20_internal_status(void);

uint8_t crc8(const uint8_t *data, int len);

#ifdef __cplusplus
}
#endif
