#include "Buscador.hpp"
#include <iostream>
#include <cctype>

using namespace std;

// --- CONSTRUÇÃO BASE ---

ACNode::ACNode() {
    fail = nullptr;
    for (int i = 0; i < 26; i++) filhos[i] = nullptr;
}

Buscador::Buscador() {
    raiz = new ACNode();
}

Buscador::~Buscador() {
    liberarMemoria(raiz);
}

void Buscador::liberarMemoria(ACNode* node) {
    if (!node) return;
    for (int i = 0; i < 26; i++) {
        liberarMemoria(node->filhos[i]);
    }
    delete node;
}

int Buscador::charParaIndice(char c) {
    if (isalpha(c)) return tolower(c) - 'a';
    return -1; // Caracteres inválidos ou pontuações
}

void Buscador::carregarPadroes(const vector<string>& palavras) {
    padroes = palavras;
}

// ==========================================
// 1. AHO-CORASICK (O Mestre das Múltiplas Buscas)
// ==========================================

void Buscador::construirAhoCorasick() {
    // 1. Inserção na Trie (Árvore de Prefixos)
    for (size_t i = 0; i < padroes.size(); i++) {
        ACNode* atual = raiz;
        for (char c : padroes[i]) {
            int idx = charParaIndice(c);
            if (idx == -1) continue;
            if (atual->filhos[idx] == nullptr) {
                atual->filhos[idx] = new ACNode();
            }
            atual = atual->filhos[idx];
        }
        atual->output.push_back(i); // Registra que a palavra 'i' termina aqui
    }

    // 2. Construção dos Fail Links via BFS (Usando vetor manual como Fila)
    vector<ACNode*> fila;
    int inicioFila = 0;

    // Inicializa os filhos da raiz
    for (int i = 0; i < 26; i++) {
        if (raiz->filhos[i] != nullptr) {
            raiz->filhos[i]->fail = raiz;
            fila.push_back(raiz->filhos[i]);
        }
    }

    // Processamento da Fila (BFS Manual)
    while (inicioFila < (int)fila.size()) {
        ACNode* atual = fila[inicioFila++];

        for (int i = 0; i < 26; i++) {
            if (atual->filhos[i] != nullptr) {
                ACNode* filho = atual->filhos[i];
                ACNode* failNode = atual->fail;

                // Volta pelos fail links até achar um caminho válido ou chegar na raiz
                while (failNode != raiz && failNode->filhos[i] == nullptr) {
                    failNode = failNode->fail;
                }

                if (failNode->filhos[i] != nullptr) {
                    filho->fail = failNode->filhos[i];
                } else {
                    filho->fail = raiz;
                }

                // Mescla os outputs (se o fail link também completa uma palavra)
                for (int outIdx : filho->fail->output) {
                    filho->output.push_back(outIdx);
                }

                fila.push_back(filho);
            }
        }
    }
}

int Buscador::buscaAhoCorasick(const string& texto) {
    int ocorrencias = 0;
    ACNode* atual = raiz;

    for (char c : texto) {
        int idx = charParaIndice(c);
        if (idx == -1) continue; // Pula espaços e pontuação

        // Segue os fail links se o caminho atual não existir
        while (atual != raiz && atual->filhos[idx] == nullptr) {
            atual = atual->fail;
        }

        if (atual->filhos[idx] != nullptr) {
            atual = atual->filhos[idx];
        }

        // Se houver output, encontramos palavras!
        ocorrencias += atual->output.size();
    }
    return ocorrencias;
}

// ==========================================
// 2. BUSCA KMP (Knuth-Morris-Pratt)
// ==========================================

vector<int> Buscador::calculaLPS(const string& padrao) {
    int m = padrao.length();
    vector<int> lps(m, 0);
    int tamanho = 0, i = 1;

    while (i < m) {
        if (padrao[i] == padrao[tamanho]) {
            tamanho++;
            lps[i] = tamanho;
            i++;
        } else {
            if (tamanho != 0) tamanho = lps[tamanho - 1];
            else { lps[i] = 0; i++; }
        }
    }
    return lps;
}

int Buscador::buscaKMP(const string& texto) {
    int ocorrencias = 0;
    
    // O KMP só busca uma palavra por vez. Logo, precisamos de um loop for extra.
    for (const string& padrao : padroes) {
        if (padrao.empty()) continue;
        
        vector<int> lps = calculaLPS(padrao);
        int i = 0, j = 0;
        int n = texto.length();
        int m = padrao.length();

        while (i < n) {
            // Simplificação para ignorar case e pontuação no match
            int cTexto = charParaIndice(texto[i]);
            int cPadrao = charParaIndice(padrao[j]);

            if (cTexto == cPadrao && cTexto != -1) {
                j++;
                i++;
            } else if (cTexto == -1) {
                i++; // Pula pontuação no texto original
            } else {
                if (j != 0) j = lps[j - 1];
                else i++;
            }

            if (j == m) {
                ocorrencias++;
                j = lps[j - 1];
            }
        }
    }
    return ocorrencias;
}

// ==========================================
// 3. BUSCA INGÊNUA (Força Bruta)
// ==========================================

int Buscador::buscaIngenua(const string& texto) {
    int ocorrencias = 0;

    // Para cada palavra...
    for (const string& padrao : padroes) {
        if (padrao.empty()) continue;
        int n = texto.length();
        int m = padrao.length();

        // Varre o texto...
        for (int i = 0; i <= n - m; i++) {
            int j = 0;
            int offsetTexto = 0;

            // Tenta dar o match caractere por caractere
            while (j < m && (i + offsetTexto) < n) {
                int cTexto = charParaIndice(texto[i + offsetTexto]);
                int cPadrao = charParaIndice(padrao[j]);

                if (cTexto == -1) { // Ignora pontuação no texto
                    offsetTexto++;
                    continue;
                }
                
                if (cTexto != cPadrao) break;
                
                j++;
                offsetTexto++;
            }
            if (j == m) ocorrencias++;
        }
    }
    return ocorrencias;
}