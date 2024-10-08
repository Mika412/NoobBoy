#include "mbc.h"

MBC::MBC(uint8_t *rom) { this->rom = rom; };
MBC::MBC(uint8_t *rom, uint8_t *ram, int rom_banks_count, int ram_banks_count) {
    this->rom = rom;
    this->ram = ram;
    this->rom_banks_count = rom_banks_count;
    this->ram_banks_count = ram_banks_count;
};

uint8_t MBC0::read_byte(uint16_t address) {
    if (address < 0x8000)
        return rom[address];

    return 0;
}

uint8_t MBC1::read_byte(uint16_t address) {
    if (address < 0x4000) {
        int bank = mode * (ram_bank << 5) % rom_banks_count;
        return rom[bank * 0x4000 + address];
    } else if (address < 0x8000) {
        int bank = ((ram_bank << 5) | rom_bank) % rom_banks_count;
        return rom[bank * 0x4000 + address - 0x4000];
    } else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled) {
            int bank = mode * ram_bank % ram_banks_count;
            return ram[bank * 0x2000 + address - 0xA000];
        }
    }
    return 0xFF;
}

void MBC1::write_byte(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        ram_enabled = (value & 0x0F) == 0x0A;
    } else if (address < 0x4000) {
        value &= 0x1F;
        if (value == 0)
            value = 1;
        rom_bank = value;
    } else if (address < 0x6000) {
        ram_bank = value & 0x3;
    } else if (address < 0x8000) {
        mode = value & 0x1;
    } else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled) {
            int bank = (ram_bank * mode) % ram_banks_count;
            ram[bank * 0x2000 + address - 0xA000] = value;
        }
    }
}

uint8_t MBC2::read_byte(uint16_t address) {
    if (address < 0x4000)
        return rom[address];
    else if (address < 0x8000)
        return rom[rom_bank * 0x4000 + address - 0x4000];
    else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled)
            return ram[ram_bank * 0x2000 + address - 0xA000];
    }

    return 0;
}

void MBC2::write_byte(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        if ((address & 0x0100) == 0)
            ram_enabled = value == 0x0a;
    } else if (address < 0x4000) {
        if ((address & 0x0100) != 0)
            rom_bank = value;
    } else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled)
            ram[ram_bank * 0x2000 + address - 0xA000] = value;
    }
}

uint8_t MBC3::read_byte(uint16_t address) {
    if (address < 0x4000)
        return rom[address];
    else if (address < 0x8000)
        return rom[rom_bank * 0x4000 + address - 0x4000];
    else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled) {
            if (ram_bank <= 0x03)
                return ram[ram_bank * 0x2000 + address - 0xA000];
        }
    }

    return 0;
}

void MBC3::write_byte(uint16_t address, uint8_t value) {
    if (address < 0x2000)
        ram_enabled = (value & 0x0f) == 0x0a;
    else if (address < 0x4000) {
        rom_bank = value & 0x7f;
        if (rom_bank == 0x00)
            rom_bank = 0x01;
    } else if (address < 0x6000)
        ram_bank = value & 0x0f;
    else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled) {
            if (ram_bank <= 0x03)
                ram[ram_bank * 0x2000 + address - 0xA000] = value;
        }
    }
}

uint8_t MBC5::read_byte(uint16_t address) {
    if (address < 0x4000)
        return rom[address];
    else if (address < 0x8000)
        return rom[rom_bank * 0x4000 + address - 0x4000];
    else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled)
            return ram[ram_bank * 0x2000 + address - 0xA000];
    }

    return 0;
}

void MBC5::write_byte(uint16_t address, uint8_t value) {
    if (address < 0x2000)
        ram_enabled = (value & 0x0f) == 0x0a;
    else if (address < 0x3000)
        rom_bank = (rom_bank & 0x100) | value;
    else if (address < 0x4000)
        rom_bank = (rom_bank & 0xff) | ((value & 0x01) << 8);
    else if (address < 0x6000)
        ram_bank = (value & 0x0f) % ram_banks_count;
    else if (address >= 0xA000 && address < 0xC000) {
        if (ram_enabled)
            ram[ram_bank * 0x2000 + address - 0xA000] = value;
    }
}
