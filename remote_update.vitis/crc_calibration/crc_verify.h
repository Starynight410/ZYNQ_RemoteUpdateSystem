#ifndef CRC_VERIFY_H_
#define CRC_VERIFY_H_

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "xparameters.h"
#include "xil_printf.h"

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
void generate_crc32_table();

uint32_t crc32(uint32_t crc, uint8_t *data, size_t length);

int verify_file_crc32(const char *input_buffer, unsigned int ReceivedCount);


#endif /* CRC_VERIFY_H_ */
