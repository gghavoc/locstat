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
        uint64_t code_lines{};
        std::ifstream file{this->_path.string()};
        if (file.is_open()) {
            loc_info::cmt_delim_set single{this->_loc_info.single_line_delimiters()};
            loc_info::cmt_delim_set block{this->_loc_info.block_delimiters()};
            for (auto &ref : single) {
                std::istreambuf_iterator<char> start{file};
                std::istreambuf_iterator<char> end{};
                std::string line;
                while (start != end) {
                    std::getline(file, line);   // consumes a line (excluding newline '\n')
                    
                    if (line.find(ref) != loc_info::comment_delimiter_t::npos) {
                        bool all_comment_in_line{true};
                        for (int pos = line.find(ref), i = 0; i < pos; ++i) {
                            if (!std::isspace(line[i])) {
                                all_comment_in_line = false;
                                break;
                            }
                        }
                        if (all_comment_in_line) {
                            ++comment_lines;
                        } else {
                            ++code_lines;
                        }
                    } else if (line.find_first_not_of(" \f\b\r\n\t\v") != loc_info::comment_delimiter_t::npos) {
                        ++code_lines;
                    } else {
                        ++blank_lines;
                    }
                    line.clear();
                }
            }
    
            this->OUT_file_stats.set_code_lines(code_lines);
            this->OUT_file_stats.set_comment_lines(comment_lines);
            this->OUT_file_stats.set_blank_lines(blank_lines);
            this->OUT_file_stats.set_file_path(this->_path);
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