#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include "src/locstat/loc_info.h"
#include "src/locstat/loc_counter.h"

int main(int argc, char *argv[])
try
{
    auto time_start = std::chrono::high_resolution_clock::now();
    
    locstat::loc_info loc{
            {"//", "/*&&*/"},
            {"h", "c", "cpp"},
            "C/C++"};
    
//    locstat::loc_info loc{
//            {"#", R"("""&&""")"},
//            {"py"},
//            "Python"};
    
    std::filesystem::path p{"/home/clarence/Downloads/sample/"};
    uint64_t total_lines{};
    uint64_t total_loc{};
    std::filesystem::recursive_directory_iterator iter(p);
    for (auto &ref : iter) {
        if (loc.contains_extension(ref.path().extension().string())) {
            locstat::file_stats s;
            locstat::loc_counter counter{ref, loc, s};
            counter();
            std::cout << std::left << std::setw(30) << s.file_path().filename().string()
                      << std::setw(10) << "code lines: " << std::setw(10) << s.code_lines()
                      << std::setw(10) << "comment lines: " << std::setw(10) << s.comment_lines()
                      << std::setw(10) << "blank lines: " << std::setw(10) << s.blank_lines()
                      << std::setw(10) << "total: " << std::setw(10) << s.total_lines() << std::endl;
            total_loc += s.code_lines();
            total_lines += s.total_lines();
        }
    }
    std::cout << std::left << "\ntotal lines read: " << std::setw(11) << total_lines
              << " total lines of code: " << std::setw(12) << total_loc << std::endl;
    
    std::cout << "\n" << std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - time_start).count() << " milliseconds elapsed" << std::endl;
    return 0;
} catch (...) {
    std::cerr << "unknown exception caught" << std::endl;
};