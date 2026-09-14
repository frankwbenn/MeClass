#include "cgd_utils.h"

const char* StripPath(const char* path)
{
    const char* file = path;
    while(*path != 0)
    {
        if (*path == '/' || *path == '\\')
            file = path + 1;
        path++;
    }
    return file;
}

int DetermineStringBasePrefix(const std::string& str)
{
    //Default is 10.
    if(str.length() >= 2)
    {
        switch (str[1])
        {
            case 'x':
            case 'X':
                return 16;
            case 'b':
            case 'B':
                return 2;
            case 'o':
            case 'O':
                return 8;
        }
    }

    return 10;
}
