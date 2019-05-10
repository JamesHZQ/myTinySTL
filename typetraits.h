//
// Created by john on 4/29/19.
//

#ifndef MYTINYSTL_TYPETRAITS_H
#define MYTINYSTL_TYPETRAITS_H

namespace mySTL {
    //定义两种类型，代表true、false，用于重载函数的参数匹配
    struct __true_type { };
    struct __false_type { };

    //定义_type_traits模板类，没有定义特化版本的T类型，全部类型特性均设为_false_type
    template<typename T>
    struct __type_traits{
        typedef __false_type		has_trivial_default_constructor;
        typedef __false_type		has_trivial_copy_constructor;
        typedef __false_type		has_trivial_assignment_operator;
        typedef __false_type		has_trivial_destructor;
        typedef __false_type		is_POD_type;
    };

    //定义若干POD对应T类型的特化版本，全部类型特性均设为_true_type
    template<>
    struct __type_traits<bool>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<char>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<unsigned char>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<signed char>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<wchar_t>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>

    struct __type_traits<short>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<unsigned short>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<int>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<unsigned int>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<long>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>

    struct __type_traits<unsigned long>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<long long>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<unsigned long long>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<float>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<double>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<long double>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };


    //定义若干POD对应T*类型的特化版本，全部类型特性均设为_true_type
    template<class T>
    struct __type_traits<T*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<class T>
    struct __type_traits<const T*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<unsigned char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<signed char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<const char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<const unsigned char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

    template<>
    struct __type_traits<const signed char*>{
        typedef __true_type		has_trivial_default_constructor;
        typedef __true_type		has_trivial_copy_constructor;
        typedef __true_type		has_trivial_assignment_operator;
        typedef __true_type		has_trivial_destructor;
        typedef __true_type		is_POD_type;
    };

}
#endif //MYTINYSTL_TYPETRAITS_H
