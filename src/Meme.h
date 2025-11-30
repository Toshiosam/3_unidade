#pragma once
#include <string>
#include <vector>
#include <cstdlib>

class Meme {
public:
    Meme(std::string nome, std::string nivel, std::string imagemPath) 
        : m_nome(nome), m_nivel(nivel), m_imagemPath(imagemPath) {}

    void adicionarCaracteristica(std::string texto) {
        m_caracteristicas.push_back(texto);
    }

    std::string getNome() const { return m_nome; }
    std::string getNivel() const { return m_nivel; }
    std::string getImagemPath() const { return m_imagemPath; }
    
    // Mantém a aleatória para casos de fallback
    std::string getCaracteristicaAleatoria() const {
        if (m_caracteristicas.empty()) return "Sem dados.";
        return m_caracteristicas[rand() % m_caracteristicas.size()];
    }

    // NOVA FUNÇÃO: Retorna uma dica específica baseada no progresso da viagem
    std::string getCaracteristica(size_t indice) const {
        if (m_caracteristicas.empty()) return "Sem dados.";
        // O operador % garante que se o índice for maior que o número de dicas, ele volta pro começo
        return m_caracteristicas[indice % m_caracteristicas.size()];
    }

private:
    std::string m_nome;
    std::string m_nivel;
    std::string m_imagemPath; 
    std::vector<std::string> m_caracteristicas;
};