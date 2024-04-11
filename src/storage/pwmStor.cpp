#include "pwmStor.h"

void PwmStor::init(Flash *flash)
{
    PwmStor::flash = flash;
}

// ==========================================================================
// ==========================================================================
// PID PWM File

void PwmStor::save_pwm(int frequency, double scaleFactor, int cycleTime, boolean autoTune)
{
    const char *filename = "pwm.dat";
    int leng = 20;
    char storageData[leng];

    union ByteInt
    {
        byte array[sizeof(int)];
        double Num;
    };
    union ByteDouble
    {
        byte array[sizeof(double)];
        double Num;
    };

    ByteInt f;
    f.Num = frequency;
    ByteDouble sf;
    sf.Num = scaleFactor;
    ByteInt ct;
    ct.Num = cycleTime;

    for (int j = 0; j < 20; j++)
    {
        if (j < 4)
        {
            storageData[j] = f.array[j];
        }
        else if (j >= 4 && j < 14)
        {
            storageData[j] = sf.array[j - 4];
        }
        else if (j >= 14 && j < 19)
        {
            storageData[j] = ct.array[j - (8 + 4)];
        }
        else
        {
            storageData[j] = '\0';
        }
    }
    storageData[19] = autoTune;

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune)
{
    const char *filename = "pwm.dat";
    int leng = 20;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    union ByteInt
    {
        byte array[sizeof(int)];
        double Num;
    };
    union ByteDouble
    {
        byte array[sizeof(double)];
        double Num;
    };

    ByteInt f;
    ByteDouble sf;
    ByteInt ct;

    for (int j = 0; j < 20; j++)
    {
        if (j < 8)
        {
            f.array[j] = storageData[j];
        }
        else if (j >= 4 && j < 14)
        {
            sf.array[j - 4] = storageData[j];
        }
        else if (j >= 14 && j < 19)
        {
            ct.array[j - (8 + 4)] = storageData[j];
        }
    }

    autoTune = storageData[19];
    frequency = f.Num;
    scaleFactor = sf.Num;
    cycleTime = ct.Num;
}

void PwmStor::save_pwm_2(int frequency, double scaleFactor, int cycleTime, boolean autoTune)
{
    const char *filename = "pwm2.dat";
    int leng = 20;
    char storageData[leng];

    union ByteInt
    {
        byte array[sizeof(int)];
        double Num;
    };
    union ByteDouble
    {
        byte array[sizeof(double)];
        double Num;
    };

    ByteInt f;
    f.Num = frequency;
    ByteDouble sf;
    sf.Num = scaleFactor;
    ByteInt ct;
    ct.Num = cycleTime;

    for (int j = 0; j < 20; j++)
    {
        if (j < 4)
        {
            storageData[j] = f.array[j];
        }
        else if (j >= 4 && j < 14)
        {
            storageData[j] = sf.array[j - 4];
        }
        else if (j >= 14 && j < 19)
        {
            storageData[j] = ct.array[j - (8 + 4)];
        }
        else
        {
            storageData[j] = '\0';
        }
    }
    storageData[19] = autoTune;

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm_2(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune)
{
    const char *filename = "pwm2.dat";
    int leng = 20;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    union ByteInt
    {
        byte array[sizeof(int)];
        double Num;
    };
    union ByteDouble
    {
        byte array[sizeof(double)];
        double Num;
    };

    ByteInt f;
    ByteDouble sf;
    ByteInt ct;

    for (int j = 0; j < 20; j++)
    {
        if (j < 8)
        {
            f.array[j] = storageData[j];
        }
        else if (j >= 4 && j < 14)
        {
            sf.array[j - 4] = storageData[j];
        }
        else if (j >= 14 && j < 19)
        {
            ct.array[j - (8 + 4)] = storageData[j];
        }
    }

    autoTune = storageData[19];
    frequency = f.Num;
    scaleFactor = sf.Num;
    cycleTime = ct.Num;
}

bool PwmStor::pwm_exists()
{
    const char *filename = "pwm.dat";
    return flash->fileExists(filename);
}

bool PwmStor::pwm_2_exists()
{
    const char *filename = "pwm2.dat";
    return flash->fileExists(filename);
}

void PwmStor::destroy()
{
    const char *filename1 = "pwm.dat";
    if (flash->fileExists(filename1))
    {
        flash->deleteFile(filename1);
    }
    const char *filename2 = "pwm2.dat";
    if (flash->fileExists(filename2))
    {
        flash->deleteFile(filename2);
    }
}

PwmStor pwmStor;