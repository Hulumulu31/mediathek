#pragma once

#include "service/media_service.hpp"

namespace cli {

class Menu {
public:
    explicit Menu(service::MediaService& service);

    void run();

private:
    service::MediaService& service_;

    void showMain();
    void tracksMenu();
    void playlistsMenu();
    void compositionMenu();

    static int readInt(const std::string& prompt);
    static int readDuration(const std::string& prompt);
    static std::string readLine(const std::string& prompt);
    static void pause();
};

} // namespace cli
