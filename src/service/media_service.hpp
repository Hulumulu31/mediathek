#pragma once

#include "domain/playlist.hpp"
#include "domain/track.hpp"
#include "repository/playlist_repository.hpp"
#include "repository/track_repository.hpp"

#include <string>
#include <vector>

namespace service {

class MediaService {
public:
    MediaService(repository::TrackRepository& tracks,
                 repository::PlaylistRepository& playlists);

    // Tracks
    domain::Track addTrack(const std::string& title,
                           const std::string& artist,
                           int durationSec);
    std::vector<domain::Track> listTracks();
    domain::Track getTrack(int id);
    void updateTrack(int id,
                     const std::string& title,
                     const std::string& artist,
                     int durationSec);
    void deleteTrack(int id);

    // Playlists
    domain::Playlist createPlaylist(const std::string& name, int capacity);
    std::vector<domain::Playlist> listPlaylists();
    domain::Playlist getPlaylist(int id);
    void renamePlaylist(int id, const std::string& name);
    void deletePlaylist(int id);

    // Composition
    void addTrackToPlaylist(int playlistId, int trackId);
    void removeTrackFromPlaylist(int playlistId, int trackId);
    std::vector<domain::PlaylistEntry> listPlaylistTracks(int playlistId);

private:
    repository::TrackRepository& tracks_;
    repository::PlaylistRepository& playlists_;

    static std::string trim(std::string s);
    static void requireNonEmpty(const std::string& value, const char* field);
};

} // namespace service
