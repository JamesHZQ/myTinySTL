//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_ITERATOR_H
#define MYTINYSTL_ITERATOR_H

#include <cstddef>

namespace mySTL {
    //五种迭代器
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    //定义5种迭代器的类型信息
    template<typename T, typename Distance>
    struct input_iterator {
        typedef input_iterator_tag          iterator_category;
        typedef T                           value_type;
        typedef Distance                    difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };
    template <typename T, typename Distance>
    struct output_iterator {
        typedef output_iterator_tag         iterator_category;
        typedef void                        value_type;
        typedef void                        difference_type;
        typedef void                        pointer;
        typedef void                        reference;
    };
    template <typename T, typename Distance>
    struct forward_iterator {
        typedef forward_iterator_tag	    iterator_category;
        typedef T					    	value_type;
        typedef Distance				    difference_type;
        typedef T*						    pointer;
        typedef T&						    reference;
    };
    template <typename T, typename Distance>
    struct bidirectional_iterator {
        typedef bidirectional_iterator_tag	iterator_category;
        typedef T							value_type;
        typedef Distance					difference_type;
        typedef T*							pointer;
        typedef T&							reference;
    };
    template <typename T, typename Distance>
    struct random_access_iterator {
        typedef random_access_iterator_tag	iterator_category;
        typedef T							value_type;
        typedef Distance					difference_type;
        typedef T*							pointer;
        typedef T&							reference;
    };

    //“标准迭代器”
    //为避免挂一漏百，自行开发的迭代器最好继承自以下这个std::iterator
    template<typename Category, typename T, typename Distance = ptrdiff_t,
             typename Pointer = T*, typename Reference = T&>
    struct iterator {
        typedef Category	iterator_category;
        typedef T			value_type;
        typedef Distance	difference_type;
        typedef Pointer		pointer;
        typedef Reference	reference;
    };

    //“榨汁机” traits
    template<typename Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category	iterator_category;
        typedef typename Iterator::value_type			value_type;
        typedef typename Iterator::difference_type		difference_type;
        typedef typename Iterator::pointer				pointer;
        typedef typename Iterator::reference 			reference;
    };

    //针对原生指针而设计的traits偏特化
    template<typename T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag 	iterator_category;
        typedef T 							value_type;
        typedef ptrdiff_t 					difference_type;
        typedef T*							pointer;
        typedef T& 							reference;
    };

    //pointer to const 的特化版本
    template<typename T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag 	iterator_category;
        typedef T 							value_type;
        typedef ptrdiff_t 					difference_type;
        typedef const T*					pointer;
        typedef const T& 					reference;
    };

    //返回迭代器类型（类别）
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator& It) {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    //返回迭代器指向的类型的指针类型
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator& It) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    //返回迭代器的distance_type
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    difference_type(const Iterator& It){
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

}

#endif //MYTINYSTL_ITERATOR_H
