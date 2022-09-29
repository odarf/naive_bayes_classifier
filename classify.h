#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cstring>

struct WORD{
    std::string word;
    int count;
};

void readProbabilityFile(std::ifstream &fin, std::vector<WORD> &word, long int &count);
void readTestFile(std::ifstream &fin, std::queue<std::string> &sms, int DATA_PERCENT);
void parseSMS(std::string sms, std::vector<std::string> &words);
void naiveBayesClassifier(long int,long int, std::queue<std::string> &sms, std::vector<WORD> &spam, std::vector<WORD> &ham, const char* file);
bool findSmsInTrainData(std::string sms);