# compiler to use
CXX = g++

# flags to pass compiler
CXXFLAGS = -std=c++11 -Wall -Werror -O3

all: guess_pass gen_markov_matrices Makefile

guess_pass: guess_pass.o CryptLib_Md5.o
	$(CXX) $(CXXFLAGS) -o $@ guess_pass.o CryptLib_Md5.o

gen_markov_matrices: gen_markov_matrices.o
	$(CXX) $(CXXFLAGS) -o $@ gen_markov_matrices.o

guess_pass.o: guess_pass.cpp CryptLib_Md5.hpp ascii_progress_bar.hpp

CryptLib_Md5.o: CryptLib_Md5.cpp CryptLib_Md5.hpp

gen_markov_matrices.o: gen_markov_matrices.cpp

clean:
	rm -f "gen_markov_matrices" "guess_pass" "markov.dat"  *.o
