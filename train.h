#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

struct WORD{
    std::string word;
    long int count;
};

void parse(std::queue<std::string> sms, const char*);