#include "i2c.h"


void scan() {
    for (int row = 0; row < 8; row++) {
        int base_addr = row << 4;
        printf("%02X: ", base_addr);
        for (int col = 0; col < 16; col++) {
            int addr = base_addr + col;
            uint8_t rxdata;
            if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78) {
                printf(". ");
            } else if (i2c_read_blocking(i2c_default, (uint8_t)addr, &rxdata, 1, false) < 0) {
                printf(". ");
            } else {
                printf("@ ");
            }
        }
        printf("\n");
    }
    printf("Done.\n");
}