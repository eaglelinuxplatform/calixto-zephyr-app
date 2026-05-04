/*
 * FlexSPI NOR configuration block for S25FL128L
 * Quad mode, QE enabled, 10 dummy cycles, 133MHz
 */

#include "imxrt1176_som_flexspi_nor_config.h"

#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag              = FLEXSPI_CFG_BLK_TAG,
            .version          = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackInternally,
            .csHoldTime       = 3u,
            .csSetupTime      = 3u,

            // Controller options
            .controllerMiscOption = 0x10,  // Safe config, DDR disabled
            .deviceType           = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType        = kSerialFlash_4Pads,
            .serialClkFreq        = kFlexSpiSerialClk_30MHz,
            .sflashA1Size         = 16u * 1024u * 1024u, // 128Mbit = 16MB

            // Flash configuration commands
            .configCmdEnable   = 1u,
            .configModeType[0] = kDeviceConfigCmdType_Generic,
            .configCmdSeqs[0]  =
                {
                    .seqNum = 1,
                    .seqId  = 12,   // LUT[12] handles QE + LC write
                },

            /* QE = 1 (bit1), LC = 3 (0b0011 = 10 dummy cycles)
             * CR1NV value = 0x32
             */
            .configCmdArgs[0] = 0x32,

            // Lookup table
            .lookupTable =
                {
                    // Quad I/O Fast Read (0xEB)
                    [0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB,
                                           RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    [1] = FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 10,
                                           READ_SDR, FLEXSPI_4PAD, 0x04),

                    // Read Status Register 1 (0x05)
                    [4 * 1 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05,
                                                   READ_SDR, FLEXSPI_1PAD, 0x01),

                    // Write Enable (0x06)
                    [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06,
                                                   STOP, FLEXSPI_1PAD, 0x00),

                    // Sector Erase (4KB) - 0x20
                    [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x20,
                                                   RADDR_SDR, FLEXSPI_1PAD, 0x18),

                    // Block Erase (64KB) - 0xD8
                    [4 * 8 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xD8,
                                                   RADDR_SDR, FLEXSPI_1PAD, 0x18),

                    // Page Program (0x02)
                    [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x02,
                                                   RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    [4 * 9 + 1] = FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0x04,
                                                   STOP, FLEXSPI_1PAD, 0x00),

                    // Chip Erase (0x60)
                    [4 * 11 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x60,
                                                    STOP, FLEXSPI_1PAD, 0x00),

                    // LUT[12] - Write Register (CR1NV, QE + LC)
                    [4 * 12 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x71,
                                                    WRITE_SDR, FLEXSPI_1PAD, 0x01),
                    [4 * 12 + 1] = FLEXSPI_LUT_SEQ(STOP, FLEXSPI_1PAD, 0x00,
                                                    0, 0, 0),
                },
        },

    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = 0x1,
    .blockSize          = 64u * 1024u,
    .isUniformBlockSize = true,
};

#endif /* XIP_BOOT_HEADER_ENABLE */
