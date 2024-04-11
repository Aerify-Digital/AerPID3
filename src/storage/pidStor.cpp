#include "pidStor.h"

void PidStor::init(Flash *flash)
{
    PidStor::flash = flash;
}

// ==========================================================================
// ==========================================================================
// PID File

void PidStor::save_pid(double kP, double kI, double kD)
{
    const char *filename = "pid.dat";
    int leng = 30;
    char storageData[leng];

    union ByteDouble
    {
        byte array[8];
        double Num;
    };

    ByteDouble p;
    p.Num = kP;
    ByteDouble i;
    i.Num = kI;
    ByteDouble d;
    d.Num = kD;

    for (int j = 0; j < 30; j++)
    {
        if (j < 8)
        {
            storageData[j] = p.array[j];
        }
        else if (j >= 10 && j < 18)
        {
            storageData[j] = i.array[j - 10];
        }
        else if (j >= 20 && j < 28)
        {
            storageData[j] = d.array[j - 20];
        }
        else
        {
            storageData[j] = '\0';
        }
    }

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PidStor::load_pid(double &kP, double &kI, double &kD)
{
    const char *filename = "pid.dat";
    int leng = 30;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    union ByteDouble
    {
        byte array[8];
        double Num;
    };

    ByteDouble p;
    ByteDouble i;
    ByteDouble d;

    for (int j = 0; j < 30; j++)
    {
        if (j < 8)
        {
            p.array[j] = storageData[j];
        }
        else if (j >= 10 && j < 18)
        {
            i.array[j - 10] = storageData[j];
        }
        else if (j >= 20 && j < 28)
        {
            d.array[j - 20] = storageData[j];
        }
    }

    kP = p.Num;
    kI = i.Num;
    kD = d.Num;
}

// ==========================================================================
// ==========================================================================
// PID 2 File

void PidStor::save_pid_2(double kP, double kI, double kD)
{
    const char *filename = "pid2.dat";
    int leng = 30;
    char storageData[leng];

    union ByteDouble
    {
        byte array[8];
        double Num;
    };

    ByteDouble p;
    p.Num = kP;
    ByteDouble i;
    i.Num = kI;
    ByteDouble d;
    d.Num = kD;

    for (int j = 0; j < 30; j++)
    {
        if (j < 8)
        {
            storageData[j] = p.array[j];
        }
        else if (j >= 10 && j < 18)
        {
            storageData[j] = i.array[j - 10];
        }
        else if (j >= 20 && j < 28)
        {
            storageData[j] = d.array[j - 20];
        }
        else
        {
            storageData[j] = '\0';
        }
    }

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PidStor::load_pid_2(double &kP, double &kI, double &kD)
{
    const char *filename = "pid2.dat";
    int leng = 30;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    union ByteDouble
    {
        byte array[8];
        double Num;
    };

    ByteDouble p;
    ByteDouble i;
    ByteDouble d;

    for (int j = 0; j < 30; j++)
    {
        if (j < 8)
        {
            p.array[j] = storageData[j];
        }
        else if (j >= 10 && j < 18)
        {
            i.array[j - 10] = storageData[j];
        }
        else if (j >= 20 && j < 28)
        {
            d.array[j - 20] = storageData[j];
        }
    }

    kP = p.Num;
    kI = i.Num;
    kD = d.Num;
}

bool PidStor::pid_exists()
{
    const char *filename = "pid.dat";
    return flash->fileExists(filename);
}

bool PidStor::pid_2_exists()
{
    const char *filename = "pid2.dat";
    return flash->fileExists(filename);
}

void PidStor::destroy()
{
    const char *filename1 = "pid.dat";
    if (flash->fileExists(filename1))
    {
        flash->deleteFile(filename1);
    }
    const char *filename2 = "pid2.dat";
    if (flash->fileExists(filename2))
    {
        flash->deleteFile(filename2);
    }
}

PidStor pidStor;