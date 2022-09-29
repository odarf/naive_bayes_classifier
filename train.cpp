#include <cstring>
#include <thread>
#include <random>
#include "train.h"

// ./train TRAIN_DATA DATA_PERCENT HAM_OUTPUT SPAM_OUTPUT

Train::Train(){
    std::queue<std::string> hamSMS;
    std::queue<std::string> spamSMS;
    long int spamCount = 0;
    long int hamCount = 0;
    std::string type;
    std::string newSMS;
}

int main(int argc, char const *argv[]){
 int DATA_PERCENT = atoi(argv[2]);
 std::queue<std::string> hamSMS;
 std::queue<std::string> spamSMS;

 long int spamCount = 0;
 long int hamCount = 0;
 std::string type;
 std::string newSMS;
 std::ifstream fin(argv[1]);
 getline(fin, newSMS);
 newSMS.clear();

 std::default_random_engine engine{std::random_device()()};
 std::uniform_int_distribution<int> rand{1, 100};

 std::cout << "Starting while loop" << std::endl;
 while(!fin.eof()){
     int r = rand(engine);
     std::cout << r << std::endl;
     getline(fin, type, ',');
     getline(fin, newSMS);
     if(r < 40){
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
     }else{
         continue;
     }
 }
 std::cout << "End of loop, ham count is: " << hamCount << ", spam count is: " << spamCount << std::endl;
 fin.close();
 writeFile(hamSMS, argv[3]);
 writeFile(spamSMS, argv[4]);

 return 0;
}

void Train::writeFile(std::queue<std::string> sms, const char* file){
    std::cout << "Now writing " << file << std::endl;
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