#include "Game.h"
#include <iostream>
#include <algorithm> 
#include <random>    
#include <fstream>
#include <sstream>

Game::Game() : 
    m_textoInfo(m_font), m_textoFeedback(m_font), m_textoTempo(m_font),
    m_labelLaser(m_font), m_labelTempo(m_font),
    m_modalTitulo(m_font), m_modalTexto(m_font), m_modalBotao(m_font),
    m_sprFundo(m_texFundo), m_sprMeme(m_texMeme), m_opcaoSelecionada(0),
    m_valorPorPista(34.0f) 
{ 
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode({1280, 720}), "Meme Hunter - v17.1 Anti-Exploit");
    m_window->setFramerateLimit(60);

    bool fonteCarregou = false;
    if (m_font.openFromFile("C:/Windows/Fonts/cour.ttf")) fonteCarregou = true;
    else if(m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) fonteCarregou = true;
    if (!fonteCarregou) std::cerr << "[ERRO] Fonte nao encontrada!" << std::endl;

    auto configTexto = [&](sf::Text& t, int size, sf::Color c, float x, float y) {
        t.setFont(m_font); t.setCharacterSize(size); t.setFillColor(c); 
        t.setPosition({x, y});
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2.0f);
    };

    m_bgTexto.setSize({1240.f, 120.f}); m_bgTexto.setPosition({20.f, 560.f});
    m_bgTexto.setFillColor(sf::Color(0, 0, 0, 240)); 
    m_bgTexto.setOutlineColor(sf::Color::Cyan); m_bgTexto.setOutlineThickness(2.f);

    configTexto(m_textoInfo, 28, sf::Color::Green, 40.f, 28.f);
    configTexto(m_textoFeedback, 22, sf::Color::White, 40.f, 580.f);

    configTexto(m_labelTempo, 18, sf::Color::White, 900.f, 25.f); m_labelTempo.setString("TEMPO RESTANTE:");
    configTexto(m_textoTempo, 30, sf::Color::Red, 1080.f, 18.f);

    configTexto(m_labelLaser, 18, sf::Color::White, 900.f, 80.f); m_labelLaser.setString("NIVEL DE ESTUDO:"); 
    
    m_barFundo.setSize({300.f, 25.f}); m_barFundo.setPosition({900.f, 110.f}); 
    m_barFundo.setFillColor(sf::Color(20,20,20, 200)); m_barFundo.setOutlineThickness(2); m_barFundo.setOutlineColor(sf::Color::White);
    m_barEnergia.setSize({0.f, 25.f}); m_barEnergia.setPosition({900.f, 110.f}); m_barEnergia.setFillColor(sf::Color(0, 255, 0));

    // UI INTRO
    m_modalFundo.setSize({1280.f, 720.f}); m_modalFundo.setFillColor(sf::Color(0, 0, 0, 240));
    m_modalCaixa.setSize({800.f, 450.f}); m_modalCaixa.setOrigin({400.f, 225.f}); m_modalCaixa.setPosition({640.f, 360.f});
    m_modalCaixa.setFillColor(sf::Color(20, 20, 50)); m_modalCaixa.setOutlineColor(sf::Color::White); m_modalCaixa.setOutlineThickness(3.f);

    configTexto(m_modalTitulo, 36, sf::Color::Yellow, 640.f, 180.f);
    m_modalTitulo.setString("CONFIDENCIAL - AGENTE M.I.B.");
    m_modalTitulo.setOrigin({m_modalTitulo.getLocalBounds().size.x/2, m_modalTitulo.getLocalBounds().size.y/2});

    configTexto(m_modalTexto, 20, sf::Color::White, 640.f, 360.f);
    m_modalTexto.setString(
        "Bem-vindo ao Nucleo de Analise de Sujeitos Anormais.\n\n"
        "Sua missao: Rastrear e catalogar brasileiros virais.\n\n"
        "PROTOCOLO:\n"
        "- Colete pistas para descobrir o proximo destino.\n"
        "- Identifique o Meme antes de tentar a interceptacao.\n\n"
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

void Game::wrapText(sf::Text& text, float maxWidth) {
    std::string str = text.getString();
    std::string wrappedString = "";
    sf::Text tempText(m_font); 
    tempText.setCharacterSize(text.getCharacterSize());
    std::stringstream ss(str); std::string word;
    while (ss >> word) {
        std::string check = wrappedString + (wrappedString.empty() || wrappedString.back() == '\n' ? "" : " ") + word;
        tempText.setString(check);
        if (tempText.getLocalBounds().size.x > maxWidth) { wrappedString += '\n'; wrappedString += word; } 
        else { if (!wrappedString.empty() && wrappedString.back() != '\n') wrappedString += ' '; wrappedString += word; }
    }
    text.setString(wrappedString);
}

// --- DADOS DOS MEMES ---
void Game::inicializarMemes() {
    auto fofao = std::make_shared<Meme>("FOFAO DA CARRETA", "Facil", "assets/fofao.jpg"); fofao->adicionarCaracteristica("Dizem que ele anda de trenzinho e sobe em muros.");
    auto dollynho = std::make_shared<Meme>("DOLLYNHO", "Facil", "assets/dollynho.jpg"); dollynho->adicionarCaracteristica("Tem formato de garrafa e diz ser nosso amiguinho.");
    auto bilu = std::make_shared<Meme>("ET BILU", "Facil", "assets/bilu.jpg"); bilu->adicionarCaracteristica("Vive se escondendo no mato e pede para buscarem conhecimento.");
    auto jeremias = std::make_shared<Meme>("JEREMIAS", "Facil", "assets/jeremias.jpg"); jeremias->adicionarCaracteristica("Foi detido embriagado e disse que se pudesse matava mil.");
    auto borabill = std::make_shared<Meme>("BORA BILL", "Medio", "assets/borabill.jpg"); borabill->adicionarCaracteristica("Ficou famoso no futebol de varzea gritando 'Bora Bill'.");
    auto freddie = std::make_shared<Meme>("FREDDIE PRATEADO", "Medio", "assets/freddie.jpg"); freddie->adicionarCaracteristica("Usa bigode, roupa de banho e tem a pele pintada de prata.");
    auto nazare = std::make_shared<Meme>("NAZARE CONFUSA", "Medio", "assets/nazare.jpg"); nazare->adicionarCaracteristica("Olha para o nada com cara de duvida vendo formulas matematicas.");
    auto gravida = std::make_shared<Meme>("GRAVIDA DE TAUBATE", "Medio", "assets/gravida.jpg"); gravida->adicionarCaracteristica("Enganou a midia com uma barriga falsa gigante de quadruplos.");
    auto gretchen = std::make_shared<Meme>("GRETCHEN", "Dificil", "assets/gretchen.jpg"); gretchen->adicionarCaracteristica("A rainha da internet, conhecida por seus gifs e rebolado.");
    auto paloma = std::make_shared<Meme>("ADVOGADO PALOMA", "Dificil", "assets/paloma.jpg"); paloma->adicionarCaracteristica("Advogado de voz fina que pede desculpas pelo vacilo.");
    auto agostinho = std::make_shared<Meme>("AGOSTINHO CARRARA", "Dificil", "assets/agostinho.jpg"); agostinho->adicionarCaracteristica("Taxista que adora usar camisas com estampas exageradas.");
    auto menina = std::make_shared<Meme>("MENINA REVOLTADA", "Dificil", "assets/menina.jpg"); menina->adicionarCaracteristica("Ficou famosa ao gritar 'Que Xou da Xuxa e esse?'");
    m_todosMemes = { fofao, dollynho, bilu, jeremias, borabill, freddie, nazare, gravida, gretchen, paloma, agostinho, menina };
}

// --- DICAS REVISADAS ---
void Game::inicializarPaises() {
    auto br = std::make_shared<Country>("BRASIL", "assets/brasil.jpg"); 
    br->adicionarLocal("Rio de Janeiro", false); br->adicionarLocal("Sao Paulo", false); br->adicionarLocal("Testemunha", true, "MEME"); 
    br->adicionarFraseNegativa("Vixi, aqui so tem boleto pra pagar.");
    br->adicionarDicaSobreMim("Ele disse que queria comprar uma camisa verde e amarela.");
    br->adicionarDicaSobreMim("Mencionou que estava com desejo de comer feijoada e ouvir samba.");

    auto jp = std::make_shared<Country>("JAPAO", "assets/japao.jpg"); 
    jp->adicionarLocal("Tokyo", false); jp->adicionarLocal("Kyoto", false); jp->adicionarLocal("Testemunha", true, "MEME"); 
    jp->adicionarFraseNegativa("Sumimasen, nao vi ninguem.");
    jp->adicionarDicaSobreMim("Perguntou onde poderia comprar mangás originais e comer sushi.");
    jp->adicionarDicaSobreMim("Disse que precisava pegar o trem-bala para ver as cerejeiras.");

    auto fr = std::make_shared<Country>("FRANCA", "assets/franca.jpg"); 
    fr->adicionarLocal("Paris", false); fr->adicionarLocal("Nice", false); fr->adicionarLocal("Testemunha", true, "MEME"); 
    fr->adicionarFraseNegativa("Non, je ne sais pas.");
    fr->adicionarDicaSobreMim("Comentou que queria tirar uma foto romântica na Torre Eiffel.");
    fr->adicionarDicaSobreMim("Disse que iria visitar o Museu do Louvre para ver a Mona Lisa.");

    auto us = std::make_shared<Country>("EUA", "assets/eua.jpg"); 
    us->adicionarLocal("Nova York", false); us->adicionarLocal("Los Angeles", false); us->adicionarLocal("Testemunha", true, "MEME"); 
    us->adicionarFraseNegativa("Nope, nothing here.");
    us->adicionarDicaSobreMim("Estava procurando onde vendem os melhores hamburgueres e hot dogs.");
    us->adicionarDicaSobreMim("Disse que queria ver a Estatua da Liberdade de perto.");

    auto it = std::make_shared<Country>("ITALIA", "assets/italia.jpg"); 
    it->adicionarLocal("Roma", false); it->adicionarLocal("Veneza", false); it->adicionarLocal("Testemunha", true, "MEME"); 
    it->adicionarFraseNegativa("Mamma mia, no!");
    it->adicionarDicaSobreMim("Perguntou qual o caminho mais rapido para o Coliseu.");
    it->adicionarDicaSobreMim("Disse que queria andar de gôndola e comer uma pizza autêntica.");

    auto eg = std::make_shared<Country>("EGITO", "assets/egito.jpg"); 
    eg->adicionarLocal("Cairo", false); eg->adicionarLocal("Luxor", false); eg->adicionarLocal("Testemunha", true, "MEME"); 
    eg->adicionarFraseNegativa("Apenas areia e camelos.");
    eg->adicionarDicaSobreMim("Estava ansioso para ver as grandes Piramides e a Esfinge.");
    eg->adicionarDicaSobreMim("Perguntou se era perigoso navegar pelo rio Nilo.");

    auto ru = std::make_shared<Country>("RUSSIA", "assets/russia.jpg"); 
    ru->adicionarLocal("Moscou", false); ru->adicionarLocal("Siberia", false); ru->adicionarLocal("Testemunha", true, "MEME"); 
    ru->adicionarFraseNegativa("Nyet.");
    ru->adicionarDicaSobreMim("Disse que precisava comprar um casaco grosso para o frio.");
    ru->adicionarDicaSobreMim("Mencionou que queria visitar a Praca Vermelha.");

    auto ch = std::make_shared<Country>("CHINA", "assets/china.jpg"); 
    ch->adicionarLocal("Pequim", false); ch->adicionarLocal("Xangai", false); ch->adicionarLocal("Testemunha", true, "MEME"); 
    ch->adicionarFraseNegativa("Mei you.");
    ch->adicionarDicaSobreMim("Falou que seu sonho era caminhar na Grande Muralha.");
    ch->adicionarDicaSobreMim("Perguntou onde poderia ver ursos pandas.");

    auto uk = std::make_shared<Country>("INGLATERRA", "assets/inglaterra.jpg"); 
    uk->adicionarLocal("Londres", false); uk->adicionarLocal("Liverpool", false); uk->adicionarLocal("Testemunha", true, "MEME"); 
    uk->adicionarFraseNegativa("Sorry, mate.");
    uk->adicionarDicaSobreMim("Perguntou se a Rainha estava no palácio.");
    uk->adicionarDicaSobreMim("Disse que queria ouvir as badaladas do Big Ben.");

    auto mx = std::make_shared<Country>("MEXICO", "assets/mexico.jpg"); 
    mx->adicionarLocal("Cancun", false); mx->adicionarLocal("Acapulco", false); mx->adicionarLocal("Testemunha", true, "MEME"); 
    mx->adicionarFraseNegativa("No senor.");
    mx->adicionarDicaSobreMim("Perguntou onde poderia comer tacos bem apimentados.");
    mx->adicionarDicaSobreMim("Disse que queria ouvir mariachis tocando na praça.");

    auto ca = std::make_shared<Country>("CANADA", "assets/canada.jpg"); 
    ca->adicionarLocal("Toronto", false); ca->adicionarLocal("Vancouver", false); ca->adicionarLocal("Testemunha", true, "MEME"); 
    ca->adicionarFraseNegativa("Sorry.");
    ca->adicionarDicaSobreMim("Disse que queria ver a neve e comer panquecas com xarope.");
    ca->adicionarDicaSobreMim("Perguntou se era facil encontrar alces nas florestas.");

    auto in = std::make_shared<Country>("INDIA", "assets/india.jpg"); 
    in->adicionarLocal("Delhi", false); in->adicionarLocal("Mumbai", false); in->adicionarLocal("Taj Mahal", true, "MEME"); 
    in->adicionarFraseNegativa("Nao, amigo.");
    in->adicionarDicaSobreMim("Disse que queria visitar o Taj Mahal.");
    in->adicionarDicaSobreMim("Perguntou onde serviam o melhor curry da cidade.");

    auto es = std::make_shared<Country>("ESPANHA", "assets/espanha.jpg"); 
    es->adicionarLocal("Madrid", false); es->adicionarLocal("Barcelona", false); es->adicionarLocal("Testemunha", true, "MEME"); 
    es->adicionarFraseNegativa("Que va.");
    es->adicionarDicaSobreMim("Disse que queria assistir a uma dança de flamenco.");
    es->adicionarDicaSobreMim("Perguntou onde poderia comer uma paella autêntica.");

    auto au = std::make_shared<Country>("AUSTRALIA", "assets/australia.jpg"); 
    au->adicionarLocal("Sydney", false); au->adicionarLocal("Melbourne", false); au->adicionarLocal("Testemunha", true, "MEME"); 
    au->adicionarFraseNegativa("No worries.");
    au->adicionarDicaSobreMim("Perguntou onde ficavam as melhores praias para surfar.");
    au->adicionarDicaSobreMim("Disse que queria ver cangurus na natureza.");

    m_todosPaises = { br, jp, fr, us, it, eg, ru, ch, uk, mx, ca, in, es, au };
}

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

    m_valorPorPista = 100.0f / (float)rota;

    m_alvoAtual = sortearMemeDisponivel(nivel);
    if (!m_alvoAtual) { m_estado = EstadoGame::ZEROU; return; }

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
        for(int k=0; k<(int)locsErr.size(); k++) errado->atualizarTextoLocal(k, errado->getFraseNegativaRegional());
        errado->definirDestinos(origem, origem, true);
        if(rand()%2) origem->definirDestinos(destino, errado, true); else origem->definirDestinos(errado, destino, false);
        
        auto locsOri = origem->getLocais();
        auto dicasDestino = destino->getTodasDicas(); 
        std::shuffle(dicasDestino.begin(), dicasDestino.end(), g); 
        int contadorDicasDestino = 0;

        for(int k=0; k<(int)locsOri.size(); k++) {
            if(locsOri[k].revelaMeme) {
                origem->atualizarTextoLocal(k, m_alvoAtual->getCaracteristicaAleatoria()); 
            } else {
                if (contadorDicasDestino < (int)dicasDestino.size()) {
                    origem->atualizarTextoLocal(k, dicasDestino[contadorDicasDestino]);
                    contadorDicasDestino++;
                } else {
                    origem->atualizarTextoLocal(k, "O suspeito perguntou como chegar em um lugar onde dizem: " + destino->getFraseNegativaRegional());
                }
            }
        }
    }
    rota[tamanho]->definirDestinos(nullptr, nullptr, false); m_paisAtual = rota[0];
    
    // --- RESET DO CONTROLE DE CLIQUES NA CIDADE INICIAL ---
    m_pistasVisitadas.assign(m_paisAtual->getLocais().size(), false);
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
                                // Lógica de Investigação
                                m_horasRestantes -= CUSTO_INVESTIGAR;
                                m_textoFeedback.setString(m_paisAtual->getLocais()[i].pistaTexto);
                                wrapText(m_textoFeedback, 1200.f); 
                                m_sound.playConfirmar();
                                
                                // --- CORREÇÃO: VERIFICA SE JÁ FOI CLICADO ---
                                if (i < m_pistasVisitadas.size() && !m_pistasVisitadas[i]) {
                                    if(m_paisAtual->getLocais()[i].revelaMeme) { 
                                        m_energiaLaser += m_valorPorPista; 
                                        if(m_energiaLaser > 100.0f) m_energiaLaser = 100.0f; 
                                    }
                                    m_pistasVisitadas[i] = true; // Marca como visitado
                                }
                            }
                        }
                        else if(m_estado == EstadoGame::VIAJANDO) {
                            m_horasRestantes -= CUSTO_VIAGEM;
                            auto prox = (i==0) ? m_paisAtual->getOpcaoA() : m_paisAtual->getOpcaoB();
                            if(prox) {
                                m_paisAtual = prox; m_estado = EstadoGame::INVESTIGANDO; m_sound.playViagem(); atualizarFundo(); 
                                // --- RESET DO CONTROLE DE CLIQUES AO MUDAR DE CIDADE ---
                                m_pistasVisitadas.assign(m_paisAtual->getLocais().size(), false);
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
    if(m_estado == EstadoGame::VITORIA) { m_textoInfo.setString("SUJEITO CONTIDO COM SUCESSO!"); m_botoesLocais.clear(); return; }
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
            if(m_paisAtual->getOpcaoA() == nullptr) act.setString(m_energiaLaser>=99 ? ">> INICIAR PROTOCOLO DE INTERCEPTACAO <<" : "ESTUDO INSUFICIENTE");
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
        m_textoInfo.setString("CONFIRMACAO VISUAL DO ALVO:");
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
        if (m_estado == EstadoGame::INVESTIGANDO && m_energiaLaser >= 99.f) {
            if(tentarCarregarImagem(m_texMeme, m_alvoAtual->getImagemPath())) {
                m_sprMeme.setTexture(m_texMeme, true);
                sf::FloatRect b = m_sprMeme.getLocalBounds();
                m_sprMeme.setOrigin({b.size.x/2, 0}); 
                m_sprMeme.setPosition({1050.f, 150.f}); 
                float s = 150.f/b.size.y; m_sprMeme.setScale({s,s});
                m_window->draw(m_sprMeme);
            }
        }
        
        if(m_estado == EstadoGame::ABDUZINDO || m_estado == EstadoGame::VITORIA) {
            if(m_texMeme.getSize().x > 0) m_window->draw(m_sprMeme);
            if(m_estado == EstadoGame::VITORIA) {
                sf::Text carimbo(m_font); carimbo.setString("MEME CATALOGADO\nPELA NASA"); 
                carimbo.setCharacterSize(60); carimbo.setFillColor(sf::Color(0,255,0,200));
                carimbo.setRotation(sf::degrees(-15.f)); carimbo.setOrigin({carimbo.getLocalBounds().size.x/2, carimbo.getLocalBounds().size.y/2});
                carimbo.setPosition({640.f, 360.f}); carimbo.setOutlineColor(sf::Color::Black); carimbo.setOutlineThickness(3.f); carimbo.setStyle(sf::Text::Bold);
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