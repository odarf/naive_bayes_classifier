#include "classify.h"

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

    readTestFile(fin, sms);
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

void readTestFile(std::ifstream &fin, std::queue<std::string> &sms){
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

void naiveBayesClassifier(long int totalHamCount,long int totalSpamCount, std::queue<std::string> &sms, std::vector<WORD> &spam,
                          std::vector<WORD> &ham, const char* file){
    // P(class_1) = count(class_1) / (count(class_1) + count(class_2)
    float spamP = ((float)totalSpamCount/(float)(totalSpamCount + totalHamCount));
    float hamP = ((float)totalHamCount/(float)(totalHamCount + totalSpamCount));

    std::vector<std::string> spamSMS;
    std::vector<std::string> hamSMS;

    std::ofstream fout(file);
    std::cout << sms.size() << std::endl;
    int counter = 0;
    while(!sms.empty()){
        std::cout << counter << " " << sms.size() << std::endl;
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

        for (int i = 0; i < words.size(); ++i) {
            WORD newWord;
            float spamWordCount = 0;
            float hamWordCount = 0;

            for (int j = 0; j < spam.size(); ++j) {
                newWord = spam[j];

                if(words[i] == newWord.word){
                    spamWordCount = newWord.count;
                    j = spam.size();
                }
            }
            for (int j = 0; j < ham.size(); ++j) {
                newWord = ham[j];

                if(words[i] == newWord.word){
                    hamWordCount = newWord.count;
                    j = ham.size();
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
            counter++;
            fout << temp << "==> ham with P = " << 100.0-spamProb << std::endl;
        }else if(spamProb > hamProb){
            counter++;
            fout << temp << "==> spam with P = " << 100.0-hamProb << std::endl;
        }else{
            fout << temp << "=> WTF" << std::endl;
        }
        //fout << " " << std::endl;
        sms.pop();
    }
    fout.close();
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