//#pragma once
//#include <SFML/Audio.hpp>
//#include <unordered_map>
//#include <string>
//#include <iostream>
//
//class AudioManager {
//private:
//    // 私有构造（单例模式）
//    AudioManager() = default;
//
//    // 禁止拷贝和赋值
//    AudioManager(const AudioManager&) = delete;
//    AudioManager& operator=(const AudioManager&) = delete;
//
//    // 存储所有音效
//    std::unordered_map<std::string, sf::SoundBuffer> buffers_;
//    std::unordered_map<std::string, sf::Sound> sounds_;
//
//    void loadSound(const std::string& id, const std::string& filepath);
//
//public:
//    static AudioManager* GetInstance();
//    void init();
//    void play(const std::string& id);
//    void setVolume(float volume);
//};

#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>   // ⬅️ 必须包含
#include <iostream>

class AudioManager {
private:
    AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, sf::SoundBuffer> buffers_;
    // ✅ 关键改动：存储 unique_ptr<sf::Sound>
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