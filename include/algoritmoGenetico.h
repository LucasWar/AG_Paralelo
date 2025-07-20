#pragma once
#include <vector>
#include <random>
#include "Individuo.h"

class GeneticAlgorithm {
private:
    typedef std::vector<Individuo> vetorIndiviudos;
    
    int numGeracoes, tamPopulacao, numNovosIndividuos, tamIndividuo;
    float probMutacao,tamElite;
    std::uint64_t seed;
    std::vector<std::vector<double>> matrizDeCaminhos;
    vetorIndiviudos populacao;
    // std::mt19937 gerador;

    vetorIndiviudos selecao(vetorIndiviudos &populacao,std::mt19937 &geradorLocal);
    
    Individuo melhorIndividuo(const vetorIndiviudos &populacao);
    double calcularDistancia(const std::vector<int>& individuo) const;
    
    void mutacao(Individuo &individuo,std::mt19937 &geradorLocal);
    Individuo crossoverOX(const Individuo &pai1, const Individuo &pai2,std::mt19937 &geradorLocal);
     
public:
    GeneticAlgorithm(int numGeracoes, float probMutacao, int tamPopulacao, 
                     float tamElite, const std::string& caminhoArquivo, std::uint64_t seed);
    void gerarPopulacao(vetorIndiviudos &populacao);
    vetorIndiviudos selecionarElite(vetorIndiviudos &atualPopulacao);
    void executarAlgoritmo();
};