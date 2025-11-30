#include "Country.h"
#include <cstdlib> // Para rand()

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
    
    // Se vier um texto fixo (ex: "MEME"), usamos ele. 
    // Caso contrário, colocamos um placeholder que será substituído pelas dicas depois.
    if (!textoFixo.empty()) {
        inv.pistaTexto = textoFixo;
    } else {
        inv.pistaTexto = "Investigando...";
    }
    
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

std::string Country::getFraseNegativaRegional() const {
    if (m_frasesNegativas.empty()) {
        return "Ninguem viu nada por aqui."; // Sem acento
    }
    // Retorna uma frase aleatória da lista deste país
    int indice = rand() % m_frasesNegativas.size();
    return m_frasesNegativas[indice];
}