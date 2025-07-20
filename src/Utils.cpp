#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<std::vector<double>> lerArquivo(const std::string& caminhoArquivo) {
    std::ifstream arquivo(caminhoArquivo);
    std::vector<std::vector<double>> matriz;
    std::string linha;
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << std::endl;
        return {};
    }
    while (std::getline(arquivo, linha)) {
        std::istringstream iss(linha);
        std::vector<double> linhaNumeros;
        double num;
        while (iss >> num) linhaNumeros.push_back(num);
        if (!linhaNumeros.empty()) matriz.push_back(std::move(linhaNumeros));
    }
    return matriz;
}


std::uint64_t lerArquivoSeed(const std::string& caminho, std::size_t inicio, std::size_t fim) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao abrir o arquivo.");
    }

    std::string linhaCompleta;
    std::getline(arquivo, linhaCompleta); // Lê toda a linha (assumindo que o número está em uma só linha)
    arquivo.close();

    if (fim > linhaCompleta.size() || inicio >= fim) {
        throw std::invalid_argument("Intervalo inválido de leitura.");
    }

    std::string trecho = linhaCompleta.substr(inicio, fim - inicio);
    
    // Remove possíveis zeros à esquerda (opcional, mas seguro)
    trecho.erase(0, trecho.find_first_not_of('0'));
    if (trecho.empty()) trecho = "0"; // se tudo for zero

    // Converte para uint64_t
    std::uint64_t resultado = 0;
    for (char c : trecho) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("Caractere inválido encontrado.");
        }
        resultado = resultado * 10 + (c - '0');
    }

    return resultado;
}