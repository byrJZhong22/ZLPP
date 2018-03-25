//
// Created by jun on 17-11-20.
//
#include <iostream>
#include <string>
#include <typeinfo>
#include "vector.h"

using namespace zh;

int main(void) {

    containers::Vector<int> vec;
    vec.Push_back(20);
    vec.Push_back(100);
    vec.Push_back(200);
    vec.Insert(1, 50);
    std::cout << vec.Size() << std::endl;
}



