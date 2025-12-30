#ifndef __PRESENCE__
#define __PRESENCE__

/**
 * @brief Abstract base class for presence sensors.
 *
 */
class PresenceSensor
{
   public:
    /**
     * Check whether presence is detected.
     *
     * @return true if presence is detected, false otherwise
     */
    virtual bool isDetected() = 0;
};

#endif
