#include "repository/track_repository.hpp"

#include "domain/exceptions.hpp"

namespace repository {
namespace {

domain::Track rowToTrack(const pqxx::row& row)
{
    domain::Track t;
    t.id = row["id"].as<int>();
    t.title = row["title"].as<std::string>();
    t.artist = row["artist"].as<std::string>();
    t.durationSec = row["duration_sec"].as<int>();
    return t;
}

} // namespace

TrackRepository::TrackRepository(Database& db)
    : db_(db)
{
}

domain::Track TrackRepository::create(const std::string& title,
                                      const std::string& artist,
                                      int durationSec)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "INSERT INTO tracks (title, artist, duration_sec) "
            "VALUES ($1, $2, $3) "
            "RETURNING id, title, artist, duration_sec",
            title,
            artist,
            durationSec);
        txn.commit();
        return rowToTrack(r[0]);
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (create track): ") + e.what());
    }
}

std::vector<domain::Track> TrackRepository::listAll()
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec(
            "SELECT id, title, artist, duration_sec "
            "FROM tracks ORDER BY id");
        txn.commit();

        std::vector<domain::Track> out;
        out.reserve(r.size());
        for (const auto& row : r) {
            out.push_back(rowToTrack(row));
        }
        return out;
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (list tracks): ") + e.what());
    }
}

std::optional<domain::Track> TrackRepository::findById(int id)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "SELECT id, title, artist, duration_sec "
            "FROM tracks WHERE id = $1",
            id);
        txn.commit();
        if (r.empty()) {
            return std::nullopt;
        }
        return rowToTrack(r[0]);
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (find track): ") + e.what());
    }
}

bool TrackRepository::update(int id,
                             const std::string& title,
                             const std::string& artist,
                             int durationSec)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "UPDATE tracks SET title = $2, artist = $3, duration_sec = $4 "
            "WHERE id = $1 "
            "RETURNING id",
            id,
            title,
            artist,
            durationSec);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (update track): ") + e.what());
    }
}

bool TrackRepository::remove(int id)
{
    try {
        pqxx::work txn(db_.connection());
        const pqxx::result r = txn.exec_params(
            "DELETE FROM tracks WHERE id = $1 RETURNING id",
            id);
        txn.commit();
        return !r.empty();
    } catch (const pqxx::sql_error& e) {
        throw domain::AppError(std::string("DB error (delete track): ") + e.what());
    }
}

} // namespace repository
