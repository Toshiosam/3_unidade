#include "Game.h"
#include <iostream>
#include <algorithm> 
#include <random>    
#include <fstream>
#include <sstream> // Necessario para stringstream

// --- CONSTRUTOR ---
Game::Game() : 
    m_textoInfo(m_font), m_textoFeedback(m_font), m_textoTempo(m_font),
    m_labelLaser(m_font), m_labelTempo(m_font),
    m_modalTitulo(m_font), m_modalTexto(m_font), m_modalBotao(m_font),
    m_sprFundo(m_texFundo), m_sprMeme(m_texMeme), m_opcaoSelecionada(0) 
{ 
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode({1280, 720}), "Meme Hunter - v15.1 Text Fix");
    m_window->setFramerateLimit(60);

    // Tenta carregar fontes comuns do Windows
    bool fonteCarregou = false;
    if (m_font.openFromFile("C:/Windows/Fonts/cour.ttf")) fonteCarregou = true;
    else if(m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) fonteCarregou = true;
    
    if (!fonteCarregou) std::cerr << "[ERRO] Fonte nao encontrada!" << std::endl;

    // Lambda para configurar textos rapidamente
    auto configTexto = [&](sf::Text& t, int size, sf::Color c, float x, float y) {
        t.setFont(m_font); t.setCharacterSize(size); t.setFillColor(c); 
        t.setPosition({x, y});
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2.0f);
    };

    // --- CONFIGURAÇÃO DA UI ---
    m_bgTexto.setSize({1240.f, 120.f}); m_bgTexto.setPosition({20.f, 560.f});
    m_bgTexto.setFillColor(sf::Color(0, 0, 0, 240)); 
    m_bgTexto.setOutlineColor(sf::Color::Cyan); m_bgTexto.setOutlineThickness(2.f);

    configTexto(m_textoInfo, 28, sf::Color::Green, 40.f, 28.f);
    configTexto(m_textoFeedback, 22, sf::Color::White, 40.f, 580.f);

    configTexto(m_labelTempo, 18, sf::Color::White, 900.f, 25.f); m_labelTempo.setString("TEMPO RESTANTE:");
    configTexto(m_textoTempo, 30, sf::Color::Red, 1080.f, 18.f);

    // [CORREÇÃO AQUI] "NÍVEL" -> "NIVEL"
    configTexto(m_labelLaser, 18, sf::Color::White, 900.f, 80.f); m_labelLaser.setString("NIVEL DE ESTUDO:"); 
    
    m_barFundo.setSize({300.f, 25.f}); m_barFundo.setPosition({900.f, 110.f}); 
    m_barFundo.setFillColor(sf::Color(20,20,20, 200)); m_barFundo.setOutlineThickness(2); m_barFundo.setOutlineColor(sf::Color::White);
    m_barEnergia.setSize({0.f, 25.f}); m_barEnergia.setPosition({900.f, 110.f}); m_barEnergia.setFillColor(sf::Color(0, 255, 0));

    // MODAL INTRO
    m_modalFundo.setSize({1280.f, 720.f}); m_modalFundo.setFillColor(sf::Color(0, 0, 0, 240));
    m_modalCaixa.setSize({800.f, 450.f}); m_modalCaixa.setOrigin({400.f, 225.f}); m_modalCaixa.setPosition({640.f, 360.f});
    m_modalCaixa.setFillColor(sf::Color(20, 20, 50)); m_modalCaixa.setOutlineColor(sf::Color::White); m_modalCaixa.setOutlineThickness(3.f);

    configTexto(m_modalTitulo, 36, sf::Color::Yellow, 640.f, 180.f);
    // [CORREÇÃO AQUI] Remoção de acentos
    m_modalTitulo.setString("CONFIDENCIAL - N.A.S.A.");
    m_modalTitulo.setOrigin({m_modalTitulo.getLocalBounds().size.x/2, m_modalTitulo.getLocalBounds().size.y/2});

    configTexto(m_modalTexto, 20, sf::Color::White, 640.f, 360.f);
    // [CORREÇÃO AQUI] Remoção de acentos no texto de introdução
    m_modalTexto.setString(
        "Bem-vindo ao Nucleo de Analise de Sujeitos Anormais.\n\n"
        "Sua missao: Localizar e coletar brasileiros virais.\n\n"
        "PROTOCOLO:\n"
        "- Capture todos do Nivel 1 para ser promovido.\n"
        "- Use as dicas para nao viajar errado.\n\n"
        "[Setas/Mouse] Navegar   [Enter] Confirmar   [M] Mute"
    );
    m_modalTexto.setOrigin({m_modalTexto.getLocalBounds().size.x/2, m_modalTexto.getLocalBounds().size.y/2});

    configTexto(m_modalBotao, 24, sf::Color::Green, 640.f, 550.f);
    m_modalBotao.setString("[PRESSIONE ENTER]");
    m_modalBotao.setOrigin({m_modalBotao.getLocalBounds().size.x/2, m_modalBotao.getLocalBounds().size.y/2});

    inicializarPaises();
    inicializarMemes();
    m_nivelAtual = 1;
    m_estado = EstadoGame::INTRO;
}

