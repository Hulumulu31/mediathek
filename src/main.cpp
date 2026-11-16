#include "cli/menu.hpp"
#include "domain/exceptions.hpp"
#include "repository/database.hpp"
#include "repository/playlist_repository.hpp"
#include "repository/track_repository.hpp"
#include "service/media_service.hpp"

#include <iostream>

int main()
{
    try {
        const std::string conn = repository::loadConnectionString();
        repository::Database db(conn);
        repository::TrackRepository tracks(db);
        repository::PlaylistRepository playlists(db);
        service::MediaService media(tracks, playlists);
        cli::Menu menu(media);
        menu.run();
        return 0;
    } catch (const domain::AppError& e) {
        std::cerr << "Startup error: " << e.what() << "\n";
        return 1;
    } catch (const pqxx::broken_connection& e) {
        std::cerr << "Cannot connect to PostgreSQL: " << e.what() << "\n"
                  << "Check PG* env vars or config.txt (see .env.example).\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}
