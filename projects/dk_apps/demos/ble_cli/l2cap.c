/**
 ****************************************************************************************
 *
 * @file l2cap.c
 *
 * @brief BLE L2CAP API implementation
 *
 * Copyright (C) 2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "osal.h"
#include "l2cap.h"
#include "cli_utils.h"
#include "debug_utils.h"

#define MAX_L2CAP_DATA_LENGTH   (512)

static bool l2cap_connect(int argc, const char **argv)
{
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t psm;
        uint16_t initial_credit;
        uint16_t scid;

        if (argc < 5) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &psm)) {
                return false;
        }

        if (!parse_u16(argv[4], &initial_credit)) {
                return false;
        }

        status = ble_l2cap_connect(conn_idx, psm, initial_credit, &scid);
        print_command("ble_l2cap_connect ( %d, 0x%04x, %d )", conn_idx, psm, initial_credit);
        if (status == BLE_STATUS_OK) {
                print_parameter("scid: 0x%04x", scid);
        }
        print_status("return: %s", get_status(status));

        return true;
}

static bool l2cap_disconnect(int argc, const char **argv)
{
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t scid;

        if (argc < 4) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &scid)) {
                return false;
        }

        status = ble_l2cap_disconnect(conn_idx, scid);
        print_command("ble_l2cap_disconnect ( %d, 0x%04x )", conn_idx, scid);
        print_status("return: %s", get_status(status));

        return true;
}

static bool l2cap_send(int argc, const char **argv)
{
        static uint8_t data[MAX_L2CAP_DATA_LENGTH];
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t scid;
        int length;

        if (argc < 4) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &scid)) {
                return false;
        }

        length = str_to_hex(argv[4], data, sizeof(data));

        if (length < 0) {
                return false;
        }

        status = ble_l2cap_send(conn_idx, scid, length, data);
        print_command("ble_l2cap_send ( %d, 0x%04x, %d )", conn_idx, scid, length);
        print_data_parameter(data, length);
        print_status("return: %s", get_status(status));

        return true;
}

static bool listen(int argc, const char **argv, bool defer_setup)
{
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t psm;
        uint16_t initial_credits;
        uint16_t scid;
        gap_sec_level_t sec_level;

        if (argc < 6) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &psm)) {
                return false;
        }

        /* security level is enum with values from 0 to 3 and is size of 1 byte */
        if (!parse_u8(argv[4], (uint8_t *) &sec_level) || (sec_level > GAP_SEC_LEVEL_4)) {
                return false;
        }

        if (!parse_u16(argv[5], &initial_credits)) {
                return false;
        }

        if (defer_setup) {
                status = ble_l2cap_listen_defer_setup(conn_idx, psm, sec_level, initial_credits,
                                                                                        &scid);
                print_command("ble_l2cap_listen_defer_setup ( %d, 0x%04x, %d, %d )", conn_idx, psm,
                                                                sec_level, initial_credits);
        } else {
                status = ble_l2cap_listen(conn_idx, psm, sec_level, initial_credits, &scid);
                print_command("ble_l2cap_listen ( %d, 0x%04x, %d, %d )", conn_idx, psm, sec_level,
                                                                                  initial_credits);
        }
        if (status == BLE_STATUS_OK) {
                print_parameter("LE PSM: 0x%04x", psm);
                print_parameter("Source CID: 0x%04x", scid);
                print_parameter("Security level: %d", sec_level);
                print_parameter("Initial credits: %d", initial_credits);
        }
        print_status("return: %s", get_status(status));

        return true;
}

static bool l2cap_listen(int argc, const char **argv)
{
        return listen(argc, argv, false);
}

static bool l2cap_listen_defer_setup(int argc, const char **argv)
{
        return listen(argc, argv, true);
}

static bool l2cap_stop_listen(int argc, const char **argv)
{
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t scid;

        if (argc < 4) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &scid)) {
                return false;
        }

        status = ble_l2cap_stop_listen(conn_idx, scid);
        print_command("ble_l2cap_stop_listen ( %d, 0x%04x)", conn_idx, scid);
        print_status("return: %s", get_status(status));

        return true;
}

static bool l2cap_add_credits(int argc, const char **argv)
{
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t scid;
        uint16_t credits;

        if (argc < 5) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &scid)) {
                return false;
        }

        if (!parse_u16(argv[4], &credits)) {
                return false;
        }

        status = ble_l2cap_add_credits(conn_idx, scid, credits);
        print_command("ble_l2cap_add_credits ( %d, 0x%04x, %d)", conn_idx, scid, credits);
        print_status("return: %s", get_status(status));

        return true;
}

static bool l2cap_connection_cfm(int argc, const char **argv)
{
        enum ble_l2cap_connection_status cfm_status;
        ble_error_t status;
        uint16_t conn_idx;
        uint16_t scid;

        if (argc < 5) {
                return false;
        }

        if (!parse_u16(argv[2], &conn_idx)) {
                return false;
        }

        if (!parse_u16(argv[3], &scid)) {
                return false;
        }

        if (!parse_u8(argv[4], (uint8_t *) &cfm_status)) {
                return false;
        }

        status = ble_l2cap_connection_cfm(conn_idx, scid, cfm_status);
        print_command("l2cap_connection_cfm ( %d, 0x%04x, 0x%02x)", conn_idx, scid, cfm_status);
        print_status("return: %s", get_status(status));

        return true;
}

