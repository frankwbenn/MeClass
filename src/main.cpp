#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "linux.h"
#include "log.h"
#include "memory.h"
#include "cgd_utils.h"
#include "timer.h"
#include "cli.h"
#include "management.h"

bool PrintBytes(pid_t pid, uintptr_t address, size_t size)
{
    std::cout << "PrintBytes" << std::endl;
    unsigned char* readBuffer = new unsigned char[size];
    if(ReadProcMem(pid,address,readBuffer,size))
    {
       for(size_t i = 0; i < size; i++)
        {
            std::printf("0x%02X ", readBuffer[i]);
            //std::cout   << "0x" 
            //            << std::hex             // Switch to hexadecimal output
            //            << std::uppercase       // Use uppercase letters
            //            << std::setw(2)         // Ensure every byte takes up 2 characters
            //            << std::setfill('0')    // Pad single digits with leading 0
            //            << static_cast<unsigned int>(readBuffer[i]) 
            //            << " ";
        }
        std::cout << std::endl;
        return 1;

    }
    else
        return 0;
}

class d_CLIController
{
private:
    pid_t m_ProcessPID;
    uintptr_t m_MemAddress;
    size_t m_Bytes;
    Process* m_Proc;
public:
    d_CLIController()
        :m_ProcessPID(0), m_MemAddress(0), m_Bytes(0), m_Proc(nullptr)
    {    
        PRINT("MeClass Started");
        if(!m_ProcessPID)
        {
            PromptForPID();
        }
    }
private:
    void PromptForPID()
    {
        pid_t targetPID = 0;
        while(!m_ProcessPID)
        {
            PRINT("Enter target Process ID (PID): ");
            std::cin >> targetPID;

            if(!targetPID)
                std::cerr << "Invalid input.\n";
            else if (DoesProcExist(targetPID))
            {
                PRINT("[+] Success! Process {} is active.",targetPID);
                m_ProcessPID = targetPID;
                m_Proc = new Process(m_ProcessPID);
                LOG_INFO("{}",m_Proc->MemMapToString());
            }
            else
                PRINT("[-] Error: Process {} not found or invalid.", targetPID);
        }
        PromptForMemoryAddress();
    }

    void PromptForMemoryAddress()
    {
        m_MemAddress = 0;
        while(m_MemAddress == 0)
        {
            PRINT("Enter memory address: ");
            std::string input;
            std::cin >> input;
            m_MemAddress = std::stoull(input,nullptr,16);
            if(m_Proc->IsAddressValid(m_MemAddress))
                PromptForBytes();
            else
            {
                m_MemAddress = 0;
                PRINT("Invalid memory address.");
            }
        }

    }

    void PromptForBytes()
    {
        m_Bytes = 0;
        while(m_Bytes <= 0)
        {
            PRINT("Enter number of bytes: ");
            std::cin >> m_Bytes;
            if(m_Bytes <= 0)
                PRINT("Invalid number of bytes.");
        }
        PrintBytes(m_ProcessPID, m_MemAddress, m_Bytes);
        PromptForMemoryAddress();
    }

};

class DummyParent
{
private:
    int health;
    double mana;
public:
    DummyParent()
        :health(100), mana(250) {}

    virtual double GetStats() const
    {
        return health + mana;
    }
};

class DummyClass : DummyParent
{
    struct Vec3 {double x,y,z;};
private:
    Vec3 Pos;
    std::string name;
    int* intPointer;
    void* falsePointer;
    int terminator;
public:
    DummyClass()
        :Pos(11345.23553, 2130.1235, 115), name("MyNameString")
    {
        terminator = -1;
        intPointer = new int[16];
        for(int i = 0; i < 16; i++)
            intPointer[i] = i;

        falsePointer = reinterpret_cast<void*>(1234567);
        const void* thisPtr = this;

        LOG_INFO("Dummy Class: {}", thisPtr);
        LOG_INFO("Int[] Member Pointer: {}", (void*)intPointer);
        LOG_INFO("False Member Pointer: {}", falsePointer);
    }

    void Tick() 
    {
        Pos.x += 1.0;
        Pos.y -= 1.0;
        Pos.z += 0.001;
    }
};

int main()
{
    Log::Get().SetLevel(Log::e_LogLevel::Info);

    DummyClass Dummy;
    std::system("ps aux | grep MeClass"); 
    

    //ObjectPool objs;
    CLIController cli; 
}

//Next:
//Make functions to show bytes in different forms?
//Check byte patterns for any pointers in these valid locations (ranges).
//Also probably want auto-char conversion
//Auto resolve simple pointers like strings. I.e. go to pointer, 
//if it's obviously a string / char ptr / int then show that.
