/*!@file
@brief utils

2009 (C) All rights reserved.

@author
@date 2009.03.20
@version 1.0
*/

#pragma once

#include <string>
#include <sstream>
#include <exception>

using std::string;
using std::istringstream;
using std::ostringstream;

namespace Utility
{
    class ConvertException : public std::exception
    {
    public:
        virtual const char* what()
        {
            return "Opps, A convertion exception has occured!\nPlease check your input!\n";
        }
    };

    template <typename T>
        string ToString(T value)
    {
        ostringstream os;
        os.setf(std::ios::fixed, std::ios::floatfield);
        os.precision(1);
        os << value;
        if (!os)
        {
            throw new ConvertException();
        }
        return os.str();
    }

    template <typename T>
        T FromString(const string &src)
    {
        T val;
        istringstream is(src);
        //is.setf(0,std::ios::floatfield);
        is.precision(1);
        is >> val;
        if(!is)
        {
            throw new ConvertException();
        }
        return val;
    }

#ifdef _DEBUG
    void realdprintf(char const * file, int lineno, char const *fmt, ...);
#endif

}

#ifdef _DEBUG

#define RTSPDEBUG(...) Utility::realdprintf(__FILE__, __LINE__, __VA_ARGS__)

#else

#define RTSPDEBUG(...) ;

#endif

