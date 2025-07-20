#include "algoritmoGenetico.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <numeric>
#include <omp.h>
#include <cmath>

typedef std::vector<Individuo> vectorIndiviudos;

GeneticAlgorithm::GeneticAlgorithm(int nGen, float pMut, int tPop, float nElite, const std::string& caminho,std::uint64_t seed) 
    : seed(seed),numGeracoes(nGen), probMutacao(pMut), tamPopulacao(tPop), tamElite(static_cast<int>(std::floor(nElite * tamPopulacao))){

    matrizDeCaminhos = lerArquivo(caminho);
    tamIndividuo = matrizDeCaminhos.empty() ? 0 : matrizDeCaminhos.size();
}

Individuo GeneticAlgorithm::melhorIndividuo(const std::vector<Individuo>& pop) {
    return *std::min_element(pop.begin(), pop.end(), [](const Individuo& a, const Individuo& b) {
        return a.fitness < b.fitness;
    });
}

double GeneticAlgorithm::calcularDistancia(const std::vector<int> &individuo) const {
    double soma = 0;
    for (size_t i = 0; i < individuo.size(); ++i) {
        int atual = individuo[i] - 1;
        int prox = individuo[(i + 1) % individuo.size()] - 1;
        if (atual < 0 || prox < 0 || atual >= matrizDeCaminhos.size()) return 1e9;
        soma += matrizDeCaminhos[atual][prox];
    }
    return soma;
}


void GeneticAlgorithm::gerarPopulacao(vectorIndiviudos &populacao){
    // std::cout << tamPopulacao;
    std::mt19937 geradorLocal;
    std::uniform_int_distribution<int> dist(0, tamIndividuo - 1);
    for(int i = 0; i < tamPopulacao;i++){
        Individuo newIndividuo;
        newIndividuo.genes.resize(tamIndividuo);
        std::iota(newIndividuo.genes.begin(),newIndividuo.genes.end(),1);
        std::shuffle(newIndividuo.genes.begin(),newIndividuo.genes.end(),geradorLocal);
        newIndividuo.fitness = calcularDistancia(newIndividuo.genes);
        populacao.push_back(newIndividuo);
    }

}

vectorIndiviudos GeneticAlgorithm::selecao(vectorIndiviudos &populacao,std::mt19937 &geradorLocal){
    std::uniform_int_distribution<int> dist(0, populacao.size() - 1);
    std::vector<Individuo> candidatos;
    std::vector<Individuo> pais;

    while(pais.size() < 2){
        candidatos.clear();

        for (int i = 0; i < 4; ++i){
            candidatos.push_back(populacao[dist(geradorLocal)]);
        }
        
        auto melhor = *std::min_element(candidatos.begin(), candidatos.end(), [](auto& a, auto& b) {
            return a.fitness < b.fitness;
        });

        pais.push_back(melhor);
        if (pais.empty() || !(melhor.genes == pais[0].genes)) { pais.push_back(melhor); }
        
    }
    

    return pais;
}

vectorIndiviudos GeneticAlgorithm::selecionarElite(vectorIndiviudos &populacao){
    vectorIndiviudos copia = populacao;
    std::partial_sort(copia.begin(), copia.begin() + tamElite, copia.end(), [](const Individuo& a, const Individuo& b) {
        return a.fitness < b.fitness;
    });

    vectorIndiviudos elite;
    std::unordered_set<std::string> vistos;

    for (const auto& ind : copia) {
        std::string hash;
        for (int gene : ind.genes)
            hash += std::to_string(gene) + ",";

        if (vistos.find(hash) == vistos.end()) {
            vistos.insert(hash);
            elite.push_back(ind);
        }

        if (elite.size() >= tamElite)
            break;
    }

    return elite;
}

void GeneticAlgorithm::mutacao(Individuo &ind, std::mt19937 &geradorLocal) {
    std::uniform_int_distribution<int> dist(0, tamIndividuo - 1);
    int a = dist(geradorLocal), b;
    do { b = dist(geradorLocal); } while (a == b);
    std::swap(ind.genes[a], ind.genes[b]);
    std::reverse(ind.genes.begin() + a, ind.genes.begin() + b);
    //ind.fitness = calcularDistancia(ind.genes);
    ind.fitness = -1;
}

Individuo GeneticAlgorithm::crossoverOX(const Individuo &pai1, const Individuo &pai2,std::mt19937 &geradorLocal) {
    std::uniform_int_distribution<int> dist(0, tamIndividuo - 1);
    int p1 = dist(geradorLocal), p2 = dist(geradorLocal);
    if (p1 > p2) std::swap(p1, p2);

    Individuo filho;
    filho.genes.resize(tamIndividuo, -1);
    std::unordered_set<int> usados;
    for (int i = p1; i < p2; ++i) {
        filho.genes[i] = pai1.genes[i];
        usados.insert(pai1.genes[i]);
    }

    int pos = 0;
    for (int gene : pai2.genes) {
        if (usados.count(gene)) continue;
        while (pos >= p1 && pos < p2) ++pos;
        filho.genes[pos++] = gene;
    }

    //filho.fitness = calcularDistancia(filho.genes);
    filho.fitness = -1;
    return filho;
}

void GeneticAlgorithm::executarAlgoritmo(){
    omp_set_num_threads(4);
    vectorIndiviudos populacao;
    gerarPopulacao(populacao);
    Individuo melhor = melhorIndividuo(populacao);
    int numFilhos =  tamPopulacao - tamElite;
    for(int geracao = 0;geracao < numGeracoes;geracao++){  
        //std::cout << "AQUIIII" << std::endl;
        vectorIndiviudos elite;
        vectorIndiviudos novaPopulacao;
        novaPopulacao.resize(numFilhos);
        elite = selecionarElite(populacao);

        //std::mt19937 geradorLocal(std::chrono::high_resolution_clock::now().time_since_epoch().count() + threadId);
        std::uniform_real_distribution<double> distLocal(0, 1);
        std::mt19937 gerador(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        #pragma omp parallel
        {   
            int threadId = omp_get_thread_num();
            std::mt19937 geradorLocal(seed + geracao + threadId);
            #pragma omp for schedule(static)
            for(int i = 0; i < numFilhos; i++){
                vectorIndiviudos pais;
                pais = selecao(populacao,geradorLocal);
                Individuo prole = crossoverOX(pais[0],pais[1],geradorLocal);
                double mutacaoPro = distLocal(geradorLocal);
                if(mutacaoPro < probMutacao){
                    mutacao(prole,geradorLocal);
                }
                novaPopulacao[i] = prole;
            }
        }

        
            for(int i = 0; i < novaPopulacao.size();i++){
                novaPopulacao[i].fitness = calcularDistancia(novaPopulacao[i].genes);
            }



        Individuo auxMelhor = melhorIndividuo(novaPopulacao);
        if(auxMelhor.fitness < melhor.fitness){
            melhor = auxMelhor;
        }
        novaPopulacao.insert(novaPopulacao.end(),elite.begin(),elite.end());
        populacao = move(novaPopulacao);
    }
    std::cout << "Melhor fitness: " << melhor.fitness << std::endl;
    std::cout << "Solução: " << melhor.cromossomo() << std::endl;
}