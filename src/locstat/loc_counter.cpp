#include <fstream>
#include <iostream>
#include <map>
#include "loc_counter.h"
#include "ls_exeptions.h"

namespace locstat
{
    void loc_counter::operator()() const noexcept(false)
    {
        uint64_t blank_lines{};
        uint64_t comment_lines{};
        uint64_t code_lines{};
        std::ifstream file{this->_path.string()};
        if (file.is_open()) {
            loc_info::cmt_delim_set single{this->_loc_info.single_line_delimiters()};
            loc_info::cmt_delim_set singular_block_tokens{this->_loc_info.separate_block_tokens()};
            std::istreambuf_iterator<char> start{file};
            std::istreambuf_iterator<char> end{};
            std::string line;
            std::map<int, locstat::loc_info::comment_delimiter_t> pos_map{};
            while (start != end) {
                std::getline(file, line);   // consumes a line (excluding newline '\n')
                
                bool line_has_comment{};
                loc_info::comment_delimiter_t comment_token{};
                
                // find the single line comment first
                for (auto &ref : single) {
                    uint64_t pos{};
                    if ((pos = line.find(ref)) != loc_info::comment_delimiter_t::npos) {
                        line_has_comment = true;
                        comment_token = ref;
                        pos_map.insert(std::make_pair(pos, ref));
                    }
                }
                
                // find out if a block comment token exists in a line
                for (auto &ref : singular_block_tokens) {
                    uint64_t pos{};
                    if ((pos = line.find(ref)) != loc_info::comment_delimiter_t::npos) {
                        line_has_comment = true;
                        comment_token = ref;
                        pos_map.insert(std::make_pair(pos, ref));
                    }
                }
                
                for (auto &[pos, token] : pos_map) {
                    std::cout << "pos token : "<< pos << " - "<< token << std::endl;
                }
                
                if (line_has_comment) {
                    // check if single line delimiter
                    if (this->_loc_info.is_single_line_token(comment_token)) {
                        if (loc_counter::is_only_blank_since_token(comment_token, line, DIRECTION::LEFT)) {
                            std::cout << line << " - [ single comment line ] - " << comment_token << std::endl;
                            ++comment_lines;
                        } else {
                            std::cout << line << " - [ code line - but has single line comment ]" << std::endl;
                            ++code_lines;
                        }
                    } else {
                        loc_info::comment_delimiter_t starting{};
                        loc_info::comment_delimiter_t ending{};
                        if (this->_loc_info.get_ending_token_of(comment_token).empty()) {
                            starting = this->_loc_info.get_starting_token_of(comment_token);
                            ending = comment_token;
                        } else {
                            starting = comment_token;
                            ending = this->_loc_info.get_ending_token_of(comment_token);
                        }
                        
                        // for single line block comment
                        if (loc_counter::is_only_blank_since_token(starting, line, DIRECTION::LEFT)
                            && loc_counter::is_only_blank_since_token(ending, line, DIRECTION::RIGHT)) {
                            std::cout << line << " - [ singular block comment ] - " << comment_token << std::endl;
                            ++comment_lines;
                        } else {
                            std::cout << line << " - [ code line - but has singular block comment ]" << std::endl;
                            ++code_lines;
                        }
                        
                        // for multiline block comment
                        while (line.find(ending) == loc_info::comment_delimiter_t::npos) {
                            std::getline(file, line);
                            
                            if (line.find(ending) != loc_info::comment_delimiter_t::npos
                                && !loc_counter::is_only_blank_since_token(ending, line, DIRECTION::RIGHT)
                                && !loc_counter::ln_contains_only_single_line_cmt
                                        (line.substr(ending.length() + 1))) {
                                std::cout << line << " - [ code line - but has multiline block comment ]" << std::endl;
                                ++code_lines;
                            } else {
                                std::cout << line << " - [ multiline block comment ] - "
                                          << comment_token << std::endl;
                                ++comment_lines;
                            }
                            if (start == end) break;
                        }
                    }
                } else if (line.find_first_not_of(" \f\b\r\n\t\v") != loc_info::comment_delimiter_t::npos) {
                    std::cout << line << " - [ code line ]" << std::endl;
                    ++code_lines;
                } else {
                    std::cout << line << " - [ blank line ]" << std::endl;
                    ++blank_lines;
                }
                // cleanup
                line.clear();
                pos_map.clear();
            }
            
            this->OUT_file_stats.set_code_lines(code_lines);
            this->OUT_file_stats.set_comment_lines(comment_lines);
            this->OUT_file_stats.set_blank_lines(blank_lines);
            this->OUT_file_stats.set_file_path(this->_path);
        } else {
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
    
    bool loc_counter::is_only_blank_since_token(const std::string &token,
                                                const std::string &line,
                                                loc_counter::DIRECTION direction) noexcept(true)
    {
        int64_t pos{};
        
        // check if token exists
        if (((uint64_t) (pos = line.find(token))) == std::string::npos) {
            return false;
        }
        
        if (direction == DIRECTION::RIGHT) {        // check right
            pos += token.length();
            while ((uint64_t) pos < line.length()) if (!std::isspace(line[pos++])) return false;
        } else if (direction == DIRECTION::LEFT) {  // check left
            --pos;
            while (pos >= 0) if (!std::isspace(line[pos--])) return false;
        } else if (direction == DIRECTION::BOTH) {
            // check right
            int64_t r_pos{(int64_t) (pos + token.length())};
            while ((uint64_t) r_pos < line.length()) if (!std::isspace(line[r_pos++])) return false;
            
            // check left
            int64_t l_pos{pos - 1};
            while (l_pos >= 0) if (!std::isspace(line[l_pos--])) return false;
        }
        
        return true;
    }
    
    bool loc_counter::ln_contains_only_single_line_cmt(const std::string &line) const noexcept(true)
    {
        for (auto &ref : this->_loc_info.single_line_delimiters()) {
            if (!loc_counter::is_only_blank_since_token(ref, line, DIRECTION::LEFT)) {
                return false;
            }
        }
        
        return true;
    }
}