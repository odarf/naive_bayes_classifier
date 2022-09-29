#include <random>
#include "classify.h"

// ./class TRAIN_DATA DATA_PERCENT SPAM_PROBS HAM_PROBS OUTPUT
int main(int argc, char const *argv[]){
    int DATA_PERCENT = 40;
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
    int TP = 0;
    int TN = 0;
    int FP = 0;
    int FN = 0;

    // P(class_1) = count(class_1) / (count(class_1) + count(class_2)
    float spamP = ((float)totalSpamCount/(float)(totalSpamCount + totalHamCount));
    float hamP = ((float)totalHamCount/(float)(totalHamCount + totalSpamCount));

    std::vector<std::string> spamSMS;
    std::vector<std::string> hamSMS;

    std::ofstream fout(file);
    std::cout << sms.size() << std::endl;
    while(!sms.empty()){
        std::vector<std::string> words;
        std::vector<float> wordSpamP;
        std::vector<float> wordHamP;
        parseSMS(sms.front(), words);
        // Conditional probabilities
        // 1. P(word) = count(word) / (total_words)
        // 2. P(spam | word) = (P(word | spam) * P(spam)) / P(word)
        // 3. P(spam | sms) = P(spam | word_1) * P(spam | word_2) * ...
        // 4. MAX(P(spam | sms), P(ham | sms))
        // 5. Classify based on max

        for(auto & word : words) {
            WORD newWord;
            float spamWordCount = 0;
            float hamWordCount = 0;

            for (int j = 0; j < spam.size(); ++j) {
                newWord = spam[j];

                if(word == newWord.word){
                    spamWordCount = (float)newWord.count;
                    j = (int)spam.size();
                }
            }
            for (int j = 0; j < ham.size(); ++j) {
                newWord = ham[j];

                if(word == newWord.word){
                    hamWordCount = (float)newWord.count;
                    j = (int)ham.size();
                }
            }

            float spamProb = (((spamWordCount / (float)totalSpamCount) * spamP) / ((spamWordCount + hamWordCount) /
                    ((float)totalSpamCount + (float)totalHamCount)));
            wordSpamP.push_back(spamProb);
            float hamProb = (((hamWordCount / (float)totalHamCount) * hamP) / ((spamWordCount + hamWordCount) /
                    ((float)totalSpamCount + (float)totalHamCount)));
            wordHamP.push_back(hamProb);
        }
        //Max and classify
        float hamProb = 0.0;
        for(float i : wordHamP) {
            hamProb += i;
        }

        float spamProb = 0.0;
        for(float i : wordSpamP) {
            spamProb += i;
        }

        std::string temp = sms.front();

        /*for (int i = 0; i <= 3; ++i) {
            temp[temp.length()-i] = ' ';
        }*/
        if(spamProb < hamProb){
            fout << "ham," << temp << std::endl;
        }else if(spamProb > hamProb){
            fout << "spam," << temp << std::endl;
        }
        //fout << " " << std::endl;
        sms.pop();
    }
    fout.close();
}

bool findSmsInTrainData(std::string sms){
    /* Ищем сообщение в spam.csv, берём первые 4 символа, если class == "ham," или "spam"
    */
    bool ret = false;
    std::fstream file("spam.csv");
    std::string str, type;
    while(getline(file, str)){
        getline(file, type, ',');
        if(sms==str){
            ret = true;
            break;
        }
    }
    file.close();
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