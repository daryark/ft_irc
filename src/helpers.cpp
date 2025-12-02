#include "../incl/Command.hpp"

std::vector<std::string> splitVec(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::set<std::string> splitSet(const std::string& input, char delimiter) {
    std::set<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.insert(token);
    }
    return tokens;
}

const std::string joinVecIntoStr(std::vector<std::string>::const_iterator start,
                                std::vector<std::string>::const_iterator end)
{
    std::string result = "";
    for (std::vector<std::string>::const_iterator it = start; it != end; it++)
        result += (*it + " ");
    return result;
}

const std::string joinSetIntoStr(std::set<std::string>::const_iterator start,
                                std::set<std::string>::const_iterator end)
{
    std::string result = "";
    for (std::set<std::string>::const_iterator it = start; it != end; it++)
        result += (*it + " ");
    return result;
}

void trim(std::string &s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");

    if (start == std::string::npos)
    {
        s.clear();
        return;
    }

    s.erase(end + 1);
    s.erase(0, start);
}