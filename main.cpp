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
    locstat::file_stats s;
    
    std::filesystem::path p{"/home/clarence/Downloads/sample/sources/ElementResultsListEntry.cpp"};
    locstat::loc_counter counter{p, loc, s};
    counter();
    
    std::cout << s.file_path().filename().string() << ": "
              << "code lines: " << s.code_lines() << ", "
              << "comment lines: " << s.comment_lines() << ", "
              << "blank lines: " << s.blank_lines()
              << ", total: "<< s.total_lines() << std::endl;
    
    std::cout << "\n" << std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - time_start).count() << " milliseconds elapsed" << std::endl;
    return 0;
} catch (...) {
    std::cerr << "unknown exception caught" << std::endl;
};