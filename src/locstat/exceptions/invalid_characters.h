#ifndef INVALID_CHARACTERS_H
#define INVALID_CHARACTERS_H
#include <exception>
#include <string>
#include <cstring>
#include <cstdio>

namespace locstat
{
    class invalid_characters final : public std::exception
    {
    public:
        invalid_characters();
        
        explicit invalid_characters(const char *what)
                : _what(new char[strlen(what) + 1])
        { memcpy(this->_what, what, strlen(what)); }
        
        ~invalid_characters() override
        { delete[] this->_what; }
    
    public:
        [[nodiscard]]
        const char *what() const noexcept(true) override
        { return this->_what; }
    
    private:
        char *_what{};
    };
}

#endif //INVALID_CHARACTERS_H
