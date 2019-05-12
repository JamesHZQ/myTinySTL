//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_VECTOR_H
#define MYTINYSTL_VECTOR_H


//#include <algorithm>
//#include <type_traits>
#include <iostream>
//#include <initializer_list>

#include "allocator.h"
#include "algorithm.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mySTL {
    /******** vector ***********/
    template<typename T, typename Alloc = alloc>
    class vector {
    public:
        //vector的嵌套型别定义
        typedef T         value_type;
        typedef T*        pointer;
        typedef T*        iterator;
        typedef const T*  const_pointer;
        typedef T&        reference;
        typedef size_t    size_type;
        typedef ptrdiff_t difference_type;

    protected:
        //simple_alloc 是空间配置器
        typedef simple_alloc<value_type, Alloc> data_allocator;

        iterator start_; //目前使用空间头
        iterator finish_;//目前使用空间尾
        iterator end_of_storage_;//目前可用空间的尾

        void insert_aux(iterator position, const T& x);
        //归还所有空间
        void deallocate() {
            if (start_) {
                data_allocator::deallocate(start_, end_of_storage_ - start_);
            }
        }

        //以value为值构造n个对象填充到vector
        void fill_initialize(size_type n, const T& value) {
            start_ = allocate_and_fill(n, value);
            finish_ = start_ + n;
            end_of_storage_ = finish_;
        }

    public:
        iterator begin() const { return start_;}
        iterator end() const  { return finish_;}
        size_type size() const { return size_type(end() - begin());}
        size_type capacity() const { return size_type(end_of_storage_ - begin());}
        bool empty() const { return begin() == end();}
        reference operator[](size_type n) {return *(begin() + n);}
        bool operator ==(const vector& other) const {
            auto first1 = begin(), last1 = end();
            auto first2 = other.begin(), last2 = other.end();
            for (; first1 != last1 && first2 != last2; ++first1, ++first2){
                if (*first1 != *first2)
                    return false;
            }
            //两个vector长度是否一直
            return (first1 == last1 && first2 == last2);
        }

        vector() : start_(0), finish_(0), end_of_storage_(0) {}
        vector(size_type n, const T& value) { fill_initialize(n, value);}
        vector(int       n, const T& value) { fill_initialize(n, value);}
        vector(long      n, const T& value) { fill_initialize(n, value);}
        vector(const std::initializer_list<T> v) {
            auto start_v = v.begin();
            auto end_v = v.end();
            size_type n = v.size();
            //开辟n个T类型的空间
            fill_initialize(n, T());
            //initializer_list里的n个对象复制到vecotr
            finish_ = copy(start_v, end_v, start_);
        }
        explicit vector(size_type n) {fill_initialize(n, T());}

        ~vector() {
            //让vector里所有对象调用自己的析构函数
            destroy(start_, finish_);
            //归还内存
            deallocate();
        }

        //返回头尾节点对象的引用
        reference front() { return *begin();}
        reference back() { return *end();}

        void push_back(const T& x) {
            //还有剩余空间
            if (finish_ != end_of_storage_) {
                construct(finish_, x);
                finish_++;
            } else {
                insert_aux(end(), x);
            }
        }

        //执行尾元素对象的析构函数，此时并没有将其所占的内存归还给系统，
        //而是将尾元素占有的空间并入vector的剩余空间里
        void pop_back() {
            --finish_;
            destroy(finish_);
        }

        void insert(iterator position, size_type n, const T& x);

        iterator erase(iterator position) {
            //将要删除的元素全部向前移动一个位置
            if(position +1 !=end())
                copy(position+1,finish_,position);
            --finish_;
            destroy(finish_);
            return position;
        }

        //last指向的元素不删除（处理类似v.erase(first,v.end()的情况）
        iterator erase(iterator first, iterator last) {
            iterator i = copy(last,finish_,first);
            destroy(i,finish_);
            finish_ = finish_-(last-first);
            return first;
        }

        //调整容器大小，如果newsize小于原大小，则清空多余部分
        //如果大于原大小，则从尾部插入
        void resize(size_type new_size, const T& x) {
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }
        void resize(size_type new_size) { resize(new_size, T());}
        void clear() { erase(begin(), end());}

    protected:
        //分配n个元素大小空间，并以x初始化
        iterator allocate_and_fill(size_type n, const T& x) {
            //获取内存空间
            iterator result = data_allocator::allocate(n);
            //在获取到的内存上构造对象
            uninitialized_fill_n(result, n, x);
            return result;
        }
    };

    template<typename T, typename Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
        if (n != 0) {
            //剩余空间足够容纳n个元素
            if (size_type(end_of_storage_ - finish_) >= n) {
                //插入元素之后的元素（需要移动的元素）
                const size_type elems_after = finish_ - position;
                iterator old_finish = finish_;
                //插入元素之后的元素的个数大于插入的元素数
                if(elems_after>n){
                    //插入元素之后的元素的后n个元素，
                    // uninitialized_copy到finish_后面的未初始化内存上
                    uninitialized_copy(finish_-n,finish_,finish_);
                    //调整尾后指针
                    finish_+=n;
                    //插入元素之后的元素的“前面的元素”，填补上面形成的空缺，
                    //由于是在已初始化内存上操作使用copy_backward(形如*it1=*it2，不用构造了)
                    copy_backward(position,old_finish-n,old_finish);
                    //将要插入的元素填充进来，无须初始化内存，仅是赋值
                    fill(position,position+n,x);
                } else{
                    //待插入的元素个数，多于position之后的元素个数，那么代插入元素一定会有一部分
                    //在finish_之后的内存（当前未初始化）上
                    uninitialized_fill_n(finish_,n-elems_after,x);
                    finish_ += n-elems_after;
                    //position到finish之间的元素，uninitialized_copy到上面插入的一部分元素的后面
                    uninitialized_copy(position,old_finish,finish_);
                    finish_ += elems_after;
                    //将剩下的要插入的元素fill进来
                    fill(position,old_finish,x);
                }
            } else {
                //内存空间不足以装下新增加的元素
                const size_type old_size = size();
                //至少增加两倍
                const size_type new_size = old_size + max(old_size, n);

                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;

                try {
                    //原来的元素+插入的元素复制到新的内存空间（未初始化的）
                    new_finish = uninitialized_copy(start_, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish,n,x);
                    new_finish = uninitialized_copy(position,finish_,new_finish);
                } catch(...) {
                    //若出现异常回滚
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, new_size);
                    throw;
                }
                //清理原来的空间
                destroy(begin(),end());     //析构
                deallocate();               //归还内存
                //修改指针
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = new_start + new_size;
            }
        }
    }

    //push_back专用版insert
    template<typename T, typename Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
        if (finish_ != end_of_storage_) {
            //还有备用空间
            if(position == finish_){
                construct(finish_,x);
                ++finish_;
            }else{
                //在备用空间开始处创建一个对象，并以vector的最后一个对象为初始值
                construct(finish_, *(finish_ - 1));
                ++finish_;
                copy_backward(position, finish_ - 2, finish_ - 1);
                //已经初始化了的内存上直接赋值就可以
                *position = x;
            }
        } else {
            const size_type old_size = size();
            //内存不足则申请原来两倍的新内存
            const size_type new_size = old_size != 0 ? 2 * old_size : 1;

            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start_, position, new_start);
                construct(new_finish ,x);  //未构造的内存，需要调用construct
                new_finish++;
                new_finish = uninitialized_copy(position, finish_, new_finish);
            } catch (...) {
                //出现异常，回滚
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }

            destroy(begin(), end());//销毁原来的vector内存空间
            deallocate();

            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + new_size;
        }
    }
}

#endif //MYTINYSTL_VECTOR_H
