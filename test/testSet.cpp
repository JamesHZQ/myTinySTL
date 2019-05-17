//
// Created by john on 5/14/19.
//

#include "set.h"
#include <iostream>

using namespace mySTL;

int main(){

    set<int> first;                           // empty set of ints

    int myints[]= {10,20,30,40,50};
    set<int> second (myints,myints+5);        // range

    set<int> third (second);                  // a copy of second

    set<int> fourth (second.begin(), second.end());  // iterator ctor.

    set<int> myset;
    set<int>::iterator itlow,itup;

    for (int i=1; i<10; i++) myset.insert(i*10); // 10 20 30 40 50 60 70 80 90
    myset.insert(30);

    itlow=myset.lower_bound (30);                //       ^
    itup=myset.upper_bound (60);                 //                   ^

    myset.erase(itlow,itup);                     // 10 20 70 80 90


    std::cout << "myset contains:";
    for (set<int>::iterator it=myset.begin(); it!=myset.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    set<int>::iterator it=myset.find(20);
    myset.erase (it);
    myset.erase (myset.find(40));

    std::cout << "myset contains:";
    for (it=myset.begin(); it!=myset.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';
    return 0;
}