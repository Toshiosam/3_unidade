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
    
    std::string getCaracteristicaAleatoria() const {
        if (m_caracteristicas.empty()) return "Sem dados.";
        return m_caracteristicas[rand() % m_caracteristicas.size()];
    }

private:
    std::string m_nome;
    std::string m_nivel;
    std::string m_imagemPath; 
    std::vector<std::string> m_caracteristicas;
};