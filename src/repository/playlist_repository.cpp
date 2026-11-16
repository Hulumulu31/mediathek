#include "repository/playlist_repository.hpp"

#include "domain/exceptions.hpp"

namespace repository {
namespace {

domain::Playlist rowToPlaylist(const pqxx::row& row)
{
    domain::Playlist p;
    p.id = row["id"].as<int>();
    p.name = row["name"].as<std::string>();
    p.capacity = row["capacity"].as<int>();
    p.createdAt = row["created_at"].as<std::string>();
    return p;
}

} // namespace

PlaylistRepository::PlaylistRepository(Database& db)
    : db_(db)
{
}

domain::Playlist PlaylistRepository::create(const std::string& name, int capacity)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "INSERT INTO playlists (name, capacity) VALUES ($1, $2) "
            "RETURNING id, name, capacity, "
            "to_char(created_at, 'YYYY-MM-DD HH24:MI') AS created_at",
            name,
            capacity);
        txn.commit();
        return rowToPlaylist(r[0]);
    } catch (const pqxx::unique_violation&) {
        throw domain::ValidationError("Playlist with this name already exists");
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (create playlist): ") + e.what());
    }
}

std::vector<domain::Playlist> PlaylistRepository::listAll()
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec(
            "SELECT id, name, capacity, "
            "to_char(created_at, 'YYYY-MM-DD HH24:MI') AS created_at "
            "FROM playlists ORDER BY id");
        txn.commit();

        std::vector<domain::Playlist> out;
        out.reserve(r.size());
        for (const auto& row : r) {
            out.push_back(rowToPlaylist(row));
        }
        return out;
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (list playlists): ") + e.what());
    }
}

std::optional<domain::Playlist> PlaylistRepository::findById(int id)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "SELECT id, name, capacity, "
            "to_char(created_at, 'YYYY-MM-DD HH24:MI') AS created_at "
            "FROM playlists WHERE id = $1",
            id);
        txn.commit();
        if (r.empty()) {
            return std::nullopt;
        }
        return rowToPlaylist(r[0]);
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (find playlist): ") + e.what());
    }
}

bool PlaylistRepository::rename(int id, const std::string& name)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "UPDATE playlists SET name = $2 WHERE id = $1 RETURNING id",
            id,
            name);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::unique_violation&) {
        throw domain::ValidationError("Playlist with this name already exists");
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (rename playlist): ") + e.what());
    }
}

bool PlaylistRepository::remove(int id)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "DELETE FROM playlists WHERE id = $1 RETURNING id",
            id);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (delete playlist): ") + e.what());
    }
}

int PlaylistRepository::countTracks(int playlistId)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "SELECT COUNT(*)::int AS cnt FROM playlist_tracks WHERE playlist_id = $1",
            playlistId);
        txn.commit();
        return r[0]["cnt"].as<int>();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (count tracks): ") + e.what());
    }
}

bool PlaylistRepository::hasTrack(int playlistId, int trackId)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "SELECT 1 FROM playlist_tracks "
            "WHERE playlist_id = $1 AND track_id = $2",
            playlistId,
            trackId);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (has track): ") + e.what());
    }
}

void PlaylistRepository::addTrack(int playlistId, int trackId, int position)
{
    try {
        pqxx::work txn(db_.connection());
        txn.exec_params(
            "INSERT INTO playlist_tracks (playlist_id, track_id, position) "
            "VALUES ($1, $2, $3)",
            playlistId,
            trackId,
            position);
        txn.commit();
    } catch (const pqxx::unique_violation&) {
        throw domain::ValidationError("Track already in playlist or position taken");
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (add track): ") + e.what());
    }
}

bool PlaylistRepository::removeTrack(int playlistId, int trackId)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "DELETE FROM playlist_tracks "
            "WHERE playlist_id = $1 AND track_id = $2 "
            "RETURNING track_id",
            playlistId,
            trackId);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (remove track): ") + e.what());
    }
}

std::vector<domain::PlaylistEntry> PlaylistRepository::listTracks(int playlistId)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "SELECT pt.position, t.id, t.title, t.artist, t.duration_sec "
            "FROM playlist_tracks pt "
            "JOIN tracks t ON t.id = pt.track_id "
            "WHERE pt.playlist_id = $1 "
            "ORDER BY pt.position",
            playlistId);
        txn.commit();

        std::vector<domain::PlaylistEntry> out;
        out.reserve(r.size());
        for (const auto& row : r) {
            domain::PlaylistEntry e;
            e.position = row["position"].as<int>();
            e.track.id = row["id"].as<int>();
            e.track.title = row["title"].as<std::string>();
            e.track.artist = row["artist"].as<std::string>();
            e.track.durationSec = row["duration_sec"].as<int>();
            out.push_back(std::move(e));
        }
        return out;
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (list playlist tracks): ") + e.what());
    }
}

} // namespace repository
