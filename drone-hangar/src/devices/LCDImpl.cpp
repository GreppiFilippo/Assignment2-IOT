#include "LCD.hpp"

#define MAX_WORDS 32
#define MAX_WORD_LEN 32

LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows)
{
    this->lcd = new LiquidCrystal_I2C(addr, cols, rows);
    this->lcd->init();
    this->lcd->backlight();
    this->_cols = cols;
    this->_rows = rows;
}

void LCD::print(const char* message)
{
    this->clear();
    if (message == nullptr || message[0] == '\0')
        return;

    int len = strlen(message);
    int maxChars = this->_cols * this->_rows;
    (void)maxChars;  // keep if you want to add truncation logic later

    this->lcd->setCursor(0, 0);

    // Split into words safely (bounded buffers)
    char words[MAX_WORDS][MAX_WORD_LEN];
    int num_words = 0;
    int wpos = 0;

    for (int i = 0; message[i] != '\0' && num_words < MAX_WORDS; ++i)
    {
        char c = message[i];
        if (c == ' ')
        {
            if (wpos > 0)
            {
                words[num_words][wpos] = '\0';
                num_words++;
                wpos = 0;
            }
        }
        else
        {
            if (wpos < MAX_WORD_LEN - 1)
            {
                words[num_words][wpos++] = c;
            }
            else
            {
                // truncate the word if it's too long; skip remaining chars of this word
                while (message[i + 1] != '\0' && message[i + 1] != ' ') ++i;
                words[num_words][wpos] = '\0';
                num_words++;
                wpos = 0;
            }
        }
    }

    if (wpos > 0 && num_words < MAX_WORDS)
    {
        words[num_words][wpos] = '\0';
        num_words++;
    }

    int idx = 0;
    int current_line = 0;
    int current_line_chars = 0;

    while (idx < num_words && current_line < this->_rows)
    {
        size_t wlen = strlen(words[idx]);

        if (current_line_chars == 0)
        {
            if ((int)wlen <= this->_cols)
            {
                this->lcd->print(words[idx]);
                current_line_chars = (int)wlen;
                idx++;
            }
            else
            {
                // Word longer than a line: print a truncated prefix
                char buf[33];
                int tocopy =
                    this->_cols < (int)sizeof(buf) - 1 ? this->_cols : (int)sizeof(buf) - 1;
                strncpy(buf, words[idx], tocopy);
                buf[tocopy] = '\0';
                this->lcd->print(buf);
                idx++;  // drop the rest of the long word
                current_line++;
                if (current_line < this->_rows)
                    this->lcd->setCursor(0, current_line);
                current_line_chars = 0;
            }
        }
        else
        {
            // Need a space before the next word
            if (current_line_chars + 1 + (int)wlen <= this->_cols)
            {
                this->lcd->print(" ");
                this->lcd->print(words[idx]);
                current_line_chars += 1 + (int)wlen;
                idx++;
            }
            else
            {
                // move to next line
                current_line++;
                if (current_line >= this->_rows)
                    break;
                this->lcd->setCursor(0, current_line);
                current_line_chars = 0;
            }
        }
    }
}

void LCD::clear() { this->lcd->clear(); }
