#include "repository/database.hpp"

#include "domain/exceptions.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace repository {
namespace {

std::string trim(std::string s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r')) {
        s.erase(s.begin());
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) {
        s.pop_back();
    }
    return s;
}

std::unordered_map<std::string, std::string> readConfigFile(const std::string& path)
{
    std::unordered_map<std::string, std::string> map;
    std::ifstream in(path);
    if (!in) {
        return map;
    }

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, pos));
        const std::string value = trim(line.substr(pos + 1));
        map[key] = value;
    }
    return map;
}

const char* envOrNull(const char* name)
{
    const char* v = std::getenv(name);
    if (v == nullptr || v[0] == '\0') {
        return nullptr;
    }
    return v;
}

} // namespace

std::string loadConnectionString()
{
    // 1) Переменные окружения
    const char* host = envOrNull("PGHOST");
    const char* port = envOrNull("PGPORT");
    const char* user = envOrNull("PGUSER");
    const char* password = envOrNull("PGPASSWORD");
    const char* dbname = envOrNull("PGDATABASE");

    std::unordered_map<std::string, std::string> file =
        readConfigFile("config.txt");

    auto pick = [&](const char* envValue, const char* fileKey, const char* fallback) {
        if (envValue != nullptr) {
            return std::string(envValue);
        }
        const auto it = file.find(fileKey);
        if (it != file.end() && !it->second.empty()) {
            return it->second;
        }
        return std::string(fallback);
    };

    const std::string h = pick(host, "host", "localhost");
    const std::string p = pick(port, "port", "5432");
    const std::string u = pick(user, "user", "postgres");
    const std::string pw = pick(password, "password", "");
    const std::string db = pick(dbname, "dbname", "mediathek");

    if (pw.empty() && file.empty() && password == nullptr) {
        throw domain::AppError(
            "No DB password. Set PGPASSWORD or create config.txt "
            "(see config.example.txt / .env.example).");
    }

    std::ostringstream oss;
    oss << "host=" << h
        << " port=" << p
        << " user=" << u
        << " password=" << pw
        << " dbname=" << db;
    return oss.str();
}

Database::Database(const std::string& connectionString)
    : conn_(connectionString)
{
}

pqxx::connection& Database::connection()
{
    return conn_;
}

} // namespace repository
