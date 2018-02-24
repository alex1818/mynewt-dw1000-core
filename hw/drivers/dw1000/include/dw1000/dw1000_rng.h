/**
 * Copyright 2018, Decawave Limited, All Rights Reserved
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _DW1000_RNG_H_
#define _DW1000_RNG_H_

#include <stdlib.h>
#include <stdint.h>

#ifndef SS_TWR_ENABLE
#define SS_TWR_ENABLE
#endif
#ifndef DS_TWR_ENABLE
#define DS_TWR_ENABLE
#endif
#ifndef DS_TWR_EXT_ENABLE
#define DS_TWR_EXT_ENABLE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <hal/hal_spi.h>
#include <dw1000/dw1000_regs.h>
#include <dw1000/dw1000_dev.h>
#include <dw1000/dw1000_ftypes.h>

typedef struct _dw1000_rng_config_t{
   uint32_t rx_holdoff_delay;        // Delay between frames, in UWB usec.
   uint32_t tx_holdoff_delay;        // Delay between frames, in UWB usec.
   uint16_t rx_timeout_period;       // Receive response timeout, in UWB usec.
}dw1000_rng_config_t;

typedef enum _dw1000_rng_modes_t{
    DWT_TWR_INVALID = 0,
    DWT_SS_TWR,
    DWT_SS_TWR_T1,
    DWT_SS_TWR_FINAL,
    DWT_SS_TWR_END,
    DWT_DS_TWR,
    DWT_DS_TWR_T1,
    DWT_DS_TWR_T2,
    DWT_DS_TWR_FINAL,
    DWT_DS_TWR_END,
    DWT_DS_TWR_EXT,
    DWT_DS_TWR_EXT_T1,
    DWT_DS_TWR_EXT_T2,
    DWT_DS_TWR_EXT_FINAL,
    DWT_DS_TWR_EXT_END,
    DWT_TDOA
}dw1000_rng_modes_t;

typedef struct _dw1000_rng_status_t{
    uint16_t selfmalloc:1;
    uint16_t initialized:1;
    uint16_t mac_error:1;
    uint16_t invalid_code_error:1;
}dw1000_rng_status_t;


#ifdef DS_TWR_EXT_ENABLE
#ifdef DS_TWR_EXT_ENABLE_INT16
typedef union __triad_t{
    struct _int16_axis{
        int16_t x,y,z;
    };
    struct _int16_spherical{
        int16_t range, azimuth, zenith;
    };
    int16_t array[sizeof(struct _int16_axis)/sizeof(int16_t)];
}int16_triad_t; 
#else
typedef union _triad_t{
    struct _axis{
        float  x,y,z;
    };
    struct _spherical{
       float range, azimuth, zenith;
    };
    float array[sizeof(struct _axis)/sizeof(float)];
}triad_t; 
#endif
#endif

typedef struct _twr_frame_final_t{
        struct _ieee_rng_response_frame_t;
        uint32_t request_timestamp;     // request transmission timestamp.
        uint32_t response_timestamp;    // response reception timestamp.
        uint16_t dummy3;
} twr_frame_final_t;

typedef struct _twr_frame_t{
        struct _twr_frame_final_t;
        union {
            struct  _fusion_ext_t{
                triad_t local_coordinate;       // position triad 
                triad_t spherical_coordinate;   // spherical triad 
                triad_t variance;               // variance triad 
            }__attribute__((__packed__));
            uint8_t payload[sizeof(struct _fusion_ext_t)];
        };
} twr_frame_t;

typedef struct _dw1000_rng_instance_t{
    struct _dw1000_dev_instance_t * dev;
    struct os_sem sem;
    dw1000_rng_config_t * config;
    dw1000_rng_status_t status;
    uint16_t nframes;
    twr_frame_t twr[];
}dw1000_rng_instance_t; 


dw1000_rng_instance_t * dw1000_rng_init(dw1000_dev_instance_t * inst, dw1000_rng_config_t * config);
void dw1000_rng_free(dw1000_rng_instance_t * inst);
dw1000_dev_status_t dw1000_rng_config(dw1000_dev_instance_t * inst, dw1000_rng_config_t * config);
void dw1000_rng_set_callbacks(dw1000_dev_instance_t * inst,  dw1000_dev_cb_t rng_tx_complete_cb, dw1000_dev_cb_t rng_rx_complete_cb, dw1000_dev_cb_t rng_rx_timeout_cb,  dw1000_dev_cb_t rng_rx_error_cb);
dw1000_dev_status_t dw1000_rng_request(dw1000_dev_instance_t * inst, uint16_t dst_address, dw1000_rng_modes_t protocal);
void dw1000_rng_set_frames(dw1000_dev_instance_t * inst, twr_frame_t twr[], uint16_t nframes);
float dw1000_rng_twr_to_tof(twr_frame_t * twr, dw1000_rng_modes_t code);
uint32_t dw1000_rng_twr_to_tof_sym(twr_frame_t * twr, dw1000_rng_modes_t code);

#define dw1000_rng_tof_to_meters(ToF) (float)(ToF * 299792458 * (1.0/499.2e6/128.0)) 
#define dw1000_rng_set_extension_cb(inst, rng_interface_extension_cb) inst->rng_interface_extension_cb = rng_interface_extension_cb 
#define dw1000_rng_set_tx_final_cb(inst, rng_tx_final_cb) inst->rng_tx_final_cb = rng_tx_final_cb 


#ifdef __cplusplus
}
#endif
#endif /* _DW1000_RNG_H_ */
