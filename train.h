#ifndef TRAIN_H
#define TRAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

class Trainer{
private:
    std::queue<std::string> hamSMS;
    std::queue<std::string> spamSMS;
public:
    struct WORD{
        std::string word;
        long int count;
    };
    Trainer();
    void train(const char* fileName);
    void readFile(const char* fileName="spam.csv");
    static void writeFile(std::queue<std::string> sms, const char*);
};

//void writeFile(std::queue<std::string> sms, const char*);

#endif // TRAIN_H