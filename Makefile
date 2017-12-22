# compiler to use
CXX = g++

# flags to pass compiler
CXXFLAGS = -std=c++11 -Wall -Werror -O3

LIBDIR = CryptLib/lib

all: gen_markov_matrices guess_pass Makefile

gen_markov_matrices: gen_markov_matrices.o
	$(CXX) $(CXXFLAGS) -o $@ gen_markov_matrices.o

guess_pass: guess_pass.o CryptLib_Md5.o
	$(CXX) $(CXXFLAGS) -o $@ guess_pass.o CryptLib_Md5.o

gen_markov_matrices.o: gen_markov_matrices.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ gen_markov_matrices.cpp

guess_pass.o: guess_pass.cpp $(LIBDIR)/CryptLib_Md5.h ascii_progress_bar.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ guess_pass.cpp

CryptLib_Md5.o: $(LIBDIR)/CryptLib_Md5.c $(LIBDIR)/CryptLib_Md5.h
	$(CXX) $(CXXFLAGS) -c -o $@ $(LIBDIR)/CryptLib_Md5.c

.PHONY: clean

clean:
	rm -f "gen_markov_matrices" "guess_pass" "markov.dat"  *.o
