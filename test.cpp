#include <iostream>
#include <random>

int main(){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 100000);
    for (int i=0;i<10;i++){
        int id = dis(gen);
        std::cout << id << std::endl;
    }
    return 0;
}