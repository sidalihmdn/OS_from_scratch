#ifndef KERNEL_CPP_UNIQUE_PTR_HPP
#define KERNEL_CPP_UNIQUE_PTR_HPP

#include <kernel/cpp/types.hpp>
#include <kernel/mem/heap.h>

inline void* operator new(unsigned long size, void* ptr) noexcept{
    return ptr;
}

namespace kernel {
    template<typename T>
    class UniquePtr {
        private :  T* ptr_;
        public : 
            explicit UniquePtr(T* p = nullptr) : ptr_(p) {}
            ~UniquePtr() {
                if (ptr_)
                    delete_object(ptr_);
            }

            UniquePtr(const UniquePtr& other) = delete;
            UniquePtr& operator=(const UniquePtr& other) = delete;

            UniquePtr(UniquePtr&& other) : ptr_(other.ptr_) {
                other.ptr_ = nullptr;
            }

            UniquePtr& operator=(UniquePtr&& other) {
                if (this != &other) {
                    if(ptr_){
                        delete_object(ptr_);
                    }
                    ptr_ = other.ptr_;
                    other.ptr_ = nullptr;
                }
                return *this;
            }

            /*
             *  Access operators
             */
            
            T* get() const {return ptr_;}
            T& operator*() const { return *ptr_;}
            T* operator->() const { return ptr_;}
            
            explicit operator bool() const { return ptr_ != nullptr;}

            T* release(){
                T* temp = ptr_;
                ptr_ = nullptr;
                return temp;
            }
             
            void reset(T* p = nullptr){
                if (ptr_){
                    delete_object(ptr_);
                }
                ptr_ = p;
            }
        private :
            static void delete_object(T* p){
                p->~T();
                kfree(p);
            }
    };
    template<typename T, typename... Args>
    UniquePtr<T> make_unique(Args&&... args){
        T* ptr  = static_cast<T*>(kmalloc(sizeof(T)));
        if(ptr){
            new (ptr) T(kernel::forward<Args>(args)...);
        }
        return UniquePtr<T>(ptr);
    }
}

#endif