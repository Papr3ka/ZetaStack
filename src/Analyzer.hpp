#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include<string>
#include<vector>

#include "Token.hpp"

bool checkrpn(const std::vector<token>& checktokens);

bool squotecount(const std::string& str);

bool quotecount(const std::string& str);

int checkleftBrac(const std::string& str);

int checkrightBrac(const std::string& str);

bool bracketOrder(const std::vector<token>& tokens);

#endif