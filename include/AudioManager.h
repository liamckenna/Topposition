#pragma once
#include <string>

namespace AudioManager {
    bool init();
    void quit();
    void update();

    bool loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name, float volume = 1.0f);
    void playMusic(const std::string& name, float volume = 0.5f);
    void stopMusic();
    void setMasterVolume(float volume);
}
