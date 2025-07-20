#pragma once
#include <vector>
#include <string>
#include "Individuo.h"
#include <random>

std::vector<std::vector<double>> lerArquivo(const std::string& caminhoArquivo);
std::uint64_t lerArquivoSeed(const std::string& caminho, std::size_t inicio, std::size_t fim);
// void printInd(const Individuo& individuo, const std::string& texto = "");
// void imprimirElementosComDistancias(const std::vector<Individuo>& elementos);