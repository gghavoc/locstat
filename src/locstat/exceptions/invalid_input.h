#ifndef INVALID_INPUT_H
#define INVALID_INPUT_H
#include <exception>
#include <cstdio>
#include <cstring>

namespace locstat
{
    class invalid_input final : std::exception
    {
    public:
        invalid_input();
        
        explicit invalid_input(const char *what)
                : _what(new char[strlen(what) + 1])
        { memcpy(this->_what, what, strlen(what)); }
        
        ~invalid_input() override
        { delete[] this->_what; }
    
    public:
        [[nodiscard]]
        const char *what() const noexcept(true) override
        { return this->_what; }
    
    private:
        char *_what{};
    };
}

#endif //INVALID_INPUT_H