Game::~Game() {}

void Game::run() {
    sf::Clock clock;
    while (m_window->isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(); 
        render();
    }
}

// Função auxiliar para carregar imagens de várias pastas
bool tentarCarregarImagem(sf::Texture& texture, std::string caminhoOriginal) {
    std::vector<std::string> tentativas = { caminhoOriginal, "src/" + caminhoOriginal, "../" + caminhoOriginal, "../src/" + caminhoOriginal };
    for (const auto& caminho : tentativas) {
        std::ifstream f(caminho.c_str());
        if (f.good()) { if (texture.loadFromFile(caminho)) return true; }
    }
    return false;
}

void Game::atualizarFundo() {
    if(m_paisAtual) {
        if(tentarCarregarImagem(m_texFundo, m_paisAtual->getImagemPath())) {
            m_sprFundo.setTexture(m_texFundo, true); m_sprFundo.setPosition({0.f, 0.f}); m_sprFundo.setColor(sf::Color(255, 255, 255)); 
            sf::Vector2u sz = m_texFundo.getSize();
            if (sz.x > 0 && sz.y > 0) m_sprFundo.setScale({ 1280.f / (float)sz.x, 720.f / (float)sz.y });
        } else { m_texFundo = sf::Texture(); }
    }
}

// Implementação da quebra de linha (Word Wrap)
void Game::wrapText(sf::Text& text, float maxWidth) {
    std::string str = text.getString(); // SFML 3: .toAnsiString() removido, usa string direta
    std::string wrappedString = "";
    
    sf::Text tempText(m_font); 
    tempText.setCharacterSize(text.getCharacterSize());
    
    std::stringstream ss(str);
    std::string word;
    
    while (ss >> word) {
        std::string check = wrappedString + (wrappedString.empty() || wrappedString.back() == '\n' ? "" : " ") + word;
        tempText.setString(check);
        
        if (tempText.getLocalBounds().size.x > maxWidth) {
            wrappedString += '\n';
            wrappedString += word;
        } else {
            if (!wrappedString.empty() && wrappedString.back() != '\n') {
                wrappedString += ' ';
            }
            wrappedString += word;
        }
    }
    text.setString(wrappedString);
}

