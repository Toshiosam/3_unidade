#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include "Country.h"
#include "Meme.h"
#include "SoundManager.h"

enum class EstadoGame { INVESTIGANDO, VIAJANDO, ABDUZINDO, VITORIA, GAME_OVER, ZEROU, INTRO, PROMOVIDO, CREDITOS };

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(); 
    void render();
    
    // --- LÓGICA UNIFICADA DE INPUT ---
    void executarAcao(int indiceBotao); // Nova função que aceita Mouse ou Enter

    void inicializarPaises();
    void inicializarMemes();
    void iniciarNovaMissao();
    void gerarMissaoLinear(int tamanhoRota);
    void resetarJogo();
    
    std::shared_ptr<Meme> sortearMemeDisponivel(std::string nivel);
    int contarMemesRestantes(std::string nivel);
    void atualizarFundo(); 

    void wrapText(sf::Text& text, float maxWidth);

    std::shared_ptr<sf::RenderWindow> m_window;
    sf::Font m_font;      
    
    // Textos
    sf::Text m_textoInfo, m_textoFeedback, m_textoTempo;
    sf::Text m_labelLaser, m_labelTempo;
    sf::Text m_modalTitulo, m_modalTexto, m_modalBotao;

    // Gráficos e UI
    sf::Texture m_texFundo; sf::Sprite m_sprFundo;
    sf::Texture m_texMeme; sf::Sprite m_sprMeme;
    sf::RectangleShape m_barFundo, m_barEnergia;
    sf::RectangleShape m_modalFundo, m_modalCaixa;
    sf::RectangleShape m_bgTexto, m_bgInfo;

    float m_energiaLaser;
    float m_valorPorPista;
    
    std::vector<bool> m_pistasVisitadas; 
    
    // Controles de Input
    sf::Clock m_inputCooldown; // Evita duplo clique acidental

    SoundManager m_sound;
    std::vector<sf::Text> m_botoesLocais; 
    int m_opcaoSelecionada;
    EstadoGame m_estado;

    std::vector<std::shared_ptr<Country>> m_todosPaises;
    std::vector<std::shared_ptr<Meme>> m_todosMemes;
    std::shared_ptr<Meme> m_alvoAtual; 
    std::shared_ptr<Country> m_paisAtual;
    std::vector<std::shared_ptr<Country>> m_rotaVitoria;
    std::vector<std::string> m_nomesCapturados;
    
    int m_nivelAtual; 
    int m_horasRestantes; 
    const int CUSTO_VIAGEM = 5;
    const int CUSTO_INVESTIGAR = 2;
};