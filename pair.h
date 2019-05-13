//
// Created by john on 5/12/19.
//

#ifndef MYTINYSTL_UTILITY_H
#define MYTINYSTL_UTILITY_H

namespace mySTL {
    template<class T1, class T2>
    struct pair {
        typedef T1 first_type;
        typedef T2 second_type;

        T1 first;
        T2 second;
        pair() : first(T1()), second(T2()) {}
        pair(const T1& a, const T2& b) : first(a), second(b) {}
    };
}

#endif //MYTINYSTL_UTILITY_H