// --- DADOS (MEMES) ---
// [CORREÇÃO AQUI] Remoção de acentos nas descrições
void Game::inicializarMemes() {
    auto fofao = std::make_shared<Meme>("FOFAO DA CARRETA", "Facil", "assets/fofao.jpg"); 
    fofao->adicionarCaracteristica("Pula muro e danca.");
    
    auto dollynho = std::make_shared<Meme>("DOLLYNHO", "Facil", "assets/dollynho.jpg"); 
    dollynho->adicionarCaracteristica("E seu amiguinho.");
    
    auto bilu = std::make_shared<Meme>("ET BILU", "Facil", "assets/bilu.jpg"); 
    bilu->adicionarCaracteristica("Busquem conhecimento.");
    
    auto jeremias = std::make_shared<Meme>("JEREMIAS", "Facil", "assets/jeremias.jpg"); 
    jeremias->adicionarCaracteristica("Se pudesse matava mil.");
    
    auto borabill = std::make_shared<Meme>("BORA BILL", "Medio", "assets/borabill.jpg"); 
    borabill->adicionarCaracteristica("Grita Bora Bill.");
    
    auto freddie = std::make_shared<Meme>("FREDDIE PRATEADO", "Medio", "assets/freddie.jpg"); 
    freddie->adicionarCaracteristica("Pintado de prata.");
    
    auto nazare = std::make_shared<Meme>("NAZARE CONFUSA", "Medio", "assets/nazare.jpg"); 
    nazare->adicionarCaracteristica("Faz contas no ar.");
    
    auto gravida = std::make_shared<Meme>("GRAVIDA DE TAUBATE", "Medio", "assets/gravida.jpg"); 
    gravida->adicionarCaracteristica("Barriga redonda gigante.");
    
    auto gretchen = std::make_shared<Meme>("GRETCHEN", "Dificil", "assets/gretchen.jpg"); 
    gretchen->adicionarCaracteristica("Rainha do rebolado.");
    
    auto paloma = std::make_shared<Meme>("ADVOGADO PALOMA", "Dificil", "assets/paloma.jpg"); 
    paloma->adicionarCaracteristica("Voz fina.");
    
    auto agostinho = std::make_shared<Meme>("AGOSTINHO CARRARA", "Dificil", "assets/agostinho.jpg"); 
    agostinho->adicionarCaracteristica("Roupas estampadas.");
    
    auto menina = std::make_shared<Meme>("MENINA REVOLTADA", "Dificil", "assets/menina.jpg"); 
    menina->adicionarCaracteristica("Que Xou da Xuxa e esse?");
    
    m_todosMemes = { fofao, dollynho, bilu, jeremias, borabill, freddie, nazare, gravida, gretchen, paloma, agostinho, menina };
}

