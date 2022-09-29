#include <random>
#include <cmath>
#include <numeric>
#include "classify.h"

int DATA_PERCENT = 100;
// ./class TRAIN_DATA DATA_PERCENT SPAM_PROBS HAM_PROBS OUTPUT
int main(int argc, char const *argv[]){

    long int totalSpamCount;
    long int totalHamCount;

    std::queue<std::string> sms;
    std::vector<WORD> spam;
    std::vector<WORD> ham;

    std::string word;
    std::string count;
    // spam prob
    //std::ifstream fin(argv[2]);
    std::ifstream fin("spam.txt");

    readProbabilityFile(fin, spam ,totalSpamCount);
    // ham prob
    //fin.open(argv[3]);
    fin.open("ham.txt");

    readProbabilityFile(fin, ham, totalHamCount);
    //test
    //fin.open(argv[1]);
    fin.open("spam.csv");

    readTestFile(fin, sms, DATA_PERCENT);
    // output
    naiveBayesClassifier(totalHamCount, totalSpamCount, sms, spam, ham, "classed.txt");//argv[4]);

    return 0;
}

void readProbabilityFile(std::ifstream &fin, std::vector<WORD> &words, long int &count){
    std::string tempTotalCount;
    std::string tempCount;
    std::string word;

    getline(fin, tempTotalCount);

    while(!fin.eof()){
        getline(fin, word, ',');
        getline(fin, tempCount);

        WORD word_;
        word_.word = word;
        word_.count = atoi(tempCount.c_str());

        words.push_back(word_);
    }
    fin.close();
}

void readTestFile(std::ifstream &fin, std::queue<std::string> &sms, int DATA_PERCENT){
    std::string buff;
    std::string newSMS;

    getline(fin, newSMS);
    newSMS.clear();

    std::default_random_engine engine{std::random_device()()};
    std::uniform_int_distribution<int> rand{1, 100};

    while(!fin.eof()){
        getline(fin, buff, ',');
        getline(fin, newSMS);
        int r = rand(engine);
        if(r <= DATA_PERCENT){
            sms.push(newSMS);
            newSMS.clear();
        }else{
            newSMS.clear();
            continue;
        }
    }
    fin.close();
}

