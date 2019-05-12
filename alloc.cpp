#include "alloc.h"

namespace mySTL{
    //初始化静态成员变量
    char* alloc::start_free = 0;
    char* alloc::end_free = 0;
    size_t alloc::heap_size = 0;

    //各区块起始位置初始化为零
    alloc::obj* volatile alloc::free_list[alloc::__NFREELIST] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    //申请bytes个字节的空间
    void* alloc::allocate(size_t bytes){
        //若bytes>128，则直接通过malloc向系统申请
        if(bytes > __MAX_BYTES){
            return malloc(bytes);
        }

        //若bytes不超过128，先确定适合提供内存空间的区块的编号
        size_t  index = FREELIST_INDEX(bytes);

        //my_list指向相应区块
        obj* my_list = free_list[index];

        //相应区块的空间已经用完，调用refill给相应区块分配新的空间
        if(my_list == 0){
            void* r = refill(ROUND_UP(bytes));
            return r;
        }

        //free_list对应标号指向下一个可用空间
        free_list[index] = my_list->free_list_next;
        return my_list;
    }

    void alloc::deallocate(void *ptr, size_t bytes) {
        //bytes大于128说明是从系统得到的，直接归还给系统即可
        if(bytes > __MAX_BYTES){
            free(ptr);
            return ;
        }

        //ptr指向的bytes个字节的空间是从内存池得到的
        //现在将其归入内存池
        //得到相应区块标号
        size_t index = FREELIST_INDEX(bytes);
        //将归还空间的指针强制转为obj类型
        obj* node = static_cast<obj*>(ptr);

        //类似于头插法node->free_list_next指向当前（会被首先使用）的一块内存的首地址
        //fefree_list[index]指向归还内存的首地址，归还内存成为会被首先使用的内存
        node->free_list_next = free_list[index];
        free_list[index] = node;
    }

    //归还旧内存，重新分配一块更大的内存
    void* alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz) {
        deallocate(ptr,old_sz);
        ptr = allocate(new_sz);
        return ptr;
    }

    //分配区块内存空间
    void* alloc::refill(size_t bytes) {
        size_t nobjs = __NOBJS;
        //chunk，在内存池中尽可能分配20个大小为bytes的区块
        //nobjs，引用传参，函数返回，保存分配到的区块数
        char* chunk = chunk_alloc(bytes,nobjs);
        //直接给free_list[FREELIST_INDEX(bytes)]赋值，修改相应编号位置指向的首块区块地址？
        obj* volatile *my_list = 0;
        obj* result = 0;
        obj* current_obj = 0;
        obj* next_obj;

        //内存池只能分配出一个bytes个字节的区块，直接把这个区块的首地址返回
        if(nobjs == 1){
            return chunk;
        }

        //my_list指向相应的编号位置
        my_list = free_list+FREELIST_INDEX(bytes);
        //result第一个区块，这是以后的返回值
        result = (obj*)(chunk);
        //使free_list里相应编号位置的指针指向第二区块
        *my_list = next_obj = (obj*)(chunk+bytes);
        //将剩余的区块全部连接起来（头插法）
        for(int i=1;;++i){
            //将从内存池中得到空间分成nobjs个obj并让它们像链表一样相连
            current_obj = next_obj;
            next_obj = (obj*)((char*)next_obj+bytes);
            if(nobjs-1 == i){
                //区块“链”尾端的free_list_next置0
                current_obj->free_list_next = 0;
                break;
            }
            current_obj->free_list_next = next_obj;
        }
        return result;
    }

    //从内存池中尽可能分配20个相应区块的空间
    //内存池一个相应区块的空间都分配不出，开辟新的内存池
    char* alloc::chunk_alloc(size_t bytes, size_t &nobjs) {
        char* result = 0;
        //需要的字节数
        size_t bytes_need = bytes*nobjs;
        //内存池剩余空间大小
        size_t bytes_left = end_free-start_free;

        //若内存池剩余的空间足够，一次分配bytes*nobjs个字节，
        //并将可用空间头指针后移bytes*nobjs
        if(bytes_left>bytes_need){
            result = start_free;
            start_free += bytes_need;
            return result;
            //若内存池没有足够的空间，但至少能分配一个区块
            //那就尽可能将剩余的空间分配出去
        }else if(bytes_left >= bytes){
            nobjs = bytes_left / bytes;
            bytes_need = nobjs * bytes;
            result = start_free;
            start_free += bytes_need;
            return result;
        }else{//若内存池一个obj大小的空间都无法分配
            //向系统申请2倍大小的bytes_need空间（ROUND_UP((heap_size >> 4))？？）
            size_t bytes_to_get = 2*bytes_need+ROUND_UP((heap_size >> 4));

            //旧内存池还剩一点零头空间 < bytes，剩余的零头空间一定是8的倍数，
            // 刚刚好是一个对应的区块的大小
            if(bytes_left>0){
                obj* volatile* my_list = free_list + FREELIST_INDEX(bytes_left);
                //将旧内存池的剩余空间（一个对应区块大小），插入到free_list相应编号位置下
                ((obj*)start_free)->free_list_next = *my_list;
                *my_list = (obj*)start_free;
            }

            //通过调用malloc创建一个新的内存池，start_free指向新内存池的首地址
            start_free = (char*)malloc(bytes_to_get);
            //malloc失败
            if(0 == start_free){
                obj* volatile* my_list = 0,*p = 0;
                //free_list的每个编号位置指向的区块“链”
                //bytes开始？还是bytes+_ALIGN开始？
                for(int i=bytes;i <= __MAX_BYTES;i += __ALIGN){
                    my_list = free_list + FREELIST_INDEX(i);
                    p = *my_list;
                    //free_list编号指向的区块存在
                    if(0 != p){
                        *my_list = p->free_list_next;
                        //将内存池的上下边界地址设为相应区块的首尾位置（此区块大小一般>bytes）
                        start_free = (char*)p;
                        end_free = start_free + i;
                        //改变内存池后再次调用chunk_alloc，能分配一个就退出了
                        return chunk_alloc(bytes,nobjs);
                    }
                }
                end_free = 0;
                //没有一级空间配置器
                //start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            }
            //新内存创建成功，再次调用一定可以分配20个相应区块出去
            //heap_size内存池总大小
            heap_size += bytes_to_get;
            //更新内存池尾地址
            end_free = start_free + bytes_to_get;
            return chunk_alloc(bytes,nobjs);
        }
    }
}





















