#include <iostream>
#include <random>
#include "train.h"
#include "classify.h"

void splitData(){
    std::cout << "=> Splitting file\n";
    std::string temp;
    std::default_random_engine engine{std::random_device()()};
    std::uniform_int_distribution<int> rand{1, 100};
    std::ifstream fin("spam.csv");
    std::ofstream foutTrain("train.txt");
    std::ofstream foutTest("test.txt");
    getline(fin, temp);
    temp.clear();
    while(!fin.eof()){
        getline(fin, temp);
        if(rand(engine) <= 33){
            foutTrain << temp << std::endl;
        }else{
            foutTest << temp << std::endl;
        }
    }
    foutTrain.close();
    foutTest.close();
}

int main(){
    Trainer trainer;
    Classifier classifier;
    for(int i = 0; i < 10; ++i){
        std::cout << "=============== " << i << " ===============" << std::endl;
        splitData();
        trainer.train("train.txt");
        classifier.classify();
    }
    classifier.getMetrix();
    return 0;
}

