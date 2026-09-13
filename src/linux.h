#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "memory.h"


bool DoesProcExist(pid_t pid);
bool ReadProcMem(pid_t pid, uintptr_t address, void* buffer, size_t size);

bool ParseProcMapsLine(const std::string& line, MemoryRegion& region);
bool ParseProcMaps(pid_t pid, std::vector<MemoryRegion>& MemRegions);
