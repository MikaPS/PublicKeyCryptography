// implements an RSA library
#include "rsa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "numtheory.h"
#include <stdlib.h>
#include <inttypes.h>

// Generates the components for a new public RSA key.
// p and q will be large primes with n their product.
// The product n will be of a specified minimum number of bits.
// The primality is tested using Miller-Rabin.
// The public exponent e will have around the same number of bits as n.
// All mpz_t arguments are expected to be initialized.
//
// p: will store the first large prime.
// q: will store the second large prime.
// n: will store the product of p and q.
// e: will store the public exponent.
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	extern gmp_randstate_t state; // gets the random state
	// assigns a specific number of bits to p and q
	mpz_t p_bits;
       	mpz_init(p_bits);
	mpz_t n_bits;
        mpz_init(n_bits);
        mpz_t q_bits;
        mpz_init(q_bits);
	uint64_t size = mpz_sizeinbase(n,2);
	while (size < nbits) { // log2(n) needs to >= than nbits
		// generate a random number in the range (nbits/4 to 3nbits/4)
		uint64_t rand = (random() % (3*nbits/4 + 1 - (nbits/4))) + (nbits/4);
		mpz_set_ui(p_bits, rand);
		mpz_set_ui(n_bits, nbits);
		mpz_sub(q_bits, n_bits, p_bits);
		// create two prime numbers p and q
		make_prime(p, mpz_get_ui(p_bits), iters);
		make_prime(q, mpz_get_ui(q_bits), iters);
		mpz_mul(n,p,q); // calculates n
		size = mpz_sizeinbase(n,2); // find log2(n)
	}
	// step 2: find the totient number of p and q
	mpz_t g;
	mpz_init(g);
	mpz_t p_1;
        mpz_init(p_1);
	mpz_sub_ui(p_1, p, 1); // p-1
	mpz_t q_1;
        mpz_init(q_1);
        mpz_sub_ui(q_1, q, 1); // q-1
	gcd(g, p_1, q_1); //gcd(p-1,q-1)
	mpz_t t;
	mpz_init(t);
	mpz_sub_ui(q_1, q, 1);
	mpz_sub_ui(p_1, p, 1);
	mpz_mul(t, p_1, q_1); // t(p-1,q-1) = (p-2)(q-2)
	// computes lcm
	mpz_div(t, t, g); // lcm(p-1, q-1)

	//part 3: find the public exponent.
	// the public componenet needs to be coprime with the lcm of p-1 and q-1.
	// this means that the gcd of the public componenet and lcm needs to be equal to 1.
	mpz_t gc;
        mpz_init(gc);
	while (1) { // while the gcd of public exponent and the totient(n) is not 1
		mpz_urandomb(e, state, nbits); // get a random number for e
		gcd(gc, e, t); // find the gcd
		// e needs to be in the range (2, n)
		if  (mpz_cmp_ui(e, 2) > 0 && (mpz_cmp(e, n) < 0) && mpz_cmp_ui(gc, 1) == 0) {
			break;
		}
	}
	mpz_clears(p_bits,q_bits,n_bits,p_1,q_1,g,t,gc,NULL);
}

//
// Writes a public RSA key to a file.
// Public key contents: n, e, signature, username.
// All mpz_t arguments are expected to be initialized.
//
// n: the public modulus.
// e: the public exponent.
// s: the signature of the username.
// username: the username that was signed as s.
//
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	// ensure we are at the start of the file
	fseek(pbfile,0,SEEK_SET);
	gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n,e,s,username);
}

//
// Reads a public RSA key from a file.
// Public key contents: n, e, signature, username.
// All mpz_t arguments are expected to be initialized.
//
// n: will store the public modulus.
// e: will store the public exponent.
// s: will store the signature.
// username: an allocated array to hold the username.
// pbfile: the file containing the public key
//
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	fseek(pbfile,0,SEEK_SET);
	gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s", n,e,s,username);
}


//
// Generates the components for a new private RSA key.
// Requires an accompanying RSA public key to complete the pair.
// All mpz_t arguments are expected to be initialized.
//
// d: will store the RSA private key.
// e: the precomputed public exponent.
// p: the first large prime from the public key generation.
// p: the second large prime from the public key generation.
//
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	mpz_t p_1;
        mpz_init(p_1);
        mpz_sub_ui(p_1, p, 1); // p-1
        mpz_t q_1;
        mpz_init(q_1);
        mpz_sub_ui(q_1, q, 1); // q-1
	mpz_t g;
	mpz_init(g);
        gcd(g, p_1, q_1); // gcd(q-1,p-1)
        mpz_t t;
        mpz_init(t);
        mpz_sub_ui(q_1, q, 1);
        mpz_sub_ui(p_1, p, 1);
        mpz_mul(t, p_1, q_1); // t = (q-1)(p-1)
        // computes lcm
        // mpz_div(t, t, g);
	mod_inverse(d,e,t); // d is equal to the mod inverse of t and the public exponent 
	mpz_clears(p_1,q_1,g,t,NULL);
}

