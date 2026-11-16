#include "cli/menu.hpp"

#include "domain/exceptions.hpp"

#include <iostream>
#include <limits>
#include <string>

namespace cli {
namespace {

void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string formatDuration(int sec)
{
    const int m = sec / 60;
    const int s = sec % 60;
    return std::to_string(m) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
}

} // namespace

Menu::Menu(service::MediaService& service)
    : service_(service)
{
}

std::string Menu::readLine(const std::string& prompt)
{
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int Menu::readInt(const std::string& prompt)
{
    while (true) {
        std::cout << prompt;
        int value = 0;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Please enter a number.\n";
        clearInput();
    }
}

void Menu::pause()
{
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void Menu::run()
{
    while (true) {
        showMain();
        const int choice = readInt("Choice: ");
        try {
            switch (choice) {
            case 1:
                tracksMenu();
                break;
            case 2:
                playlistsMenu();
                break;
            case 3:
                compositionMenu();
                break;
            case 0:
                std::cout << "Bye.\n";
                return;
            default:
                std::cout << "Unknown menu item.\n";
                pause();
                break;
            }
        } catch (const domain::AppError& e) {
            std::cout << "Error: " << e.what() << "\n";
            pause();
        } catch (const std::exception& e) {
            std::cout << "Unexpected error: " << e.what() << "\n";
            pause();
        }
    }
}

void Menu::showMain()
{
    std::cout << "\n========================================\n";
    std::cout << "           MEDIATHEK / PLAYLISTS\n";
    std::cout << "========================================\n";
    std::cout << " 1. Tracks\n";
    std::cout << " 2. Playlists\n";
    std::cout << " 3. Playlist contents\n";
    std::cout << " 0. Exit\n";
    std::cout << "----------------------------------------\n";
}

void Menu::tracksMenu()
{
    while (true) {
        std::cout << "\n--- Tracks ---\n";
        std::cout << " 1. List\n";
        std::cout << " 2. Add\n";
        std::cout << " 3. Update\n";
        std::cout << " 4. Delete\n";
        std::cout << " 0. Back\n";
        const int choice = readInt("Choice: ");

        if (choice == 0) {
            return;
        }

        try {
            if (choice == 1) {
                const auto tracks = service_.listTracks();
                if (tracks.empty()) {
                    std::cout << "(no tracks)\n";
                } else {
                    for (const auto& t : tracks) {
                        std::cout << "  #" << t.id << "  " << t.artist << " — "
                                  << t.title << "  [" << formatDuration(t.durationSec)
                                  << "]\n";
                    }
                }
            } else if (choice == 2) {
                const std::string title = readLine("Title: ");
                const std::string artist = readLine("Artist: ");
                const int duration = readInt("Duration (seconds): ");
                const auto t = service_.addTrack(title, artist, duration);
                std::cout << "Created track #" << t.id << "\n";
            } else if (choice == 3) {
                const int id = readInt("Track id: ");
                const std::string title = readLine("New title: ");
                const std::string artist = readLine("New artist: ");
                const int duration = readInt("New duration (seconds): ");
                service_.updateTrack(id, title, artist, duration);
                std::cout << "Updated.\n";
            } else if (choice == 4) {
                const int id = readInt("Track id: ");
                service_.deleteTrack(id);
                std::cout << "Deleted.\n";
            } else {
                std::cout << "Unknown item.\n";
            }
        } catch (const domain::AppError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        pause();
    }
}

void Menu::playlistsMenu()
{
    while (true) {
        std::cout << "\n--- Playlists ---\n";
        std::cout << " 1. List\n";
        std::cout << " 2. Create\n";
        std::cout << " 3. Rename\n";
        std::cout << " 4. Delete\n";
        std::cout << " 0. Back\n";
        const int choice = readInt("Choice: ");

        if (choice == 0) {
            return;
        }

        try {
            if (choice == 1) {
                const auto playlists = service_.listPlaylists();
                if (playlists.empty()) {
                    std::cout << "(no playlists)\n";
                } else {
                    for (const auto& p : playlists) {
                        std::cout << "  #" << p.id << "  " << p.name
                                  << "  (capacity " << p.capacity << ", created "
                                  << p.createdAt << ")\n";
                    }
                }
            } else if (choice == 2) {
                const std::string name = readLine("Name: ");
                const int capacity = readInt("Capacity: ");
                const auto p = service_.createPlaylist(name, capacity);
                std::cout << "Created playlist #" << p.id << "\n";
            } else if (choice == 3) {
                const int id = readInt("Playlist id: ");
                const std::string name = readLine("New name: ");
                service_.renamePlaylist(id, name);
                std::cout << "Renamed.\n";
            } else if (choice == 4) {
                const int id = readInt("Playlist id: ");
                service_.deletePlaylist(id);
                std::cout << "Deleted.\n";
            } else {
                std::cout << "Unknown item.\n";
            }
        } catch (const domain::AppError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        pause();
    }
}

void Menu::compositionMenu()
{
    while (true) {
        std::cout << "\n--- Playlist contents ---\n";
        std::cout << " 1. Show tracks in playlist\n";
        std::cout << " 2. Add track to playlist\n";
        std::cout << " 3. Remove track from playlist\n";
        std::cout << " 0. Back\n";
        const int choice = readInt("Choice: ");

        if (choice == 0) {
            return;
        }

        try {
            if (choice == 1) {
                const int playlistId = readInt("Playlist id: ");
                const auto entries = service_.listPlaylistTracks(playlistId);
                if (entries.empty()) {
                    std::cout << "(playlist is empty)\n";
                } else {
                    for (const auto& e : entries) {
                        std::cout << "  " << e.position << ". [#" << e.track.id << "] "
                                  << e.track.artist << " — " << e.track.title << "  ["
                                  << formatDuration(e.track.durationSec) << "]\n";
                    }
                }
            } else if (choice == 2) {
                const int playlistId = readInt("Playlist id: ");
                const int trackId = readInt("Track id: ");
                service_.addTrackToPlaylist(playlistId, trackId);
                std::cout << "Added.\n";
            } else if (choice == 3) {
                const int playlistId = readInt("Playlist id: ");
                const int trackId = readInt("Track id: ");
                service_.removeTrackFromPlaylist(playlistId, trackId);
                std::cout << "Removed.\n";
            } else {
                std::cout << "Unknown item.\n";
            }
        } catch (const domain::AppError& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        pause();
    }
}

} // namespace cli
