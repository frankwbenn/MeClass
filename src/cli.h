#pragma once
#include "memory.h"
#include <functional>
#include <memory>
#include <unordered_map>

class CLIController
{
private:
    std::shared_ptr<Process> m_Proc;

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
};


