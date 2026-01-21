#include "model/Context.hpp"

void Context::closeDoor()
{
    this->closeDoorRequested = true;
}

void Context::openDoor()
{
    this->openDoorRequested = true;
}

bool Context::isDoorOpen()
{
    return this->doorOpen;
}

void Context::setDoorOpened()
{
    this->doorOpen = true;
}

void Context::setDoorClosed()
{
    this->doorOpen = false;
}

void Context::setAlarm(bool active)
{
    this->alarmActive = active;
}

bool Context::isAlarmActive()
{
    return this->alarmActive;
}

void Context::setPreAlarm(bool active)
{
    this->preAlarmActive = active;
}

bool Context::isPreAlarmActive()
{
    return this->preAlarmActive;
}

void Context::blink()
{
    this->ledBlinking = true;
}

void Context::stopBlink()
{
    this->ledBlinking = false;
}

bool Context::isBlinking()
{
    return this->ledBlinking;
}