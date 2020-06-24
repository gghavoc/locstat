#include "file_stats.h"

#include <utility>

namespace locstat
{
    file_stats::file_stats(file_path_t path,
                           const uint64_t &lines_of_code,
                           const uint64_t &comment_lines,
                           const uint64_t &blank_lines)
            : _file_path{std::move(path)},
              _code_lines{lines_of_code},
              _comment_lines{comment_lines},
              _blank_lines{blank_lines}
    {}
}