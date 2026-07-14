//#include "AudioManager.h"
//
//// 加载音频的内部函数
//void AudioManager::loadSound(const std::string& id, const std::string& filepath) 
//{
//    sf::Sound testSound;  // 如果这行编译报错，那就是默认构造不可用
//    sf::SoundBuffer buffer;
//    if (!buffer.loadFromFile(filepath)) {
//        std::cerr << "加载音频失败: " << filepath << std::endl;
//        return;
//    }
//    buffers_[id] = std::move(buffer);
//    sounds_[id].setBuffer(buffers_[id]);
//}
//
//// 获取单例实例（线程安全 C++11+）
//AudioManager* AudioManager::GetInstance() 
//{
//    static AudioManager instance;  // 静态局部变量，线程安全
//    return &instance;
//}
//
//// 初始化所有音频（在程序启动时调用一次）
//void AudioManager::init() 
//{
//    loadSound("select", "assets/Audio/select.mp3");
//    loadSound("click", "assets/Audio/click.wav");
//    loadSound("kick", "assets/Audio/kick.wav");
//}
//
//// 播放音效的公开接口
//void AudioManager::play(const std::string & id) 
//{
//    auto it = sounds_.find(id);
//    if (it != sounds_.end()) {
//        it->second.play();  // sf::Sound::play() 会自动重新播放
//    }
//    else {
//        std::cerr << "未找到音效: " << id << std::endl;
//    }
//}
//
//// 设置全局音量（0-100）
//void AudioManager::setVolume(float volume) 
//{
//    for (auto& pair : sounds_) {
//        pair.second.setVolume(volume);
//    }
//}

#include "AudioManager.h"

void AudioManager::loadSound(const std::string& id, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "加载音频失败: " << filepath << std::endl;
        return;
    }
    // 存储 buffer
    buffers_[id] = std::move(buffer);
    // 直接使用带 buffer 的构造函数，无需默认构造
    sounds_[id] = std::make_unique<sf::Sound>(buffers_[id]);
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
}

void AudioManager::play(const std::string& id) {
    auto it = sounds_.find(id);
    if (it != sounds_.end() && it->second) {
        it->second->play();   // ⬅️ 用 -> 调用
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