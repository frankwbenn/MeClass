#include <string>
#include "cli.h"
#include "linux.h"
#include "log.h"
#include "cgd_utils.h"

CLIController::CLIController()
    :m_Proc(nullptr)
{    
    PRINT("MeClass CLI started");
    RegisterCommands();
    Run();
}

void CLIController::Run()
{
    bool run = true;
    while(run)
    {
        run = HandleUserInput();
    }
}

bool CLIController::HandleUserInput()
{
    std::string input;
    int returnVal = true;

    std::cout << "(MeClass) " << std::flush;
    std::getline(std::cin, input);

    std::vector<std::string> inputs = ParseInputs(input);
    if(!inputs.empty())
    {
        std::string cmdName = inputs[0];
        cmdName == "exit" ? returnVal = false: returnVal = true;
        inputs.erase(inputs.begin());
        ExecuteCommand(cmdName,inputs);
    }

    return returnVal;
}

std::vector<std::string> CLIController::ParseInputs(std::string& str)
{
    std::vector<std::string> inputs;
    std::string nextToken;
    while(!str.empty())
    {
        nextToken = GetNextToken(str);
        inputs.push_back(nextToken);
    }

    return inputs;
}

std::string CLIController::GetNextToken(std::string& str)
{
    //Get next word delimited by space or newline char.
    //Trim str to remove the returned substring + delimiting character.
    auto pos = str.find_first_of(" \n");
    if(pos != std::string::npos)
    {
        std::string nextToken = str.substr(0,pos);
        str.erase(0,pos + 1);
        return nextToken;
    }
    else
    {
        std::string nextToken = str;
        str.clear();
        return nextToken;
    }
}

void CLIController::RegisterCommands()
{
   m_CommandMap["proc"] = [this](auto& args) { this->CMDproc(args); }; 
   m_CommandMap["struct"] = [this](auto& args) { this->CMDstruct(args); };
   m_CommandMap["pstruct"] = [this](auto& args) { this->CMDpstruct(args); };
}

bool CLIController::CheckCMDArgLen(const std::vector<std::string>& args,
        const unsigned long& correctArgLength,const std::string& CMDname)
{    
    if(args.size() != correctArgLength)
    {
        std::string argstr;
        for(std::string arg : args)
            argstr += arg;

        LOG_INFO("Command {} error: {}",CMDname,argstr);
        PRINT("Invalid arguments for {}: {}",CMDname,argstr);

        return false;
    }
    else
        return true;
}

void CLIController::ExecuteCommand(const std::string& name, 
        const std::vector<std::string>& args)
{
    auto it = m_CommandMap.find(name);
    if(it != m_CommandMap.end())
    {
        LOG_INFO("Found command {} in command map", name);
        it->second(args);
    }
    else
    {
        PRINT("No such command: {}.", name);
    }
}

//-----Command Functions

void CLIController::CMDproc(const std::vector<std::string>& args)
{
    //On success, a new Process class is spawned and assigned to the CliControllers m_Proc
    //variable. proc command takes 1 argument which should be a valid PID for the process
    //which to read memory from.
    if(!CheckCMDArgLen(args,1,"proc"))
        return;

    std::optional<int> pid = FromChars<int>(args[0]);

    if(!pid.has_value())
    {
        PRINT("Process {} not found or invalid.", args[0]);
        return;
    }
    
    if (DoesProcExist(pid.value()))
    {
        PRINT("Success. Process {} is active.", pid.value());
        m_Proc = std::make_unique<Process>(pid.value());
    }
    else
        PRINT("Process {} not found or invalid.", pid.value());
}

void CLIController::CMDstruct(const std::vector<std::string>& args)
{
    //On success, creates a new UserStruct class which is defined initially by a
    //memory address and size. struct command takes 2 arguments which should resolve to
    //a valid memory address within the CLI controller's target process (m_Proc) and a
    //size in number of bytes.
    if(!m_Proc)
    {
        PRINT("Attach to process first (pid command).");
        return;
    }
    if(!CheckCMDArgLen(args,3,"struct"))
    {
        PRINT("Usage: struct <name> <address> <size>");
        return;
    }

    
    std::optional<uint64_t> startPos = FromChars<uint64_t>(args[1]);
    std::optional<uint> size = FromChars<uint>(args[2]);

    if(!startPos.has_value())
    {
        PRINT("Invalid memory address. {}", args[0]);
        return;
    }
    if(!size.has_value())
    {
        PRINT("Invalid struct size. {}", args[1]);
        return;
    }

    m_Proc->MakeUserStruct(args[0], startPos.value(), size.value());

}

void CLIController::CMDpstruct(const std::vector<std::string>& args)
{
    if(!m_Proc)
    {
        PRINT("Attach to process first (pid command).");
        return;
    }
    if(!CheckCMDArgLen(args,1,"pstruct"))
    {
        PRINT("Usage: psruct <struct name>");
        return;
    }
    if(!m_Proc->GetUserStruct(args[0]))
    {
        PRINT("No such struct {}.",args[0]);
        return;
    }

    PRINT("Struct {} found.");


}
