#ifndef _PWM_STOR_H
#define _PWM_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

class PwmStor
{
public:
    void init(Flash *flash);

    bool needSave() { return !_saved; }

    void load_pwm(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup, int &pwmRes, int &pidRes);
    void save_pwm(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup, int pwmRes, int pidRes);
    bool pwm_exists();

    void load_pwm_2(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup, int &pwmRes, int &pidRes);
    void save_pwm_2(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup, int pwmRes, int pidRes);
    bool pwm_2_exists();

    void destroy();

private:
    Flash *flash;

    bool _saved = false;

    union ByteInt
    {
        byte array[sizeof(int)];
        int number;
    };
    union ByteDouble
    {
        byte array[sizeof(double)];
        double number;
    };

};

extern PwmStor pwmStor;

#endif