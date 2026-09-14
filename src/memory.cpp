#include "memory.h"
#include "linux.h"
#include "log.h"
#include "timer.h"

//---MemoryRegion
bool MemoryRegion::Readable() const
{
    return m_perms[0] == 'r';
}

//---Data Type
size_t SizeOf(DataType type)
{    
    switch (type) 
    {
        case DataType::Int8:
        case DataType::UInt8:
        case DataType::Boolean:
            return 1; // 1 byte

        case DataType::Int16:
        case DataType::UInt16:
            return 2; // 2 bytes

        case DataType::Int32:
        case DataType::UInt32:
        case DataType::Float:
            return 4; // 4 bytes

        case DataType::Int64:
        case DataType::UInt64:
        case DataType::Double:
            return 8; // 8 bytes

        default:
            return 0;
    }
}

//---Process
Process::Process(pid_t pid)
    :m_PID(pid), m_UserStructs()
{
    m_MemMap.reserve(100);
    GenerateMemoryMap();
    DetectPointerSize();


    Update();

    LOG_INFO("New Process spawned. PID: {}, PointerSize: DataType={}", m_PID, 
            static_cast<int>(m_PointerSize));
}

void Process::Update()
{
    GenerateMemoryMap();
}

void Process::GenerateMemoryMap()
{
    ParseProcMaps(m_PID,m_MemMap);
}

std::string Process::MemMapToString() const
{
    std::string outstr;
    for (const auto& region : m_MemMap)
    {
        outstr = outstr + region.toString() + '\n';
    }
    return outstr;
}

bool Process::IsAddressValid(uint64_t addr) const
{
    for(const auto& region : m_MemMap)
    {
        if(addr >= region.m_start && addr <= region.m_end)
        {
            if(!region.Readable())
            {
                LOG_WARN("Address is valid but not readable. {}", region.toString());
                return false;
            }
            else
                return true;
        }
    }

    return false;
}

void Process::DetectPointerSize()
{
    for(const MemoryRegion& region : m_MemMap)
    {
        //32 bit int max = 0xFFFFFFFF
        if(region.m_end > 0xFFFFFFFF || region.m_start > 0xFFFFFFFF)
        {
            m_PointerSize = DataType::Int64;
            return;
        }
    }

    m_PointerSize = DataType::Int32;
}

std::vector<UserStruct>& Process::GetUserStructs()
{
    return m_UserStructs;
}

UserStruct* Process::GetUserStruct(const std::string& structName)
{
    for(UserStruct& stc : m_UserStructs)
    {
        if(stc.GetName() == structName)
        {
            return &stc;
        }
    }

    return nullptr;
}

void Process::MakeUserStruct(const std::string& name, uint64_t startPos, int size)
{
    
    if(!IsAddressValid(startPos) || !IsAddressValid(startPos+size))
    {
        LOG_ERROR("0d{} - 0d{} is not a valid memory region.", startPos, startPos+size);
        return;
    }

    for(UserStruct stc : m_UserStructs)
    {
        if(stc.GetName() == name)
        {
            LOG_ERROR("Struct with name {} already exists.", name);
            return;
        }
    }

    m_UserStructs.emplace_back(*this, name, startPos);
    PRINT("Successfully create struct '{}'", name);
}

//---UserStruct

Process& UserStruct::GetOwningProcess()
{
    return m_OwningProc;
}

std::string UserStruct::GetName()
{
    return m_Name;
}
