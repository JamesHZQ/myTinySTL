//
// Created by john on 5/5/19.
//

#ifndef MYTINYSTL_MEMORY_H
#define MYTINYSTL_MEMORY_H

#include<atomic>
#include<functional>
#include<memory>

namespace mySTL {
    //默认删除器（函数类型）
    template<typename T>
    struct _default_delete{
        void operator()(T* ptr){
            if(ptr)
                delete ptr;
        }
    };
/**
 *上一版本引用计数存在线程安全的问题（先对引用计数“-1”，再判断引用计数是否减为零
 *若为零的话，销毁指向的对象。在多线程中，假设有两个线程AB各自的shared_ptr指向同
 *一个对象，线程A对引用计数“-1”后，此时发生了线程切换，线程B的shared_ptr也析构了
 *引用计数此时减为零，于是销毁指向的对象，之后由线程B切换回线程A后，会发生core dump）

 *所以为了解决这一问题，选择使用__sync_getch_and_add函数来更新并判断引用计数，
 *__sync_add_and_fetch使引用计数“+1”或“-1”并返回此次更新后的引用计数，这两个操作
 *之间，不会发生线程切换，所以不会出现上述的线程安全的问题。
*/

    template<typename T>
    struct ref_t{
        using deleter_type = std::function<void(T*)>;
        //std::atomic<size_t> ncount_;   //使用原子量，确保引用计数的线程安全性
        volatile size_t ncount_;
        T*              data_;
        deleter_type    deleter_;
        
        explicit ref_t(T*p=nullptr,deleter_type pfunc = deleter_type(_default_delete<T>()))
            : ncount_(0),data_(p),deleter_(pfunc)
        {
            if(data_)
                ncount_ = 1;
        }
        ref_t(const ref_t&) = delete;
        ref_t& operator=(const ref_t&) = delete;

        ~ref_t(){
            //--ncount_;
            if(ncount_ == 0)
                deleter_(data_);
        }
        size_t count()const{ return ncount_; }
        T* get_data()const{ return data_; }
        //前置
        ref_t& operator++(){
            ++ncount_;
            return *this;
        }
        //后置
        ref_t operator++(int){
            auto t = *this;
            ++*this;
            return t;
        }
        //前置
        ref_t& operator--(){
            --ncount_;
            return *this;
        }
        //后置
        ref_t operator--(int){
            auto t = *this;
            --*this;
            return t;
        }
        size_t decrement(){
            return __sync_add_and_fetch(&ncount_,-1);
        }
        size_t increment(){
            return __sync_add_and_fetch(&ncount_,1);
        }
    };
    template<typename T>
    bool operator==(const ref_t<T>& lhs,const ref_t<T>& rhs){
        return lhs.get_data() == rhs.get_data();
    }
    template<typename T>
    bool operator!=(const ref_t<T>&lhs,const ref_t<T>& rhs){
        return !(lhs==rhs);
    }

    template<typename T>
    struct default_delete{
        void operator()(T* ptr){
            if(ptr)
                delete ptr;
        }
    };
    template<typename T>
    struct default_delete<T[]>{
        void operator()(T* ptr){
            if(ptr)
                delete[] ptr;
        }
    };

    template<typename T,typename D = default_delete<T>>
    class unique_ptr{
    public:
        typedef T  element_type;
        typedef D  deleter_type;
        typedef T* pointer;

        explicit unique_ptr(T* data=nullptr):data_(data){}
        unique_ptr(T* data, deleter_type del):data_(data),deleter_(del){}

        //unique_ptr可进行移动拷贝和赋值
        unique_ptr(unique_ptr&& up):data_(nullptr){
            using std::swap;
            swap(data_,up.data_);
        }
        unique_ptr& operator=(unique_ptr&& up){
            if(&up!=this){
                //释放自己持有的指针
                clean();
                //接管up的指针
                swap(up);
            }
            return *this;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        ~unique_ptr(){
            clean();
        }
        const pointer get()const{ return data_; }
        pointer get(){ return data_; }

        deleter_type& get_deleter(){ return deleter_; }
        const deleter_type& get_deleter()const{ return deleter_; }

        operator bool()const{ return get()!=nullptr; }

        //放弃对原始指针的控制
        pointer release(){
            T* p=nullptr;
            using std::swap;
            swap(p,data_);
            return p;
        }
        
        void reset(pointer p = nullptr){
            clean();
            data_ = p;
        }

        void swap(unique_ptr& up){
            using std::swap;
            swap(data_,up.data_);
        }

        const element_type& operator*()const{ return *data_; }
        const pointer operator->()const{ return data_; }
        element_type& operator*(){ return *data_; }
        pointer operator->(){ return data_; }

    private:
        void clean(){
            deleter(data_);
            data_ = nullptr;
        }
        pointer      data_;
        deleter_type deleter_;
    };

