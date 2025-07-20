#include "algoritmoGenetico.h"
#include "Utils.h"
#include <chrono>
#include <iostream>
#include <omp.h>

int main() {
    int numExecucoes = 10;
    int seedSize = 18;
    for(int i = 0; i < numExecucoes; i++){
        std::uint64_t seed = lerArquivoSeed("pi.txt",i*seedSize,(i + 1)*seedSize);
        GeneticAlgorithm ga(2000, 0.2f, 120, 0.1f, "entradas/ATT48.txt",seed);
        auto inicio = std::chrono::high_resolution_clock::now();
        ga.executarAlgoritmo();
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duracao = fim - inicio;
        std::cout << "Semente enviada " << seed << std::endl;
        std::cout << "\nTempo de execucao: " << duracao.count() << " segundos\n";

    }
   
    return 0;
}
