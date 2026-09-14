#pragma once
#include "memory.h"
#include <functional>
#include <memory>
#include <unordered_map>

//The CLI Controller is for running in the command line.
//It is not really designed for release and is mostly so I can easily
//run and test features.
class CLIController
{
private:
    std::unique_ptr<Process> m_Proc;

    using CMDptr = std::function<void(const std::vector<std::string>&)>;
    std::unordered_map<std::string, CMDptr> m_CommandMap;
public:
    CLIController();
private:
    void Run();
    bool HandleUserInput();
    std::vector<std::string> ParseInputs(std::string& str);
    std::string GetNextToken(std::string& str);
    void RegisterCommands();
    bool CheckCMDArgLen(const std::vector<std::string>& args,
        const unsigned long& correctArgLength,const std::string& CMDname);
    void ExecuteCommand(const std::string& name, const std::vector<std::string>& args);
    //-----Command functions
    void CMDproc(const std::vector<std::string>& args);
    void CMDstruct(const std::vector<std::string>& args);
    void CMDpstruct(const std::vector<std::string>& args);
};


