#include <cctype>
#include <iostream>
#include "loc_info.h"
#include "ls_exeptions.h"

namespace locstat
{
    loc_info::loc_info(const cmt_delim_set &delimiters,
                       const file_ext_set &extensions,
                       const language_t &lang) noexcept(false)
            : _comment_delimiters{new cmt_delim_set{delimiters}},
              _file_extensions{new file_ext_set{extensions}},
              _language{new language_t{lang}}
    { this->check_internal_extensions(); }
    
    loc_info::loc_info() noexcept(true)
            : _comment_delimiters{new cmt_delim_set{}},
              _file_extensions{new file_ext_set{}},
              _language{new std::string{}}
    {}
    
    loc_info::loc_info(const loc_info &ref) noexcept(true)
            : _comment_delimiters{new cmt_delim_set{*ref._comment_delimiters}},
              _file_extensions{new file_ext_set{*ref._comment_delimiters}},
              _language{new language_t{*ref._language}}
    {}
    
    loc_info::loc_info(loc_info &&rvalue) noexcept(true)
            : _comment_delimiters{rvalue._comment_delimiters},
              _file_extensions{rvalue._file_extensions},
              _language{rvalue._language}
    {
        rvalue._comment_delimiters = nullptr;
        rvalue._file_extensions = nullptr;
        rvalue._language = nullptr;
    }
    
    loc_info::~loc_info() noexcept(true)
    {
        delete this->_comment_delimiters;
        delete this->_file_extensions;
        delete this->_language;
    }
    
    loc_info &loc_info::operator=(const loc_info &ref) noexcept(true)
    {
        // clang tidy is complaining but it is handled correctly
        delete this->_file_extensions;
        delete this->_comment_delimiters;
        delete this->_language;
        
        // copy the the vectors, no need for special code, std::vector has copy constructor
        this->_file_extensions = new file_ext_set{*ref._file_extensions};
        this->_comment_delimiters = new cmt_delim_set{*ref._comment_delimiters};
        this->_language = new std::string{*ref._language};
        
        return *this;
    }
    
    loc_info &loc_info::operator=(loc_info &&rvalue) noexcept(true)
    {
        this->_file_extensions = rvalue._file_extensions;
        this->_comment_delimiters = rvalue._comment_delimiters;
        this->_language = rvalue._language;
        
        rvalue._file_extensions = nullptr;
        rvalue._comment_delimiters = nullptr;
        rvalue._comment_delimiters = nullptr;
        return *this;
    }
    
    bool loc_info::contains_extension(const loc_info::file_extension_t &ext) const noexcept(true)
    { return this->_file_extensions->count(loc_info::extract_extension(ext)); }
    
    bool loc_info::check_internal_extensions() noexcept(false)
    {
        for (auto &ref : *this->_file_extensions) {
            if (!loc_info::is_valid_extension(ref)) {
                return false;
            }
        }
        return true;
    }
    
    loc_info::file_extension_t loc_info::extract_extension(const std::string &s) noexcept(true)
    {
        std::string return_string{};
        for (uint64_t i{s.rfind('.') + 1}; i <= s.length(); ++i) {
            if (std::isalnum(s[i]) || s[i] == '_') {
                return_string.push_back(s[i]);
            }
        }
        return return_string;
    }
    
    bool loc_info::is_valid_extension(const std::string &s) noexcept(false)
    {
        if (s.find_first_of("\t\\/:*\"<>|\n") != std::string::npos) {
            throw invalid_characters{"invalid characters found in extension"};
        }
        return true;
    }
    
    bool loc_info::has_block_delimiter() const noexcept(true)
    {
        for (auto &ref : *this->_comment_delimiters) {
            if (loc_info::is_block_delimiter(ref)) {
                return true;
            }
        }
        return false;
    }
    
    loc_info::cmt_delim_set loc_info::block_delimiters() const noexcept(true)
    {
        cmt_delim_set block_delimiters{};
        if (this->has_block_delimiter()) {
            for (auto& ref : *this->_comment_delimiters) {
                if (loc_info::is_block_delimiter(ref)) {
                    block_delimiters.insert(ref);
                }
            }
        }
        return block_delimiters;
    }
    
    loc_info::cmt_delim_set loc_info::single_line_delimiters() const noexcept(true)
    {
        if (this->has_block_delimiter()) {
            cmt_delim_set single_lines_delimiters{};
            for (auto &ref : *this->_comment_delimiters) {
                if (!loc_info::is_block_delimiter(ref)) {
                    single_lines_delimiters.insert(ref);
                }
            }
            return single_lines_delimiters;
        }
        return *this->_comment_delimiters;
    }
    
    bool loc_info::is_block_delimiter(const loc_info::comment_delimiter_t &token) noexcept(true)
    {
        return (token.find("&&") != 0 && token.find("&&") != std::string::npos);
    }
    
    loc_info::comment_delimiter_t
    loc_info::extract_starting_delimiter(const loc_info::comment_delimiter_t &s) noexcept(false)
    {
        comment_delimiter_t starting;
        if (loc_info::is_block_delimiter(s)) {
            comment_delimiter_t::const_iterator iter{s.begin()};
            comment_delimiter_t::const_iterator end(s.begin() + s.find("&&"));
            for (; iter != end ; ++iter) {
                if (!std::isspace(*iter)) {
                    starting.push_back(*iter);
                }
            }
        } else {
            throw locstat::invalid_input{"invalid input, expected two tokens separated by \'&&\'."};
        }
        return starting;
    }
    
    loc_info::comment_delimiter_t
    loc_info::extract_ending_delimiter(const loc_info::comment_delimiter_t &s) noexcept(false)
    {
        comment_delimiter_t ending;
        if (loc_info::is_block_delimiter(s)) {
            comment_delimiter_t::const_iterator iter{s.begin()};
            std::advance(iter, s.find("&&") + 2);
            for (; iter != s.end(); ++iter) {
                if (!std::isspace(*iter)) {
                    ending.push_back(*iter);
                }
            }
        } else {
            throw locstat::invalid_input{"invalid input, expected two tokens separated by \"&&\"."};
        }
        return ending;
    }
}


