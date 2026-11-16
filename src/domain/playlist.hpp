#pragma once

#include "domain/track.hpp"

#include <string>
#include <vector>

namespace domain {

struct Playlist {
    int id = 0;
    std::string name;
    int capacity = 0;
    std::string createdAt;
};

struct PlaylistEntry {
    int position = 0;
    Track track;
};

} // namespace domain
