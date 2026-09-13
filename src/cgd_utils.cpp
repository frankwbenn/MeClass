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

