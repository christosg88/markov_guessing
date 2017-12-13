#include "matrix.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define MAX_PASSWORD_LENGTH 10

int main(int argc, char const *argv[]) {
    std::vector<Matrix<size_t> *> markov_matrices(MAX_PASSWORD_LENGTH);

    markov_matrices[0] = new Matrix<size_t>(94, 1);
    for (int i = 1; i < MAX_PASSWORD_LENGTH; ++i) {
        markov_matrices[i] = new Matrix<size_t>(94, 94);
    }

    std::ifstream infile("rockyou.txt", std::ios::in);

    // Create count matrices
    char *buffer = (char *) malloc((MAX_PASSWORD_LENGTH + 1) * sizeof(char));

    while (infile.getline(buffer, MAX_PASSWORD_LENGTH + 1)) {
        markov_matrices[0]->get(buffer[0]-'!', 0)++;

        int cnt_markov = 1;
        for (int i = 1; buffer[i] != '\0'; ++i) {
            markov_matrices[cnt_markov]->get(buffer[i-1]-'!', buffer[i]-'!')++;
            cnt_markov++;
        }
    }

    for (int cnt_markov = 0; cnt_markov < MAX_PASSWORD_LENGTH; ++cnt_markov) {
        std::stringstream ss;
        ss << std::setw(3) << std::setfill('0') << cnt_markov << ".txt";
        std::ofstream outfile(ss.str());
        outfile << *(markov_matrices[cnt_markov]);
    }

    free(buffer);
    return 0;
}
