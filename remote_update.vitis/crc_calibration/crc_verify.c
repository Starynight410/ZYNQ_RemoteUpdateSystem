/***************************** Include Files *********************************/
#include "crc_verify.h"
#include "xtime_l.h"
#include "stdio.h"

/**************************** Type Definitions *******************************/
#define POLY 			0xEDB88320
#define BUFFER_SIZE 	(4 * 1024 * 1024) 	// 4MB

/************************** Variable Definitions *****************************/
// È«¾ÖCRC32±í
uint32_t crc32_table[256];
uint32_t crc32_context = 0xFFFFFFFF;

/*****************************************************************************/
/**
 *
 * This function is used for CRC verification function, using CRC32
 * lookup table method. After receiving verification, subsequent flash
 * update operations can be executed.
 *
 * @param	None.
 *
 * @return	EXIT_SUCCESS if successful, else EXIT_FAILURE.
 *
 * @note		None.
 *
 *****************************************************************************/

// CRC32 table generate function
void generate_crc32_table() {
    int i, j;
    for (i = 0; i < 256; i++) {
        uint32_t c = i;
        for (j = 8; j != 0; --j) {
            if ((c & 1) == 1)
                c = (c >> 1) ^ POLY;
            else
                c >>= 1;
        }
        crc32_table[i] = c;
    }
}

uint32_t crc32(uint32_t crc, uint8_t *data, size_t length) {
    while (length--) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *data++];
    }
    return crc ^ 0xFFFFFFFF;
}

int verify_file_crc32(const char *input_buffer, unsigned int ReceivedCount) {

	// Read the original data part (excluding the last 4 bytes for CRC)
	u8 original_buffer[ReceivedCount-4];
	uint32_t stored_crc;

	for (int i = 0; i < ReceivedCount; i++) {
		if (i < ReceivedCount-4) {
		    // Read the original data part (excluding the last 4 bytes for CRC)
			original_buffer[i] = input_buffer[i];
		}
		else {
		    // Read the stored CRC32 value from the file
			stored_crc = (uint32_t)input_buffer[ReceivedCount-1] << 24 |
						 (uint32_t)input_buffer[ReceivedCount-2] << 16 |
						 (uint32_t)input_buffer[ReceivedCount-3] << 8  |
						 (uint32_t)input_buffer[ReceivedCount-4] ;
		}
	}

    // Calculate the CRC32 for the original data
    uint32_t calculated_crc = crc32(0xFFFFFFFF, original_buffer, ReceivedCount-4);

    return calculated_crc == stored_crc ? EXIT_SUCCESS : EXIT_FAILURE;
}
