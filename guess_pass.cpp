#include "guess_pass.hpp"

int main(int argc, const char **argv) {
    u8 hashed[16];
    u32 min_pass_len = 1;
    u32 max_pass_len = 10;
    u32 threshold = char_space_len;

    // Parse command line arguments
    parse_args(argc, argv, hashed, min_pass_len, max_pass_len, threshold);

    // Crete a guess_pass object to guess the password
    guess_pass gp(hashed, min_pass_len, max_pass_len, threshold);

    // Invoke one of the two guessers, guess_markov_chains() or guess_bruteforce()
    std::string pass = gp.guess_markov_chains();
//    std::string pass = gp.guess_bruteforce();
    if (pass.length()) {
        std::cout << "Password found: " << pass << "\n";
    }
    else {
        std::cout << "Password not found. Try increasing the character space or a different password length.\n";
    }

    return 0;
}

/**
 * Parses the arguments passed in the command line.
 * @param argc The count of the arguments.
 * @param argv An array of strings that holds the arguments.
 */
void parse_args(int &argc, const char **argv, u8 *hashed, u32 &min_pass_len, u32 &max_pass_len, u32 &threshold) {
    try {
        // The number of converted characters
        size_t pos;
        switch (argc) {
            case 5:
                threshold = std::min<u32>(threshold, static_cast<u32>(std::stoul(argv[4], &pos)));
                if (pos != strlen(argv[4]) || threshold < 1 || threshold > char_space_len) {
                    throw std::invalid_argument("threshold(" + std::string(argv[4]) + ") must be a positive integer less than the character space length(" + std::to_string(char_space_len) + ").\n");
                }
            case 4:
                max_pass_len = std::min<u32>(max_pass_len, static_cast<u32>(std::stoul(argv[3], &pos)));
                if (pos != strlen(argv[3]) || max_pass_len < 1 || max_pass_len < min_pass_len) {
                    throw std::invalid_argument("max-pass-length(" + std::string(argv[3]) + ") must be a positive integer greater than min-pass-len(" + std::to_string(min_pass_len) + ").\n");
                }
            case 3:
                min_pass_len = std::max<u32>(min_pass_len, static_cast<u32>(std::stoul(argv[2], &pos)));
                if (pos != strlen(argv[2]) || min_pass_len < 1 || max_pass_len < min_pass_len) {
                    throw std::invalid_argument("min-pass-length(" + std::string(argv[2]) + ") must be a positive integer less than max-pass-len(" + std::to_string(max_pass_len) + ").\n");
                }
            case 2:
                if (strlen(argv[1]) != 32) {
                    throw std::invalid_argument("The MD5 hash must comprise of 32 HEX characters exactly (currently " + std::to_string(strlen(argv[1])) + ").\n");
                }
                else {
                    // Convert the hash from its hex representation into a Byte array
                    for (int i = 0; i < 16; ++i) {
                        u8 first = hex_char_to_byte(argv[1][2*i]) << 4;
                        u8 second = hex_char_to_byte(argv[1][2*i+1]);
                        hashed[i] = first + second;
                    }
                }
                break;
            default:
                std::cerr << "usage: " << argv[0] << " <MD5-hashed-password> [<min-pass-length> [<max-pass-length> [<threshold>]]]\n";
                exit(1);
        }
    }
    catch (std::invalid_argument &e) {
        std::cout << e.what();
        exit(2);
    }
    catch (std::logic_error &e) {
        std::cout << e.what();
        exit(3);
    }
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
