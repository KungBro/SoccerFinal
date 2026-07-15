#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>   
#include <iostream>

class AudioManager {
private:
    AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, sf::SoundBuffer> buffers_;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> sounds_;

    void loadSound(const std::string& id, const std::string& filepath);
    sf::Music music_;
    

public:
    static AudioManager* GetInstance();
    void init();
    void play(const std::string& id);
    void setVolume(float volume);
    void pauseMusic();
    void playMusic();
    void restartMusic();
    bool isMusicPaused = false;
};