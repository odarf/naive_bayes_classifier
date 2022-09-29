#include <random>
#include <cmath>
#include <numeric>
#include "classify.h"

// ./class TRAIN_DATA DATA_PERCENT SPAM_PROBS HAM_PROBS OUTPUT
Classifier::Classifier()= default;

void Classifier::classify(){
    long int totalSpamCount;
    long int totalHamCount;

    std::queue<std::string> sms;
    std::vector<WORD> spam;
    std::vector<WORD> ham;

    std::string word;
    std::string count;

    std::ifstream fin("spam.txt");

    readProbabilityFile(fin, spam ,totalSpamCount);

    fin.open("ham.txt");

    readProbabilityFile(fin, ham, totalHamCount);

    fin.open("test.txt");

    readTestFile(fin, sms);
    // output
    naiveBayesClassifier(totalHamCount, totalSpamCount, sms, spam, ham, "classed.txt");//argv[4]);
}

void Classifier::readProbabilityFile(std::ifstream &fin, std::vector<WORD> &words, long int &count){
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

void Classifier::readTestFile(std::ifstream &fin, std::queue<std::string> &sms){
    std::string buff;
    std::string newSMS;

    getline(fin, newSMS);
    newSMS.clear();

    while(!fin.eof()){
        getline(fin, buff, ',');
        getline(fin, newSMS);
        sms.push(newSMS);
        newSMS.clear();
    }
    fin.close();
}

void Classifier::naiveBayesClassifier(long int totalHamCount,long int totalSpamCount, std::queue<std::string> &sms, std::vector<WORD> &spam,
                          std::vector<WORD> &ham, const char* file){
    // P(class_1) = count(class_1) / (count(class_1) + count(class_2)
    float spamProbability = (float)(totalSpamCount/(float)(totalSpamCount + totalHamCount));
    float hamProbability = (float)(totalHamCount/(float)(totalHamCount + totalSpamCount));

    std::vector<std::string> spamSMS;
    std::vector<std::string> hamSMS;

    std::ofstream fout(file);
    std::cout << "=> SMS size: " <<sms.size() << std::endl;

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

        if (spamProb < hamProb) {
            compareWithTrainData(temp, "ham") ? TP++ : FP++;
            fout << "[ham]  | " << temp << std::endl;
        } else if (spamProb > hamProb) {
            compareWithTrainData(temp, "spam") ? TN++ : FN++;
            fout << "[spam] | " << temp << std::endl;
        }
        sms_.pop();
    }
    printf("=> TP: %d, FP: %d, TN: %d, FN: %d\n", TP, FP, TN, FN);
    accuracy = (double)(TP+TN)/(double)(TP+TN+FP+FN);
    precision = (double)TP/(double)(TP+FP);
    recall = (double)TP/(double)(TP+FN);
    beta = 1;
    f1_score = (1 + pow(beta, 2)) * (precision * recall) / (pow(beta, 2) * precision + recall);
    accuracies.push_back(accuracy);
    precisions.push_back(precision);
    recalls.push_back(recall);
    f1_scores.push_back(f1_score);
    fout.close();
}

bool Classifier::compareWithTrainData(std::string sms, std::string type){
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
    return ret;
}

void Classifier::getMetrix(){
    double acc = std::accumulate(accuracies.begin(), accuracies.end(), 0.0) / (double)(accuracies.size());
    double prec = std::accumulate(precisions.begin(), precisions.end(), 0.0) / (double)(precisions.size());
    double rec = std::accumulate(recalls.begin(), recalls.end(), 0.0) / (double)(recalls.size());
    double f1 = std::accumulate(f1_scores.begin(), f1_scores.end(), 0.0) / (double)(f1_scores.size());
    std::cout << "=> Average scores: \n - Accuracy: " << acc << "\n - Precision: " << prec << "\n - Recall: "
    << rec <<"\n - F1_Score: " << f1 << std::endl;
    /*printf("=> Average scores: \n - Accuracy: %f \n - Precision: %f \n - Recall: %f \n - F1_Score: %f \n",
           acc, prec, rec, f1);*/

}

void Classifier::parseSMS(std::string sms, std::vector<std::string> &words){
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