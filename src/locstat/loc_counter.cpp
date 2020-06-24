#include <fstream>
#include <iostream>
#include "loc_counter.h"
#include "ls_exeptions.h"

namespace locstat
{
    void loc_counter::operator()() const noexcept(false)
    {
        uint64_t blank_lines{};   // initialize to one
        uint64_t comment_lines{};
        uint64_t lines_of_code{};
        std::ifstream file{this->_path.string()};
        if (file.is_open()) {
            std::istreambuf_iterator<char> start{file};
            std::istreambuf_iterator<char> end{};
            std::string line;
            loc_info::cmt_delim_set single{this->_loc_info.get_single_line_delimiters()};
            loc_info::cmt_delim_set block{this->_loc_info.get_block_delimiters()};
            while (start != end) {
                
                ++start;
            }
    
            this->OUT_file_stats.set_code_lines(lines_of_code);
            this->OUT_file_stats.set_comment_lines(comment_lines);
            this->OUT_file_stats.set_blank_lines(blank_lines);
            this->OUT_file_stats.set_file_path(this->_path);
            std::cout << this->OUT_file_stats.lines_of_code() << ", "
                      << this->OUT_file_stats.comment_lines() << ", "
                      << this->OUT_file_stats.blank_lines() << std::endl;
        }
        else {
            file.close();
            throw invalid_file{std::string{"can't open file: " + this->_path.string()}.c_str()};
        }
        
        file.close();
    }
    
    bool loc_counter::file_is_good() const noexcept(false)
    {
        if (std::filesystem::is_directory(this->_path)) {
            throw invalid_file{"is a directory, needed a file"};
        }
    
        if (this->_loc_info.contains_extension(this->_path.extension().string())) {
            std::string s;
            for (auto &ref : this->_loc_info.file_extensions()) {
                s.append(ref + " ");
            }
            throw invalid_file{std::string{"invalid file, needed: " + s + "file"}.c_str()};
        }
        
        return true;
    }
}