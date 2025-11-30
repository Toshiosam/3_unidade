#pragma once
#include <string>
#include <vector>
#include <memory>

struct Investigacao {
    std::string nomeLocal;
    std::string pistaTexto;
    bool revelaMeme; 
};

class Country {
public:
    Country(std::string nome, std::string caminhoImagem);

    void definirDestinos(std::shared_ptr<Country> opcaoA, std::shared_ptr<Country> opcaoB, bool aEcorreto);
    void adicionarLocal(std::string nomeLocal, bool ehDoMeme, std::string textoFixo = "");
    void atualizarTextoLocal(int indice, std::string novoTexto);
    
    void adicionarFraseNegativa(std::string frase);
    
    std::string getNome() const { return m_nome; }
    std::string getImagemPath() const { return m_imagemPath; }
    const std::vector<Investigacao>& getLocais() const { return m_locais; }
    std::string getFraseNegativaRegional() const;

    std::shared_ptr<Country> getOpcaoA() { return m_proxA; }
    std::shared_ptr<Country> getOpcaoB() { return m_proxB; }

private:
    std::string m_nome;
    std::string m_imagemPath;
    
    std::shared_ptr<Country> m_proxA;
    std::shared_ptr<Country> m_proxB;
    bool m_opcaoA_EhCorreta;
    
    std::vector<Investigacao> m_locais;
    std::vector<std::string> m_frasesNegativas;
};