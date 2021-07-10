#include "pch.h"
#include "scan.h"

// Unused function that scans for a byte patern in memory
uintptr_t ScanBasic(uintptr_t dwAddress, uintptr_t dwLength, const char* szPattern)
{
    const char* pattern{ szPattern };
    uintptr_t firstMatch{ 0 };

    uintptr_t start{ dwAddress };
    uintptr_t end{ dwAddress + dwLength };

    for (uintptr_t pos{ start }; pos < end; ++pos)
    {
        if (*pattern == 0)
            return firstMatch;
        // uint8_t == BYTE
        const uint8_t currentPattern{ *reinterpret_cast<const uint8_t*>(pattern) };
        const uint8_t currentMemory{ *reinterpret_cast<const uint8_t*>(pos) };

        if (currentPattern == '\?' || currentMemory == GetBytes(pattern))
        {
            if (firstMatch == 0)
                firstMatch = pos;

            if (pattern[2] == 0)
            {
                std::cerr << "Found pattern \\" << szPattern << "\\ at " << std::hex << firstMatch << std::dec << '\n';
                return firstMatch;
            }

            pattern += currentPattern != '\?' ? 3 : 2;
        }
        else
        {
            pattern = szPattern;
            firstMatch = 0;
        }
    }
    std::cerr << "Could not find pattern \\" << szPattern << "\\\n";
    return 0;
}
