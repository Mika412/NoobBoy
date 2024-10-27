#pragma once

#include <iostream>

class MBC {
   public:
    uint8_t *rom;
    uint8_t *ram;
    int rom_banks_count = 1;
    int ram_banks_count = 1;

    MBC(uint8_t *rom);
    MBC(uint8_t *rom, uint8_t *ram, int rom_banks_count, int ram_banks_count);
    virtual uint8_t read_byte(uint16_t address) = 0;
    virtual void write_byte(uint16_t address, uint8_t value) = 0;
};

class MBC0 : public MBC {
   public:
    using MBC::MBC;
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value) {};
};

class MBC1 : public MBC {
   public:
    bool ram_enabled = false;
    bool mode = false;
    uint8_t rom_bank = 1;
    uint8_t ram_bank = 0;

    using MBC::MBC;
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);
};

class MBC2 : public MBC1 {
   public:
    using MBC1::MBC1;
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);
};

class MBC3 : public MBC1 {
   public:
    using MBC1::MBC1;
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);
};

class MBC5 : public MBC1 {
   public:
    using MBC1::MBC1;
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);
};
