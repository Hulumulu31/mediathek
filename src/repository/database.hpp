#pragma once

#include <pqxx/pqxx>
#include <string>

namespace repository {

// Строка подключения: переменные окружения PG* или файл config.txt рядом с exe.
std::string loadConnectionString();

class Database {
public:
    explicit Database(const std::string& connectionString);

    pqxx::connection& connection();

private:
    pqxx::connection conn_;
};

} // namespace repository
