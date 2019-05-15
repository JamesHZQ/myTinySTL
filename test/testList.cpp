//
// Created by john on 5/12/19.
//
#include "list.h"
#include <iostream>

using namespace mySTL;

int main(){
    list<int> l;
    l.push_back(2);
    l.push_back(1);
    l.push_back(19);
    l.push_back(3);
    l.push_front(28);
    l.push_back(6);
    l.push_back(4);
    l.erase(l.begin());
    l.sort();
    list<int> l1;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(4);
    l1.push_back(6);
    l1.push_back(2);
    l1.push_back(4);
    l1.push_back(3);
    l1.push_back(1);
    l1.sort();
    l.merge(l1);
    l.reverse();
    for(auto it = l.begin();it!=l.end();++it){
        std::cout<<*it<<" ";
    }
    std::cout<<"\n";
    int myints[]= {10,20,30,40,50};
    list<int> l2(myints,myints+5);
    for(auto it = l2.begin();it!=l2.end();++it){
        std::cout<<*it<<" ";
    }
    return 0;
}
