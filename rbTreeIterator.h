//
// Created by john on 5/12/19.
//

#ifndef MYTINYSTL_RBTREEITERATOR_H
#define MYTINYSTL_RBTREEITERATOR_H

#include "iterator.h"
namespace mySTL {

    typedef bool __rb_tree_color_type;
    const __rb_tree_color_type __rb_tree_red = false;
    const __rb_tree_color_type __rb_tree_black = true;

    struct __rb_tree_node_base {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base* base_ptr;

        color_type  color;  //颜色（红/黑）
        base_ptr    parent; //父节点
        base_ptr    left;   //左孩子节点
        base_ptr    right;  //右孩子节点

        static base_ptr minimum(base_ptr x) {
            while (x->left != 0) x=x->left;
            return x;
        }

        static base_ptr maximum(base_ptr x) {
            while (x->right != 0) x=x->right;
            return x;
        }
    };

    //继承基础树节点，添加数据域
    template <typename Value>
    struct __rb_tree_node : public __rb_tree_node_base {
        typedef __rb_tree_node<Value>* link_type;
        Value value_field;
    };

    struct __rb_tree_base_iterator {
        typedef __rb_tree_node_base::base_ptr   base_ptr;
        typedef bidirectional_iterator_tag      iterator_category;
        typedef ptrdiff_t                       difference_type;

        //基础红黑树节点指针
        base_ptr node;

        //用于实现红黑树迭代器递增
        void increment() {
            //右子树存在，则将node移动到右子树最左边的节点
            if (node->right != 0) {
                node = node->right;
                while(node->left != 0)
                    node = node->left;
            } else {
                //右子树不存在，若node是父节点的左孩子，则node是父节点左子树最大的孩子，
                // node移动到自己的父节点，若node是父节点的右孩子，node就是以父节点为
                //根的树中最大的节点，然后一直往上找
                base_ptr y = node->parent;
                while(node == y->right) {
                    node = y;
                    y = y->parent;
                }
                if (node->right != y)
                    node = y;
            }
        }

        //用于实现红黑树迭代器递减（类似递增）
        void decrement() {
            //用于对尾迭代器（指向header节点）进行递减操作，此时迭代应该移动到树中最大的节点，即header的右孩子节点
            if (node->color == __rb_tree_red && node->parent->parent == node)
                node = node->right;
            else if (node->left != 0) {
                base_ptr y = node->left;
                while(y->right != 0) y = y->right;
                node = y;
            } else {
                base_ptr y = node->parent;
                while (node == y->left) {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }
    };

    template <typename Value, typename Ref, typename Ptr>
    struct __rb_tree_iterator : public __rb_tree_base_iterator {
        typedef Value   value_type;
        typedef Ref     reference;
        typedef Ptr     pointer;
        typedef __rb_tree_iterator<Value, Value&, Value*>               iterator;
        typedef __rb_tree_iterator<Value, const Value&, const Value*>   const_iterator;
        typedef __rb_tree_iterator<Value, Ref, Ptr>                     self;
        typedef __rb_tree_node<Value>*                                  link_type;

        ///注意node是基础节点的指针类型

        __rb_tree_iterator() {}
        __rb_tree_iterator(link_type x) {node = x;}
        __rb_tree_iterator(const iterator& it) { node = it.node;}

        reference operator*() const { return link_type(node)->value_field;}
        pointer operator->() const { return &(operator*());}

        self& operator++() {increment(); return *this;}
        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--() { decrement(); return *this;}
        self operator--(int) {
            self tmp = *this;
            decrement();
            return tmp;
        }
    };
    //__rb_tree_base_iterator，__rb_tree_iterator通用的相等比较函数
    //不用定义为成员函数
    inline bool operator==(const __rb_tree_base_iterator& x,
                           const __rb_tree_base_iterator& y) {
        return x.node == y.node;
    }

    inline bool operator!=(const __rb_tree_base_iterator& x,
                           const __rb_tree_base_iterator& y) {
        return x.node != y.node;
    }

    //左旋
    inline void
    __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
    {
        __rb_tree_node_base* y = x->right;
        x->right = y->left;
        if (y->left !=0)
            y->left->parent = x;
        y->parent = x->parent;

        if (x == root)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    //右旋
    inline void
    __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
    {
        __rb_tree_node_base* y = x->left;
        x->left = y->right;
        if (y->right != 0)
            y->right->parent = x;
        y->parent = x->parent;

        if (x == root)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    //插入调整平衡
    inline void
    __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
    {
        x->color = __rb_tree_red;
        while (x != root && x->parent->color == __rb_tree_red) {
            if (x->parent == x->parent->parent->left) {
                __rb_tree_node_base* y = x->parent->parent->right;
                if (y && y->color == __rb_tree_red) {
                    x->parent->color = __rb_tree_black;
                    y->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent;
                }
                else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        __rb_tree_rotate_left(x, root);
                    }
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_right(x->parent->parent, root);
                }
            }
            else {
                __rb_tree_node_base* y = x->parent->parent->left;
                if (y && y->color == __rb_tree_red) {
                    x->parent->color = __rb_tree_black;
                    y->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent;
                }
                else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        __rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x->parent->parent, root);
                }
            }
        }
        root->color = __rb_tree_black;
    }