//
// Writes a private RSA key to a file.
// Private key contents: n, d.
// All mpz_t arguments are expected to be initialized.
//
// n: the public modulus.
// d: the private key.
// pvfile: the file to write the private key to.
//
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	// ensure we are at the beginning of the file 
	fseek(pvfile,0,SEEK_SET); 
	gmp_fprintf(pvfile, "%Zx\n%Zx\n", n,d);
}

//
// Reads a private RSA key from a file.
// Private key contents: n, d.
// All mpz_t arguments are expected to be initialized.
//
// n: will store the public modulus.
// d: will store the private key.
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	// ensure we are at the beginning of the file 
	fseek(pvfile,0,SEEK_SET);
	gmp_fscanf(pvfile, "%Zx\n%Zx\n", n,d);
}

//
// Encrypts a message given an RSA public exponent and modulus.
// All mpz_t arguments are expected to be initialized.
//
// c: will store the encrypted message.
// m: the message to encrypt.
// e: the public exponent.
// n: the public modulus.
//
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	// c = m^e (mod n)
	pow_mod(c,m, e,n);
}
//
// Encrypts an entire file given an RSA public modulus and exponent.
// All mpz_t arguments are expected to be initialized.
// All FILE * arguments are expected to be properly opened.
//
// infile: the input file to encrypt.
// outfile: the output file to write the encrypted input to.
// n: the public modulus.
// e: the public exponent.
//
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	//reset files
	fseek(infile,0,SEEK_SET);
	fseek(outfile,0,SEEK_SET);
	// calculating the size of a block
	uint64_t k = (mpz_sizeinbase(n,2)-1)/8;
	// dynamically allocate an array with the size of the block
	uint8_t * arr = (uint8_t *) malloc(k);
	arr[0] = 0xFF;
	//last step
	mpz_t m;
	mpz_init(m);
	mpz_t c;
	mpz_init(c);
	while (!feof(infile)) { // while there are more bytes in infile
		// save to arr, k-1 bytes from infile, each element with size 1
		uint64_t j  =  fread(arr+1, sizeof(arr[0]), k-1, infile);  //part 1
		// convert the message from bytes to mpz 
		mpz_import(m, j+1, 1, 1, 1, 0, arr); 
		// encrypt the block
		rsa_encrypt(c, m, e, n);
		// write the encrypted message to outfile
		gmp_fprintf(outfile, "%Zx\n", c);
	}
	// clear all variables
	free(arr);
	mpz_clear(m);
	mpz_clear(c);
}

//
// Decrypts some ciphertext given an RSA private key and public modulus.
// All mpz_t arguments are expected to be initialized.
//
// m: will store the decrypted message.
// c: the ciphertext to decrypt.
// d: the private key.
// n: the public modulus.
//
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	// m = c^d (mod n)
	pow_mod(m,c,d,n);
}
//
// Decrypts an entire file given an RSA public modulus and private key.
// All mpz_t arguments are expected to be initialized.
// All FILE * arguments are expected to be properly opened.
//
// infile: the input file to decrypt.
// outfile: the output file to write the decrypted input to.
// n: the public modulus.
// d: the private key.
//
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	//reset files
        fseek(infile,0,SEEK_SET);
        fseek(outfile,0,SEEK_SET);
	// calculating the size of a block
        uint64_t k = mpz_sizeinbase(n,2);
        k = (k-1)/8;
  	// dynamically allocate an array
        uint8_t * arr = (uint8_t *) malloc(k);
        //last step
        mpz_t m;
        mpz_init(m);
        mpz_t c;
        mpz_init(c);
        while (gmp_fscanf(infile, "%Zx\n", c) != EOF) { // while there are more bytes in infile
		// decrypt the message
		rsa_decrypt(m, c, d, n);
		// store the decrypted message in the array
		size_t j = 0;
		mpz_export(arr, &j, 1, 1, 1, 0, m);
		// write j-1 of the bytes to outfile
		fwrite(arr+1, 1, j-1, outfile);
        }
	
        free(arr);
	mpz_clear(m);
	mpz_clear(c);
}


//
// Signs some message given an RSA private key and public modulus.
// All mpz_t arguments are expected to be initialized.
//
// s: will store the signed message (the signature).
// m: the message to sign.
// d: the private key.
// n: the public modulus.
//
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	// s = m^d (mod n)
	pow_mod(s, m, d,n);
}

//
// Verifies some signature given an RSA public exponent and modulus.
// Requires the expected message for verification.
// All mpz_t arguments are expected to be initialized.
//
// m: the expected message.
// s: the signature to verify.
// e: the public exponent.
// n: the public modulus.
// returns: true if signature is verified, false otherwise.
//
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	mpz_t t;
	mpz_init(t);
	// t = s^e (mod n)
	pow_mod(t, s, e, n);
	if (mpz_cmp(t, m) == 0) {
		mpz_clear(t);
		return true;
	}
	mpz_clear(t);
	return false;
}
