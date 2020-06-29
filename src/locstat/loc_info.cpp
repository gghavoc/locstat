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
    {
        this->fix_extensions();
    }
    
    loc_info::loc_info() noexcept(true)
            : _comment_delimiters{new cmt_delim_set{}},
              _file_extensions{new file_ext_set{}},
              _language{new std::string{}}
    { this->fix_extensions(); }
    
    loc_info::loc_info(const loc_info &ref) noexcept(true)
            : _comment_delimiters{new cmt_delim_set{*ref._comment_delimiters}},
              _file_extensions{new file_ext_set{*ref._comment_delimiters}},
              _language{new language_t{*ref._language}}
    { this->fix_extensions(); }
    
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
    
    void loc_info::add_file_extension(const std::string &s) noexcept(true)
    {
        this->_file_extensions->insert(loc_info::extract_extension(s));
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
    
    loc_info::file_extension_t
    loc_info::extract_extension(const std::string &s) noexcept(true)
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
        if (s.find_first_of(".\t\\/:*\"<>|\n") != std::string::npos) {
            throw invalid_characters{"invalid characters found in extension \n"
                                     "these characters are not allowed: . < > * : \\ / | and space characters"};
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
    
    loc_info::cmt_delim_set
    loc_info::block_delimiters() const noexcept(true)
    {
        cmt_delim_set block_delimiters{};
        if (this->has_block_delimiter()) {
            for (auto &ref : *this->_comment_delimiters) {
                if (loc_info::is_block_delimiter(ref)) {
                    block_delimiters.insert(ref);
                }
            }
        }
        return block_delimiters;
    }
    
    loc_info::cmt_delim_set
    loc_info::single_line_delimiters() const noexcept(true)
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
        return (token.find("&&") != std::string::npos           // contains '&&'
                && token.find("&&") != 0                        // does not start with '&&'
                && token.find("&&") != (token.length() - 2));   // does not end with '&&'
    }
    
    loc_info::comment_delimiter_t
    loc_info::extract_starting_delimiter(const loc_info::comment_delimiter_t &s) noexcept(false)
    {
        comment_delimiter_t starting;
        if (loc_info::is_block_delimiter(s)) {
            comment_delimiter_t::const_iterator iter{s.begin()};
            comment_delimiter_t::const_iterator end(s.begin() + s.find("&&"));
            for (; iter != end; ++iter) {
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
    
    loc_info::cmt_delim_vec
    loc_info::separate_tokens() const noexcept(true)
    {
        cmt_delim_vec tokens{};
        for (auto &ref : *this->_comment_delimiters) {
            if (loc_info::is_block_delimiter(ref)) {
                tokens.push_back(loc_info::extract_starting_delimiter(ref));
                tokens.push_back(loc_info::extract_ending_delimiter(ref));
                continue;
            }
            tokens.push_back(ref);
        }
        return tokens;
    }
    
    loc_info::cmt_delim_vec
    loc_info::separate_block_tokens() const noexcept(true)
    {
        cmt_delim_vec tokens{};
        for (auto &ref : this->block_delimiters()) {
            if (loc_info::is_block_delimiter(ref)) {
                tokens.push_back(loc_info::extract_starting_delimiter(ref));
                tokens.push_back(loc_info::extract_ending_delimiter(ref));
            }
        }
        return tokens;
    }
    
    loc_info::comment_delimiter_t
    loc_info::get_starting_token_of(const loc_info::comment_delimiter_t &token) const noexcept(true)
    {
        comment_delimiter_t ending{};
        for (auto &ref : this->block_delimiters()) {
            if (ref.rfind(token) != comment_delimiter_t::npos && ref.rfind(token) != 0) {
                ending = loc_info::extract_starting_delimiter(ref);
                return ending;
            }
        }
        return ending;
    }
    
    loc_info::comment_delimiter_t
    loc_info::get_ending_token_of(const loc_info::comment_delimiter_t &token) const noexcept(true)
    {
        comment_delimiter_t starting{};
        for (auto &ref : this->block_delimiters()) {
            if (ref.find(token) != comment_delimiter_t::npos && ref.find(token) != ref.length() - token.length()) {
                starting = loc_info::extract_ending_delimiter(ref);
                return starting;
            }
        }
        return starting;
    }
    
    loc_info::cmt_delim_set loc_info::starting_tokens() const noexcept(true)
    {
        cmt_delim_set s_tokens{};
        for (auto &ref : this->block_delimiters()) {
            s_tokens.insert(loc_info::extract_starting_delimiter(ref));
        }
        return s_tokens;
    }
    
    loc_info::cmt_delim_set loc_info::ending_tokens() const noexcept(true)
    {
        cmt_delim_set e_tokens{};
        for (auto &ref : this->block_delimiters()) {
            e_tokens.insert(loc_info::extract_ending_delimiter(ref));
        }
        return e_tokens;
    }
    
    bool loc_info::is_single_line_token(const comment_delimiter_t &s) const noexcept(true)
    {
        return this->single_line_delimiters().count(s);
    }
    
    void loc_info::fix_extensions() noexcept(true)
    {
        for (const auto &ref : *this->_file_extensions) {
            if (ref.find('.') != comment_delimiter_t::npos) {
                this->_file_extensions->erase(ref);
                this->_file_extensions->insert(loc_info::extract_extension(ref));
            }
        }
    }
}