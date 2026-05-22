#include "AudioManager.h"
#include <SDL3/SDL.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

namespace AudioManager {

struct CachedSound {
    SDL_AudioSpec spec;
    Uint8* buf = nullptr;
    Uint32 len = 0;
};

static SDL_AudioDeviceID deviceId = 0;
static std::map<std::string, CachedSound> sounds;
static std::vector<SDL_AudioStream*> activeStreams;
static SDL_AudioStream* musicStream = nullptr;
static const CachedSound* currentMusic = nullptr;
static float masterVolume = 1.0f;

bool init() {
    deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (deviceId == 0) {
        SDL_Log("AudioManager: failed to open audio device: %s", SDL_GetError());
        return false;
    }
    return true;
}

void quit() {
    stopMusic();
    for (SDL_AudioStream* s : activeStreams)
        SDL_DestroyAudioStream(s);
    activeStreams.clear();
    for (auto& [name, sound] : sounds)
        SDL_free(sound.buf);
    sounds.clear();
    if (deviceId)
        SDL_CloseAudioDevice(deviceId);
    deviceId = 0;
}

void update() {
    // Re-queue music when it finishes to loop it
    if (musicStream && currentMusic) {
        if (SDL_GetAudioStreamQueued(musicStream) == 0)
            SDL_PutAudioStreamData(musicStream, currentMusic->buf, currentMusic->len);
    }

    // Destroy one-shot streams once their data has been consumed
    activeStreams.erase(
        std::remove_if(activeStreams.begin(), activeStreams.end(),
            [](SDL_AudioStream* s) {
                if (SDL_GetAudioStreamQueued(s) == 0) {
                    SDL_DestroyAudioStream(s);
                    return true;
                }
                return false;
            }),
        activeStreams.end()
    );
}

bool loadSound(const std::string& name, const std::string& path) {
    CachedSound s;
    if (!SDL_LoadWAV(path.c_str(), &s.spec, &s.buf, &s.len)) {
        SDL_Log("AudioManager: failed to load '%s': %s", path.c_str(), SDL_GetError());
        return false;
    }
    sounds[name] = s;
    return true;
}

void playSound(const std::string& name, float volume) {
    if (!deviceId) return;
    auto it = sounds.find(name);
    if (it == sounds.end()) return;

    const CachedSound& s = it->second;
    SDL_AudioStream* stream = SDL_CreateAudioStream(&s.spec, NULL);
    if (!stream) return;
    SDL_BindAudioStream(deviceId, stream);
    SDL_SetAudioStreamGain(stream, volume * masterVolume);
    SDL_PutAudioStreamData(stream, s.buf, s.len);
    SDL_FlushAudioStream(stream);
    activeStreams.push_back(stream);
}

void playMusic(const std::string& name, float volume) {
    if (!deviceId) return;
    auto it = sounds.find(name);
    if (it == sounds.end()) return;

    stopMusic();
    currentMusic = &it->second;
    musicStream = SDL_CreateAudioStream(&currentMusic->spec, NULL);
    if (!musicStream) { currentMusic = nullptr; return; }
    SDL_BindAudioStream(deviceId, musicStream);
    SDL_SetAudioStreamGain(musicStream, volume * masterVolume);
    SDL_PutAudioStreamData(musicStream, currentMusic->buf, currentMusic->len);
}

void stopMusic() {
    if (musicStream) {
        SDL_DestroyAudioStream(musicStream);
        musicStream = nullptr;
        currentMusic = nullptr;
    }
}

void setMasterVolume(float volume) {
    masterVolume = volume;
}

} // namespace AudioManager
