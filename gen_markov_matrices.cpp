#include <fstream>
#include <iostream>

const size_t max_pass_len = 10;
const size_t buffer_size = 1024;
const char char_space_start = ' ';
const char char_space_end = '~';
const size_t char_space_len = char_space_end - char_space_start + 1;

typedef uint32_t u32;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <wordlist>\n";
        return 1;
    }

    // 3D matrix that holds the frequencies of occurrence at
    // [place_in_pass][previous_char][current_char]
    u32 markov[max_pass_len][char_space_len][char_space_len] = {0};

    std::cout << "Reading Dictionary...\n";

    // Open input file stream for reading
    std::ifstream infile(argv[1], std::ios::in | std::ios::binary);
    if (!infile) {
        std::cerr << "Couldn't open " << argv[1] << " for reading. Terminating...\n";
        return 2;
    }

    // Buffer to store read line
    char buffer[buffer_size];

    // Fill the Markov matrix
    while (infile.getline(buffer, buffer_size)) {
        // Make sure all characters of the password are in our character space
        // range
        bool in_char_space = true;
        for (char *c = buffer; *c; ++c) {
            if (*c < char_space_start || *c > char_space_end) {
                in_char_space = false;
                break;
            }
        }
        if (!in_char_space) {
            continue;
        }

        // Generate the Markov table
        markov[0][0][(*buffer) - char_space_start]++;
        unsigned cnt_markov = 1;
        for (char *c = buffer + 1; *c && cnt_markov < max_pass_len; ++c, ++cnt_markov) {
            markov[cnt_markov][*(c-1) - char_space_start][*(c) - char_space_start]++;
        }
    }

    std::cout << "Writing markov.dat...\n";

    // Open output file for writing in binary mode, and write the Markov
    // matrix
    std::ofstream outfile("markov.dat", std::ios::binary | std::ios::out);
    if (!outfile) {
        std::cerr << "Couldn't open markov.dat for writing. Terminating...\n";
        return 3;
    }
    outfile.write(reinterpret_cast<char *>(markov), max_pass_len * char_space_len * char_space_len * sizeof(u32));

    return 0;
}
