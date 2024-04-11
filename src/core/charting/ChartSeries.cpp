#include "ChartSeries.h"

bool ChartSeries::open()
{
    const uint hdrLen = 32;
    const int len = sLength + hdrLen;
    // const char *filename = (std::string("chart_").append(this->name).append(".dat")).c_str();
    String fn = "chart_";
    fn.concat(this->name.c_str());
    fn.concat(".dat");
    const char *filename = fn.c_str();

    char header[hdrLen];
    char data[len];

    uint crcIndex = 28;
    uint crcSplit = sLength / 4;

    // fill buffer with padding
    /*for (uint i = 0; i < len; i++)
    {
        data[i] = 0;
    }*/

    if (!flash->openFile(filename, (char *)data, len))
    {
        Serial.println("ChartSeries File not found!");
        return false;
    }

    // parse header into buffer...
    for (int i = 0; i < hdrLen; i++)
    {
        header[i] = data[i];
    }

    uint version = 0;
    uint entryCount = 0;
    uint currentIndex = 0;

    uint h = 0;
    cEntryInt32 hdr0;
    cEntryInt32 hdr1;
    cEntryInt32 hdr2;

    // version
    for (uint i = 0; i < sizeof(hdr0.hex); i++)
    {
        hdr0.hex[i] = header[h++];
    }
    version = hdr0.num;
    // entry count
    for (uint i = 0; i < sizeof(hdr1.hex); i++)
    {
        hdr1.hex[i] = header[h++];
    }
    entryCount = hdr1.num;
    // currnet index
    for (uint i = 0; i < sizeof(hdr2.hex); i++)
    {
        hdr2.hex[i] = header[h++];
    }
    currentIndex = hdr2.num;

    h += 2;
    crcIndex = h;
    uint8_t crc1 = header[h++]; // crc 1
    uint8_t crc2 = header[h++]; // crc 2
    uint8_t crc3 = header[h++]; // crc 3
    uint8_t crc4 = header[h++]; // crc 4

    // Serial.println("Loading File CRC");
    // Serial.print("crcIndex: ");
    // Serial.println(crcIndex);

    uint8_t crcs[4];
    // crcs[0] = CRC::crc_bytes((const uint8_t *)data, hdrLen, sLength);
    //  Serial.print("CRC: ");
    //  Serial.println(crcs[0], HEX);

    /*for (int i = 0; i < 4; i++)
    {
        // crcs[i] = CRC::crc_bytes((const uint8_t *)data, hdrLen + crcSplit * i, hdrLen + crcSplit * (i + 1));
        crcs[i] = CRC::crc_bytes((const uint8_t *)data, hdrLen, sLength);

        Serial.print(crcs[i], HEX);
        Serial.print(" ");
    }*/
    // Serial.println();

    /*if (crc1 != crcs[0])
    {
        Serial.print(crc1, HEX);
        Serial.print(F(" != "));
        Serial.print(crcs[0], HEX);
        Serial.println();

        Serial.println("CRC data Mismatch on file open!");
        return false;
    }*/

    // Serial.println("Read Chart ");
    //  read data
    for (int i = hdrLen; i < len; i++)
    {
        eChartType t = (eChartType)data[i];
        if (t == 0xff || t == 0)
        {
            continue;
        }
        uint8_t s = ChartEntry::size(t);
        if (s == 0)
        {
            continue;
        }
        uint8_t b = 0;
        uint8_t buf[s];

        // Serial.print("type: ");
        // Serial.print(t);
        // Serial.print(" ");

        // Serial.print("size: ");
        // Serial.print(s);
        // Serial.print(" ");
        for (uint8_t t = ++i; t < i + s; t++)
        {
            buf[b] = data[t];
            // Serial.print(buf[b], HEX);
            // Serial.print(" ");
            b++;
        }
        // Serial.print(" ");
        ChartEntry e = ChartEntry(entries.size(), std::string(""), t, buf);
        entries.push_back(e);
        // Serial.print(e.vDouble(), 8);
        // Serial.println(" ---");
    }

    return true;
}

