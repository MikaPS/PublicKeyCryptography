// implements encrypt program
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

int print_error(void) {
	fprintf(stderr, "Usage: ./encrypt [options]\n  ./encrypt encrypts an input file using the specified public key file,\n  writing the result to the specified output file.\n    -i <infile> : Read input from <infile>. Default: standard input.\n    -o <outfile>: Write output to <outfile>. Default: standard output.\n    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
	return 0;
}

int main (int argc, char ** argv) {
    int opt = 0; // used for getopt
    // set default numbers
    char input[] = "stdin"; 
    char output[] = "stdout";
    char file[] = "rsa.pub";
    int give_out = 0;
    int give_in = 0;
    uint32_t message = 0;
  
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
         	print_error();
		return 0;
        }
	// if it's not in the above options, return an error number
	if (opt!='h' && opt!='v' && opt!='n' && opt!='o' && opt!= 'i') {
		print_error();
		return 1;
	}
	
    }
 
	mpz_t n;
	mpz_init(n);
	mpz_t e;
        mpz_init(e);
	char username[1024];
	mpz_t user;
	mpz_init(user);
	mpz_t s;
	mpz_init(s);
	
	// files
    	FILE *public = fopen(file, "r");
	if (!public) {// if there was an error with opening the file
		fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory\n", file);
		return 1;
	}
	// deafult is stdout and stdin, but if the user specifies a different file, use them
	FILE *out = stdout;
	FILE *in = stdin;
        if (give_in == 1) {
		in = fopen(input, "r");
	}
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


	rsa_read_pub(n, e, s, username, public);
	// verbose
	if (message == 1) {	
		gmp_fprintf(stderr, "username: %s\nuser signature: %Zd\nn - modulus (%d bits): %Zd\ne - public exponenet (%d bits): %Zd\n", username, s, mpz_sizeinbase(n,2), n, mpz_sizeinbase(e,2), e);
	}

	mpz_set_str(user, username, 62);
	bool r = rsa_verify(user,s,e,n);
	if (r==false) {		
		fprintf(stderr,"./encrypt: Couldn't verify user signature - exiting!\n");
		return 1;
	}

	rsa_encrypt_file(in,out,n,e);
	fclose(public);
	if (give_in == 1) { fclose(in); }
	if (give_out == 1) { fclose(out); } 
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(s);
	mpz_clear(user);
	return 0;
}