// --- DADOS (PAÍSES) ---
// [CORREÇÃO AQUI] Remoção de acentos nos locais e frases
void Game::inicializarPaises() {
    auto br = std::make_shared<Country>("BRASIL", "assets/brasil.jpg"); 
    br->adicionarLocal("Rio de Janeiro", false); br->adicionarLocal("Sao Paulo", false); br->adicionarLocal("Testemunha", true, "MEME"); 
    br->adicionarFraseNegativa("Vixi, vi ninguem nao.");

    auto jp = std::make_shared<Country>("JAPAO", "assets/japao.jpg"); 
    jp->adicionarLocal("Tokyo", false); jp->adicionarLocal("Kyoto", false); jp->adicionarLocal("Testemunha", true, "MEME"); 
    jp->adicionarFraseNegativa("Sumimasen, nao vi.");

    auto fr = std::make_shared<Country>("FRANCA", "assets/franca.jpg"); 
    fr->adicionarLocal("Paris", false); fr->adicionarLocal("Nice", false); fr->adicionarLocal("Testemunha", true, "MEME"); 
    fr->adicionarFraseNegativa("Non, jamais.");

    auto us = std::make_shared<Country>("EUA", "assets/eua.jpg"); 
    us->adicionarLocal("Nova York", false); us->adicionarLocal("Los Angeles", false); us->adicionarLocal("Testemunha", true, "MEME"); 
    us->adicionarFraseNegativa("Nope.");

    auto it = std::make_shared<Country>("ITALIA", "assets/italia.jpg"); 
    it->adicionarLocal("Roma", false); it->adicionarLocal("Veneza", false); it->adicionarLocal("Testemunha", true, "MEME"); 
    it->adicionarFraseNegativa("Mamma mia!");

    auto eg = std::make_shared<Country>("EGITO", "assets/egito.jpg"); 
    eg->adicionarLocal("Cairo", false); eg->adicionarLocal("Luxor", false); eg->adicionarLocal("Testemunha", true, "MEME"); 
    eg->adicionarFraseNegativa("Areia.");

    auto ru = std::make_shared<Country>("RUSSIA", "assets/russia.jpg"); 
    ru->adicionarLocal("Moscou", false); ru->adicionarLocal("Siberia", false); ru->adicionarLocal("Testemunha", true, "MEME"); 
    ru->adicionarFraseNegativa("Nyet.");

    auto ch = std::make_shared<Country>("CHINA", "assets/china.jpg"); 
    ch->adicionarLocal("Pequim", false); ch->adicionarLocal("Xangai", false); ch->adicionarLocal("Testemunha", true, "MEME"); 
    ch->adicionarFraseNegativa("Mei you.");

    auto uk = std::make_shared<Country>("INGLATERRA", "assets/inglaterra.jpg"); 
    uk->adicionarLocal("Londres", false); uk->adicionarLocal("Liverpool", false); uk->adicionarLocal("Testemunha", true, "MEME"); 
    uk->adicionarFraseNegativa("Sorry.");

    auto mx = std::make_shared<Country>("MEXICO", "assets/mexico.jpg"); 
    mx->adicionarLocal("Cancun", false); mx->adicionarLocal("Acapulco", false); mx->adicionarLocal("Testemunha", true, "MEME"); 
    mx->adicionarFraseNegativa("No senor.");

    auto ca = std::make_shared<Country>("CANADA", "assets/canada.jpg"); 
    ca->adicionarLocal("Toronto", false); ca->adicionarLocal("Vancouver", false); ca->adicionarLocal("Testemunha", true, "MEME"); 
    ca->adicionarFraseNegativa("Sorry.");

    auto in = std::make_shared<Country>("INDIA", "assets/india.jpg"); 
    in->adicionarLocal("Delhi", false); in->adicionarLocal("Mumbai", false); in->adicionarLocal("Taj Mahal", true, "MEME"); 
    in->adicionarFraseNegativa("Nao.");

    auto es = std::make_shared<Country>("ESPANHA", "assets/espanha.jpg"); 
    es->adicionarLocal("Madrid", false); es->adicionarLocal("Barcelona", false); es->adicionarLocal("Testemunha", true, "MEME"); 
    es->adicionarFraseNegativa("Que va.");

    auto au = std::make_shared<Country>("AUSTRALIA", "assets/australia.jpg"); 
    au->adicionarLocal("Sydney", false); au->adicionarLocal("Melbourne", false); au->adicionarLocal("Testemunha", true, "MEME"); 
    au->adicionarFraseNegativa("No worries.");

    m_todosPaises = { br, jp, fr, us, it, eg, ru, ch, uk, mx, ca, in, es, au };
}

// --- LÓGICA DO JOGO ---
int Game::contarMemesRestantes(std::string nivel) {
    int conta = 0;
    for (auto m : m_todosMemes) {
        bool capturado = false;
        for(auto& nome : m_nomesCapturados) if(nome == m->getNome()) capturado = true;
        if (m->getNivel() == nivel && !capturado) conta++;
    }
    return conta;
}

std::shared_ptr<Meme> Game::sortearMemeDisponivel(std::string nivel) {
    std::vector<std::shared_ptr<Meme>> validos;
    for(auto m : m_todosMemes) {
        bool capturado = false;
        for(const auto& nome : m_nomesCapturados) if(nome == m->getNome()) capturado = true;
        if(!capturado && m->getNivel() == nivel) validos.push_back(m);
    }
    if(validos.empty()) return nullptr; 
    return validos[rand() % validos.size()];
}

