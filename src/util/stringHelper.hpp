#ifndef _AER_STRING_HELPER_H_
#define _AER_STRING_HELPER_H_

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

template <typename T>
static std::string to_string(T const &value)
{
    std::ostringstream sstr;
    sstr << value;
    return sstr.str();
}

static std::string _toFixed(std::string str, uint precision)
{
    int len = strlen(str.c_str());
    if (len == 0)
    {
        return str;
    }
    std::string delimiter = ".";
    size_t del_pos = str.find(delimiter);
    if (precision == 0)
    {
        if (del_pos >= 255)
        {
            del_pos = len;
        }
        return str.substr(0, del_pos);
    }

    if (del_pos >= 255)
    {
        str = str + ".";
        del_pos = len;
    }
    std::string dec = str.substr(del_pos);
    uint dlen = strlen(dec.c_str());

    if (precision == 1)
    {
        if (dlen == 1)
        {
            str = str + "0";
        }
        else if (dlen == 2)
        {
            str = str + "";
        }
        else if (dlen >= 3)
        {
            str = str.substr(0, del_pos + 2);
        }
    }
    if (precision == 2)
    {
        if (dlen == 1)
        {
            str = str + "00";
        }
        else if (dlen == 2)
        {
            str = str + "0";
        }
        else if (dlen >= 3)
        {
            str = str.substr(0, del_pos + 3);
        }
    }
    return str;
}

static std::string paddedStr(std::string str, uint maxlen, bool paddFront = false)
{
    int len = strlen(str.c_str());
    if (len == maxlen)
    {
        str = str;
    }
    else if (len > maxlen)
    {
        str = str.substr(0, maxlen);
    }
    else if (len < maxlen)
    {
        for (int i = len; i < maxlen; i++)
        {
            if (paddFront)
            {
                str = " " + str;
            }
            else
            {
                str = str + " ";
            }
        }
    }
    return str;
}

#endif