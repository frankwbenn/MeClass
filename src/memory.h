#pragma once
#include <vector>
#include <cstdint>
#include <string>

//Storage for a line of data from /proc/pid/maps
struct MemoryRegion
{
    uintptr_t m_start;
    uintptr_t m_end;
    char m_perms[5]; //rwxp, read, write, execute, private
    std::string m_pathname;
    std::string toString() const;

    bool Readable() const;
};

enum class DataType
{   
    Int8=0, UInt8,
    Int16,  UInt16,
    Int32,  UInt32,
    Int64,  UInt64,
    Float,
    Double,
    Boolean
};

size_t SizeOf(DataType type);

class Process
{
private:
    pid_t m_PID;
    std::vector<MemoryRegion> m_MemMap;
    DataType m_PointerSize;
public:
    Process(pid_t pid);
    void Update();
    void GenerateMemoryMap();
    std::string MemMapToString() const;
    bool IsAddressValid(uintptr_t& addr) const;
    void DetectPointerSize();
};

//Represents a user defined / deduced variable
//TODO: Add sizeof function for data types enum.
class DataDef
{
private:
    std::string m_name;
    int m_offset;
    DataType m_type;
    bool m_isPointer = false;
    size_t m_sizeOf;
private:
    void Update();
public:
    DataDef(std::string& name, int offset, DataType type, bool isPointer)
        :m_name(name), m_offset(offset), m_type(type), m_isPointer(isPointer)
    {}

    //update function
    //sizeof variable (sets on update.)
};

//Represents a section of byte data along with all DataDefs for that byte data.
//I.e. the class / struct we are trying to view with the program.
class UserStruct
{
    Process m_OwningProc;
    std::string m_Name;
    std::vector<DataDef> m_MemberVars;
};

