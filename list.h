//
// Created by john on 5/11/19.
//

#ifndef MYTINYSTL_LIST_H
#define MYTINYSTL_LIST_H

#include "allocator.h"
#include "construct.h"
#include "algorithm.h"
#include "listIterator.h"

namespace mySTL {
    template <class T, class Alloc = alloc>
    class list {
    public:
        typedef T                               value_type;
        typedef value_type*                     pointer;
        typedef value_type&                     reference;
        typedef __list_node<T>                  list_node;
        typedef size_t                          size_type;
        typedef ptrdiff_t                       difference_type;
        typedef list_node*                      link_type;
        typedef simple_alloc<list_node, Alloc>  list_node_allocator;
        typedef __list_iterator<T, T&, T*>      iterator;

        //分配一个节点的空间，返回首地址
        link_type get_node() {
            return list_node_allocator::allocate();
        }
        //归还一个节点的空间
        void put_node(link_type x) {
            list_node_allocator::deallocate(x);
        }
        //以值x，构造一个节点
        link_type create_node(const T& x) {
            link_type newListNode = get_node();
            construct(&newListNode->data, x); //所指的数据，创建
            return newListNode;
        }
        //对结点执行相应的析构函数，并归还内存空间
        void destroy_node(link_type d) {
            destroy(&d->data);
            put_node(d);
        }

        iterator begin() const {return node->next;}
        iterator end() const {return node;}
        bool empty() const {return node->next == node;}
        size_type size() const {
            return static_cast<size_type >(distance(begin(), end()));
        }
        reference front() {return *begin();}
        reference back() {return *(--end());}

        //产生一个空链表
        list() { empty_initialize();}
        list(size_type n, const T& value) { fill_initialize(n, value); }
        list(int n, const T& value) { fill_initialize(n, value); }
        list(long n, const T& value) { fill_initialize(n, value); }
        explicit list(size_type n) { fill_initialize(n, T()); }
        template <class InputIterator>
        list(InputIterator first, InputIterator last) {
            range_initialize(first, last);
        }
        list(const list<T, Alloc>& x) {
            range_initialize(x.begin(), x.end());
        }

        ~list() {
            clear();
            //销毁空（哨兵）节点
            erase(end());
        }
        //尾部插入节点
        void push_back(const T& x) {
            insert(end(), x);
        }
        //头部插入节点
        void push_front(const T& x) {
            insert(begin(), x);
        }
        //删除头部节点
        void pop_front() {
            erase(begin());
        }
        //删除尾部节点
        void pop_back() {
            erase(--end());
        }

        //清除所有list节点
        void clear() {
            iterator cur = begin();
            while(cur != end()) {
                cur = erase(cur);
            }
            //空list的状态
            node->next=node;
            node->prev=node;
        }

        //删除所有值为x的节点
        void remove(const T& x) {
            iterator cur = begin();
            while(cur != end()) {
                if(*cur == x)
                    cur = erase(cur);
                else
                    cur++;
            }
        }

        //移除连续并相同的元素
        void unique() {
            iterator cur = begin();
            iterator next = cur;
            if(cur == end()) return;  //空链表
            while(++next != end()) {
                if (*cur == *next)
                    erase(next);
                else
                    cur = next;
                next = cur;
            }
        }

        iterator erase(iterator position) {
            link_type next_node = position.node->next;
            link_type prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            //真正销毁要删除的节点
            destroy_node(position.node);
            //返回删除节点的下一个节点的迭代器
            //由原始指针初始化得到迭代器
            return iterator(next_node);
        }

        iterator insert(iterator position, const T& x) {
            link_type tmp = create_node(x);
            //插入节点tmp，插在position指向的节点之前
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            position.node->prev->next = tmp;
            position.node->prev = tmp;
            //返回指向插入节点的迭代器
            return tmp;
        }
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last){
            for ( ; first != last; ++first)
                insert(position, *first);
        }
        void insert(iterator position, size_type n, const T& x){
            for ( ; n > 0; --n)
                insert(position, x);
        }
        void insert(iterator pos, int n, const T& x) {
            insert(pos, (size_type)n, x);
        }
        //将[first,last)之间的元素移动到position之前
        void transfer(iterator position, iterator first, iterator last) {
            if(first==last)
                return;
            if(position != last){
                //要移动的节点的最后一个节点连接->position.node指向的节点
                //（last）指向的元素不用移动，或者说last.node指向的节点不用移动
                last.node->prev->next = position.node;
                //要移动的节点的前一个结点连接到->last.node
                first.node->prev->next = last.node;
                //position指向的节点前一个节点连接->first.node
                position.node->prev->next = first.node;
                link_type tmp = position.node->prev;
                position.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
            }
        }

        //将list：x拼接到本list的position处
        void splice(iterator position, list& x) {
            if(x.empty())
                return;
            transfer(position, x.begin(), x.end());
        }

        //将i指向的元素拼接到position处
        void splice(iterator position, iterator i) {
            iterator j = i;
            j++;
            transfer(position, i, j);
        }

        //同transfer
        void splice(iterator position, iterator first, iterator last) {
            if(position == last)
                return;
            transfer(position, first, last);
        }

        //类似于归并排序的merge操作，两个list都是递增的
        void merge(list& x) {
            iterator l1cur= begin();
            iterator l2cur = x.begin();

            while(l1cur != end() && l2cur != x.end()) {
                if(*l2cur<*l1cur){
                    iterator next = l2cur;
                    transfer(l1cur,l2cur,++next);
                    l2cur = next;
                }else{
                    ++l1cur;
                }

            }

            //如果x没完就添加到尾巴上
            if(!x.empty())
                transfer(end(),l2cur,x.end());
        }

        //reverse颠倒顺序
        void reverse() {
            //只有空（哨兵）节点，或只有一个节点直接退出
            if(node->next == node||node->next->next == node)
                return;

            iterator cur = begin();
            ++cur;
            while(cur != end()) {
                iterator tmp = cur;
                ++cur;
                transfer(begin(),tmp,cur);
            }
        }

        //与x交换成员 交换完成后原来两个list上的迭代器要注意
        void swap(list& x) {
            link_type tmp = x.node;
            x.node = this->node;
            this->node = tmp;
        }

        //sort list不能使用STL的sort，因为迭代器不是random的
        //这里使用的是冒泡排序
        void sort() {
            if(node->next == node||node->next->next == node)
                return;
            for(iterator it = begin();it!=end();++it)
                for(iterator itt = (--end());itt!=it;){
                    iterator tmp = itt;
                    if(*tmp<*(--itt)){
                        itt.node->prev->next = tmp.node;
                        tmp.node->prev = itt.node->prev;
                        itt.node->prev = tmp.node;
                        itt.node->next = tmp.node->next;
                        tmp.node->next->prev = itt.node;
                        tmp.node->next = itt.node;
                        if(itt == it)
                            it = tmp;
                        itt = tmp;
                    }
                }
        }

    protected:
        //list是一个环状双向链表，一个指针即可表示整个环状双向链表，指向尾端的空白节点
        link_type node;

        void empty_initialize() {
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        void fill_initialize(size_type n, const T& value) {
            empty_initialize();
            try {
                    insert(begin(), n, value);
            }catch(...){
                clear();
                put_node(node);
                throw;
            }
        }

        template <class InputIterator>
        void range_initialize(InputIterator first, InputIterator last) {
            empty_initialize();
            try{
                insert(begin(), first, last);
            }catch(...){
                clear();
                put_node(node);
                throw;
            }
        }
    };
}

#endif //MYTINYSTL_LIST_H
