#include "stm32crc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// -- Fake Flash Parameters -- //

#define FLASH_PAGE_NUM  (64u)
#define FLASH_PAGE_SIZE (2048u)
#define FLASH_BASE      (uint64_t)(&flash_memory)

// Pretend 22-page (44 kB) bootloader
#define MAX_NUM_BOOTLOADER_PAGES (22u)


// -- State for testing -- //

static uint8_t flash_memory[FLASH_PAGE_NUM][FLASH_PAGE_SIZE];
static uint8_t flash_erased[FLASH_PAGE_NUM];

static bool flash_locked = true;
static bool crc32_enabled = false;
static uint32_t curr_crc = 0xfffffffful;
static uint32_t bl_crc32 = 0;

// Fake boot-ROM, for the firmware update
static uint8_t rom[44*1024] = {0};


// -- Error Messages and STM32-like Constants -- //

static const char* kImageCRCError = "Firmware CRC32 failed";
static const char* kImageLengthError = "Firmware image length is incorrect";
static const char* kFlashEraseError = "Erasing Flash failed";
static const char* kFlashWriteError = "Writing to Flash failed";

static const uint32_t _estack = 0x20008000;
static const uint32_t Reset_Handler = 0x08004d49;
static const uint32_t NMI_Handler = 0x08004cd9;
static const uint32_t HardFault_Handler = 0x08004cdd;

static const uint32_t jumpToApplication[4] = {
    _estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler, // Not required, but suppresses array-size warning
    };


// Initialisation of the ROM contents, on start-up.
static void fill_rom(void)
{
    int len = sizeof(rom) / 4;
    uint32_t* ptr = (uint32_t*)rom;
    for (int i=len; i--;) {
        ptr[i] = (uint32_t)rand();
    }
}

// Mimics a routine often found in STM32 code.
static void Error_Handler(void)
{
    assert(false);
}


// -- CRC32 Routines -- //

bool start_crc32(const uint8_t* buf, uint32_t len, uint32_t* crc)
{
    assert(crc32_enabled == false);
    crc32_enabled = true;
    curr_crc = stm32crc_calc(buf, len);
    *crc = curr_crc;
    return true;
}

uint32_t accum_crc32(const uint8_t* buf, uint32_t len)
{
    uint32_t crc = curr_crc;
    assert(crc32_enabled == true);
    for (int i=0; i<len; i++) {
        crc = stm32crc_next(crc, buf[i]);
    }
    return (curr_crc = crc);
}

bool finish_crc32(void)
{
    assert(crc32_enabled);
    crc32_enabled = false;
    curr_crc = 0xfffffffful;
    return true;
}


// -- Flash Routines -- //

bool flash_erase(uint32_t page, uint32_t num)
{
    assert(page < FLASH_PAGE_NUM || (page + num) <= FLASH_PAGE_NUM);
    assert(page + num <= MAX_NUM_BOOTLOADER_PAGES);
    if (page >= FLASH_PAGE_NUM || page + num > FLASH_PAGE_NUM) {
        return false;
    }
    for (int i=page; i<page+num; i++) {
        flash_erased[i] = 1;
    }
    return true;
}

bool flash_write(uint64_t loc, uint64_t val)
{
    assert(flash_locked == false);
    assert((loc & 0x07ull) == 0ull);

    uint64_t offset = loc - FLASH_BASE;
    uint32_t page = offset / FLASH_PAGE_SIZE;
    assert(page < MAX_NUM_BOOTLOADER_PAGES);
    assert(flash_erased[page] == 1);

    uint32_t index = offset % FLASH_PAGE_SIZE;
    uint64_t* ptr = (uint64_t*)(void*)&flash_memory[page][index];
    *ptr = val;

    return true;
}

void HAL_FLASH_Unlock(void)
{
    assert(flash_locked);
    flash_locked = false;
}

void HAL_FLASH_Lock(void)
{
    assert(!flash_locked);
    flash_locked = true;
}


// -- Fake Bootloader-update Routines -- //

