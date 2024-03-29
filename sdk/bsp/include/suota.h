/**
 * \addtogroup PLA_BSP_SYSTEM
 * \{
 * \addtogroup BSP_SUOTA Software Update Over The Air
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file suota.h
 *
 * @brief SUOTA structure definitions
 *
 * Copyright (C) 2015-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SUOTA_H_
#define SUOTA_H_

#include <stdint.h>

#define SUOTA_VERSION_1_1       11
#define SUOTA_VERSION_1_2       12
#define SUOTA_VERSION_1_3       13
#define SUOTA_VERSION_1_4       14      // Security extension

#ifndef SUOTA_VERSION
#define SUOTA_VERSION           SUOTA_VERSION_1_3
#endif

#if SUOTA_PSM && (SUOTA_VERSION < SUOTA_VERSION_1_2)
#       error "SUOTA_PSM is only applicable to SUOTA_VERSION >= 1.2"
#endif

/**
 * \struct suota_1_1_product_header_t;
 *
 * \brief SUOTA 1.1 product header as defined by Dialog SUOTA specification.
 *
 * \note the same header is used for any SUOTA version newer than 1.1
 *
 */
typedef struct {
        uint8_t signature[2];
        uint16_t flags;
        uint32_t current_image_location;
        uint32_t update_image_location;
        uint8_t reserved[8];
} __attribute__((packed)) suota_1_1_product_header_t;

#define SUOTA_1_1_PRODUCT_HEADER_SIGNATURE_B1   0x70
#define SUOTA_1_1_PRODUCT_HEADER_SIGNATURE_B2   0x62

/**
 * \struct suota_1_1_image_header_t
 *
 * \brief SUOTA 1.1 image header as defined by Dialog SUOTA specification.
 *
 * \note the same header is used for any SUOTA version newer than 1.1
 *
 */
typedef struct {
        uint8_t signature[2];
        uint16_t flags;
        uint32_t code_size;
        uint32_t crc;
        uint8_t version[16];
        uint32_t timestamp;
        uint32_t exec_location;
} __attribute__((packed)) suota_1_1_image_header_t;

#define SUOTA_1_1_IMAGE_HEADER_SIGNATURE_B1     0x70
#define SUOTA_1_1_IMAGE_HEADER_SIGNATURE_B2     0x61

#define SUOTA_1_1_IMAGE_FLAG_FORCE_CRC          0x01
#define SUOTA_1_1_IMAGE_FLAG_VALID              0x02
#define SUOTA_1_1_IMAGE_FLAG_RETRY1             0x04
#define SUOTA_1_1_IMAGE_FLAG_RETRY2             0x08

typedef suota_1_1_image_header_t suota_image_header_t;

/**
 * \struct suota_1_1_image_header_da1469x_t
 *
 * \brief SUOTA 1.1 image header for DA1469x devices.
 *
 */
typedef struct {
        /** Image identifier */
        uint8_t image_identifier[2];
        /** Code size */
        uint32_t size;
        /** Code CRC */
        uint32_t crc;
        /** Version string (ends with '\0') */
        uint8_t version_string[16];
        /** Timestamp - seconds elapsed since epoch 1/1/1970 */
        uint32_t timestamp;
        /** Address of interrupt vector table */
        uint32_t pointer_to_ivt;
} __attribute__((packed)) suota_1_1_image_header_da1469x_t;

#define SUOTA_1_1_IMAGE_DA1469x_HEADER_SIGNATURE_B1       0x51
#define SUOTA_1_1_IMAGE_DA1469x_HEADER_SIGNATURE_B2       0x71

#define SUOTA_1_1_PRODUCT_DA1469x_HEADER_SIGNATURE_B1     0x50
#define SUOTA_1_1_PRODUCT_DA1469x_HEADER_SIGNATURE_B2     0x70

#endif /* SUOTA_H_ */

/**
 * \}
 * \}
 */
