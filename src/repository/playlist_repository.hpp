#pragma once

#include "domain/playlist.hpp"
#include "repository/database.hpp"

#include <optional>
#include <vector>

namespace repository {

class PlaylistRepository {
public:
    explicit PlaylistRepository(Database& db);

    domain::Playlist create(const std::string& name, int capacity);
    std::vector<domain::Playlist> listAll();
    std::optional<domain::Playlist> findById(int id);
    bool rename(int id, const std::string& name);
    bool remove(int id);

    int countTracks(int playlistId);
    bool hasTrack(int playlistId, int trackId);
    void addTrack(int playlistId, int trackId, int position);
    bool removeTrack(int playlistId, int trackId);
    std::vector<domain::PlaylistEntry> listTracks(int playlistId);

private:
    Database& db_;
};

} // namespace repository
