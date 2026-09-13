//Any thing with a linux system call should go here.
#include "linux.h"
#include <signal.h>
#include <fstream>
#include "timer.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <cstring>

bool DoesProcExist(pid_t pid)
{
    // kill with signal 0 does error checking without harming the process
    if (kill(pid, 0) == 0) {
        return true;
    }

    if (errno == EPERM) {
        //The process exists, but we don't have permission to access it  
        //LOG_INFO("Process {} exists, but permission is denied.", pid);
        return true;
    }

    //If errno is ESRCH, the process does not exist
    //LOG_INFO("Process {} does not exist.", pid);
    return false;
}

bool ReadProcMem(pid_t pid, uintptr_t address, void* buffer, size_t size)
{
    iovec local_iov;
    local_iov.iov_base = buffer;
    local_iov.iov_len = size;

    iovec remote_iov;
    remote_iov.iov_base = reinterpret_cast<void*>(address);
    remote_iov.iov_len = size;

    ssize_t nread = process_vm_readv(pid, &local_iov, 1, &remote_iov,1,0);
    if(nread == -1)
    {
        int errorCode = errno;
        std::string errorMsg = strerror(errorCode);
        LOG_ERROR("process_vm_readv failed: {}",errorMsg);
    }
    return nread == static_cast<ssize_t>(size);
}

std::string MemoryRegion::toString() const
{
    std::string out = std::format("0x{:x}-0x{:x} {}", m_start, m_end, m_perms);
    if(!m_pathname.empty())
        out += " (" + m_pathname + ")";

    return out;
}

bool ParseProcMapsLine(const std::string& line, MemoryRegion& region)
{
    //example line: "55ea11fd0000-55ea11fd4000 r--p 00000000 fd:00 20717812 /path/to/file"
    const char* ptr = line.data();
    const char* end_ptr = ptr + line.size();

    //Parse start address up to "-"
    region.m_start = 0;
    while(ptr < end_ptr && *ptr != '-')
    {
        char c = *ptr++;
        ParseHexCharToDecVal(c, region.m_start);
    }

    if (ptr >= end_ptr)
        return false;
    ptr++; //skip '-'
    
    //Parse end address of to first space
    region.m_end = 0;
    while(ptr < end_ptr && *ptr != ' ')
    {
        char c = *ptr++;
        ParseHexCharToDecVal(c, region.m_end);
    }

    while(ptr < end_ptr && *ptr == ' ') ptr++; //skip spaces
    
    //Parse permissions (rwxp)
    for(int i = 0; i < 4 && ptr < end_ptr; i++)
    {
        region.m_perms[i] = *ptr++;
    }
    region.m_perms[4] = 0;

    //Ignore the next 3 columns (offset, device, and inode)
    for(int token = 0; token < 4; token++)
    {
        while(ptr < end_ptr && *ptr != ' ') ptr++;
        while(ptr < end_ptr && *ptr == ' ') ptr++;
    }

    //The remainder is the pathname (can be blank)
    if(ptr < end_ptr)
    {
        region.m_pathname.assign(ptr, end_ptr - ptr);
    }
    else
    {
        region.m_pathname.clear();
    }

    return true;                             
}

bool ParseProcMaps(pid_t pid, std::vector<MemoryRegion>& MemRegions)
{
    PROFILE_SCOPE();
    std::string path = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream file(path);
    if(!file.is_open())
    {
        LOG_ERROR("Could not open file: {}", path);
        return false;
    }

    std::string line;
    while(std::getline(file,line))
    {
        MemoryRegion region;
        if(ParseProcMapsLine(line, region))
            MemRegions.push_back(region);
        else
        {
            LOG_ERROR("Could not parse /proc/ maps line: {}", line);
            return false;
        }
    }

    return true;
}
