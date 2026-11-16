#pragma once

#include "domain/track.hpp"
#include "repository/database.hpp"

#include <optional>
#include <vector>

namespace repository {

class TrackRepository {
public:
    explicit TrackRepository(Database& db);

    domain::Track create(const std::string& title,
                         const std::string& artist,
                         int durationSec);
    std::vector<domain::Track> listAll();
    std::optional<domain::Track> findById(int id);
    bool update(int id,
                const std::string& title,
                const std::string& artist,
                int durationSec);
    bool remove(int id);

private:
    Database& db_;
};

} // namespace repository
