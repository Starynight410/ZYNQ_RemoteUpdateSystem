#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POLY 0xEDB88320
#define BUFFER_SIZE (4 * 1024 * 1024) // 4MB

// 全局CRC32表
uint32_t crc32_table[256];
uint32_t crc32_context = 0xFFFFFFFF;

// CRC32表生成函数
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

size_t file_crc32(const char *filename, uint8_t **buffer) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    *buffer = (uint8_t *)malloc(file_size);
    if (!*buffer) {
        perror("Memory allocation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t size = fread(*buffer, 1, file_size, file);
    uint32_t crc = crc32(0xFFFFFFFF, *buffer, size);
    fclose(file);
    return size;
}

int verify_file_crc32(const char *input_filename) {
    FILE *file = fopen(input_filename, "rb");
    if (!file) {
        perror("Error opening file for CRC32 verification");
        return EXIT_FAILURE;
    }

    // Allocate buffer to store the file size minus the CRC value
    uint8_t *original_buffer = malloc(BUFFER_SIZE);
    if (!original_buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Read the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Read the original data part (excluding the last 4 bytes for CRC)
    size_t original_size = file_size > sizeof(uint32_t) ? file_size - sizeof(uint32_t) : 0;
    fread(original_buffer, 1, original_size, file);

    // Read the stored CRC32 value from the file
    uint32_t stored_crc = 0;
    fread(&stored_crc, sizeof(stored_crc), 1, file);

    fclose(file);

    // Calculate the CRC32 for the original data
    uint32_t calculated_crc = crc32(0xFFFFFFFF, original_buffer, original_size);

    // Output the result
    if (calculated_crc == stored_crc) {
        printf("CRC32 verification succeeded.\n");
    } else {
        printf("CRC32 verification failed, expected %08X, got %08X.\n", stored_crc, calculated_crc);
    }

    free(original_buffer);
    return calculated_crc == stored_crc ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main() {
    const char *input_filename = "E:\\32_remote_update\\bootimage\\LEDsyn_CRC\\BOOT_CRC.BIN";

    printf("Starting CRC32 verification for file: %s\n", input_filename);

    // CRC32表生成函数
    generate_crc32_table();

    // Perform the verification
    int result = verify_file_crc32(input_filename);
    if (result == EXIT_SUCCESS) {
        printf("The file is intact and has passed the CRC32 check.\n");
    } else {
        printf("The file has failed the CRC32 check and may be corrupted.\n");
    }

    return result;
}