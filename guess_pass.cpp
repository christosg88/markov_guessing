#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cmath>

#include "CryptLib_Md5.hpp"
#include "ascii_progress_bar.hpp"

typedef uint32_t u32;
typedef uint8_t u8;

const char char_space_start = ' ';
const char char_space_end = '~';
const u32 char_space_len = char_space_end - char_space_start + 1;

u32 min_pass_len = 1;
u32 max_pass_len = 10;
u32 threshold = char_space_len;

/**
 * Prototypes
 */

u8 hex_char_to_byte(char c);
bool check_password(const u8 *hash1, const u8 *hash2);
void parse_args(int &argc, const char **argv);

int main(int argc, const char **argv) {
    // Parse command line arguments
    parse_args(argc, argv);

    // Convert the hash from its hex representation into a Byte array
    u8 hashed[16];
    for (int i = 0; i < 16; ++i) {
        u8 first = hex_char_to_byte(argv[1][2*i]) << 4;
        u8 second = hex_char_to_byte(argv[1][2*i+1]);
        hashed[i] = first + second;
    }

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
    char *pass = new char[max_pass_len + 1];
    Md5Context md5_context;
    MD5_HASH md5_hash;
    for (u32 pass_len = min_pass_len; pass_len <= max_pass_len; ++pass_len) {
        std::cout << "testing length: " << pass_len << "\n";
        ascii_progress_bar bar(static_cast<u64>(std::pow(threshold, pass_len)));

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
                std::cout << "Password found: " << pass << "\n";
                return 0;
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
    std::cout << "Password not found. Try increasing the character space or a different password length.\n";

    return 0;
}

/**
 * Convert a HEX character to the 4-bit value it represents.
 * @param c The HEX character to be converted.
 * @return An 8-bit value with the 4 LSBs holding the value c represents.
 */
u8 hex_char_to_byte(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<u8>(c - '0');
    }
    else if (c >= 'A' && c <= 'F') {
        return static_cast<u8>(c - 'A' + 10);
    }
    else if (c >= 'a' && c <= 'f') {
        return static_cast<u8>(c - 'a' + 10);
    }
    else {
        std::cerr << "The MD5 hash must comprise HEX characters only.\n";
        exit(1);
    }
}

/**
 * Check if the two hashes are equal.
 * @param hash1 The first hash in the form of a Byte array.
 * @param hash2 The second hash in the form of a Byte array.
 * @return true if the hashes match, else false.
 */
bool check_password(const u8 *hash1, const u8 *hash2) {
    for (int i = 0; i < 16; ++i, ++hash1, ++hash2) {
        if (*hash1 != *hash2) {
            return false;
        }
    }
    return true;
}

/**
 * Parses the arguments passed in the command line.
 * @param argc The count of the arguments.
 * @param argv An array of strings that holds the arguments.
 */
void parse_args(int &argc, const char **argv) {
    try {
        switch (argc) {
            case 5:
                // The number of converted characters
                size_t pos;
                max_pass_len = std::min<u32>(max_pass_len, static_cast<u32>(std::stoul(argv[4], &pos)));
                if (pos != strlen(argv[4]) || max_pass_len < 1 || max_pass_len < min_pass_len) {
                    throw std::invalid_argument("max-pass-length(" + std::string(argv[4]) + ") must be a positive integer greater than min-pass-len(" + std::to_string(min_pass_len) + ").\n");
                }
            case 4:
                // The number of converted characters
                min_pass_len = std::max<u32>(min_pass_len, static_cast<u32>(std::stoul(argv[3], &pos)));
                if (pos != strlen(argv[3]) || min_pass_len < 1 || max_pass_len < min_pass_len) {
                    throw std::invalid_argument("min-pass-length(" + std::string(argv[3]) + ") must be a positive integer less than max-pass-len(" + std::to_string(max_pass_len) + ").\n");
                }
            case 3:
                // The number of converted characters
                threshold = std::min<u32>(threshold, static_cast<u32>(std::stoul(argv[2], &pos)));
                if (pos != strlen(argv[2]) || threshold < 1 || threshold > char_space_len) {
                    throw std::invalid_argument("threshold(" + std::string(argv[2]) + ") must be a positive integer less than the character space length(" + std::to_string(char_space_len) + ").\n");
                }
            case 2:
                if (strlen(argv[1]) != 32) {
                    throw std::invalid_argument("The MD5 hash must comprise of 32 HEX characters exactly (currently " + std::to_string(strlen(argv[1])) + ").\n");
                }
                break;
            default:
                std::cerr << "usage: " << argv[0] << " MD5-hashed-password [min-pass-length] [max-pass-length]\n";
                exit(2);
        }
    }
    catch (std::invalid_argument &e) {
        std::cerr << e.what();
        exit(1);
    }
}