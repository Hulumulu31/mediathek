#include "service/media_service.hpp"

#include "domain/exceptions.hpp"

namespace service {

MediaService::MediaService(repository::TrackRepository& tracks,
                           repository::PlaylistRepository& playlists)
    : tracks_(tracks)
    , playlists_(playlists)
{
}

std::string MediaService::trim(std::string s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
        s.erase(s.begin());
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
    return s;
}

void MediaService::requireNonEmpty(const std::string& value, const char* field)
{
    if (value.empty()) {
        throw domain::ValidationError(std::string(field) + " must not be empty");
    }
}

domain::Track MediaService::addTrack(const std::string& title,
                                     const std::string& artist,
                                     int durationSec)
{
    const std::string t = trim(title);
    const std::string a = trim(artist);
    requireNonEmpty(t, "title");
    requireNonEmpty(a, "artist");
    if (durationSec <= 0) {
        throw domain::ValidationError("duration must be > 0");
    }
    return tracks_.create(t, a, durationSec);
}

std::vector<domain::Track> MediaService::listTracks()
{
    return tracks_.listAll();
}

domain::Track MediaService::getTrack(int id)
{
    auto t = tracks_.findById(id);
    if (!t) {
        throw domain::NotFoundError("Track not found");
    }
    return *t;
}

void MediaService::updateTrack(int id,
                               const std::string& title,
                               const std::string& artist,
                               int durationSec)
{
    const std::string t = trim(title);
    const std::string a = trim(artist);
    requireNonEmpty(t, "title");
    requireNonEmpty(a, "artist");
    if (durationSec <= 0) {
        throw domain::ValidationError("duration must be > 0");
    }
    if (!tracks_.update(id, t, a, durationSec)) {
        throw domain::NotFoundError("Track not found");
    }
}

void MediaService::deleteTrack(int id)
{
    if (!tracks_.remove(id)) {
        throw domain::NotFoundError("Track not found");
    }
}

domain::Playlist MediaService::createPlaylist(const std::string& name, int capacity)
{
    const std::string n = trim(name);
    requireNonEmpty(n, "name");
    if (capacity <= 0) {
        throw domain::ValidationError("capacity must be > 0");
    }
    return playlists_.create(n, capacity);
}

std::vector<domain::Playlist> MediaService::listPlaylists()
{
    return playlists_.listAll();
}

domain::Playlist MediaService::getPlaylist(int id)
{
    auto p = playlists_.findById(id);
    if (!p) {
        throw domain::NotFoundError("Playlist not found");
    }
    return *p;
}

void MediaService::renamePlaylist(int id, const std::string& name)
{
    const std::string n = trim(name);
    requireNonEmpty(n, "name");
    if (!playlists_.rename(id, n)) {
        throw domain::NotFoundError("Playlist not found");
    }
}

void MediaService::deletePlaylist(int id)
{
    if (!playlists_.remove(id)) {
        throw domain::NotFoundError("Playlist not found");
    }
}

void MediaService::addTrackToPlaylist(int playlistId, int trackId)
{
    auto playlist = playlists_.findById(playlistId);
    if (!playlist) {
        throw domain::NotFoundError("Playlist not found");
    }
    if (!tracks_.findById(trackId)) {
        throw domain::NotFoundError("Track not found");
    }
    if (playlists_.hasTrack(playlistId, trackId)) {
        throw domain::ValidationError("Track is already in this playlist");
    }

    const int count = playlists_.countTracks(playlistId);
    if (count >= playlist->capacity) {
        throw domain::ValidationError("Playlist is full (capacity reached)");
    }

    playlists_.addTrack(playlistId, trackId, count + 1);
}

void MediaService::removeTrackFromPlaylist(int playlistId, int trackId)
{
    if (!playlists_.findById(playlistId)) {
        throw domain::NotFoundError("Playlist not found");
    }
    if (!playlists_.removeTrack(playlistId, trackId)) {
        throw domain::NotFoundError("Track is not in this playlist");
    }
}

std::vector<domain::PlaylistEntry> MediaService::listPlaylistTracks(int playlistId)
{
    if (!playlists_.findById(playlistId)) {
        throw domain::NotFoundError("Playlist not found");
    }
    return playlists_.listTracks(playlistId);
}

} // namespace service
