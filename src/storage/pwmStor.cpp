#include "pwmStor.h"

void PwmStor::init(Flash *flash)
{
    PwmStor::flash = flash;
}

// ==========================================================================
// ==========================================================================
// PID PWM File

void PwmStor::save_pwm(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup, int pwmRes, int pidRes)
{
    const char *filename = "pwm1.dat";
    int leng = 45;
    char storageData[leng];

    ByteInt f;
    f.number = frequency;
    ByteDouble sf;
    sf.number = scaleFactor;
    ByteInt ct;
    ct.number = cycleTime;
    ByteDouble bi;
    bi.number = bias;
    ByteDouble wu;
    wu.number = windup;
    ByteInt pwm;
    pwm.number = pwmRes;
    ByteInt pid;
    pid.number = pidRes;

    storageData[0] = f.array[0];
    storageData[1] = f.array[1];
    storageData[2] = f.array[2];
    storageData[3] = f.array[3];

    storageData[4] = sf.array[0];
    storageData[5] = sf.array[1];
    storageData[6] = sf.array[2];
    storageData[7] = sf.array[3];
    storageData[8] = sf.array[4];
    storageData[9] = sf.array[5];
    storageData[10] = sf.array[6];
    storageData[11] = sf.array[7];

    storageData[12] = ct.array[0];
    storageData[13] = ct.array[1];
    storageData[14] = ct.array[2];
    storageData[15] = ct.array[3];

    storageData[18] = autoTune;

    storageData[20] = bi.array[0];
    storageData[21] = bi.array[1];
    storageData[22] = bi.array[2];
    storageData[23] = bi.array[3];
    storageData[24] = bi.array[4];
    storageData[25] = bi.array[5];
    storageData[26] = bi.array[6];
    storageData[27] = bi.array[7];

    storageData[28] = wu.array[0];
    storageData[29] = wu.array[1];
    storageData[30] = wu.array[2];
    storageData[31] = wu.array[3];
    storageData[32] = wu.array[4];
    storageData[33] = wu.array[5];
    storageData[34] = wu.array[6];
    storageData[35] = wu.array[7];

    storageData[36] = pwm.array[0];
    storageData[37] = pwm.array[1];
    storageData[38] = pwm.array[2];
    storageData[39] = pwm.array[3];

    storageData[40] = pid.array[0];
    storageData[41] = pid.array[1];
    storageData[42] = pid.array[2];
    storageData[43] = pid.array[3];

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup, int &pwmRes, int &pidRes)
{
    const char *filename = "pwm1.dat";
    int leng = 45;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    ByteInt f;
    ByteDouble sf;
    ByteInt ct;
    ByteDouble bi;
    ByteDouble wu;
    ByteInt pwm;
    ByteInt pid;

    f.array[0] = storageData[0];
    f.array[1] = storageData[1];
    f.array[2] = storageData[2];
    f.array[3] = storageData[3];
    frequency = f.number;

    sf.array[0] = storageData[4];
    sf.array[1] = storageData[5];
    sf.array[2] = storageData[6];
    sf.array[3] = storageData[7];
    sf.array[4] = storageData[8];
    sf.array[5] = storageData[9];
    sf.array[6] = storageData[10];
    sf.array[7] = storageData[11];
    scaleFactor = sf.number;

    ct.array[0] = storageData[12];
    ct.array[1] = storageData[13];
    ct.array[2] = storageData[14];
    ct.array[3] = storageData[15];
    cycleTime = ct.number;

    autoTune = storageData[18];

    bi.array[0] = storageData[20];
    bi.array[1] = storageData[21];
    bi.array[2] = storageData[22];
    bi.array[3] = storageData[23];
    bi.array[4] = storageData[24];
    bi.array[5] = storageData[25];
    bi.array[6] = storageData[26];
    bi.array[7] = storageData[27];
    bias = bi.number;

    wu.array[0] = storageData[28];
    wu.array[1] = storageData[29];
    wu.array[2] = storageData[30];
    wu.array[3] = storageData[31];
    wu.array[4] = storageData[32];
    wu.array[5] = storageData[33];
    wu.array[6] = storageData[34];
    wu.array[7] = storageData[35];
    windup = wu.number;

    pwm.array[0] = storageData[36];
    pwm.array[1] = storageData[37];
    pwm.array[2] = storageData[38];
    pwm.array[3] = storageData[39];
    pwmRes = pwm.number;

    pid.array[0] = storageData[40];
    pid.array[1] = storageData[41];
    pid.array[2] = storageData[42];
    pid.array[3] = storageData[43];
    pidRes = pid.number;
}

