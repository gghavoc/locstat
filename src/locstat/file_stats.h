#ifndef FILE_STATS_H
#define FILE_STATS_H
#include <cstdint>
#include <filesystem>

namespace locstat
{
    struct [[nodiscard]] file_stats final
    {
    public:     // aliases
        using file_path_t = std::filesystem::path;
        using file_name_t = std::string;

    public:     // ctor and dtor
        file_stats() = default;
        
        file_stats(file_path_t path,
                   const uint64_t &lines_of_code,
                   const uint64_t &comment_lines,
                   const uint64_t &blank_lines);

    public:     // setters and getters
        void set_file_path(const file_path_t &path)
        { this->_file_path = path; }
    
        void set_file_path(file_path_t &&path)
        { this->_file_path = std::move(path); }
    
        void set_code_lines(const uint64_t &loc)
        { this->_code_lines = loc; }
    
        void set_comment_lines(const uint64_t &comments)
        { this->_comment_lines = comments; }
    
        void set_blank_lines(const uint64_t &blanks)
        { this->_blank_lines = blanks; }
        
        [[nodiscard]]
        uint64_t total_lines() const noexcept(true)
        { return this->_code_lines + this->_blank_lines + this->_comment_lines; }
    
        [[nodiscard]]
        const file_path_t &file_path() const noexcept(true)
        { return this->_file_path; }
        
        [[nodiscard]]
        file_name_t file_name() const noexcept(true)
        { return this->_file_path.filename().string(); }
    
        [[nodiscard]]
        uint64_t code_lines() const noexcept(true)
        { return this->_code_lines; }
    
        [[nodiscard]]
        uint64_t comment_lines() const noexcept(true)
        { return this->_comment_lines; }
    
        [[nodiscard]]
        uint64_t blank_lines() const noexcept(true)
        { return this->_blank_lines; }
        
    private:     // member vars
        //  To store file path, file_path_t is an alias for std::filesystem::path
        file_path_t _file_path{};
        // Total lines excluding blank lines and comment lines
        uint64_t _code_lines{};
        // Lines that just contains comments
        uint64_t _comment_lines{};
        // Lines that just store space characters (newline, tab, blank space, etc)
        uint64_t _blank_lines{};
    };
}

#endif //FILE_STATS_H
