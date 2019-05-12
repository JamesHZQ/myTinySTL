//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_ALGORITHM_H
#define MYTINYSTL_ALGORITHM_H

#include <cstring>
#include <utility>

#include "iterator.h"
#include "typetraits.h"
namespace mySTL {
    //***** fill O(N)******
    template<typename ForwardIterator, typename T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value) {
        for (; first != last; ++first)
            *first = value;
    }
    //针对char*和wchar_t*的特化版本
    inline void fill(char *first, char *last, const char& value) {
        memset(first, static_cast<unsigned char>(value), last - first);
    }
    inline void fill(wchar_t *first, wchar_t *last, const wchar_t& value) {
        memset(first, static_cast<unsigned char>(value), (last - first) * sizeof(wchar_t));
    }

    //***** fill_n O(N)******
    template<typename OutputIterator, typename Size, typename T>
    OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
        for (; n > 0; --n, ++first)
            *first = value;
        return first;
    }
    //针对char*和wchar_t*的特化版本
    template<typename Size>
    char *fill_n(char *first, Size n, const char& value) {
        memset(first, static_cast<unsigned char>(value), n);
        return first + n;
    }
    template<typename Size>
    wchar_t *fill_n(wchar_t *first, Size n, const wchar_t& value) {
        memset(first, static_cast<unsigned char>(value), n * sizeof(wchar_t));
        return first + n;
    }

    //计算迭代器距离
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag){
        typename iterator_traits<InputIterator>::difference_type dist = 0;
        while (first++ != last){
            ++dist;
        }
        return dist;
    }

    template<typename RandomIterator>
    typename iterator_traits<RandomIterator>::difference_type
    __distance(RandomIterator first, RandomIterator last, random_access_iterator_tag){
        auto dist = last - first;
        return dist;
    }

    template<typename Iterator>
    typename iterator_traits<Iterator>::difference_type
    distance(Iterator first, Iterator last){
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        return __distance(first, last, iterator_category());
    }


    //迭代器移动
    template<typename InputIterator, typename Distance>
    void __advance(InputIterator& it, Distance n, input_iterator_tag){
        assert(n >= 0);
        while (n--){
            ++it;
        }
    }
    template<typename BidirectionIterator, typename Distance>
    void __advance(BidirectionIterator& it, Distance n, bidirectional_iterator_tag){
        if (n < 0){
            while (n++){
                --it;
            }
        }else{
            while (n--){
                ++it;
            }
        }
    }
    template<typename RandomIterator, typename Distance>
    void __advance(RandomIterator& it, Distance n, random_access_iterator_tag){
        if (n < 0){
            it -= (-n);
        }else{
            it += n;
        }
    }

    template <typename InputIterator, typename Distance>
    void advance(InputIterator& it, Distance n){
        typedef typename iterator_traits<InputIterator>::iterator_category iterator_category;
        __advance(it, n, iterator_category());
    }

    //********** [copy] ******************************

    //迭代器是原始指针，指向的对象不是pod类型 OR 迭代器是random_access_iterator_tag
    //非pod类型不能逐位拷贝，需要调用对象的拷贝复值函数
    template <typename RandomAccessIterator,typename OutputIterator,typename Distance>
    inline OutputIterator __copy_d(RandomAccessIterator first,RandomAccessIterator last,
                                   OutputIterator result, Distance*){
        for(Distance n = last-first;n>0;--n,++result,++first)
            *result = *first;
        return result;
    }

    //迭代器是原始指针，指向的对象是pod类型直接memmove
    template <typename T>
    inline T* __copy_t(const T* first,const T* last,T* result, __true_type){
        memmove(result,first,sizeof(T)*(last-first));
        return result+(last-first);
    }
    //迭代器是原始指针，指向的对象不是pod类型，调用__copy_d
    template <typename T>
    inline T* __copy_t(const T* first,const T* last,T* result, __false_type){
        return __copy_d(first,last,result,(ptrdiff_t*)0);
    }

    //input_iterator_tag
    template <typename InputIterator,typename OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last,OutputIterator result,input_iterator_tag){
        for(;first!=last;++result,++first)
            *result = *first;
        return result;
    }

    //random_access_iterator_tag
    template <typename RandomAccessIterator,typename OutputIterator>
    inline OutputIterator __copy(RandomAccessIterator first,RandomAccessIterator last,
                                 OutputIterator result,random_access_iterator_tag){
        return __copy_d(first,last,result,distance_type(first));
    }

    //__copy_dispatch泛化版
    template <typename InputIterator,typename OutputIterator>
    struct __copy_dispatch{
        OutputIterator operator()(InputIterator first,InputIterator last, OutputIterator result){
            return __copy(first,last,result,iterator_category(first));
        }
    };
    //__copy_dispatch 原始指针特化版
    template <typename T>
    struct __copy_dispatch<T*,T*>{
        T* operator()(T* first,T* last, T* result){
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first,last,result,t());
        }
    };
    //__copy_dispatch const原始指针特化版
    template <typename T>
    struct __copy_dispatch<const T*,T*>{
        T* operator()(const T* first,const T* last,T* result){
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first,last,result,t());
        }
    };

    template <typename InputIterator, typename OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result){
        //第一对括号构造匿名（临时）函数对象第二对括号用于传参
        return __copy_dispatch<InputIterator,OutputIterator>()(first, last, result);
    }

    template <typename InputIterator, typename OutputIterator>
    OutputIterator copy_backward(InputIterator first, InputIterator last, OutputIterator result){
        OutputIterator new_result = result - (last - first);
        return __copy_dispatch<InputIterator,OutputIterator>()(first, last, result);
    }

    template<>
    inline char *copy(char *first, char *last, char *result){
        memmove(result,first,last-first);
        return result + (last-first);
    }

    template<>
    inline wchar_t *copy(wchar_t *first, wchar_t *last, wchar_t *result){
        memmove(result,first,sizeof(wchar_t)*(last-first));
        return result + (last-first);
    }

    //min/max
    template <typename T1, typename T2>
    T1 max(T1 a, T2 b) {
        return a > b ? a : b;
    }

    template <typename T1, typename T2>
    T1 min(T1 a, T2 b) {
        return a < b ? a : b;
    }

    template <typename InputIterator, typename T>
    InputIterator find(InputIterator first, InputIterator last, const T& val){
        for (; first != last; ++first){
            if (*first == val)
                break;
        }
        return first;
    }
}

#endif //MYTINYSTL_ALGORITHM_H
