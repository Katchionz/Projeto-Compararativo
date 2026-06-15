#ifndef BUSCADOR_HPP
#define BUSCADOR_HPP

#include <string>
#include <vector>

// Nó do Autômato do Aho-Corasick
struct ACNode {
    ACNode* filhos[26];
    ACNode* fail;
    std::vector<int> output; // Guarda os índices das palavras encontradas neste nó

    ACNode();
};

class Buscador {
private:
    // --- Estruturas do Aho-Corasick ---
    ACNode* raiz;
    std::vector<std::string> padroes;
    
    void liberarMemoria(ACNode* node);
    int charParaIndice(char c);

    // --- Estruturas do KMP ---
    std::vector<int> calculaLPS(const std::string& padrao);

public:
    Buscador();
    ~Buscador();

    // Carrega a lista de palavras que vamos buscar no texto
    void carregarPadroes(const std::vector<std::string>& palavras);

    // Algoritmo 1: Aho-Corasick (Construção e Busca)
    void construirAhoCorasick();
    int buscaAhoCorasick(const std::string& texto);

    // Algoritmo 2: KMP (Aplicado para múltiplas palavras)
    int buscaKMP(const std::string& texto);

    // Algoritmo 3: Busca Ingênua (Força Bruta)
    int buscaIngenua(const std::string& texto);
};

#endif