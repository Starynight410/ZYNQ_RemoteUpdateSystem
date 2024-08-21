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

// CRC32计算函数
uint32_t crc32(uint32_t crc, uint8_t *data, size_t length) {
    while (length--) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *data++];
    }
    return crc;
}

// 读取文件并计算CRC32
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
    crc32_context = crc32(0xFFFFFFFF, *buffer, size);

    fclose(file);
    return file_size;
}

int main() {
    const char *input_filename = "E:\\32_remote_update\\bootimage\\LEDsyn_CRC\\BOOT.BIN";
    const char *output_filename = "E:\\32_remote_update\\bootimage\\LEDsyn_CRC\\BOOT_CRC.BIN";

    // 打印开始信息
    printf("Starting CRC32 calculation for file: %s\n", input_filename);

    // CRC32表生成函数
    generate_crc32_table();

    // 读取文件并计算CRC32
    uint8_t *buffer;
    size_t buffer_size = file_crc32(input_filename, &buffer);
    if (buffer == NULL) {
        printf("Failed to read file or allocate memory.\n");
        return EXIT_FAILURE;
    }

    // 计算CRC32
    crc32_context = crc32(0xFFFFFFFF, buffer, buffer_size);
    crc32_context ^= 0xFFFFFFFF; // 完成CRC计算后需要异或操作0xFFFFFFFF

    // 打开输出文件准备写入
    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("Error opening output file");
        free(buffer);
        return EXIT_FAILURE;
    }

    // 写入原始文件内容
    fwrite(buffer, 1, buffer_size, output_file);

    // 写入CRC32校验码
    if (fwrite(&crc32_context, sizeof(crc32_context), 1, output_file) != 1) {
        perror("Error writing CRC32 to output file");
        fclose(output_file);
        free(buffer);
        return EXIT_FAILURE;
    }

    fclose(output_file);
    free(buffer);

    // 打印完成信息
    printf("File with appended CRC32 has been written to: %s\n", output_filename);

    return EXIT_SUCCESS;
}