void Game::iniciarNovaMissao() {
    if (m_nomesCapturados.size() >= m_todosMemes.size()) { m_estado = EstadoGame::ZEROU; m_sound.playSucesso(); return; }
    m_estado = EstadoGame::INVESTIGANDO; m_energiaLaser = 0.0f; m_horasRestantes = 120; m_opcaoSelecionada = 0;

    int facilRestante = contarMemesRestantes("Facil");
    int medioRestante = contarMemesRestantes("Medio");
    std::string nivel = "Facil"; int rota = 3;

    if (facilRestante > 0) { nivel = "Facil"; rota = 3; m_nivelAtual = 1; } 
    else if (medioRestante > 0) { nivel = "Medio"; rota = 5; m_nivelAtual = 2; } 
    else { nivel = "Dificil"; rota = 7; m_nivelAtual = 3; }

    m_alvoAtual = sortearMemeDisponivel(nivel);
    if (!m_alvoAtual) { m_estado = EstadoGame::ZEROU; return; }

    // [CORREÇÃO AQUI] "NÍVEL" e "NÃO" corrigidos para evitar bugs visuais
    m_textoFeedback.setString("ALVO: " + m_alvoAtual->getNome() + " | NIVEL: " + nivel + " (RESTAM " + std::to_string(contarMemesRestantes(nivel)) + " para Nivel " + std::to_string(m_nivelAtual) + ")");
    wrapText(m_textoFeedback, 1200.f); 

    gerarMissaoLinear(rota);
    atualizarFundo(); 
}

void Game::gerarMissaoLinear(int tamanho) {
    if((int)m_todosPaises.size() < tamanho*2 + 1) return;
    std::random_device rd; std::mt19937 g(rd());
    std::shuffle(m_todosPaises.begin(), m_todosPaises.end(), g);
    m_rotaVitoria.clear();
    std::vector<std::shared_ptr<Country>> rota, errados;
    for(int i=0; i<=tamanho; i++) rota.push_back(m_todosPaises[i]);
    for(int i=tamanho+1; i<(int)m_todosPaises.size(); i++) errados.push_back(m_todosPaises[i]);
    m_rotaVitoria = rota;

    for(int i=0; i<tamanho; i++) {
        auto origem = rota[i]; auto destino = rota[i+1]; auto errado = errados[i % errados.size()];
        auto locsErr = errado->getLocais();
        
        // Frase negativa se errar o país
        for(int k=0; k<(int)locsErr.size(); k++) errado->atualizarTextoLocal(k, errado->getFraseNegativaRegional());
        errado->definirDestinos(origem, origem, true);
        
        // Define as rotas (destino certo vs país errado)
        if(rand()%2) origem->definirDestinos(destino, errado, true); else origem->definirDestinos(errado, destino, false);
        
        auto locsOri = origem->getLocais();
        
        // Lógica de Dicas
        std::vector<int> indicesDicasViagem;
        for(int k=0; k<(int)locsOri.size(); k++) if(!locsOri[k].revelaMeme) indicesDicasViagem.push_back(k);
        std::shuffle(indicesDicasViagem.begin(), indicesDicasViagem.end(), g);
        
        // [CORREÇÃO AQUI] Remoção de acentos nas dicas
        std::string dicaViagem = "A testemunha diz que o suspeito viajou para um lugar onde: " + destino->getFraseNegativaRegional();

        if (indicesDicasViagem.size() > 0) origem->atualizarTextoLocal(indicesDicasViagem[0], dicaViagem); 
        if (indicesDicasViagem.size() > 1) origem->atualizarTextoLocal(indicesDicasViagem[1], dicaViagem);
        
        for(int k=0; k<(int)locsOri.size(); k++) {
            if(locsOri[k].revelaMeme) { 
                origem->atualizarTextoLocal(k, "Dizem que ele: " + m_alvoAtual->getCaracteristicaAleatoria()); 
            } else if (k > 1 && indicesDicasViagem.size() < 3) { 
                origem->atualizarTextoLocal(k, origem->getFraseNegativaRegional()); 
            }
        }
    }
    rota[tamanho]->definirDestinos(nullptr, nullptr, false); m_paisAtual = rota[0];
}