    template<typename T,typename D>
    void swap(unique_ptr<T, D>& x,unique_ptr<T, D>& y){
        x.swap(y);
    }

    template<typename T1, typename D1, typename T2, typename D2>
    bool operator==(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs){
        return lhs.get()==rhs.get();
    }

    template<typename T, typename D>
    bool operator==(const unique_ptr<T, D>&up, nullptr_t p){
        return up.get()==p;
    }
    template<typename T, typename D>
    bool operator==(nullptr_t p, const unique_ptr<T, D>&up){
        return up.get()==p;
    }

    template<typename T1, typename D1, typename T2, typename D2>
    bool operator!=(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs){
        return !(lhs==rhs);
    }

    template<typename T, typename D>
    bool operator!=(const unique_ptr<T, D>&up, nullptr_t p){
        return !(up == p);
    }
    template<typename T, typename D>
    bool operator!=(nullptr_t p, const unique_ptr<T, D>&up){
        return !(up == p);
    }

    template<typename T,typename... Args>
    unique_ptr<T>make_unique(Args&&... args){
        //转发参数包给T类型的构造函数
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
    template<typename T>
    class shared_ptr{
    public:
        typedef T element_type;

        explicit shared_ptr(T* p=nullptr){
            if(p!=nullptr)
                ref_=(new ref_t<T>(p));
        }
        template<typename D>
        shared_ptr(T* p, D del):ref_(new ref_t<T>(p,del)){}
        shared_ptr(const shared_ptr& sp){
            copy_ref(sp.ref_);
        }
        shared_ptr& operator=(const shared_ptr& sp){
            if(this != &sp){
                decrease_ref();
                copy_ref(sp.ref_);
            }
            return *this;
        }

        ~shared_ptr(){ decrease_ref(); }

        const element_type& operator*()const{ return *(get()); }
         const element_type* operator->()const{ return get(); }
        element_type& operator*(){ return *(get()); }
        element_type* operator->(){ return get(); }

        const element_type* get()const{ return ref_==nullptr?nullptr:ref_->get_data();} 
        element_type* get(){ return ref_==nullptr?nullptr:ref_->get_data(); }
        size_t use_count()const{ return ref_==nullptr?0:ref_->count(); }

        operator bool()const{ return get()!=nullptr; }
    private:
        //递减引用计数，并在引用计数减到0时销毁ref_，
        //ref_的析构函数会销毁真正的对象
        void decrease_ref(){
            if(ref_!=nullptr){
                if(ref_->decrement()==0)
                    delete ref_;
            }
        }
        //拷贝时改变指针指向，并递增新的ref_的引用计数
        //（同时要调用decrease_ref递减原来的ref_的引用计数）
        void copy_ref(ref_t<T>* r){
            ref_ = r;
            if(ref_!=nullptr)
                ++(*ref_);
        }
        ref_t<T> *ref_=nullptr;
    };
    template<typename T1, typename T2>
    bool operator==(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs){
        return lhs.get() == rhs.get();
    }
    template<typename T>
    bool operator==(const shared_ptr<T>& sp, nullptr_t p){
        return sp.get() == p;
    }
    template<typename T>
    bool operator==(nullptr_t p,const shared_ptr<T>& sp){
        return sp.get()==p;
    }
    template<typename T1, typename T2>
    bool operator!=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs){
        return !(lhs == rhs); 
    }
    template<typename T>
    bool operator!=(const shared_ptr<T>& sp,const nullptr_t p){
        return !(sp == p);
    }
    template<typename T>
    bool operator!=(const nullptr_t p,const shared_ptr<T>& sp){
        return !(sp == p);
    }
    template<typename T,typename... Args>
    shared_ptr<T> make_shared(Args... args){
        //转发参数包给T类型的构造函数
        return shared_ptr<T>(new T(std::forward<Args>(args)...));
    }
} 

#endif //MYTINYSTL_MEMORY_H