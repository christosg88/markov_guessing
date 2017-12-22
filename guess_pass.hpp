#ifndef GUESS_PASS_HPP
#define GUESS_PASS_HPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cmath>

#include "CryptLib/lib/CryptLib_Md5.h"
#include "ascii_progress_bar.hpp"

typedef uint32_t u32;
typedef uint8_t u8;

const char char_space_start = ' ';
const char char_space_end = '~';
const u32 char_space_len = char_space_end - char_space_start + 1;

void parse_args(int &argc, const char **argv, u8 *hashed, u32 &min_pass_len, u32 &max_pass_len, u32 &threshold);
u8 hex_char_to_byte(char c);

class guess_pass {
private:
    u8 *hashed;
    u32 min_pass_len;
    u32 max_pass_len;
    u32 threshold;

    /**
     * Check if the two hashes are equal.
     * @param hash1 The first hash in the form of a Byte array.
     * @param hash2 The second hash in the form of a Byte array.
     * @return true if the hashes match, else false.
     */
    bool check_password(const u8 *hash1, const u8 *hash2) const {
        for (int i = 0; i < 16; ++i, ++hash1, ++hash2) {
            if (*hash1 != *hash2) {
                return false;
            }
        }
        return true;
    }

public:
    explicit guess_pass(u8 *hashed, const u32 &min_pass_len, const u32 &max_pass_len, const u32 &threshold) :
            min_pass_len(min_pass_len),
            max_pass_len(max_pass_len),
            threshold(threshold) {
        this->hashed = new u8[16];
        std::copy(hashed, hashed + 16, this->hashed);
    }

    std::string guess_markov_chains() const;

    std::string guess_bruteforce() const;
};

std::string
guess_pass::guess_markov_chains() const {
    // 3D matrix that holds the frequencies of occurrence at
    // [place_in_pass][previous_char][current_char]
    u32 markov[max_pass_len][char_space_len][char_space_len];

    std::cout << "Reading markov.dat...\n";

    // Open input file stream for reading
    std::ifstream infile("markov.dat", std::ios::binary | std::ios::in);
    infile.read(reinterpret_cast<char *>(markov), max_pass_len * char_space_len * char_space_len * sizeof(u32));

    // Create the pairs of char and frequency
    std::pair<char, u32> pairs[max_pass_len][char_space_len][char_space_len];
    for (u32 pos = 0; pos < max_pass_len; ++pos) {
        for (u32 row = 0; row < char_space_len; ++row) {
            for (u32 col = 0; col < char_space_len; ++col) {
                pairs[pos][row][col].first = static_cast<char>(col) + char_space_start;
                pairs[pos][row][col].second = markov[pos][row][col];
            }
        }
    }

    // Sort for each password position and for each preceding character, all the
    // characters that follow based on their frequency
    for (u32 pos = 0; pos < max_pass_len; ++pos) {
        for (u32 row = 0; row < char_space_len; ++row) {
            std::sort(pairs[pos][row], pairs[pos][row]+char_space_len, [](std::pair<char, u32> &a, std::pair<char, u32> &b) {
                if (a.second != b.second)
                    return a.second > b.second;
                else
                    return a.first < b.first;
            });
        }
    }

    std::cout << "Starting Password Guessing...\n";

    // Try to guess the password
    char pass[max_pass_len + 1];
    Md5Context md5_context;
    MD5_HASH md5_hash;
    for (u32 pass_len = min_pass_len; pass_len <= max_pass_len; ++pass_len) {
        std::cout << "testing length: " << pass_len << "\n";
        ascii_progress_bar bar(static_cast<u64>(std::pow(threshold, pass_len)), 100);

        // Terminate the string
        *(pass+pass_len) = '\0';

        u32 cnt[pass_len] = {0};

        *pass = pairs[0][0][cnt[0]].first;
        for (u32 pos = 1; pos < pass_len; ++pos) {
            *(pass+pos) = pairs[pos][*(pass+pos-1) - char_space_start][cnt[pos]].first;
        }

        while (true) {
            bar.progress();
            Md5Initialise(&md5_context);
            Md5Update(&md5_context, pass, pass_len);
            Md5Finalise(&md5_context, &md5_hash);

            if (check_password(hashed, md5_hash.bytes)) {
                bar.stop();
                return std::string(pass);
            }

            u32 pos = pass_len - 1;
            cnt[pos]++;
            while (cnt[pos] == threshold) {
                if (pos == 0)
                    goto finished;

                cnt[pos] = 0;
                pos--;
                cnt[pos]++;
            }

            if (pos == 0) {
                *pass = pairs[0][0][cnt[0]].first;
                pos++;
            }
            for (u32 up = pos; up < pass_len; ++up) {
                *(pass+up) = pairs[up][*(pass+up-1) - char_space_start][cnt[up]].first;
            }
        }
        finished:;
        bar.finish();
    }
    return std::string();
}

std::string
guess_pass::guess_bruteforce() const {
    std::cout << "Starting Password Guessing...\n";

    // Try to guess the password
    char pass[max_pass_len + 1];
    Md5Context md5_context;
    MD5_HASH md5_hash;
    for (u32 pass_len = min_pass_len; pass_len <= max_pass_len; ++pass_len) {
        std::cout << "testing length: " << pass_len << "\n";
        ascii_progress_bar bar(static_cast<u64>(std::pow(char_space_len, pass_len)), 100);

        // Terminate the string
        *(pass+pass_len) = '\0';

        for (u32 pos = 0; pos < pass_len; ++pos) {
            *(pass+pos) = char_space_start;
        }

        while (true) {
            bar.progress();
            Md5Initialise(&md5_context);
            Md5Update(&md5_context, pass, pass_len);
            Md5Finalise(&md5_context, &md5_hash);

            if (check_password(hashed, md5_hash.bytes)) {
                bar.stop();
                return std::string(pass);
            }

            u32 pos = pass_len - 1;
            while (++pass[pos] == char_space_end) {
                if (pos == 0)
                    goto finished;

                pass[pos] = char_space_start;
                pos--;
            }
        }
        finished:;
        bar.finish();
    }
    return std::string();
}

#endif //MARKOV_GUESSING_GUESS_PASS_HPP