void Game::processEvents() {
    while (const std::optional event = m_window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window->close();
        
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) m_window->close();
            if (kp->code == sf::Keyboard::Key::M) m_sound.toggleMute();

            if (m_estado == EstadoGame::INTRO) {
                if (kp->code == sf::Keyboard::Key::Enter || kp->code == sf::Keyboard::Key::Space) { m_sound.playConfirmar(); iniciarNovaMissao(); } return;
            }

            if (kp->code == sf::Keyboard::Key::Up) { m_opcaoSelecionada--; if(m_opcaoSelecionada<0) m_opcaoSelecionada=0; m_sound.playNavegar(); }
            if (kp->code == sf::Keyboard::Key::Down) { if(m_opcaoSelecionada < (int)m_botoesLocais.size()-1) m_opcaoSelecionada++; m_sound.playNavegar(); }
            
            if (kp->code == sf::Keyboard::Key::Enter) {
                 if (m_estado == EstadoGame::VITORIA || m_estado == EstadoGame::GAME_OVER) { iniciarNovaMissao(); }
            }
        }

        if (m_estado == EstadoGame::INTRO) continue;

        if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mp->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
                if (m_estado == EstadoGame::VITORIA || m_estado == EstadoGame::GAME_OVER) { iniciarNovaMissao(); return; }
                if (m_estado == EstadoGame::ZEROU) return;

                for(size_t i=0; i<m_botoesLocais.size(); ++i) {
                    if(m_botoesLocais[i].getGlobalBounds().contains(pos)) {
                        if(m_estado == EstadoGame::INVESTIGANDO) {
                            if(i == m_paisAtual->getLocais().size()) { 
                                if (m_paisAtual->getOpcaoA() == nullptr) {
                                    if (m_energiaLaser >= 99.f) { 
                                        if(tentarCarregarImagem(m_texMeme, m_alvoAtual->getImagemPath())) {
                                            m_sprMeme.setTexture(m_texMeme, true);
                                            sf::FloatRect b = m_sprMeme.getLocalBounds();
                                            m_sprMeme.setOrigin({b.size.x/2, b.size.y/2});
                                            m_sprMeme.setPosition({900.f, 360.f});
                                            float s = 200.f/b.size.y; m_sprMeme.setScale({s,s});
                                        }
                                        m_estado = EstadoGame::ABDUZINDO; m_sound.playConfirmar();
                                    } else { m_estado = EstadoGame::GAME_OVER; m_sound.playErro(); }
                                } else { m_estado = EstadoGame::VIAJANDO; m_sound.playConfirmar(); }
                            } else {
                                m_horasRestantes -= CUSTO_INVESTIGAR;
                                m_textoFeedback.setString(m_paisAtual->getLocais()[i].pistaTexto);
                                wrapText(m_textoFeedback, 1200.f); 
                                m_sound.playConfirmar();
                                if(m_paisAtual->getLocais()[i].revelaMeme) { m_energiaLaser += 34.0f; if(m_energiaLaser>100) m_energiaLaser=100; }
                            }
                        }
                        else if(m_estado == EstadoGame::VIAJANDO) {
                            m_horasRestantes -= CUSTO_VIAGEM;
                            auto prox = (i==0) ? m_paisAtual->getOpcaoA() : m_paisAtual->getOpcaoB();
                            if(prox) {
                                m_paisAtual = prox; m_estado = EstadoGame::INVESTIGANDO; m_sound.playViagem(); atualizarFundo(); 
                                bool ac = false; for(auto p:m_rotaVitoria) if(p==m_paisAtual) ac=true;
                                if(ac) { m_energiaLaser+=35.f; if(m_energiaLaser>100) m_energiaLaser=100; }
                            }
                        }
                        else if(m_estado == EstadoGame::ABDUZINDO) {
                            auto todos = m_todosMemes;
                            if(i < todos.size()) {
                                if(todos[i] == m_alvoAtual) { 
                                    m_estado = EstadoGame::VITORIA; m_nomesCapturados.push_back(m_alvoAtual->getNome());
                                    if(tentarCarregarImagem(m_texMeme, m_alvoAtual->getImagemPath())) {
                                        m_sprMeme.setTexture(m_texMeme, true);
                                        sf::FloatRect b = m_sprMeme.getLocalBounds();
                                        m_sprMeme.setOrigin({b.size.x/2, b.size.y/2});
                                        m_sprMeme.setPosition({640.f, 360.f});
                                        float s = 450.f/b.size.y; m_sprMeme.setScale({s,s});
                                    }
                                    m_sound.playSucesso();
                                } else { m_estado = EstadoGame::GAME_OVER; m_sound.playErro(); }
                            }
                        }
                    }
                }
            }
        }
        
        if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
             sf::Vector2f pos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
             for(size_t i=0; i<m_botoesLocais.size(); ++i) if(m_botoesLocais[i].getGlobalBounds().contains(pos)) m_opcaoSelecionada = i;
        }
    }
}

