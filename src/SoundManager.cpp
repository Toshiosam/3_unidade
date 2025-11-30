#include "SoundManager.h"
#include <cmath>
#include <random>
#include <iostream>

const unsigned SAMPLE_RATE = 44100;
const unsigned AMPLITUDE = 20000;

SoundManager::SoundManager() : m_sound(m_bufNavegar), m_isMuted(false) {
    // Gera os sons na memória
    gerarTom(m_bufNavegar, 440.0f, 0.05f, 0); 
    gerarTom(m_bufConfirmar, 880.0f, 0.1f, 1);
    gerarTom(m_bufErro, 150.0f, 0.3f, 2);
    gerarSweep(m_bufViagem, 200.0f, 1500.0f, 1.0f);
    gerarTom(m_bufSucesso, 1200.0f, 0.5f, 0);
    
    // --- VOLUME MUITO BAIXO (8%) PARA ESCRITÓRIO ---
    m_sound.setVolume(5.f); 
}

void SoundManager::toggleMute() {
    m_isMuted = !m_isMuted;
    if (m_isMuted) {
        m_sound.setVolume(0.0f);
    } else {
        m_sound.setVolume(8.0f); // Volta para o volume baixo
    }
}

bool SoundManager::isMuted() const {
    return m_isMuted;
}

void SoundManager::playNavegar() {
    float pitch = 1.0f + ((rand() % 10) / 100.0f);
    m_sound.setBuffer(m_bufNavegar);
    m_sound.setPitch(pitch);
    m_sound.play();
}
void SoundManager::playConfirmar() { m_sound.setBuffer(m_bufConfirmar); m_sound.setPitch(1.0f); m_sound.play(); }
void SoundManager::playErro() { m_sound.setBuffer(m_bufErro); m_sound.setPitch(1.0f); m_sound.play(); }
void SoundManager::playViagem() { m_sound.setBuffer(m_bufViagem); m_sound.setPitch(1.0f); m_sound.play(); }
void SoundManager::playSucesso() { m_sound.setBuffer(m_bufSucesso); m_sound.setPitch(1.0f); m_sound.play(); }

void SoundManager::gerarTom(sf::SoundBuffer& buffer, float freq, float dur, int tipo) {
    std::vector<std::int16_t> samples;
    unsigned numSamples = (unsigned)(SAMPLE_RATE * dur);
    for (unsigned i = 0; i < numSamples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double val = 0;
        if (tipo == 0) val = AMPLITUDE * std::sin(2 * 3.14159 * freq * t);
        else if (tipo == 1) val = (std::sin(2 * 3.14159 * freq * t) > 0 ? 1 : -1) * AMPLITUDE * 0.5;
        else if (tipo == 2) { double x = freq*t; val = (2*(x-(int)x)-1) * AMPLITUDE * 0.7; }
        if (i > numSamples - 500) val *= (double)(numSamples - i) / 500.0;
        samples.push_back((std::int16_t)val); samples.push_back((std::int16_t)val); 
    }
    // SFML 3.0 requer tratamento de erro ou ignora retorno
    if(!buffer.loadFromSamples(&samples[0], samples.size(), 2, SAMPLE_RATE, {sf::SoundChannel::FrontLeft, sf::SoundChannel::FrontRight})) {}
}

void SoundManager::gerarSweep(sf::SoundBuffer& buffer, float fStart, float fEnd, float dur) {
    std::vector<std::int16_t> samples;
    unsigned numSamples = (unsigned)(SAMPLE_RATE * dur);
    for (unsigned i = 0; i < numSamples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double val = AMPLITUDE * std::sin(2 * 3.14159 * (fStart + (fEnd - fStart) * (t/dur)) * t) * 0.5;
        samples.push_back((std::int16_t)val); samples.push_back((std::int16_t)val);
    }
    if(!buffer.loadFromSamples(&samples[0], samples.size(), 2, SAMPLE_RATE, {sf::SoundChannel::FrontLeft, sf::SoundChannel::FrontRight})) {}
}