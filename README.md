# markov_guessing
This project is an attempt to implement and use Markov chains in order to reduce the character space and improve the sequence, of the passwords generated when trying to guess the plain-text password that was used to generate a given MD5 hashed password.

The project doesn't try to achieve best performance or generalized use with other hash functions, only show a proof of concept for the advantages of Markov chains. In case you want to use them for real-world application where performance and generalization are critical, take a look at [hashcat](https://github.com/hashcat/hashcat), which is a tool used for password guessing, and more specifically at [hcstatgen](https://github.com/hashcat/hashcat-utils/blob/master/src/hcstatgen.c) under [hashcat-utils](https://github.com/hashcat/hashcat-utils), which is a tool for generating Markov matrices. Hashcat is the main source for the idea of the usage of Markov chains.

## Compilation
To compile the source code, just run:
```
make
```
This will  compile all the necessary files and their dependencies.

## File Explanation and Usage
-   `gen_markov_matrices` will generate a binary file, that contains information about the dictionary of passwords that was passed as argument to the executable. The file will be called `markov.dat` and will have a size of 361KB exactly. The correct usage of `gen_markov_matrices` is:

    ```
    ./gen_markov_matrices <wordlist>
    ```

-   `guess_pass` will use the aforementioned generated file, to try and guess the plain-text password that was passed as argument to the executable. You can additionally specify the minimum length of passwords to be tested, the maximum length of passwords to be tested, and the threshold, denoting the number of characters to try at each position in the password. In case you want to try out the brute-force attack mode, just comment line 16 and uncomment line 17, then recompile. The threshold argument is not taken into consideration in this case. The correct usage of `guess_pass` is:

    ```
    ./guess_pass <MD5-hashed-password> [<min-pass-length> [<max-pass-length> [<threshold>]]]
    ```

## Example Execution and Results

### Compile
```
make
```
Output:
```
g++ -std=c++11 -Wall -Werror -O3 -c -o gen_markov_matrices.o gen_markov_matrices.cpp
g++ -std=c++11 -Wall -Werror -O3 -o gen_markov_matrices gen_markov_matrices.o
g++ -std=c++11 -Wall -Werror -O3 -c -o guess_pass.o guess_pass.cpp
g++ -std=c++11 -Wall -Werror -O3 -c -o CryptLib_Md5.o CryptLib/lib/CryptLib_Md5.c
g++ -std=c++11 -Wall -Werror -O3 -o guess_pass guess_pass.o CryptLib_Md5.o
```

### Generate Markov Matrices
```
./gen_markov_matrices rockyou.txt
```
Output:
```
Reading Dictionary...
Writing markov.dat...
```

### Generate a Hash for Testing
```
echo -n 'apple' | md5sum
```
Output:
```
1f3870be274f6c49b3e31a0c6728957f  -
```
### Guess Password
```
./guess_pass 1f3870be274f6c49b3e31a0c6728957f 1 8 30
```
Output:
```
Reading markov.dat...
Starting Password Guessing...
testing length: 1
100% [====================================================================================================]
testing length: 2
100% [====================================================================================================]
testing length: 3
100% [====================================================================================================]
testing length: 4
100% [====================================================================================================]
testing length: 5
 11% [===========                                                                                         ]
Password found: apple
```