void naiveBayesClassifier(long int totalHamCount,long int totalSpamCount, std::queue<std::string> &sms, std::vector<WORD> &spam,
                          std::vector<WORD> &ham, const char* file){
    // P(class_1) = count(class_1) / (count(class_1) + count(class_2)
    float spamProbability = (float)(totalSpamCount/(float)(totalSpamCount + totalHamCount));
    float hamProbability = (float)(totalHamCount/(float)(totalHamCount + totalSpamCount));

    std::vector<int> accuracies;
    std::vector<int> precisions;
    std::vector<int> recalls;
    std::vector<double> f1_scores;

    std::vector<std::string> spamSMS;
    std::vector<std::string> hamSMS;

    std::ofstream fout(file);
    std::cout << sms.size() << std::endl;

    int TP, TN, FP, FN;
    int accuracy, precision, recall, beta;
    double f1_score;
    for(int _ = 0; _ < 10; ++_) {
        std::queue<std::string> sms_ = sms;
        TP = 0;
        TN = 0;
        FP = 0;
        FN = 0;
        while (!sms_.empty()) {
            std::vector<std::string> words;
            std::vector<float> wordSpamProbabilities;
            std::vector<float> wordHamProbabilities;

            parseSMS(sms_.front(), words);
            // Conditional probabilities
            // 1. P(word) = count(word) / (total_words)
            // 2. P(spam | word) = (P(word | spam) * P(spam)) / P(word)
            // 3. P(spam | sms) = P(spam | word_1) * P(spam | word_2) * ...
            // 4. MAX(P(spam | sms), P(ham | sms))
            // 5. Classify based on max

            for (auto &word: words) {
                WORD newWord;
                float spamWordCount = 0;
                float hamWordCount = 0;

                for (int j = 0; j < spam.size(); ++j) {
                    newWord = spam[j];

                    if (word == newWord.word) {
                        spamWordCount = newWord.count;
                        j = spam.size();
                    }
                }
                for (int j = 0; j < ham.size(); ++j) {
                    newWord = ham[j];

                    if (word == newWord.word) {
                        hamWordCount = newWord.count;
                        j = ham.size();
                    }
                }

                float spamProb = (((spamWordCount / (float)totalSpamCount) * spamProbability) /
                        ((spamWordCount + hamWordCount) / (totalSpamCount + totalHamCount)));

                wordSpamProbabilities.push_back(spamProb);

                float hamProb = (((hamWordCount / (float)totalHamCount) * hamProbability) /
                        ((spamWordCount + hamWordCount) / (totalSpamCount + totalHamCount)));

                wordHamProbabilities.push_back(hamProb);
            }

            //Max and classify
            float hamProb = 0;
            for (float i: wordHamProbabilities) {
                hamProb += i;
            }

            float spamProb = 0.0;
            for (float i: wordSpamProbabilities) {
                spamProb += i;
            }

            std::string temp = sms_.front();

            /*for (int i = 0; i <= 3; ++i) {
                temp[temp.length()-i] = ' ';
            }*/
            if (spamProb < hamProb) {
                compareWithTrainData(temp, "ham") ? TP++ : FP++;
                fout << "ham," << temp << std::endl;
            } else if (spamProb > hamProb) {
                compareWithTrainData(temp, "spam") ? TN++ : FN++;
                fout << "spam," << temp << std::endl;
            }/*else{
                std::cout << "WTF???" << " spam: " << spamProb << ", ham: " << hamProb << std::endl;
            }*/
            sms_.pop();
        }
        printf("For: %i | TP: %i, FP: %i, TN: %i, FN: %i\n", _, TP, FP, TN, FN);
        accuracy = (TP+TN)/(TP+TN+FP+FN);
        precision = TP/(TP+FP);
        recall = TP/(TP+FN);
        beta = 1;
        f1_score = (1 + pow(beta, 2)) * (precision * recall) / (pow(beta, 2) * precision + recall);
        accuracies.push_back(accuracy);
        precisions.push_back(precision);
        recalls.push_back(recall);
        f1_scores.push_back(f1_score);
    }
    fout.close();
    std::cout << "Near printf" << std::endl;
    printf("Average scores: \n Accuracy: %lu \n Precision: %lu \n Recall: %lu \n F1_Score: %f",
           std::accumulate(accuracies.begin(), accuracies.end(), 0) / accuracies.size(),
           std::accumulate(precisions.begin(), precisions.end(), 0) / precisions.size(),
           std::accumulate(recalls.begin(), recalls.end(), 0) / recalls.size(),
           std::accumulate(f1_scores.begin(), f1_scores.end(), 0.0) / f1_scores.size());
}

bool compareWithTrainData(std::string sms, std::string type){
    bool ret = false;
    std::fstream file("spam.csv");
    std::string str, type_;
    getline(file, str);
    str.clear();
    while(!file.eof()){
        getline(file, type_, ',');
        getline(file, str);
        if(sms==str && type == type_){
            ret = true;
            break;
        }
        str.clear();
    }
    file.close();
    //std::cout << "Ret is:" << ret << std::endl;
    return ret;
}

void parseSMS(std::string sms, std::vector<std::string> &words){
    if(sms.empty())
        return;

    char* cpy = new char[sms.size()+1];
    strcpy(cpy, sms.c_str());

    std::string newWord;

    for (int i = 0; i < sms.size(); ++i) {
        if(cpy[i] == ' ' || cpy[i] == ',' || cpy[i] == '.' || cpy[i] == '\"' ||
           cpy[i] == ':' || cpy[i] == '?' || cpy[i] == '(' || cpy[i] == ')' ||
           cpy[i] == '{' || cpy[i] == '}' || cpy[i] == '[' || cpy[i] == ']' ||
           cpy[i] == '!' || cpy[i] == '\n'|| cpy[i] == '\r'){

            if(!newWord.empty()){
                words.push_back(newWord);
                newWord.clear();
            }
        }else{
            newWord += cpy[i];
        }
    }
}