static const debug_handler_t l2cap_handlers[] = {
        {"connect", "<conn_idx> <psm> <initial_credits>", l2cap_connect },
        {"disconnect", "<conn_idx> <scid>", l2cap_disconnect },
        {"send", "<conn_idx> <scid> <data>", l2cap_send },
        {"listen", "<conn_idx> <psm> <sec_level> <initial_credits>", l2cap_listen},
        {"listen_defer", "<conn_idx> <psm> <sec_level> <initial_credits>",
                                                                        l2cap_listen_defer_setup},
        {"stop_listen", "<conn_idx> <scid>", l2cap_stop_listen},
        {"connection_cfm", "<conn_idx> <scid> <status>", l2cap_connection_cfm},
        {"add_credits", "<conn_idx> <scid> <credits>", l2cap_add_credits},
        { NULL },
};

void l2cap_command(int argc, const char *argv[], void *user_data)
{
        debug_handle_message(argc, argv, l2cap_handlers);
}

static void handle_ble_evt_l2cap_connected(ble_evt_l2cap_connected_t *info)
{
        print_event("BLE_EVT_L2CAP_CONNECTED");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("LE protocol/service multiplexer: %d", info->psm);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Destination CID: %d", info->dcid);
        print_parameter("Local credits available: %d", info->local_credits);
        print_parameter("Remote credits available: %d", info->remote_credits);
        print_parameter("Negotiated MTU: %d", info->mtu);
}

static void handle_ble_evt_l2cap_connection_failed(ble_evt_l2cap_connection_failed_t *info)
{
        print_event("BLE_EVT_L2CAP_CONNECTION_FAILED");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Status: %s", get_status(info->status));
}

static void handle_ble_evt_l2cap_disconnected(ble_evt_l2cap_disconnected_t *info)
{
        print_event("BLE_EVT_L2CAP_DISCONNECTED");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Reason: 0x%02x", info->reason);
}

static void handle_ble_evt_l2cap_sent(ble_evt_l2cap_sent_t *info)
{
        print_event("BLE_EVT_L2CAP_SENT");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Remote credits available: %d", info->remote_credits);
        print_parameter("Status: %s", get_status(info->status));
}

static void handle_ble_evt_l2cap_data_ind(ble_evt_l2cap_data_ind_t *info)
{
        print_event("BLE_EVT_L2CAP_DATA_IND");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Local credits consumed by received data: %d", info->local_credits_consumed);
        print_data_parameter(info->data, info->length);
}

static void handle_ble_evt_l2cap_credit_changed(ble_evt_l2cap_credit_changed_t *info)
{
        print_event("BLE_EVT_L2CAP_CREDIT_CHANGED");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Remote credits available: %d", info->remote_credits);
}

static void handle_ble_evt_l2cap_connection_req(ble_evt_l2cap_connection_req_t *info)
{
        print_event("BLE_EVT_L2CAP_CONNECTION_REQ");
        print_parameter("Connection index: %d", info->conn_idx);
        print_parameter("LE protocol/service multiplexer: %d", info->psm);
        print_parameter("Source CID: %d", info->scid);
        print_parameter("Destination CID: %d", info->dcid);
        print_parameter("Remote credits available: %d", info->remote_credits);
        print_parameter("Negotiated MTU: %d", info->mtu);
}

bool l2cap_handle_event(ble_evt_hdr_t *event)
{
        switch (event->evt_code) {
        case BLE_EVT_L2CAP_CONNECTED:
                handle_ble_evt_l2cap_connected((ble_evt_l2cap_connected_t *) event);
                break;
        case BLE_EVT_L2CAP_CONNECTION_FAILED:
                handle_ble_evt_l2cap_connection_failed((ble_evt_l2cap_connection_failed_t *) event);
                break;
        case BLE_EVT_L2CAP_DISCONNECTED:
                handle_ble_evt_l2cap_disconnected((ble_evt_l2cap_disconnected_t *) event);
                break;
        case BLE_EVT_L2CAP_REMOTE_CREDITS_CHANGED:
                handle_ble_evt_l2cap_credit_changed((ble_evt_l2cap_credit_changed_t *) event);
                break;
        case BLE_EVT_L2CAP_DATA_IND:
                handle_ble_evt_l2cap_data_ind((ble_evt_l2cap_data_ind_t *) event);
                break;
        case BLE_EVT_L2CAP_SENT:
                handle_ble_evt_l2cap_sent((ble_evt_l2cap_sent_t *) event);
                break;
        case BLE_EVT_L2CAP_CONNECTION_REQ:
                handle_ble_evt_l2cap_connection_req((ble_evt_l2cap_connection_req_t *) event);
                break;
        default:
                return false;
        }

        return true;
}
