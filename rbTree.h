//
// Created by john on 5/12/19.
//

#ifndef MYTINYSTL_RBTREE_H
#define MYTINYSTL_RBTREE_H

#include "pair.h"
#include "rbTreeIterator.h"
#include "construct.h"
#include "allocator.h"

#include <algorithm>

namespace mySTL {

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc = alloc>
    class rb_tree {

    protected:
        typedef void*                               void_pointer;
        typedef __rb_tree_node_base*                base_ptr;
        typedef __rb_tree_node<Value>               rb_tree_node;
        typedef simple_alloc<rb_tree_node, Alloc>   rb_tree_node_allocator;
        typedef __rb_tree_color_type                color_type;

    public:
        typedef Key                                 key_type;
        typedef Value                               value_type;
        typedef value_type*                         pointer;
        typedef const value_type*                   const_pointer;
        typedef value_type&                         reference;
        typedef const value_type&                   const_reference;

        typedef rb_tree_node*                       link_type;
        typedef size_t                              size_type;
        typedef ptrdiff_t                           difference_type;

        typedef __rb_tree_iterator<value_type, reference, pointer>              iterator;
        typedef __rb_tree_iterator<value_type, const_reference, const_pointer>  const_iterator;

    protected:
        link_type get_node() {return rb_tree_node_allocator::allocate();}
        void put_node(link_type p) {rb_tree_node_allocator::deallocate(p);}
        link_type create_node(const value_type& x) {
            //分配节点空间
            link_type tmp = get_node();
            //构造节点
            construct(&tmp->value_field, x);
            return tmp;
        }

        //深拷贝
        link_type clone_node(link_type x) {
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right= 0;
            return tmp;
        }

        void destroy_node(link_type p) {
            destroy(&p->value_field);    //单独析构节点的value_field，其它的成员没有析构函数
            put_node(p);                 //归还内存
        }

        //rbtree 只以三笔数据表现
        size_type   node_count;     //关键节点数
        link_type   header;         //头结点指针，头节点与根结点互为父节点
        Compare     key_compare;    //关键节点比较方法（函数对象）

        //获取header的成员
        link_type& root() const {return (link_type&) header->parent;}
        link_type& leftmost() const {return (link_type&) header->left;}
        link_type& rightmost() const {return (link_type&) header->right;}

        //获取rb_tree_node*类型的x指向的节点的成员
        static link_type& left(link_type x){ return (link_type&)(x->left);}
        static link_type& right(link_type x){ return (link_type&)(x->right);}
        static link_type& parent(link_type x){ return (link_type&)(x->parent);}
        static reference value(link_type x){ return x->value_field;}
        //KeyOfValue()得到匿名函数对象，该函数对象接受一个值并返回值的键
        static const Key& key(link_type x){ return KeyOfValue() (value(x));}
        static color_type& color(link_type x){ return (color_type&) (x->color);}

        //获取__rb_tree_node_base*类型的x指向的节点的成员
        static link_type& left(base_ptr x){ return (link_type&)(x->left);}
        static link_type& right(base_ptr x){ return (link_type&)(x->right);}
        static link_type& parent(base_ptr x){ return (link_type&)(x->parent);}
        static reference value(base_ptr x){ return ((link_type)x)->value_field;}
        static const Key& key(base_ptr x){ return KeyOfValue() (value(link_type(x)));}
        static color_type& color(base_ptr x){ return (color_type&) (link_type(x)->color);}

        static link_type minimum (link_type x) {
            return (link_type) __rb_tree_node_base::minimum(x);
        }
        static link_type maximum(link_type x) {
            return (link_type) __rb_tree_node_base::maximum(x);
        }


    private:
        //内部insert、copy、erase
        iterator __insert(base_ptr x_, base_ptr y_, const value_type& v);
        link_type __copy(link_type x, link_type p);
        void __erase(link_type x);

        void init() {
            header = get_node();
            color(header) = __rb_tree_red; //header为红色，区分header与root

            root() = 0;
            leftmost() = header;
            rightmost()= header;
        }

    public:
        rb_tree(const Compare& comp = Compare())
                : node_count(0), key_compare(comp) { init();}

        ~rb_tree() {
            clear();            //销毁根结点下的所有节点
            put_node(header);   //归还header的空间，没有构造value无须析构
        }
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
        operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

        Compare key_comp() const {return key_compare;}
        iterator begin() { return leftmost(); }
        const_iterator begin() const { return leftmost(); }
        iterator end() { return header; }
        const_iterator end() const { return header; }
        bool empty() const { return node_count == 0;}
        size_type size() const {return node_count;}
        size_type max_size() const { return size_type(-1);}

