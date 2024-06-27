#include "pwmStor.h"

void PwmStor::init(Flash *flash)
{
    PwmStor::flash = flash;
}

// ==========================================================================
// ==========================================================================
// PID PWM File

void PwmStor::save_pwm(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup)
{
    const char *filename = "pwm.dat";
    int leng = 40;
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
    ByteDouble bi;
    bi.Num = bias;
    ByteDouble wu;
    wu.Num = windup;

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

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup)
{
    const char *filename = "pwm.dat";
    int leng = 40;
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
    ByteDouble bi;
    ByteDouble wu;

    f.array[0] = storageData[0];
    f.array[1] = storageData[1];
    f.array[2] = storageData[2];
    f.array[3] = storageData[3];
    frequency = f.Num;

    sf.array[0] = storageData[4];
    sf.array[1] = storageData[5];
    sf.array[2] = storageData[6];
    sf.array[3] = storageData[7];
    sf.array[4] = storageData[8];
    sf.array[5] = storageData[9];
    sf.array[6] = storageData[10];
    sf.array[7] = storageData[11];
    scaleFactor = sf.Num;

    ct.array[0] = storageData[12];
    ct.array[1] = storageData[13];
    ct.array[2] = storageData[14];
    ct.array[3] = storageData[15];
    cycleTime = ct.Num;

    autoTune = storageData[18];

    bi.array[0] = storageData[20];
    bi.array[1] = storageData[21];
    bi.array[2] = storageData[22];
    bi.array[3] = storageData[23];
    bi.array[4] = storageData[24];
    bi.array[5] = storageData[25];
    bi.array[6] = storageData[26];
    bi.array[7] = storageData[27];
    bias = bi.Num;

    wu.array[0] = storageData[28];
    wu.array[1] = storageData[29];
    wu.array[2] = storageData[30];
    wu.array[3] = storageData[31];
    wu.array[4] = storageData[32];
    wu.array[5] = storageData[33];
    wu.array[6] = storageData[34];
    wu.array[7] = storageData[35];
    windup = wu.Num;
}

void PwmStor::save_pwm_2(int frequency, double scaleFactor, int cycleTime, boolean autoTune, double bias, double windup)
{
    const char *filename = "pwm2.dat";
    int leng = 40;
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
    ByteDouble bi;
    bi.Num = bias;
    ByteDouble wu;
    wu.Num = windup;

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

    flash->saveFile(filename, storageData, leng);

    this->_saved = true;
}

void PwmStor::load_pwm_2(int &frequency, double &scaleFactor, int &cycleTime, boolean &autoTune, double &bias, double &windup)
{
    const char *filename = "pwm2.dat";
    int leng = 40;
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
    ByteDouble bi;
    ByteDouble wu;

    f.array[0] = storageData[0];
    f.array[1] = storageData[1];
    f.array[2] = storageData[2];
    f.array[3] = storageData[3];
    frequency = f.Num;

    sf.array[0] = storageData[4];
    sf.array[1] = storageData[5];
    sf.array[2] = storageData[6];
    sf.array[3] = storageData[7];
    sf.array[4] = storageData[8];
    sf.array[5] = storageData[9];
    sf.array[6] = storageData[10];
    sf.array[7] = storageData[11];
    scaleFactor = sf.Num;

    ct.array[0] = storageData[12];
    ct.array[1] = storageData[13];
    ct.array[2] = storageData[14];
    ct.array[3] = storageData[15];
    cycleTime = ct.Num;

    autoTune = storageData[18];

    bi.array[0] = storageData[20];
    bi.array[1] = storageData[21];
    bi.array[2] = storageData[22];
    bi.array[3] = storageData[23];
    bi.array[4] = storageData[24];
    bi.array[5] = storageData[25];
    bi.array[6] = storageData[26];
    bi.array[7] = storageData[27];
    bias = bi.Num;

    wu.array[0] = storageData[28];
    wu.array[1] = storageData[29];
    wu.array[2] = storageData[30];
    wu.array[3] = storageData[31];
    wu.array[4] = storageData[32];
    wu.array[5] = storageData[33];
    wu.array[6] = storageData[34];
    wu.array[7] = storageData[35];
    windup = wu.Num;
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