void Game::update() {
    if(m_estado == EstadoGame::INTRO) return;

    m_barEnergia.setSize({300.f * (m_energiaLaser/100.f), 30.f});
    if(m_estado == EstadoGame::ZEROU) { m_textoInfo.setString("VOCE ZEROU O JOGO!"); m_botoesLocais.clear(); return; }
    if(m_estado == EstadoGame::VITORIA) { m_textoInfo.setString("VITORIA!"); m_botoesLocais.clear(); return; }
    if(m_estado == EstadoGame::GAME_OVER || m_horasRestantes <= 0) { 
        if(m_estado != EstadoGame::GAME_OVER) m_sound.playErro();
        m_estado = EstadoGame::GAME_OVER; m_textoInfo.setString("GAME OVER"); m_botoesLocais.clear(); return; 
    }

    int horas = m_horasRestantes / 60;
    int minutos = m_horasRestantes % 60;
    std::string horasStr = std::to_string(horas);
    std::string minutosStr = std::to_string(minutos);
    if (minutos < 10) minutosStr = "0" + minutosStr;
    
    m_textoTempo.setString("T-" + horasStr + ":" + minutosStr);
    m_botoesLocais.clear(); float y = 120.f;

    if(m_estado == EstadoGame::INVESTIGANDO) {
        if (m_paisAtual) {
            m_textoInfo.setString(m_paisAtual->getNome());
            for(auto l : m_paisAtual->getLocais()) {
                sf::Text t(m_font); t.setString("[ ] " + l.nomeLocal); t.setCharacterSize(24); t.setPosition({50.f, y});
                t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2.f); m_botoesLocais.push_back(t); y+=40.f;
            }
            sf::Text act(m_font); act.setCharacterSize(24); act.setPosition({50.f, y+30.f}); act.setOutlineColor(sf::Color::Black); act.setOutlineThickness(2.f);
            // [CORREÇÃO AQUI] Texto do botão de abdução/transporte
            if(m_paisAtual->getOpcaoA() == nullptr) act.setString(m_energiaLaser>=99 ? ">> INICIAR ABDUCAO (ESTUDO EM 99%) <<" : "ESTUDO INSUFICIENTE");
            else act.setString(">> SOLICITAR TRANSPORTE <<");
            m_botoesLocais.push_back(act);
        }
    }
    else if(m_estado == EstadoGame::VIAJANDO) {
        m_textoInfo.setString("SELECIONE O DESTINO:");
        sf::Text tA(m_font); if(m_paisAtual && m_paisAtual->getOpcaoA()) tA.setString("[ ] " + m_paisAtual->getOpcaoA()->getNome()); tA.setCharacterSize(30); tA.setPosition({50.f, 200.f}); tA.setOutlineColor(sf::Color::Black); tA.setOutlineThickness(2.f); m_botoesLocais.push_back(tA);
        sf::Text tB(m_font); if(m_paisAtual && m_paisAtual->getOpcaoB()) tB.setString("[ ] " + m_paisAtual->getOpcaoB()->getNome()); tB.setCharacterSize(30); tB.setPosition({50.f, 260.f}); tB.setOutlineColor(sf::Color::Black); tB.setOutlineThickness(2.f); m_botoesLocais.push_back(tB);
    }
    else if(m_estado == EstadoGame::ABDUZINDO) {
        // [CORREÇÃO AQUI] "IDENTIFICAÇÃO" -> "IDENTIFICACAO"
        m_textoInfo.setString("IDENTIFICACAO VISUAL:");
        for(auto m : m_todosMemes) {
            sf::Text t(m_font); t.setString(m->getNome()); t.setCharacterSize(24); t.setPosition({50.f, y}); t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2.f); m_botoesLocais.push_back(t); y+=40.f;
        }
    }

    for(int i=0; i<(int)m_botoesLocais.size(); ++i) {
        if(i == m_opcaoSelecionada) m_botoesLocais[i].setFillColor(sf::Color::Yellow); else m_botoesLocais[i].setFillColor(sf::Color::White);
    }
}