        void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
            std::swap(header, t.header);
            std::swap(node_count, t.node_count);
            std::swap(key_compare, t.key_compare);
        }

        //不同的插入操作
        pair<iterator,bool> insert_unique(const value_type& x);
        iterator insert_equal(const value_type& x);
        iterator insert_unique(iterator position, const value_type& x);
        iterator insert_equal(iterator position, const value_type& x);
        template <typename InputIterator>
        void insert_unique(InputIterator first, InputIterator last);
        template <typename InputIterator>
        void insert_equal(InputIterator first, InputIterator last);


        void erase(iterator position);
        size_type erase(const key_type& x);
        void erase(iterator first, iterator last);
        void erase(const key_type* first, const key_type* last);
        void clear() {
            if (node_count != 0) {
                __erase(root());
                leftmost() = header;
                root() = 0;
                rightmost() = header;
                node_count = 0;
            }
        }

        //set、map相关的函数
        iterator find(const key_type& x);
        const_iterator find(const key_type& x) const;
        size_type count(const key_type& x) const;
        iterator lower_bound(const key_type& x);
        const_iterator lower_bound(const key_type& x) const;
        iterator upper_bound(const key_type& x);
        const_iterator upper_bound(const key_type& x) const;
        pair<iterator,iterator> equal_range(const key_type& x);
        pair<const_iterator, const_iterator> equal_range(const key_type& x) const;
    };
    //非成员的相等比较函数
    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
        return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                          const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
        //避免自赋值
        if (this != &x) {
            // Note that Key may be a constant type.
            clear();
            node_count = 0;
            key_compare = x.key_compare;
            if (x.root() == 0) {
                root() = 0;
                leftmost() = header;
                rightmost() = header;
            }
            else {
                //深拷贝
                root() = __copy(x.root(), header);
                leftmost() = minimum(root());
                rightmost() = maximum(root());
                node_count = x.node_count;
            }
        }
        return *this;
    }

    //x_为插入点，y为插入点的父节点,v为插入值
    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    __insert(base_ptr x_, base_ptr y_, const Value& v) {
        link_type x = (link_type) x_;
        link_type y = (link_type) y_;
        link_type z;

        if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
            //插入的值的键比父节点小，将插入节点挂在父节点的左边
            z = create_node(v);
            left(y) = z;
            if (y == header) {
                root() = z;
                rightmost() = z;            //更新最大节点
            }
            else if (y == leftmost())
                leftmost() = z;             //更新最小将节点
        }
        else {
            //插入的值的键比父节点大放在父节点右边
            z = create_node(v);
            right(y) = z;
            if (y == rightmost())
                rightmost() = z;            //更新最大节点
        }

        parent(z) = y;
        left(z) = 0;
        right(z) = 0;

        //插入节点后维护红黑树的性质
        __rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
    {
        link_type y = header;
        link_type x = root();
        //按二叉搜索树的原则，找到插入位置
        while (x != 0) {
            y = x;
            x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, v);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
    {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(KeyOfValue()(v), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j = iterator(y);
        //comp不为0表示找到的插入的值小于要插入的位置的父节点的值
        if (comp)
            if (j == begin())
                return pair<iterator,bool>(__insert(x, y, v), true);
            //如果父节点不是最小的节点，则插入的值的键可能等于其祖父节点，将j移动到祖父节点进行下面的比较
            else
                --j;
        //若j指向的节点（插入节点的父节点或祖父节点）小于插入的值的键，说明插入的值是唯一的
        if (key_compare(key(j.node), KeyOfValue()(v)))
            return pair<iterator,bool>(__insert(x, y, v), true);
        return pair<iterator,bool>(j, false);
    }
    template <typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const Val& v) {
        if (position.node == header->left) // begin()
            if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
                return __insert(position.node, position.node, v);
                // first argument just needs to be non-null
            else
                return insert_unique(v).first;
        else if (position.node == header) // end()
            if (key_compare(key(rightmost()), KeyOfValue()(v)))
                return __insert(0, rightmost(), v);
            else
                return insert_unique(v).first;
        else {
            iterator before = position;
            --before;
            if (key_compare(key(before.node), KeyOfValue()(v))
                && key_compare(KeyOfValue()(v), key(position.node)))
                if (right(before.node) == 0)
                    return __insert(0, before.node, v);
                else
                    return __insert(position.node, position.node, v);
                // first argument just needs to be non-null
            else
                return insert_unique(v).first;
        }
    }

    template <typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const Val& v) {
        if (position.node == header->left) // begin()
            if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
                return __insert(position.node, position.node, v);
                // first argument just needs to be non-null
            else
                return insert_equal(v);
        else if (position.node == header) // end()
            if (!key_compare(KeyOfValue()(v), key(rightmost())))
                return __insert(0, rightmost(), v);
            else
                return insert_equal(v);
        else {
            iterator before = position;
            --before;
            if (!key_compare(KeyOfValue()(v), key(before.node))
                && !key_compare(key(position.node), KeyOfValue()(v)))
                if (right(before.node) == 0)
                    return __insert(0, before.node, v);
                else
                    return __insert(position.node, position.node, v);
                // first argument just needs to be non-null
            else
                return insert_equal(v);
        }
    }
    template <typename K, typename V, typename KoV, typename Cmp, typename Al> template<typename II>
    void rb_tree<K, V, KoV, Cmp, Al>::insert_equal(II first, II last) {
        for ( ; first != last; ++first)
            insert_equal(*first);
    }

    template <typename K, typename V, typename KoV, typename Cmp, typename Al> template<typename II>
    void rb_tree<K, V, KoV, Cmp, Al>::insert_unique(II first, II last) {
        for ( ; first != last; ++first)
            insert_unique(*first);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    inline void
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
        //删除节点后，维护红黑树的性质
        link_type y = (link_type) __rb_tree_rebalance_for_erase(
                                      position.node, header->parent, header->left, header->right);
        destroy_node(y);
        --node_count;
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& x) {
        pair<iterator,iterator> p = equal_range(x);
        size_type n = 0;
        distance(p.first, p.second, n);
        erase(p.first, p.second);
        return n;
    }

    //将以x为根的树深拷贝到p节点下
    template <typename K, typename V, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<K, V, KeyOfValue, Compare, Alloc>::link_type
    rb_tree<K, V, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
        // structural copy.  x and p must be non-null.
        link_type top = clone_node(x);
        top->parent = p;

        try {
                if (x->right)
                top->right = __copy(right(x), top);
                p = top;
                x = left(x);

                while (x != 0) {
                    link_type y = clone_node(x);
                    p->left = y;
                    y->parent = p;
                    if (x->right)
                        y->right = __copy(right(x), y);
                    p = y;
                    x = left(x);
                }
        }catch(...){
            __erase(top);
            throw;
        }


        return top;
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x) {
        // erase without rebalancing
        while (x != 0) {
            __erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first,
                                                                iterator last) {
        if (first == begin() && last == end())
            clear();
        else
            while (first != last) erase(first++);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key* first,
                                                                const Key* last) {
        while (first != last) erase(*first++);
    }

    //查找的const与非const版
    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
        link_type y = header;        // Last node which is not less than k.
        link_type x = root();        // Current node.

        while (x != 0)
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);

        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
        link_type y = header; /* Last node which is not less than k. */
        link_type x = root(); /* Current node. */

        while (x != 0) {
            //y记录大于等于k的最近的节点
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);
        }
        const_iterator j = const_iterator(y);
        //若k也大于等于j指向的节点（y），说明k等于y节点值的键
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = 0;
        distance(p.first, p.second, n);
        return n;
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) {
        link_type y = header; /* Last node which is not less than k. */
        link_type x = root(); /* Current node. */

        while (x != 0)
            //y记录大于等于k的最浅的节点
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);

        return iterator(y);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) const {
        link_type y = header; /* Last node which is not less than k. */
        link_type x = root(); /* Current node. */

        while (x != 0)
            //y记录大于等于k的最浅的节点
            if (!key_compare(key(x), k))
                y = x, x = left(x);
            else
                x = right(x);

        return const_iterator(y);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) {
        link_type y = header; /* Last node which is greater than k. */
        link_type x = root(); /* Current node. */

        while (x != 0)
            //y记录大于等于k的最深的节点
            if (key_compare(k, key(x)))
                y = x, x = left(x);
            else
                x = right(x);

        return iterator(y);
    }

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) const {
        link_type y = header; /* Last node which is greater than k. */
        link_type x = root(); /* Current node. */

        while (x != 0)
            //y记录大于等于k的最深的节点
            if (key_compare(k, key(x)))
                y = x, x = left(x);
            else
                x = right(x);

        return const_iterator(y);
    }

    //lower_bound返回的迭代器的右边全是键大于等于k的节点（包含返回迭代器指向的节点）
    //upper_bound返回的迭代器的左边全是键小于等于k的节点（不包含返回迭代器指向的节点）
    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    inline pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
            typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }

    template <typename Key, typename Value, typename KoV, typename Compare, typename Alloc>
    inline pair<typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator,
            typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator>
    rb_tree<Key, Value, KoV, Compare, Alloc>::equal_range(const Key& k) const {
        return pair<const_iterator,const_iterator>(lower_bound(k), upper_bound(k));
    }

}

#endif //MYTINYSTL_RBTREE_H
