#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <cstdint>

class SoundManager {
public:
    SoundManager(); 

    void playNavegar();
    void playConfirmar();
    void playErro();
    void playViagem();
    void playSucesso();

    // --- AS FUNÇÕES QUE FALTAVAM ---
    void toggleMute(); 
    bool isMuted() const;

private:
    sf::SoundBuffer m_bufNavegar;
    sf::SoundBuffer m_bufConfirmar;
    sf::SoundBuffer m_bufErro;
    sf::SoundBuffer m_bufViagem;
    sf::SoundBuffer m_bufSucesso;

    sf::Sound m_sound;
    bool m_isMuted; // Variável de controle

    void gerarTom(sf::SoundBuffer& buffer, float frequencia, float duracao, int tipo);
    void gerarSweep(sf::SoundBuffer& buffer, float freqInicio, float freqFim, float duracao);
};