bool ChartSeries::close()
{
    const uint hdrLen = 32;
    const int len = sLength + hdrLen;
    // const char *filename = std::string("chart_").append(this->name).append(".dat").c_str();
    String fn = "chart_";
    fn.concat(this->name.c_str());
    fn.concat(".dat");
    const char *filename = fn.c_str();

    char header[hdrLen];
    char data[len];

    uint crcIndex = 28;
    uint crcSplit = sLength / 4;

    // fill buffer with padding
    for (uint i = 0; i < len; i++)
    {
        data[i] = 0;
    }

    // ------------------------------------------
    // file header data...

    uint version = 1;
    uint entryCount = entries.size();
    uint currentIndex = 0;

    uint h = 0;
    cEntryInt32 hdr0;
    hdr0.num = version;
    cEntryInt32 hdr1;
    hdr1.num = entryCount;
    cEntryInt32 hdr2;
    hdr2.num = currentIndex;

    // version
    for (uint i = 0; i < sizeof(hdr0.hex); i++)
    {
        header[h++] = hdr0.hex[i];
    }
    // entry count
    for (uint i = 0; i < sizeof(hdr1.hex); i++)
    {
        header[h++] = hdr1.hex[i];
    }
    // currnet index
    for (uint i = 0; i < sizeof(hdr2.hex); i++)
    {
        header[h++] = hdr2.hex[i];
    }
    header[h++] = 0x01; // padding
    header[h++] = 0x01; // padding
    crcIndex = h;
    header[h++] = 0xff; // crc 1
    header[h++] = 0xff; // crc 2
    header[h++] = 0xff; // crc 3
    header[h++] = 0xff; // crc 4

    // fill rest of header with padding
    for (uint i = h; i < hdrLen; i++)
    {
        header[i] = 0;
    }

    // ------------------------------------------
    // first 32 bytes of data file are header
    for (uint i = 0; i < hdrLen; i++)
    {
        data[i] = header[i];
    }

    // ------------------------------------------
    uint t = hdrLen;
    // iterate over data entries of chart series
    for (uint i = 0; i < entryCount; i++)
    {
        ChartEntry e = entries[i];
        if (!e.isBuilt())
        {
            continue;
        }
        // break in case data becomes longer than file...
        if (t >= len - 1 || t + e.size() + 1 > len - 1)
        {
            break;
        }
        byte *hex = e.toHexVal();
        // push entry type into data store
        data[t++] = e.eType();
        // Serial.print("t=");
        // Serial.print((uint8_t)e.eType());
        // Serial.print(" ");
        //  iterate over hex of value and push into data
        for (uint j = 0; j < e.size(); j++)
        {
            data[t++] = hex[j];
            // Serial.print(hex[j], HEX);
            // Serial.print(" ");
        }
        // Serial.println(" ---");
    }
    data[t++] = '\0';
    for (int i = t; i < len; i++)
    {
        data[i] = 0;
    }

    // Serial.println("Saving File CRC");
    // Serial.print("crcIndex: ");
    // Serial.println(crcIndex);
    //  int p = 0;

    data[crcIndex] = CRC::crc_bytes((const uint8_t *)data, hdrLen, sLength);
    // Serial.print("CRC: ");
    // Serial.println(data[crcIndex], HEX);

    /*for (int i = crcIndex; i < crcIndex + 4; i++)
    {
        // data[i] = CRC::crc_bytes((const uint8_t *)data, hdrLen + crcSplit * p, hdrLen + crcSplit * (p + 1));
        data[i] = CRC::crc_bytes((const uint8_t *)data, hdrLen, sLength);

        Serial.print(data[i], HEX);
        Serial.print(" ");
        p++;
    }*/
    // Serial.println();

    // Serial.println("Saving File...");
    flash->saveFile(filename, (char *)data, len);

    return true;
}

void ChartSeries::addEntry(double v)
{
    ChartEntry e = ChartEntry(entries.size(), std::string(""), v);
    if (e.isBuilt())
    {
        entries.push_back(e);
    }
}
