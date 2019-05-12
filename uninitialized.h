//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_UNINITIALIZED_H
#define MYTINYSTL_UNINITIALIZED_H

#include "construct.h"
#include "algorithm.h"

namespace mySTL {

    /*
    *  未初始化的拷贝,在已获得的内存上构造若干对象
    */
    //POD版本的未初始化拷贝
    template<typename InputIterator, typename ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                             ForwardIterator result, __true_type) {
        //若迭代器指向的pod类型，直接通过内存拷贝构造对象，通过algorithm里的copy实现
        return copy(first,last,result);
    }

    //非POD版本的未初始化拷贝
    template<typename InputIterator, typename ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                             ForwardIterator result, __false_type) {
        //若迭代器指向的时非pod类型，则要通过construct构造对象，返回尾后迭代器
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
        return cur;
    }

    template<typename InputIterator, typename ForwardIterator, typename T>
    ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
                                         ForwardIterator result, T*) {
        //提取迭代器指向的类型的类型信息is_POD_type
        typedef typename __type_traits<T>::is_POD_type isPOD;
        //根据是否是pod类型，调用不同版本的__uninitialized_copy_aux
        return __uninitialized_copy_aux(first, last, result, isPOD());
    }

    template<typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                       ForwardIterator result) {
        //提取迭代器指向的类型
        __uninitialized_copy(first, last, result, value_type(result));

    }

    /*
	*  未初始化的填充，以某一特定值初始化（类似于uninitialized_copy）
	*/
    template<typename ForwardIterator, typename T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                         const T& value, __true_type) {
        fill(first, last, value);
    }

    template<typename ForwardIterator, typename T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                  const T& value, __false_type) {
        for (;first != last; ++first) {
            construct(first, value);
        }
    }

    template<typename ForwardIterator, typename T,typename T1>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                     const T& x,T1*) {
        typedef typename __type_traits<T1>::is_POD_type isPOD;
        __uninitialized_fill_aux(first, last, x, isPOD());
    }

    template <typename ForwardIterator,typename T>
    inline void uninitialized_fill(ForwardIterator first,ForwardIterator last,
                                   const T& x){
        __uninitialized_fill(first,last,x,value_type(first));
    }

    /*
	*  未初始化的拷贝，以某一特定值初始化n个（类似于uninitialized_copy）
	*/
    template<typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                      const T& x, __true_type) {
        return fill_n(first, n, x);
    }

    template<typename ForwardIterator, typename Size, typename T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                               const T& x, __false_type) {
        int i = 0;
        for(; i != n; ++i) {
            construct((T*)(first + i), x);
        }
        return (first + i);
    }

    template<typename ForwardIterator, typename Size, typename T,typename T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x,T1*) {
        typedef typename __type_traits<T1>::is_POD_type isPOD;
        return __uninitialized_fill_n_aux(first, n, x, isPOD());
    }

    template <typename ForwardIterator,typename Size,typename T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first,Size n, const T& x){
        return __uninitialized_fill_n(first,n,x,value_type(first));
    }
}

#endif //MYTINYSTL_UNINITIALIZED_H
