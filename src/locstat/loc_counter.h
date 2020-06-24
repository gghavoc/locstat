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
        enum class [[nodiscard]] LINE_CONTENT
        {
            CODE = 0,
            SINGLE_LINE_COMMENT,
            BLOCK_COMMENT,
            BLANK,
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