const char* update_bootloader(const uint8_t* bootrom, uint32_t length, uint32_t crc32)
{
    const char* result = NULL;

    // -- Preparation: Check that we are given a valid bootloader -- //

    if (!start_crc32(bootrom, length, &bl_crc32) || !finish_crc32()) {
        Error_Handler(); // HAL/config/internal/other error
    }

    if (bl_crc32 != crc32) {
        return kImageCRCError;
    }

    uint32_t num_pages = (length + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    if (num_pages > MAX_NUM_BOOTLOADER_PAGES) {
        return kImageLengthError;
    }

    // In case bootloader-update fails, we can try again ...
    const uint64_t* jmp = (uint64_t*)jumpToApplication;

    HAL_FLASH_Unlock();

    // -- Stage I: Erase current bootloader, and set a backup-jump -- //

    if (!flash_erase(0, num_pages)) {
        result = kFlashEraseError;
        goto flash_lock;
    }

    if (!flash_write(FLASH_BASE, jmp[0]) || !flash_write(FLASH_BASE+8, jmp[1])) {
        result = kFlashWriteError;
        goto flash_lock;
    }

    // -- Stage II: Write new bootloader, except for the first page -- //

    // Write all pages except for the first
    const uint64_t* rom = (uint64_t*)(void*)bootrom;
    uint64_t dst = FLASH_BASE + (uint64_t)FLASH_PAGE_SIZE;
    int len = (length + 7) / 8;
    for (int i=FLASH_PAGE_SIZE/8; i<len; i++) {
        if (!flash_write(dst, rom[i])) {
            result = kFlashWriteError;
            goto flash_lock;
        }
        dst += 8;
    }

    HAL_FLASH_Lock();

    // -- Stage III: Check to see if any errors, so far -- //

    // Calculate the "partial" CRC32 using the first 2 kB of the boot ROM, and
    // then the rest from the updated Flash
    if (!start_crc32(bootrom, FLASH_PAGE_SIZE, &bl_crc32)) {
        Error_Handler(); // HAL/config/internal/other error
    }

    dst = FLASH_BASE + (uint64_t)FLASH_PAGE_SIZE;
    bl_crc32 = accum_crc32((uint8_t*)(void*)dst, length - FLASH_PAGE_SIZE);

    if (!finish_crc32()) {
        Error_Handler(); // HAL/config/internal/other error
    }

    if (bl_crc32 != crc32) {
        printf("CRC: 0x%08x (LEN = %u)\n", bl_crc32, length);
        return kImageCRCError;
    }

    // -- Stage IV: Write 'page[0]' of the new bootloader, to finish -- //

    HAL_FLASH_Unlock();

    // Write the first page of the new bootloader
    if (!flash_erase(0, 1)) {
        result = kFlashEraseError;
        goto flash_lock;
    }

    dst = FLASH_BASE;
    for (int i=0; i < FLASH_PAGE_SIZE/8; i++) {
        if (!flash_write(dst, rom[i])) {
            result = kFlashWriteError;
            goto flash_lock;
        }
        dst += 8;
    }

flash_lock:
    HAL_FLASH_Lock();

    if (result != NULL) {
        return result;
    }

    // -- Finalise: Perform one last CRC32 check of entire bootloader -- //

    if (!start_crc32((const uint8_t*)FLASH_BASE, length, &bl_crc32) || !finish_crc32()) {
        Error_Handler(); // HAL/config/internal/other error
    }

    if (bl_crc32 != crc32) {
        result = kImageCRCError;
    }

    return result;
}


/**
 * Testbench for firmware-update style code, that performs a two-stage firmware
 * update of a "bootloader," of an embedded device. The update is performed from
 * "application" code, where the bootloader is overwritten with a new version.
 *
 * Note:
 *  - a "jump-to-app" section is written first, so that if the bootloader-update
 *    fails, the application continues to operate -- minimising the number of
 *    events that can brick a device;
 */
void fwupdate_tb()
{
    printf("\nFake 2-stage Firmware Update Test\n");

    fill_rom();
    uint32_t len = sizeof(rom);
    uint32_t crc = stm32crc_calc(rom, len);
    const char* res = update_bootloader(rom, len, crc);

    if (res != NULL) {
        printf("Failed: %s\n", res);
        assert(false);
    }

    printf("passed\n");
}
