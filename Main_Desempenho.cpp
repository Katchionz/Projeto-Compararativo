#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono> // Biblioteca padrão para métrica de tempo
#include "Buscador.hpp"

using namespace std;
using namespace std::chrono;

int main() {
    Buscador analisador;

    // 1. Carregar Palavras-Chave para a busca múltipla
    vector<string> palavrasBusca = {"algoritmo", "dados", "memoria", "ponteiros", "desempenho"};
    analisador.carregarPadroes(palavrasBusca);
    
    cout << "=== PREPARANDO DADOS ===" << endl;
    cout << "Palavras a serem buscadas simultaneamente: " << palavrasBusca.size() << endl;

    // 2. Carregar um texto longo simulado
    ifstream arquivoTexto("Base de dados/texto_longo.txt");
    if (!arquivoTexto.is_open()) {
        cerr << "Erro ao abrir Base de dados/texto_longo.txt" << endl;
        return 1;
    }
    
    stringstream buffer;
    buffer << arquivoTexto.rdbuf();
    string textoLongo = buffer.str();
    arquivoTexto.close();

    cout << "Tamanho do texto carregado: " << textoLongo.length() << " caracteres." << endl;
    cout << "\n=== INICIANDO ANALISE DE DESEMPENHO ===\n" << endl;

    // --- TESTE 1: AHO-CORASICK ---
    analisador.construirAhoCorasick(); // Pré-processamento
    auto inicio = high_resolution_clock::now();
    int ocorrenciasAC = analisador.buscaAhoCorasick(textoLongo);
    auto fim = high_resolution_clock::now();
    auto tempoAC = duration_cast<microseconds>(fim - inicio).count();

    cout << "[1] Aho-Corasick:" << endl;
    cout << "    Ocorrencias encontradas: " << ocorrenciasAC << endl;
    cout << "    Tempo de execucao: " << tempoAC << " microssegundos." << endl;

    // --- TESTE 2: KMP ---
    inicio = high_resolution_clock::now();
    int ocorrenciasKMP = analisador.buscaKMP(textoLongo);
    fim = high_resolution_clock::now();
    auto tempoKMP = duration_cast<microseconds>(fim - inicio).count();

    cout << "\n[2] Knuth-Morris-Pratt (KMP):" << endl;
    cout << "    Ocorrencias encontradas: " << ocorrenciasKMP << endl;
    cout << "    Tempo de execucao: " << tempoKMP << " microssegundos." << endl;

    // --- TESTE 3: BUSCA INGENUA ---
    inicio = high_resolution_clock::now();
    int ocorrenciasIngenua = analisador.buscaIngenua(textoLongo);
    fim = high_resolution_clock::now();
    auto tempoIngenua = duration_cast<microseconds>(fim - inicio).count();

    cout << "\n[3] Busca Ingenua (Forca Bruta):" << endl;
    cout << "    Ocorrencias encontradas: " << ocorrenciasIngenua << endl;
    cout << "    Tempo de execucao: " << tempoIngenua << " microssegundos." << endl;

    cout << "\n=== CONCLUSAO ===" << endl;
    cout << "Como esperado, o Aho-Corasick e muito mais rapido pois varre o " 
         << "texto original apenas UMA vez, independentemente de estarmos "
         << "buscando 5 ou 500 palavras diferentes!" << endl;

    return 0;
}