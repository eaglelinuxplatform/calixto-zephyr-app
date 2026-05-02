#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#define SDRAM_BASE_ADDR    (0x80000000U)
#define SDRAM_SIZE_BYTES   (32U * 1024U * 1024U)
#define TEST_BLOCK_BYTES   (1U * 1024U * 1024U)

#define WORD16_PER_BLOCK   (TEST_BLOCK_BYTES / sizeof(uint16_t))
#define BLOCK_COUNT        (SDRAM_SIZE_BYTES / TEST_BLOCK_BYTES)

int sdram_block_test(void)
{
    volatile uint16_t *p16;
    uint32_t block, i;
    uint16_t write_pattern, read_val;

    for (block = 0; block < BLOCK_COUNT; ++block)
    {
        p16 = (volatile uint16_t *)(SDRAM_BASE_ADDR + (block * TEST_BLOCK_BYTES));

        for (i = 0; i < WORD16_PER_BLOCK; ++i)
        {
            write_pattern = (uint16_t)((block & 0xFF) ^ (i & 0xFFFF));
            p16[i] = write_pattern;
        }

        __DSB();

        // Read + verify
        for (i = 0; i < WORD16_PER_BLOCK; ++i)
        {
            write_pattern = (uint16_t)((block & 0xFF) ^ (i & 0xFFFF));
            read_val = p16[i];

            if (read_val != write_pattern)
            {
                uint32_t bad_addr =
                    SDRAM_BASE_ADDR + (block * TEST_BLOCK_BYTES) + (i * sizeof(uint16_t));

                printk("\nSDRAM ERROR: block %u idx %u addr 0x%08x exp 0x%04x got 0x%04x\n",
                       block, i, bad_addr, write_pattern, read_val);

                return -1;
            }
        }

        printk("Block %u/%u OK\n", block + 1, BLOCK_COUNT);
    }

    printk("SDRAM FULL TEST PASS\n");
    return 0;
}

int main(void)
{
    printk("Starting SDRAM test...\n");

    int ret = sdram_block_test();

    if (ret == 0)
    {
        printk("SDRAM test SUCCESS\n");
    }
    else
    {
        printk("SDRAM test FAILED\n");
    }
    while (true)
    {
        ;
    }
    return 0;
    
}