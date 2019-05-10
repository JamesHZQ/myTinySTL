//
// Created by john on 4/28/19.
//

#ifndef MYTINYSTL_ALLOC_H
#define MYTINYSTL_ALLOC_H

#include <cstdlib>

namespace mySTL{
    class alloc{
    private:
        enum{
            __ALIGN = 8,                     //最小对齐单位8字节（64位）
            __MAX_BYTES = 128,               //内存池最多提取128字节
            __NFREELIST = __MAX_BYTES/__ALIGN, //区块编号数16（8，16，24，。。。，128）
            __NOBJS = 20                     //一次分配20*（区块编号+1）*8字节的空间
        };

        //不需要额外空间存地址
        //obj未被真正使用时，只用来保存下一个obj空间的地址
        //当obj被分配出去使用时，原来obj保存地址的空间直接被覆盖
        union obj{
            union obj* free_list_next;
            char client[1];
        };

        //创建16位的表，通过相应编号可以找到对应大小区块内存的地址
        static obj* volatile free_list[__NFREELIST];

        //内存池上下边界位置
        static char* start_free;
        static char* end_free;
        static size_t heap_size;

        //根据申请的字节数确定对大小区块的编号（向上提升，8的倍数）
        static size_t FREELIST_INDEX(size_t bytes){
            return ((bytes+__ALIGN-1)/__ALIGN-1);
        }

        //bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes){
            return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
        }
        static void *refill(size_t bytes);

        static char* chunk_alloc(size_t bytes,size_t& nobjs);

    public:
        static void*    allocate(size_t bytes);
        static void     deallocate(void* ptr,size_t bytes);
        static void*    reallocate(void *ptr, size_t old_sz,size_t new_sz);

    };
}

#endif //MYTINYSTL_ALLOC_H


