void PwmStor::save_pwm_2(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup, int pwmRes, int pidRes)
{
    const char *filename = "pwm2.dat";
    int leng = 45;
    char storageData[leng];

    ByteInt f;
    f.number = frequency;
    ByteDouble sf;
    sf.number = scaleFactor;
    ByteInt ct;
    ct.number = cycleTime;
    ByteDouble bi;
    bi.number = bias;
    ByteDouble wu;
    wu.number = windup;
    ByteInt pwm;
    pwm.number = pwmRes;
    ByteInt pid;
    pid.number = pidRes;

    storageData[0] = f.array[0];
    storageData[1] = f.array[1];
    storageData[2] = f.array[2];
    storageData[3] = f.array[3];

    storageData[4] = sf.array[0];
    storageData[5] = sf.array[1];
    storageData[6] = sf.array[2];
    storageData[7] = sf.array[3];
    storageData[8] = sf.array[4];
    storageData[9] = sf.array[5];
    storageData[10] = sf.array[6];
    storageData[11] = sf.array[7];

    storageData[12] = ct.array[0];
    storageData[13] = ct.array[1];
    storageData[14] = ct.array[2];
    storageData[15] = ct.array[3];

    storageData[18] = autoTune;

    storageData[20] = bi.array[0];
    storageData[21] = bi.array[1];
    storageData[22] = bi.array[2];
    storageData[23] = bi.array[3];
    storageData[24] = bi.array[4];
    storageData[25] = bi.array[5];
    storageData[26] = bi.array[6];
    storageData[27] = bi.array[7];

    storageData[28] = wu.array[0];
    storageData[29] = wu.array[1];
    storageData[30] = wu.array[2];
    storageData[31] = wu.array[3];
    storageData[32] = wu.array[4];
    storageData[33] = wu.array[5];
    storageData[34] = wu.array[6];
    storageData[35] = wu.array[7];

    storageData[36] = pwm.array[0];
    storageData[37] = pwm.array[1];
    storageData[38] = pwm.array[2];
    storageData[39] = pwm.array[3];

    storageData[40] = pid.array[0];
    storageData[41] = pid.array[1];
    storageData[42] = pid.array[2];
    storageData[43] = pid.array[3];

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm_2(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup, int &pwmRes, int &pidRes)
{
    const char *filename = "pwm2.dat";
    int leng = 45;
    char storageData[leng];

    flash->openFile(filename, storageData, leng);

    ByteInt f;
    ByteDouble sf;
    ByteInt ct;
    ByteDouble bi;
    ByteDouble wu;
    ByteInt pwm;
    ByteInt pid;

    f.array[0] = storageData[0];
    f.array[1] = storageData[1];
    f.array[2] = storageData[2];
    f.array[3] = storageData[3];
    frequency = f.number;

    sf.array[0] = storageData[4];
    sf.array[1] = storageData[5];
    sf.array[2] = storageData[6];
    sf.array[3] = storageData[7];
    sf.array[4] = storageData[8];
    sf.array[5] = storageData[9];
    sf.array[6] = storageData[10];
    sf.array[7] = storageData[11];
    scaleFactor = sf.number;

    ct.array[0] = storageData[12];
    ct.array[1] = storageData[13];
    ct.array[2] = storageData[14];
    ct.array[3] = storageData[15];
    cycleTime = ct.number;

    autoTune = storageData[18];

    bi.array[0] = storageData[20];
    bi.array[1] = storageData[21];
    bi.array[2] = storageData[22];
    bi.array[3] = storageData[23];
    bi.array[4] = storageData[24];
    bi.array[5] = storageData[25];
    bi.array[6] = storageData[26];
    bi.array[7] = storageData[27];
    bias = bi.number;

    wu.array[0] = storageData[28];
    wu.array[1] = storageData[29];
    wu.array[2] = storageData[30];
    wu.array[3] = storageData[31];
    wu.array[4] = storageData[32];
    wu.array[5] = storageData[33];
    wu.array[6] = storageData[34];
    wu.array[7] = storageData[35];
    windup = wu.number;

    pwm.array[0] = storageData[36];
    pwm.array[1] = storageData[37];
    pwm.array[2] = storageData[38];
    pwm.array[3] = storageData[39];
    pwmRes = pwm.number;

    pid.array[0] = storageData[40];
    pid.array[1] = storageData[41];
    pid.array[2] = storageData[42];
    pid.array[3] = storageData[43];
    pidRes = pid.number;
}

bool PwmStor::pwm_exists()
{
    const char *filename = "pwm1.dat";
    return flash->fileExists(filename);
}

bool PwmStor::pwm_2_exists()
{
    const char *filename = "pwm2.dat";
    return flash->fileExists(filename);
}

void PwmStor::destroy()
{
    const char *filename1 = "pwm1.dat";
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