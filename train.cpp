#include <cstring>
#include <thread>
#include "train.h"

// ./train TRAIN_DATA DATA_PERCENT HAM_OUTPUT SPAM_OUTPUT

Trainer::Trainer() = default;

void Trainer::train(const char* fileName){
    readFile(fileName);
    writeFile(hamSMS, "ham.txt");
    writeFile(spamSMS, "spam.txt");
}

void Trainer::readFile(const char* fileName){
 long int spamCount = 0;
 long int hamCount = 0;
 std::string type;
 std::string newSMS;
 std::ifstream fin(fileName);
 getline(fin, newSMS);
 newSMS.clear();

 std::cout << "=> Starting while loop" << std::endl;
 while(!fin.eof()){
     getline(fin, type, ',');
     getline(fin, newSMS);
     if(type == "ham"){
       hamSMS.push(newSMS);
       type.clear();
       newSMS.clear();
       hamCount++;
     }else if(type == "spam"){
         spamSMS.push(newSMS);
         type.clear();
         newSMS.clear();
         spamCount++;
     }
 }
 std::cout << "=> Done reading file, ham SMS count is: " << hamCount
                               << ", spam SMS count is: " << spamCount << std::endl;
 fin.close();
}

void Trainer::writeFile(std::queue<std::string> sms, const char* file){
    std::cout << "=> Now writing " << file << std::endl;
    std::vector<std::string> words;
    std::queue<WORD> wordsCount;
    long int totalWordCount;
    while(!sms.empty()){
        std::string buff = sms.front();
        char* cpy = new char[buff.length()+1];
        strcpy(cpy, buff.c_str());

        std::string newWord;

        for (int i = 0; i < buff.length(); ++i) {
            if(cpy[i] == ' ' || cpy[i] == ',' || cpy[i] == '.' || cpy[i] == '\"' ||
            cpy[i] == ':' || cpy[i] == '?' || cpy[i] == '(' || cpy[i] == ')' ||
            cpy[i] == '{' || cpy[i] == '}' || cpy[i] == '[' || cpy[i] == ']' ||
            cpy[i] == '!' || cpy[i] == '\n' || cpy[i] == '\r'){
                if(!newWord.empty()){
                    words.push_back(newWord);
                    newWord.clear();
                }
            }else{
                newWord += cpy[i];
            }
        }
        sms.pop();
    }

    long int index = 0;

    while(index < words.size()){
        if(words[index] == "\0"){
            index++;
        }else{
            WORD word_;
            word_.word = words[index];
            word_.count = 0;

            for(auto & word : words) {
                if(word_.word == word){
                    word_.count++;
                    word = "\0";
                }
            }
            wordsCount.push(word_);
            index++;
        }
    }

    std::ofstream fout(file);
    totalWordCount = (long int)wordsCount.size();
    fout << totalWordCount << std::endl;
//WORD word_ = wordsCount.front();
    while(!wordsCount.empty()){
        WORD word_ = wordsCount.front();
        fout << word_.word << "," << word_.count << std::endl;
        wordsCount.pop();
    }
    fout.close();
}