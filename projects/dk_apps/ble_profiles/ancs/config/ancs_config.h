/**
 ****************************************************************************************
 *
 * @file ble_ancs_config.h
 *
 * @brief Application configuration
 *
 * Copyright (C) 2016-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef BLE_ANCS_CONFIG_H_
#define BLE_ANCS_CONFIG_H_

/*
 * Non-zero value enables extended printouts from application, like more detailed information about
 * requests made over ANCS.
 *
 * When disabled, notifications only are printed when all informations have been retrieved.
 */
#ifndef CFG_VERBOSE_LOG
#       define CFG_VERBOSE_LOG (0)
#endif

/*
 * Maximum length for attributes retrieved for notification (where applicable)
 */
#define CFG_TITLE_ATTRIBUTE_MAXLEN      (25) // Title attribute
#define CFG_MESSAGE_ATTRIBUTE_MAXLEN    (75) // Message attribute

/*
 * If set to 1, old notifications received upon connection will be discarded
 */
#define CFG_DROP_PREEXISTING_NOTIFICATIONS      (1)

/*
 * Set a free heap threshold in bytes under which all incoming notifications will be dropped
 */
#define CFG_DROP_ALL_NOTIF_THRESHOLD            (200)

/*
 * Set a maximum number of notifications to be temporarily stored in the application's notification
 * list.
 *
 * \note If set to 0, this will be ignored.
 */
#define CFG_NOTIF_QUEUE_MAX                     (200)
/*
 * Timeout for Data Source requests
 */
#define CFG_REQUEST_TIMEOUT_MS  (10000)

/*
 * Delay before starting browse request for ANCS
 */
#define CFG_BROWSE_DELAY_MS  (1000)

#endif /* BLE_ANCS_CONFIG_H_ */
