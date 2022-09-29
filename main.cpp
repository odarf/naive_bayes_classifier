#include <iostream>
#include <random>
#include "train.h"
#include "classify.h"

void splitData(){
    std::cout << "Started splitting file\n";
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
        if(rand(engine) <= 66){
            foutTrain << temp << std::endl;
        }else{
            foutTest << temp << std::endl;
        }
    }
    foutTrain.close();
    foutTest.close();
    std::cout << "Done splitting file\n";
}

int main(){
    Trainer trainer;
    Classifier classifier;
    for(int i = 0; i < 3; ++i){
        std::cout << "=========" << i << "=========" << std::endl;
        splitData();
        trainer.train("train.txt");
        classifier.classify();
    }
    classifier.getMetrix();
    return 0;
}

