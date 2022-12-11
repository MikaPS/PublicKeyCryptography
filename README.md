**How to run the scripts (keygen.c, encrypt.c, decrypt.c)** <br>
The scripts go through the process of public key cryptography. To compile the script, type in the command line “make all”. Afterward, you can run each program individually. To generate a pair of private-public keys, type ./keygen followed by the command line options. To encrypt a message, either type “echo [message] | ./encrypt” or use the command line options. To decrypt a message, either type “echo [encrypted message] | ./decrypt” or use the command line options.
<br>

**Command Line Options** <br>
Keygen program options: -b (min number of bits for the public modulus, default 1024), -i (number of iterations for testing primes, default 50), -n pbfile (public key file, default rsa.pub), -d pvfile (private key file, default rsa.priv), -s (seed, default is seconds since the UNIX epoch), -v (enables verbose output), and -h (displays program synopsis and usage). The number of bits has to be greater than 50, if a smaller number is entered, an error would return. You can mix and match the command options. For example, you are allowed to call -b and -i to both set the number of bits and input file. 


Encrypt program options: -i (input file to encrypt, default is stdin), -o (output file to encrypt, default is stdout), -n (public key file, default is rsa.pub), -v (enables verbose output), -h (displays program synopsis and usage). Again, you can enter multiple commands.


Decrypt program options: -i (input file to decrypt, default is stdin), -o (output file to decrypt, default is stdout), -n (public key file, default is rsa.priv), -v (enables verbose output), -h (displays program synopsis and usage). Again, you can enter multiple commands.


For more information, type any program name with -h. For example, “./keygen -h”, “./encrypt -h”, or “./decrypt -h”

**Files** <br>
DESIGN.pdf - shows my general idea and pseudo-code for my code. It has both my initial design and the final one. 

WRITEUP.pdf - contains my reflection on the assignment (what I learned) and citations of websites I used while completing the assignment.

README.md - has descriptions on how to run the script, files in the directory, and citations. 

Makefile - a script used to compile my sorting file and clean the files after running. You can use it by writing “make {name of function}”. 

decrypt.c - implements a decrypt program that deciphers an encrypted message based on a key

encrypt.c - implements an encrypt program that cipher a message based on a key

keygen.c - implements a keygen program that generates the keys that would be used in the abovementioned programs.

numtheory.c - implements an interface for num theory functions that are used for most calculations in the program.

numtheory.h -  a header file that has the declaration of all functions used in numtheory.c and specifies its interface

randstate.c - implements an interface for randstate functions that are used to generate random numbers in the program

randstate.h - a header file that has the declaration of all functions used in randstate.c and specifies its interface

rsa.c - implements rsa functions that are used to write and read files as well as encrypt and decrypt messages. 

rsa.h - a header file that has the declaration of all functions used in rsa.c and specifies its interface


**Citations** <br>
1)) GMP lib manual - https://gmplib.org/manual/Integer-Functions 

2)) Tutorial on GMP - https://home.cs.colorado.edu/~srirams/courses/csci2824-spr14/gmpTutorial.html 

3)) Fchmod https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-fchmod-change-mode-file-directory-by-descriptor 

4)) Fopen - https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm 

5)) Files - https://www.cs.utah.edu/~germain/PPS/Topics/C_Language/file_IO.html#:~:text=The%20basic%20steps%20for%20using,if%20the%20variable%20%3D%3D%20NULL. 

6)) RSA algorithm visual representation video - https://www.youtube.com/watch?v=j2NBya6ADSY&ab_channel=Programmingw%2FProfessorSluiter 

7)) GMP clears - https://machinecognitis.github.io/Math.Gmp.Native/html/a909faa8-4939-1ba3-5647-af8836c9a291.htm

8)) ASCII Table - https://www.freecodecamp.org/news/ascii-table-hex-to-ascii-value-character-code-chart-2/ 

9)) Diff command -
https://www.geeksforgeeks.org/diff-command-linux-examples/ 

