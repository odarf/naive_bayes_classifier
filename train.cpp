#include <cstring>
#include <chrono>
#include <thread>
#include "train.h"

int main(int argc, char const *argv[]){
 std::queue<std::string> hamSMS;
 std::queue<std::string> spamSMS;

 long int spamCount = 0;
 long int hamCount = 0;
 std::string type;
 std::string newSMS;
 std::cout << "Created variables, starting ifstream" << std::endl;
 std::ifstream fin(argv[1]);
 std::cout << "Done ifstream, starting getline" << std::endl;
 getline(fin, newSMS);
 std::cout << "Done getline" << std::endl;
 newSMS.clear();

 std::cout << "Starting while loop" << std::endl;
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
     std::cout << "End of loop, ham count is: " << hamCount << ", spam count is: " << spamCount << std::endl;
 }
 fin.close();
 parse(hamSMS, argv[2]);
 parse(spamSMS, argv[3]);

 return 0;
}

void parse(std::queue<std::string> sms, const char* file){
    std::cout << "In parse now..." << std::endl;
    std::vector<std::string> words;
    std::queue<WORD> wordsCount;
    long int totalWordCount = 0;
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

            for(int i = 0; i < words.size(); ++i) {
                if(word_.word == words[i]){
                    word_.count++;
                    words[i] = "\0";
                }
            }
            wordsCount.push(word_);
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
        wordsCount.pop();
    }
    fout.close();
}