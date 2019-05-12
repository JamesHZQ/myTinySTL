//
// Created by john on 4/29/19.
//

#include "alloc.h"

#include <cassert>
#include <new>

namespace mySTL {
    template<typename T, typename Alloc>
    class simple_alloc {
    public:
        //分配n个T对象的空间，返回首地址
        static T *allocate(size_t n)
        { return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof(T));}

        //默认n=1
        static T *allocate(void)
        { return (T*) Alloc::allocate( sizeof(T) );}

        //归还n个T对象占有的空间，p指向归还空间的首地址
        static void deallocate(T *p, size_t n)
        { if(0 != n) Alloc::deallocate(p, n * sizeof(T));}

        //默认n=1
        static void deallocate(T *p)
        { Alloc::deallocate(p, sizeof (T));}
    };
}