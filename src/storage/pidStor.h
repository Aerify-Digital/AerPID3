#ifndef _PID_STOR_H
#define _PID_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

class PidStor
{
public:
    void init(Flash *flash);

    bool needSave() { return !_saved; }

    void load_pid(double &kP, double &kI, double &kD);
    void save_pid(double kP, double kI, double kD);
    bool pid_exists();

    void load_pid_2(double &kP, double &kI, double &kD);
    void save_pid_2(double kP, double kI, double kD);
    bool pid_2_exists();

    void destroy();

private:
    Flash *flash;

    bool _saved = false;
};

extern PidStor pidStor;

#endif