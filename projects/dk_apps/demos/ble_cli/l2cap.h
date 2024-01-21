/**
 ****************************************************************************************
 *
 * @file l2cap.h
 *
 * @brief BLE L2CAP API
 *
 * Copyright (C) 2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef L2CAP_H_
#define L2CAP_H_

#include "ble_l2cap.h"
#include "ble_common.h"

void l2cap_command(int argc, const char *argv[], void *user_data);

bool l2cap_handle_event(ble_evt_hdr_t *event);


#endif /* L2CAP_H_ */
