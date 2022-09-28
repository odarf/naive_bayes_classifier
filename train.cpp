#include <cstring>
#include "train.h"

int main(int argc, char const *argv[]){
 std::cout << "In main..." << std::endl;
 std::vector<std::string> hamSMS;
 std::vector<std::string> spamSMS;

 int spamCount = 0;
 int hamCount = 0;
 std::string type;
 std::string newSMS;
 std::ifstream fin(argv[2]);
 getline(fin, newSMS);
 newSMS.clear();

 while(!fin.eof()){
     getline(fin, type, ',');
     getline(fin, newSMS);
     if(type == "ham"){
         hamSMS.push_back(newSMS);
         type.clear();
         newSMS.clear();
         hamCount++;
     }else if(type == "spam"){
         spamSMS.push_back(newSMS);
         type.clear();
         newSMS.clear();
         spamCount++;
     }
 }
 fin.close();
 parse(hamSMS, argv[4]);
 parse(spamSMS, argv[6]);

 return 0;
}

void parse(std::vector<std::string> sms, const char* file){
    std::cout << "In parse now..." << std::endl;
    std::vector<std::string> words;
    std::vector<WORD> wordsCount;
    int totalWordCount = 0;
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
        sms.pop_back();
    }

    int index = 0;

    while(index < words.size()){
        if(words[index] == "\0"){
            index++;
        }else{
            WORD word_;
            word_.word = words[index];
            word_.count = 0;

            for(int i = 0; i < words.size(); ++i) {
                if(word_.word == words[i]){
                    word_.count++;
                    words[i] = "\0";
                }
            }
            wordsCount.push_back(word_);
            index++;
        }
    }

    std::ofstream fout(file);
    totalWordCount = wordsCount.size();
    fout << totalWordCount << std::endl;
    WORD word_ = wordsCount.front();
    while(!wordsCount.empty()){
        WORD word_ = wordsCount.front();
        fout << word_.word << "," << word_.count << std::endl;
        wordsCount.pop_back();
    }
    fout.close();
}