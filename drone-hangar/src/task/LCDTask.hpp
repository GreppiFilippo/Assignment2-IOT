#ifndef __LCD_TASK__
#define __LCD_TASK__

#include "devices/LCD.hpp"
#include "kernel/Task.hpp"
#include "model/Context.hpp"

/**
 * @brief Task to manage the LCD display.
 *
 * This task periodically updates the LCD with relevant information from the system context.
 */
class LCDTask : public Task
{
   private:
    Context* pContext;
    LCD* lcd;
    const char* lastMsg;

   public:
    LCDTask(LCD* lcd, Context* pContext);
    void tick();
};

#endif  // __LCD_TASK__
