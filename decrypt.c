// implement decrypt program

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <gmp.h>
#include <sys/stat.h>
#include <string.h> 
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

int print_file(void) {
	fprintf(stderr, "Usage: ./decrypt [options]\n  ./decrypt decrypts an input file using the specified private key file,\n  writing the result to the specified output file.\n    -i <infile> : Read input from <infile>. Default: standard input.\n    -o <outfile>: Write output to <outfile>. Default: standard output.\n    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
	return 0;
}

int main (int argc, char ** argv) {
    int opt = 0; // used for getopt
    // set default numbers
    char input[] = "stdin"; 
    char output[] = "stdout";
    char file[] = "rsa.priv";
    uint32_t message = 0;
    int give_out = 0;  
    int give_in = 0;

    // gets user input and runs until processes all the commands
    while ((opt = getopt(argc, argv, "i:o:nvh")) != -1) { //list of valid commands
        // specifies inputfile
	if (opt == 'i') {
		give_in = 1;
		strcpy(input, optarg);
	}
	// specifies output file
	if (opt=='o') {
		give_out = 1;
		strcpy(output, optarg);
	}
	// public key name
	if (opt=='n') {
        	strcpy(file, optarg);
	}
	// enables verbose
	if (opt=='v') { 
		 message = 1;
        }
	 // usage message
	if (opt=='h') { 
         	print_file();
		return 0;
        }
	// if it's not in the above options, return an error number
	if (opt!='h' && opt!='v' && opt!='n' && opt!='o' && opt!= 'i') {
		print_file();
		return 1;
	}
	
    }
 
	mpz_t n;
	mpz_init(n);
	mpz_t d;
        mpz_init(d);
	mpz_t s;
	mpz_init(s);
		
    	FILE *priv = fopen(file, "r");
	if (!priv) {// if there was an error with opening the file
		fprintf(stderr, "can't open private file :(\n");
		return 1;
	}

	// use stdin and stdout as default values, but if the user specifies a different file, use it
	FILE *in = stdin;
	if (give_in == 1) { in = fopen(input, "r"); }
	FILE *out = stdout;
	if (give_out == 1) {
		out = fopen(output, "w");
	}
	if (!in) {// if there was an error with opening the file
                fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory\n", input);
                return 1;
        }
	if (!out) {// if there was an error with opening the file
                fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory\n", output);
                return 1;
        }

	rsa_read_priv(n, d, priv);
	// verbose
	if (message == 1) {	
		gmp_fprintf(stderr, "n - modulus (%d bits): %Zd\nd - private key (%d bits): %Zd\n",  mpz_sizeinbase(n,2), n, mpz_sizeinbase(d,2), d);
	}

	rsa_decrypt_file(in,out,n,d);
	
	// close files and clear vars
	fclose(priv);
	if (give_in == 1) { fclose(in); }
	if (give_out == 1) { fclose(out); }
	mpz_clear(n);
	mpz_clear(d);
	mpz_clear(s);
	return 0;
}


