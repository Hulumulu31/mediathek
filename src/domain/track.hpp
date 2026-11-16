#pragma once

#include <string>

namespace domain {

struct Track {
    int id = 0;
    std::string title;
    std::string artist;
    int durationSec = 0;
};

} // namespace domain
