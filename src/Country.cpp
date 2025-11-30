#include "Country.h"
#include <cstdlib> 

Country::Country(std::string nome, std::string caminhoImagem)
    : m_nome(nome), m_imagemPath(caminhoImagem), m_opcaoA_EhCorreta(false) {
}

void Country::definirDestinos(std::shared_ptr<Country> opcaoA, std::shared_ptr<Country> opcaoB, bool aEcorreto) {
    m_proxA = opcaoA;
    m_proxB = opcaoB;
    m_opcaoA_EhCorreta = aEcorreto;
}

void Country::adicionarLocal(std::string nomeLocal, bool ehDoMeme, std::string textoFixo) {
    Investigacao inv;
    inv.nomeLocal = nomeLocal;
    inv.revelaMeme = ehDoMeme;
    if (!textoFixo.empty()) inv.pistaTexto = textoFixo; 
    else inv.pistaTexto = "Investigando...";
    m_locais.push_back(inv);
}

void Country::atualizarTextoLocal(int indice, std::string novoTexto) {
    if (indice >= 0 && indice < (int)m_locais.size()) {
        m_locais[indice].pistaTexto = novoTexto;
    }
}

void Country::adicionarFraseNegativa(std::string frase) {
    m_frasesNegativas.push_back(frase);
}

// Implementação da nova função de dicas
void Country::adicionarDicaSobreMim(std::string dica) {
    m_dicasSobreMim.push_back(dica);
}

std::string Country::getFraseNegativaRegional() const {
    if (m_frasesNegativas.empty()) return "Ninguem viu nada.";
    return m_frasesNegativas[rand() % m_frasesNegativas.size()];
}