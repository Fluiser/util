#ifndef UTIL_HPP
#define UTIL_HPP
#ifdef DEBUG
#include <iostream>
#include <string>
template<class... Args>
inline void sys_log(Args&&... args)
{
    auto t = {
        (std::cout << args, 0)...
    };

    (void)t;
    std::cout << "\n";
}
#else
#define sys_log(x) /* x */
#endif

