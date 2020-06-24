#ifndef INVALID_FILE_H
#define INVALID_FILE_H
#include <exception>
#include <cstdio>
#include <cstring>

namespace locstat
{
    class invalid_file final : std::exception
    {
    public:
        invalid_file();
    
        explicit invalid_file(const char* what)
                : _what(new char [strlen(what) + 1])
        { memcpy(this->_what, what, strlen(what)); }
    
        ~invalid_file() override
        { delete[] this->_what; }

    public:
        [[nodiscard]]
        const char * what() const noexcept(true) override
        { return this->_what; }

    private:
        char* _what{};
    
    };
}

#endif //INVALID_FILE_H
