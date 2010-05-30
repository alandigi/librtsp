

#ifdef _DEBUG

#include "util.h"
#include <cstring>
#include <cstdarg>

void Utility::realdprintf( char const * file, int lineno, char const *fmt, ... )
{
    char buf[256];
    ::memset(buf, 0, sizeof(char) * 256);
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buf, 256, fmt, vl);
    va_end(vl);
    std::string filepath(file);
    std::string filename = filepath.substr(filepath.rfind('/')+1,filepath.length());
    fprintf(stdout, "%s (%s,%d)\n", buf, filename.c_str(), lineno);
}
#endif
