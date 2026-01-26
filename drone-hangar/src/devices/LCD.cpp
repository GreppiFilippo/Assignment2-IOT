#include "LCD.hpp"

#include "config.hpp"
#include "kernel/Logger.hpp"

LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows)
{
    this->_lcd = new LiquidCrystal_I2C(addr, cols, rows);
    this->_lcd->init();
    this->_lcd->backlight();
    this->_cols = cols;
    this->_rows = rows;
    this->_addr = addr;
}

void LCD::print(const char* message)
{
    this->clear();

    if (!message || message[0] == '\0')
    {
        return;
    }

    uint16_t maxChars = this->_cols * this->_rows;

    for (uint16_t i = 0; i < maxChars && message[i] != '\0'; i++)
    {
        this->_lcd->print(message[i]);
    }
}

void LCD::clear() { this->_lcd->clear(); }
