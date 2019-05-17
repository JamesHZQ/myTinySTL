//
// Created by john on 5/14/19.
//

#ifndef MYTINYSTL_SET_H
#define MYTINYSTL_SET_H

#include "rbTree.h"
#include <functional>

namespace mySTL{
    //std::less<Key>是一个函数对象类型，Compare()得到匿名的该函数对象，该对象接受两个参数，同过Key类型的<运算符，比较关键字的大小
    template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
    class set{
    public:
        typedef Key         key_type;
        typedef Key         value_type;
        typedef Compare     key_compare;
        typedef Compare     value_compare;
    private:
        //std::_Identity<value_type>是一个函数对象类型，该对象接受一个参数，返回该参数作为键值
        typedef rb_tree<key_type, value_type, std::_Identity<value_type>, key_compare, Alloc> rep_type;
        //以tbTree作为内部容器
        rep_type t; // red-black tree representing set
    public:
        typedef typename rep_type::const_pointer    pointer;
        typedef typename rep_type::const_pointer    const_pointer;
        typedef typename rep_type::const_reference  reference;
        typedef typename rep_type::const_reference  const_reference;
        typedef typename rep_type::const_iterator   iterator;
        typedef typename rep_type::const_iterator   const_iterator;
        typedef typename rep_type::size_type        size_type;
        typedef typename rep_type::difference_type  difference_type;

        set() : t(Compare()) {}
        explicit set(const Compare& comp) : t(comp) {}

        template <class InputIterator>
        set(InputIterator first, InputIterator last)
                : t(Compare()) { t.insert_unique(first, last); }

        template <class InputIterator>
        set(InputIterator first, InputIterator last, const Compare& comp)
                : t(comp) { t.insert_unique(first, last); }

        set(const set<Key, Compare, Alloc>& x) : t(x.t) {}

        set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
            t = x.t;
            return *this;
        }

        key_compare key_comp() const { return t.key_comp(); }
        value_compare value_comp() const { return t.key_comp(); }

        iterator begin() const { return t.begin(); }
        iterator end() const { return t.end(); }

        bool empty() const { return t.empty(); }
        size_type size() const { return t.size(); }
        size_type max_size() const { return t.max_size(); }
        void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }


        // insert/erase

        typedef  pair<iterator, bool> pair_iterator_bool;
        pair<iterator,bool> insert(const value_type& x) {
            pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
            return pair<iterator, bool>(p.first, p.second);
        }
        iterator insert(iterator position, const value_type& x) {
            typedef typename rep_type::iterator rep_iterator;
            //首先尝试在position指向的节点处插入x，如不能插入，还需按照二叉排序树的插入规则来插入x
            return t.insert_unique((rep_iterator&)position, x);
        }
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            t.insert_unique(first, last);
        }

        void erase(iterator position) {
            if(position == end())
                return;
            typedef typename rep_type::iterator rep_iterator;
            t.erase((rep_iterator&)position);
        }
        size_type erase(const key_type& x) {
            return t.erase(x);
        }
        void erase(iterator first, iterator last) {
            typedef typename rep_type::iterator rep_iterator;
            t.erase((rep_iterator&)first, (rep_iterator&)last);
        }
        void clear() { t.clear(); }

        // set operations:

        iterator find(const key_type& x) const { return t.find(x); }
        size_type count(const key_type& x) const { return t.count(x); }
        iterator lower_bound(const key_type& x) const {
            return t.lower_bound(x);
        }
        iterator upper_bound(const key_type& x) const {
            return t.upper_bound(x);
        }
        pair<iterator,iterator> equal_range(const key_type& x) const {
            return t.equal_range(x);
        }
        template <class __Key, class __Compare, class __Alloc>
        friend bool operator==(const set<__Key, __Compare, __Alloc>&, const set<__Key, __Compare, __Alloc>&);

        template <class __Key, class __Compare, class __Alloc>
        friend bool operator<(const set<__Key, __Compare, __Alloc>&, const set<__Key, __Compare, __Alloc>&);
    };


    template <class Key, class Compare, class Alloc>
    inline bool operator==(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
        return x.t == y.t;
    }

    template <class Key, class Compare, class Alloc>
    inline bool operator<(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
        return x.t < y.t;
    }
    template <class Key, class Compare, class Alloc>
    inline void swap(set<Key, Compare, Alloc>& x, set<Key, Compare, Alloc>& y) {
        x.swap(y);
    }

}


#endif //MYTINYSTL_SET_H
