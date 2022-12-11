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
#include <limits.h>
#include <time.h>
void print_error(void) {
	fprintf(stderr,"Usage: ./keygen [options]\n  ./keygen generates a public / private key pair, placing the keys into the public and private\n  key files as specified below. The keys have a modulus (n) whose length is specified in\n  the program options.\n    -s <seed>   : Use <seed> as the random number seed. Default: time()\n    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
}
int main (int argc, char ** argv) {
    int opt = 0; // used for getopt
    // set default numbers
    uint32_t iter = 50; 
    char public_name[] = "rsa.pub";
    char private_name[] = "rsa.priv";
    uint32_t seed = time(NULL);
//    extern gmp_randstate_t state;
    uint32_t bit = 1024;
    uint32_t message = 0;
  
    // gets user input and runs until processes all the commands
    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) { //list of valid commands
        // min number of bits needed for public modulus
	if (opt == 'b') {
		 bit = strtoul(optarg, NULL, 10);
		 if (bit < 50 || bit > 4096) {
			 fprintf(stderr, "./keygen: Number of bits must be 50-4096, not %d.\n", bit);
		 	 print_error();
			 return 1;
		}
	}
	// number of iterations for testing primes
	if (opt=='i') {
		iter = strtoul(optarg, NULL, 10);
		if (iter < 1 || iter > 500) {
                         fprintf(stderr, "./keygen: Number of iterations must be 1-500, not %d.\n", iter);
			 print_error();
                         return 1;
		}
	}
	// public key name
	if (opt=='n') {
        	strcpy(public_name, optarg);
	}
	 // private key name
	if (opt=='d') {
        	strcpy(private_name, optarg);
	}
	 // set seed
	if (opt=='s') {
                seed = strtoul(optarg, NULL, 10);
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
	if (opt!='h' && opt!='v' && opt!='s' && opt!='d' && opt!='n' && opt!='i' && opt!= 'b') {
		print_error();
		return 1;
	}
	
    }
	randstate_init(seed); // intialize the random state 
	mpz_t p;
        mpz_init(p);
        mpz_t q;
        mpz_init(q);
	mpz_t n;
        mpz_init(n);
	mpz_t e;
        mpz_init(e);
	mpz_t d;
        mpz_init(d);
      	
  	FILE *public = fopen(public_name, "w");  
	if (!public) {// if there was an error with opening the file
		fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory\n", public_name);
		return 1;
	}
	FILE *private = fopen(private_name, "w");
	int priv = fileno(private);
	 if (priv<=0) {// if there was an error with opening the file
                fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory\n", private_name);
                return 1;
        }
	if (fchmod(priv, S_IRUSR |  S_IWUSR) != 0) {
		fprintf(stderr, "chmod error");
	}
	rsa_make_pub(p, q, n, e, bit, iter);
	rsa_make_priv(d,e,p,q);
	// get user name
	char username[LOGIN_NAME_MAX];
	char* u = getenv("USER");
	if (u != NULL) {
		strncpy(username, u, LOGIN_NAME_MAX-1);
	}
	mpz_t user;
	mpz_init(user);
	mpz_set_str(user, username, 62);
	//sign
	mpz_t sign;
	mpz_init(sign);
	rsa_sign(sign, user, d, n);
	rsa_write_pub(n,e,sign, username, public);
	rsa_write_priv(n,d,private);
	
	// verbose
	//mpz_t size_p;
	int size_p = mpz_sizeinbase(p,2);
	int size_q = mpz_sizeinbase(q,2);
	if (message == 1) {
		gmp_fprintf(stderr, "username: %s\nuser signature: %Zd\np (%d bits): %Zd\nq (%d bits): %Zd\nn - modulus (%d bits): %Zd\ne - public exponent (%d bits): %Zd\nd - private exponent (%d bits): %Zd\n", username, sign, size_p, p, size_q, q,mpz_sizeinbase(n,2), n,mpz_sizeinbase(e,2), e, mpz_sizeinbase(d,2), d);
	}

	// end
	randstate_clear();
	fclose(public);
	fclose(private);
	mpz_clears(sign, user, p, q, n, e, d, NULL);
	return 0;
} 

