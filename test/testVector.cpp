//
// Created by john on 5/9/19.
//
#include "vector.h"
#include <iostream>

class ElementsV{
public:
    ElementsV() = default;
    ElementsV(int x):a(x){};

    virtual int peek(){
        return a;
    }
protected:
    int a;
};
class DerElementsV:public ElementsV{
public:
    DerElementsV() = default;
    DerElementsV(int x,int y):ElementsV(x),b(y){}
    int peek(){
        return a+b;
    }
private:
    int b;
};
using namespace mySTL;
int main(){
    // constructors used in the same order as described above:
    vector<int> first;                                // empty vector of ints
    vector<int> second {1,2,3,4,5,6};                       // four ints with value 100
//
    std::cout<<second.capacity()<<std::endl;
    second.insert(second.begin()+1,2,20);
    second.erase(second.begin()+3);
    second.erase(second.begin(),second.end()-3);
    std::cout<<*(second.end()-1)<<std::endl;
    for(auto it=second.begin();it<second.end();++it){
        std::cout<<*it<<" ";
    }

    for(int i=0;i<second.size();++i){
        std::cout<<second[i]<<" ";
    }
    std::cout<<"\n"<<"-----------------------------"<<"\n";
    vector<ElementsV> v1{ElementsV(1),ElementsV(2),ElementsV(3)};
    for(int i=0;i<v1.size();++i){
        std::cout<<v1[i].peek()<<" ";
    }
    std::cout<<"\n";

    vector<DerElementsV> v{DerElementsV(1,10),DerElementsV(2,20),DerElementsV(3,30),DerElementsV(4,40),DerElementsV(5,50)};
    v.erase(v.begin(),v.begin()+2);
    v.push_back(DerElementsV(6,60));
    v.insert(v.begin(),2,DerElementsV(10,100));
    std::cout<<"v-size: "<<v.size()<<"\n";
    std::cout<<"v-capcity: "<<v.capacity()<<"\n";
    for(int i=0;i<v.size();++i){
        std::cout<<v[i].peek()<<" ";
    }



    return 0;
}
