//
// Created by john on 5/14/19.
//
#include "map.h"
#include <iostream>

using namespace mySTL;

int main(){
    map<char,int> first;

    first['a']=10;
    first['b']=30;
    first['c']=50;
    first['d']=70;

    map<char,int> second (first.begin(),first.end());

    map<char,int> third (second);

    map<char,int> mymap;
    map<char,int>::iterator itlow,itup;

    mymap['a']=20;
    mymap['b']=40;
    mymap['c']=60;
    mymap['d']=80;
    mymap['e']=100;

    itlow=mymap.lower_bound ('b');  // itlow points to b
    itup=mymap.upper_bound ('d');   // itup points to e (not d!)

    mymap.erase(itlow,itup);        // erases [itlow,itup)

    // print content:
    for (map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';

    map<char,int> mymap1;
    map<char,int>::iterator it;

    mymap1['a']=50;
    mymap1['b']=100;
    mymap1['c']=150;
    mymap1['d']=200;

    it = mymap1.find('b');
    if (it != mymap1.end())
        mymap1.erase (it);

    // print content:
    std::cout << "elements in mymap:" << '\n';
    std::cout << "a => " << mymap1.find('a')->second << '\n';
    std::cout << "c => " << mymap1.find('c')->second << '\n';
    std::cout << "d => " << mymap1.find('d')->second << '\n';

    return 0;
}
