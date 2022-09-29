#ifndef TRAIN_H
#define TRAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

struct WORD{
    std::string word;
    long int count;
};

class Train{
private:
public:
    Train();
    void writeFile(std::queue<std::string> sms, const char*);
};

//void writeFile(std::queue<std::string> sms, const char*);

#endif // TRAIN_H