    //删除调整平衡
    inline __rb_tree_node_base*
    __rb_tree_rebalance_for_erase(__rb_tree_node_base*  z,
                                  __rb_tree_node_base*& root,
                                  __rb_tree_node_base*& leftmost,
                                  __rb_tree_node_base*& rightmost)
    {
        __rb_tree_node_base* y = z;
        __rb_tree_node_base* x = 0;
        __rb_tree_node_base* x_parent = 0;
        if (y->left == 0)             // z has at most one non-null child. y == z.
            x = y->right;               // x might be null.
        else
        if (y->right == 0)          // z has exactly one non-null child.  y == z.
            x = y->left;              // x is not null.
        else {                      // z has two non-null children.  Set y to
            y = y->right;             //   z's successor.  x might be null.
            while (y->left != 0)
                y = y->left;
            x = y->right;
        }
        if (y != z) {                 // relink y in place of z.  y is z's successor
            z->left->parent = y;
            y->left = z->left;
            if (y != z->right) {
                x_parent = y->parent;
                if (x) x->parent = y->parent;
                y->parent->left = x;      // y must be a left child
                y->right = z->right;
                z->right->parent = y;
            }
            else
                x_parent = y;
            if (root == z)
                root = y;
            else if (z->parent->left == z)
                z->parent->left = y;
            else
                z->parent->right = y;
            y->parent = z->parent;

            auto temp = y->color;
            y->color = z->color;
            z->color = temp;

            y = z;
            // y now points to node to be actually deleted
        }
        else {                        // y == z
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            if (root == z)
                root = x;
            else
            if (z->parent->left == z)
                z->parent->left = x;
            else
                z->parent->right = x;
            if (leftmost == z)
                if (z->right == 0)        // z->left must be null also
                    leftmost = z->parent;
                    // makes leftmost == header if z == root
                else
                    leftmost = __rb_tree_node_base::minimum(x);
            if (rightmost == z)
                if (z->left == 0)         // z->right must be null also
                    rightmost = z->parent;
                    // makes rightmost == header if z == root
                else                      // x == z->left
                    rightmost = __rb_tree_node_base::maximum(x);
        }
        if (y->color != __rb_tree_red) {
            while (x != root && (x == 0 || x->color == __rb_tree_black))
                if (x == x_parent->left) {
                    __rb_tree_node_base* w = x_parent->right;
                    if (w->color == __rb_tree_red) {
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_left(x_parent, root);
                        w = x_parent->right;
                    }
                    if ((w->left == 0 || w->left->color == __rb_tree_black) &&
                        (w->right == 0 || w->right->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    } else {
                        if (w->right == 0 || w->right->color == __rb_tree_black) {
                            if (w->left) w->left->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_right(w, root);
                            w = x_parent->right;
                        }
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->right) w->right->color = __rb_tree_black;
                        __rb_tree_rotate_left(x_parent, root);
                        break;
                    }
                } else {                  // same as above, with right <-> left.
                    __rb_tree_node_base* w = x_parent->left;
                    if (w->color == __rb_tree_red) {
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_right(x_parent, root);
                        w = x_parent->left;
                    }
                    if ((w->right == 0 || w->right->color == __rb_tree_black) &&
                        (w->left == 0 || w->left->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    } else {
                        if (w->left == 0 || w->left->color == __rb_tree_black) {
                            if (w->right) w->right->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_left(w, root);
                            w = x_parent->left;
                        }
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->left) w->left->color = __rb_tree_black;
                        __rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            if (x) x->color = __rb_tree_black;
        }
        return y;
    }

}

#endif //MYTINYSTL_RBTREEITERATOR_H
