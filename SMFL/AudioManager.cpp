#include "AudioManager.h"

void AudioManager::loadSound(const std::string& id, const std::string& filepath) {
    sf::SoundBuffer buffer;

    music_.openFromFile("assets/Audio/theme.mp3");
    music_.play();
    music_.setVolume(15);
    music_.setLooping(true);

    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "加载音频失败: " << filepath << std::endl;
        return;
    }
    // 存储 buffer
    buffers_[id] = std::move(buffer);
    // 直接使用带 buffer 的构造函数，无需默认构造
    sounds_[id] = std::make_unique<sf::Sound>(buffers_[id]);
}

void AudioManager::pauseMusic()
{
    music_.pause();
}

void AudioManager::playMusic()
{
    music_.play();
}

void AudioManager::restartMusic()
{
    music_.stop();
    music_.play();
}

AudioManager* AudioManager::GetInstance() {
    static AudioManager instance;
    return &instance;
}

void AudioManager::init() {
    loadSound("select", "assets/Audio/select.mp3");
    loadSound("click", "assets/Audio/click.wav");
    loadSound("kick", "assets/Audio/kick.wav");
    loadSound("hit", "assets/Audio/hit.wav");
    loadSound("goal", "assets/Audio/goal.wav");
    for (auto& [id, sound] : sounds_) {
        if (sound) {
            sound->setVolume(0.0f);
            sound->play();
            sound->stop();
            sound->setVolume(100.0f);
        }
    }
}

void AudioManager::play(const std::string& id) {
    auto it = sounds_.find(id);
    if (it != sounds_.end() && it->second) {
        it->second->play();   
    }
    else {
        std::cerr << "未找到音效: " << id << std::endl;
    }
}

void AudioManager::setVolume(float volume) {
    for (auto& pair : sounds_) {
        if (pair.second) {
            pair.second->setVolume(volume);
        }
    }
}