#ifndef LOC_INFO_H
#define LOC_INFO_H
#pragma once
#include <set>
#include <string>

namespace locstat
{
    class loc_info final
    {
    public:     // aliases
        using cmt_delim_set = std::set< std::string >;
        using file_ext_set = std::set< std::string >;
        using file_extension_t = std::string;
        using comment_delimiter_t = std::string;
        using language_t = std::string;
    
    public:     // static variables
        // use default language
        inline static const language_t no_lang{};
    
    public:     // ctor and dtor
        loc_info(const cmt_delim_set &delimiters,
                 const file_ext_set &extensions,
                 const language_t &lang = no_lang) noexcept(false);
        
        loc_info() noexcept(true);
        
        loc_info(const loc_info &ref) noexcept(true);
        
        loc_info(loc_info &&rvalue) noexcept(true);
        
        ~loc_info() noexcept(true);
    
    public:     // operator overload
        loc_info &operator=(const loc_info &ref) noexcept(true);
        
        loc_info &operator=(loc_info &&ref) noexcept(true);
    
    public:     // getters and setters
        void set_comment_delimiters(const cmt_delim_set &delimiters) noexcept(true)
        { *this->_comment_delimiters = delimiters; }
        
        void set_file_extensions(const file_ext_set &extensions) noexcept(false)
        {
            *this->_file_extensions = extensions;
            this->check_internal_extensions();
        }
        
        void set_language(const language_t &language) noexcept(true)
        { *this->_language = language; }
        
        void set_comment_delimiters(cmt_delim_set &&delimiters) noexcept(true)
        { *this->_comment_delimiters = std::move(delimiters); }
        
        void set_file_extensions(file_ext_set &&extensions) noexcept(true)
        { *this->_file_extensions = std::move(extensions); }
        
        void set_language(language_t &&language) noexcept(true)
        { *this->_language = std::move(language); }
        
        [[nodiscard]]
        cmt_delim_set &comment_delimiters() const noexcept(true)
        { return *this->_comment_delimiters; }
        
        [[nodiscard]]
        file_ext_set &file_extensions() const noexcept(true)
        { return *this->_file_extensions; }
        
        [[nodiscard]]
        std::string &language() const noexcept(true)
        { return *this->_language; }
    
    public:     // public helpers
        void add_comment_delimiter(const std::string &s) noexcept(true)
        { this->_comment_delimiters->insert(s); }
        
        void add_file_extension(const std::string &s) noexcept(true)
        { this->_file_extensions->insert(s); }
        
        [[nodiscard]]
        bool contains_extension(const file_extension_t &ext) const noexcept(true);
        
        [[nodiscard]]
        bool contains_delimiter(const comment_delimiter_t &delim) const noexcept(true)
        { return this->_comment_delimiters->count(delim); }
        
        [[nodiscard]]
        bool has_block_delimiter() const noexcept(true);
        
        [[nodiscard]]
        cmt_delim_set get_block_delimiters() const noexcept(true);
        
        [[nodiscard]]
        cmt_delim_set get_single_line_delimiters() const noexcept(true);
        
        [[nodiscard]]
        static bool is_block_delimiter(const comment_delimiter_t &token) noexcept(true);
    
        // Checks if an extension doesn't have invalid characters
        [[maybe_unused]]
        static bool is_valid_extension(const std::string &s) noexcept(false);
        
        [[nodiscard]]
        static comment_delimiter_t extract_starting_delimiter(const comment_delimiter_t &s) noexcept(false);
        
        [[nodiscard]]
        static comment_delimiter_t extract_ending_delimiter(const comment_delimiter_t &s) noexcept(false);


    private:    // private helpers
        // checks internal set if all elements are valid
        [[maybe_unused]]
        bool check_internal_extensions() noexcept(false);
        
        // Get the file just the file extension of a string without '.'
        [[nodiscard]]
        static file_extension_t extract_extension(const std::string &s) noexcept(true);
    

    
    private:    // private members
        cmt_delim_set *_comment_delimiters{};
        file_ext_set *_file_extensions{};
        language_t *_language{};
    };
}

#endif // LOC_INFO_H
