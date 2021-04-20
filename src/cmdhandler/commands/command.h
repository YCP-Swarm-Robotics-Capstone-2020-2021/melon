#ifndef MELON_COMMAND_H
#define MELON_COMMAND_H

#include <string>
#include <vector>
#include <algorithm>

class Command
{
public:
    Command() = delete;

    Command(const std::string& cmd,
            const std::string& system,
            const std::string& variable,
            const std::vector<std::string>& values) :
                cmd(to_lower(cmd)),
                system(to_lower(system)),
                variable(to_lower(variable)),
                values(values)
    { }

    const std::string cmd;
    const std::string system;
    const std::string variable;
    const std::vector<std::string> values;

private:
    static std::string to_lower(const std::string& str)
    {
        std::string out;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
        return out;
    }
};

#endif //MELON_COMMAND_H
