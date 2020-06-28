#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include "loc_counter.h"
#include "ls_exeptions.h"

namespace locstat
{
    void loc_counter::operator()() const noexcept(false)
    {
        std::ifstream file{this->_path.string()};
        if (file.is_open()) {
            uint64_t blank_lines{},
                    comment_lines{},
                    code_lines{};
            loc_info::cmt_delim_vec separate_tokens{this->_loc_info.separate_tokens()};
            
            std::istreambuf_iterator<char> start{file};
            std::string line;
            std::map<uint64_t, loc_info::comment_delimiter_t> pos_map;
            bool inside_block{}, has_code{}, has_comment{}, line_fully_read{true}, ok_to_break{false};
            loc_info::comment_delimiter_t token{};  // store current token
            uint64_t current_line{};
            while (!ok_to_break) {
                if (line_fully_read) {
                    ++current_line;
                    has_code = false;
                    if (!inside_block) has_comment = false;
                    std::getline(file, line);
                    std::cout << '[' <<std::setw(4)  << current_line << ']' << ": " << line << std::endl;
                }
                
                for (const auto &ref : separate_tokens) {
                    uint64_t pos{};
                    if ((pos = line.find(ref)) != loc_info::comment_delimiter_t::npos) {
                        pos_map.insert(std::make_pair(pos, ref));
                    }
                }
                
                if (inside_block) {
                    loc_info::comment_delimiter_t ending_token{this->_loc_info.get_ending_token_of(token)};
                    if (line.find(ending_token)
                        != loc_info::comment_delimiter_t::npos) {
                        std::cout << "in block: " << line << std::endl;
                        line = loc_counter::substr_after_token(ending_token, line);
                        inside_block = false;
                        has_comment = true;
                        if (!loc_counter::isblank(line)) line_fully_read = false;
                    } else {
                        line_fully_read = true;
                    }
                } else {
                    if (loc_counter::isblank(line)) {
                        line_fully_read = true;
                    } else if (!pos_map.empty()) {
                        token = pos_map.begin()->second;
                        std::cout << "token: " << token << std::endl;
                        has_comment = true;
                        if (loc_counter::is_blank_since_token(token, line, DIRECTION::LEFT)) {
                            line_fully_read = true;
                            std::cout << "found comment: " << line << std::endl;
                        } else {
                            has_code = true;
                        }
                        
                        if (!this->_loc_info.is_single_line_token(token)) {
                            std::cout << "found block token: " << token << std::endl;
                            inside_block = true;
                            line_fully_read = false;
                            line = loc_counter::substr_after_token(token, line);
                        } else {
                            std::cout << "found single line token: " << token << std::endl;
                            line = loc_counter::substr_before_token(token, line);
                        }
    
                    } else {
                        has_code = true;
                        line_fully_read = true;
                        std::cout << "code: " << line << std::endl;
                    }
                }
                std::cout << std::left << std::setw(20) << "line fully read: " << line_fully_read << std::endl;
                std::cout << std::left << std::setw(20) << "has code: " << has_code << std::endl;
                std::cout << std::left << std::setw(20) << "has comment: " << has_comment << std::endl;
                std::cout << std::left << std::setw(20) << "inside block: " << inside_block << std::endl;
                
                if (line_fully_read) {
                    if (!has_code && !has_comment) {
                        ++blank_lines;
                        line_fully_read = true;
                        std::cout << "+++++++ incremented blank lines" << std::endl;
                    } else if (has_comment && !has_code) {
                        std::cout << "+++++++ incremented comment lines" << std::endl;
                        ++comment_lines;
                    } else if (has_code) {
                        std::cout << "+++++++ incremented code lines" << std::endl;
                        ++code_lines;
                    }
                    line.clear();
                }
                pos_map.clear();
                
                if (*start == EOF && line_fully_read) {
                    ok_to_break = true;
                }
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
    
    bool loc_counter::is_blank_since_token(const std::string &token,
                                           const std::string &line,
                                           loc_counter::DIRECTION direction) noexcept(false)
    {
        int64_t pos{};
        
        // check if token exists
        if (((uint64_t) (pos = line.find(token))) == std::string::npos) {
            throw invalid_characters{"Token not found in the string"};
            return false;
        }
        
        if (direction == DIRECTION::RIGHT) {        // check right
            pos += token.length();
            while ((uint64_t) pos < line.length()) {
                if (!std::isspace(line[pos++])) {
                    return false;
                }
            }
        } else if (direction == DIRECTION::LEFT) {  // check left
            --pos;
            while (pos >= 0) {
                if (!std::isspace(line[pos--])) {
                    return false;
                }
            }
        } else if (direction == DIRECTION::BOTH) {
            // check right
            int64_t r_pos{(int64_t) (pos + token.length())};
            while ((uint64_t) r_pos < line.length()) {
                if (!std::isspace(line[r_pos++])) {
                    return false;
                }
            }
            
            // check left
            int64_t l_pos{pos - 1};
            while (l_pos >= 0) {
                if (!std::isspace(line[l_pos--])) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool loc_counter::ln_contains_only_single_line_cmt(const std::string &line) const noexcept(true)
    {
        for (auto &ref : this->_loc_info.single_line_delimiters()) {
            if (!loc_counter::is_blank_since_token(ref, line, DIRECTION::LEFT)) {
                return false;
            }
        }
        return true;
    }
    
    std::string loc_counter::substr_after_token(const std::string &token, const std::string &string) noexcept(true)
    {
        uint64_t pos{};
        if ((pos = string.find(token)) != std::string::npos) {
            return string.substr(pos + token.length());
        }
        return string;
    }
    
    std::string loc_counter::substr_before_token(const std::string &token, const std::string &string) noexcept(true)
    {
        uint64_t pos = string.find(token);
        if (pos != std::string::npos && pos != 0) {
            return string.substr(0, pos);       // extract pos characters from index 0
        } else if (pos == 0) {
            return std::string{};
        }
        return string;
    }
}