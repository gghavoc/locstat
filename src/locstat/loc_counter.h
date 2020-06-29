#ifndef LOC_COUNTER_H
#define LOC_COUNTER_H
#include <filesystem>
#include <utility>

#include "loc_info.h"
#include "file_stats.h"
#include "ls_exeptions.h"

namespace locstat
{
    struct loc_counter
    {
    public:     // alias
        using path_t = std::filesystem::path;
    
    public:     // public enum
        enum class [[nodiscard]] DIRECTION : signed int
        {
            LEFT = -1,
            BOTH = 0,
            RIGHT = 1,
        };
    
    public:
        loc_counter(path_t &&file_path, loc_info &&loc_info_, file_stats &OUT_file_stats)
                : _path{std::move(file_path)},
                  _loc_info{std::move(loc_info_)},
                  OUT_file_stats{OUT_file_stats}
        { this->file_is_good(); }
        
        loc_counter(const path_t &file_path, const loc_info &loc_info_, file_stats &OUT_file_stats)
                : _path{file_path},
                  _loc_info{loc_info_},
                  OUT_file_stats{OUT_file_stats}
        { this->file_is_good(); }
    
    public:     // operator overloading
        // For threading purposes
        void operator()() const noexcept(false);
        
        // finds a token from a line and iterates from a given direction
        // to find if it only contains blank characters(space, newline, tab, null) from that token
        // returns true if only contains spaces, return false otherwise
        // returns false if the token doesn't exist
        [[nodiscard]]
        static bool is_blank_since_token(const std::string &token,
                                         const std::string &line,
                                         DIRECTION direction) noexcept(false);
        
        // Returns true if the argument has only a single line comment
        [[nodiscard]]
        bool ln_contains_only_single_line_cmt(const std::string &line) const noexcept(true);
        
        // Returns true if printing character is not found
        [[nodiscard]]
        static bool isblank(const std::string &line) noexcept(true) {
            return line.find_first_not_of(" \t\n\v\f\r") == loc_info::comment_delimiter_t::npos;
        }
        
        static std::string substr_after_token(const std::string &token, const std::string &string) noexcept(true);
        
        static std::string substr_before_token(const std::string &token, const std::string &string) noexcept(true);
        
    private:    // private helper
        // Returns true if the passed file path is valid,
        // will throw an exception otherwise
        [[maybe_unused]]
        bool file_is_good() const noexcept(false);
    
    private:
        path_t _path{};
        loc_info _loc_info{};
        file_stats &OUT_file_stats;
    };
}


#endif //LOC_COUNTER_H
