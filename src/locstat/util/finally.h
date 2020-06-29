#ifndef FINALLY_H
#define FINALLY_H
#include <functional>
#include <iostream>
#include <utility>

namespace util
{
    class finally final
    {
    public:
        explicit finally(std::function<void(void)> &&func)
                : _func{std::move(func)}
        {}
        
        ~finally()
        { this->_func(); }
    
    private:
        std::function<void(void)> _func;
    };
}

#endif //FINALLY_H