void Game::render() {
    m_window->clear();
    if(m_texFundo.getSize().x > 0) { m_window->draw(m_sprFundo); } else { sf::RectangleShape erro({1280.f, 720.f}); erro.setFillColor(sf::Color::Red); m_window->draw(erro); }

    if(m_estado == EstadoGame::INTRO) {
        m_window->draw(m_modalFundo); m_window->draw(m_modalCaixa); m_window->draw(m_modalTitulo); m_window->draw(m_modalTexto);
        static sf::Clock blink; if(blink.getElapsedTime().asSeconds() < 0.6f) m_window->draw(m_modalBotao);
        if(blink.getElapsedTime().asSeconds() > 1.2f) blink.restart();
    } else {
        if(m_estado == EstadoGame::ABDUZINDO || m_estado == EstadoGame::VITORIA) {
            if(m_texMeme.getSize().x > 0) m_window->draw(m_sprMeme);
            if(m_estado == EstadoGame::VITORIA) {
                sf::Text carimbo(m_font); carimbo.setString("COLETADO"); carimbo.setCharacterSize(100); carimbo.setFillColor(sf::Color(255,0,0,200));
                carimbo.setRotation(sf::degrees(-20.f)); carimbo.setOrigin({carimbo.getLocalBounds().size.x/2, carimbo.getLocalBounds().size.y/2});
                carimbo.setPosition({640.f, 360.f}); carimbo.setOutlineColor(sf::Color::White); carimbo.setOutlineThickness(5.f); carimbo.setStyle(sf::Text::Bold);
                m_window->draw(carimbo);
            }
        }
        
        m_window->draw(m_bgTexto); 
        m_window->draw(m_textoInfo); m_window->draw(m_textoFeedback); m_window->draw(m_textoTempo);
        m_window->draw(m_labelLaser); m_window->draw(m_labelTempo);
        m_window->draw(m_barFundo); m_window->draw(m_barEnergia);
        if(m_sound.isMuted()) { sf::Text t(m_font); t.setString("[MUDO]"); t.setPosition({1180.f, 10.f}); m_window->draw(t); }
        for(auto t : m_botoesLocais) m_window->draw(t);
    }
    m_window->display();
}