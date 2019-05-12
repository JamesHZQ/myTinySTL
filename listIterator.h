//
// Created by john on 5/11/19.
//

#ifndef MYTINYSTL_LISTITERATOR_H
#define MYTINYSTL_LISTITERATOR_H

#include "iterator.h"

namespace mySTL {
    //list节点有：前驱、后继、数据三个域
    template <typename T>
    struct __list_node {
        __list_node<T>* prev;
        __list_node<T>* next;
        T data;
    };

    //迭代器本身不是指针，因为list不是连续的区间
    template<typename T, typename Ref, typename Ptr>
    struct __list_iterator {
        typedef __list_iterator<T, T&, T*>   iterator; //指向内部元素值得迭代器
        typedef __list_iterator<T, Ref, Ptr> self; //指向list节点的迭代器

        typedef bidirectional_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef Ptr                         pointer;
        typedef Ref                         reference;
        typedef __list_node<T>*             link_type;
        typedef size_t                      size_type;
        typedef ptrdiff_t                   difference_type;

        //指向list节点的原始指针
        link_type node;
        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator& x) : node(x.node) {}

        bool operator==(const self& x) const {return node == x.node;}
        bool operator!=(const self& x) const {return node != x.node;}

        //解（迭代器）引用
        reference operator*() const {return (*node).data;}
        //获取数据域对象的地址，再通过->调用对象的接口
        pointer   operator->() const {return &(operator*());}

        //迭代器向前移动一个位置
        //前置++返回移动后的迭代器
        self& operator++() {
            node = node->next;
            return *this;
        }
        //后置++返回移动前的迭代器
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        //对迭代器向后移动一个位置（同++）
        self& operator--() {
            node = node->prev;
            return *this;
        }
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
    };
}

#endif //MYTINYSTL_LISTITERATOR_H
