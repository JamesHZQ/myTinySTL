//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_CONSTRUCT_H
#define MYTINYSTL_CONSTRUCT_H

#include "typetraits.h"
#include "iterator.h"

#include <new>

namespace mySTL{

    //在ptr指向的内存上以value构造T1类型对象
    template <typename T1,typename T2>
    inline void construct(T1* ptr,const T2& value){
        new(ptr) T1(value);
    }

    //执行ptr指向的T类型对象的析构函数
    template <typename T>
    inline void destroy(T* ptr){
        ptr->~T();
    }

    template <typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first,ForwardIterator last,__false_type){
        //T类型有自己的析构函数，就在内存归还前调用之
        for(;first<last;++first)
            destroy(&*first);
    }

    template <typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type){}

    template <typename ForwardIterator,typename T>
    inline void __destroy(ForwardIterator first,ForwardIterator last,T*){
        //T为迭代器指向的类型，通过__type_traits提取类型信息has_trivial_destructor
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
        //根据T是否含有自己的析构函数，调用不同版本的__destroy_aux
        __destroy_aux(first,last,trivial_destructor());
    }

    //提取迭代器指向的类型，交给__destroy
    template <typename ForwardIterator>
    inline void destroy(ForwardIterator first,ForwardIterator last){
        __destroy(first,last,value_type(first));
    }


    //inline void destroy(char*, char*) {}
    //inline void destroy(wchar_t*, wchar_t*) {}

}

#endif //MYTINYSTL_CONSTRUCT_H
