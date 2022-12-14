#ifndef CLASSIFY_H
#define CLASSIFY_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cstring>

class Classifier{
private:
    int TP, TN, FP, FN;
    double accuracy, precision, recall, beta, f1_score;
    std::vector<double> accuracies, precisions, recalls, f1_scores;
public:
    struct WORD{
        std::string word;
        int count;
    };
    Classifier();
    void classify();
    static void readProbabilityFile(std::ifstream &fin, std::vector<WORD> &word, long int &count, int n);
    static void readTestFile(std::ifstream &fin, std::queue<std::string> &sms);
    static void parseSMS(std::string sms, std::vector<std::string> &words);
    void naiveBayesClassifier(long int,long int, std::queue<std::string> &sms, std::vector<WORD> &spam, std::vector<WORD> &ham, const char* file);
    static bool compareWithTrainData(std::string sms, std::string type);
    void getMetrix();
};

#endif // CLASSIFY_H