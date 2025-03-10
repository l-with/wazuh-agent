#include "cmdHelper.hpp"

#include <cstdio>
#include <memory>
#include <pal.h>
#include <string>
#include <vector>

namespace Utils
{
    struct FileSmartDeleter
    {
        void operator()(FILE* file)
        {
            pclose(file);
        }
    };

    std::string Exec(const std::string& cmd, const size_t bufferSize)
    {
        const std::unique_ptr<FILE, FileSmartDeleter> file {popen(cmd.c_str(), "r")};
        std::vector<char> buffer(bufferSize);
        std::string result;

        if (file)
        {
            while (fgets(buffer.data(), static_cast<int>(bufferSize), file.get()))
            {
                result += buffer.data();
            }
        }

        return result;
    }
} // namespace Utils
