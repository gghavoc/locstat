#include <iostream>
#include <filesystem>
#include <chrono>
#include "src/locstat/loc_info.h"
#include "src/locstat/loc_counter.h"

int main(int argc, char *argv[])
try {
    auto time_start = std::chrono::high_resolution_clock::now();

    locstat::loc_info loc{
            { "//", "<--&&-->", "/*&&*/", R"("""&&""")", "?" },
            { "h", "c", "cpp" },
            "C/C++" };
    locstat::file_stats s;
    std::cout << loc.has_block_delimiter() << std::endl;

    std::filesystem::path p{ "/home/clarence/Downloads/sample/sources/MolarMass.cpp" };
    locstat::loc_counter counter{ p, loc,  s};
    counter();
    
    
    std::cout << "\n" << std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - time_start).count() << " milliseconds elapsed" << std::endl;
    return 0;
} catch (...) {
    std::cerr << "unknown exception caught" << std::endl;
};