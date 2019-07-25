#include"memory.hpp"
#include<iostream>
using namespace mySTL;
int main(){
    shared_ptr<int> fooo;
    {
        shared_ptr<int> foo = make_shared<int>(5);
        fooo=foo;
        std::cout<<fooo.use_count()<<"/"<<foo.use_count()<<std::endl;
    }
    std::cout<<fooo.use_count()<<std::endl;
    if(fooo)
        std::cout<<"alive:"<<*fooo<<std::endl;
    else
        std::cout<<"dead"<<std::endl;
    return 0;
}