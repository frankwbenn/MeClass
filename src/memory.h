#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

class UserStruct;

//Storage for a line of data from /proc/pid/maps
struct MemoryRegion
{
    uint64_t m_start;
    uint64_t m_end;
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
    std::vector<UserStruct> m_UserStructs;
public:
    Process(pid_t pid);
    void Update();
    void GenerateMemoryMap();
    std::string MemMapToString() const;
    bool IsAddressValid(uint64_t addr) const;
    void DetectPointerSize();
    std::vector<UserStruct>& GetUserStructs();
    UserStruct* GetUserStruct(const std::string& structName);
    void MakeUserStruct(const std::string& name, uint64_t startPos, int size);
};

//Represents a single user defined / program deduced variable
class DataDef
{
private:
    std::string m_Name;
    int m_Offset;
    DataType m_Type;
    bool m_isPointer = false;
    int m_Size;
private:
    void Update();
public:
    DataDef(std::string& name, int offset, DataType type, bool isPointer)
        :m_Name(name), m_Offset(offset), m_Type(type), m_isPointer(isPointer)
    {}

    //update function
    //sizeof variable (sets on update.)
};

//Represents a section of byte data along with all DataDefs for that byte data.
//I.e. a class / struct we are trying to view with the program.
//TODO: Add serialization functionality so that struct patterns can be saved.
//TODO: Add ability to copy struct pattern to use in new memory locations.
class UserStruct
{
private:
    Process& m_OwningProc;
    std::string m_Name;
    std::vector<DataDef> m_MemberVars;
    uint64_t m_StartPos;
    int m_Size;
public:
    UserStruct(Process& proc, std::string name, uint64_t startPos)
        :m_OwningProc(proc), m_Name(name), m_StartPos(startPos)
    {}

    Process& GetOwningProcess();
    std::string GetName();
    void MakeDataDef(const std::string& name, DataType type, int offset, bool isPointer);
};

