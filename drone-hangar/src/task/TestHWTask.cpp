#include "task/TestHWTask.hpp"

#include <Arduino.h>

#include "config.hpp"

TestHWTask::TestHWTask(HWPlatform* pHW) : pHW(pHW) {}

void TestHWTask::tick() { pHW->test(); }
