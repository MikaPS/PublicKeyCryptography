// implements number theory functions
#include "numtheory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "randstate.h"

// Computes the gretest common divisor of two argumetns a and b
// Saves the final value in the argument d
void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t t; // initalizes a temp variable t
	mpz_init(t);
	// since the arguments are passed by reference, to not change the original values of the arguments, we copy them to local vars
	mpz_t aa; // copies a to aa
        mpz_init_set(aa,a);
 	mpz_t bb; // copies b to bb
        mpz_init_set(bb,b);

	// the loop is used to find the greater number that can divide both numbers with zero reminder. Since bb represents the reminder, we run the code until it is equal to 0. We use the mod function to calculates the reminder of both vars.
	while (mpz_sgn(bb)) {
		mpz_set(t,bb);
		mpz_mod(bb, aa, bb);
		mpz_set(aa, t);
	}
	mpz_set(d, aa);
	mpz_clear(t);
	mpz_clear(aa);
	mpz_clear(bb);
}

// Computes the inverse of arg a mod n.
// Saves the value in the argument o
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
	// Since the variables are passed by reference, we create copies of each argument to not change their original values
	mpz_t r;
        mpz_init(r);
	mpz_set(r, n); // r = n
	mpz_t(rp);
	mpz_init(rp);
	mpz_set(rp, a); // rp = a
	mpz_t t;
        mpz_init(t); // t = 0
	mpz_t tp; 
        mpz_init(tp);
	mpz_set_ui(tp,1); // t' = 1
	mpz_t q;
        mpz_init(q);
	mpz_t temp_r;
        mpz_init(temp_r);
	mpz_t temp_rp;
        mpz_init(temp_rp);
 	mpz_t temp_t;
        mpz_init(temp_t);
        mpz_t temp_tp;
        mpz_init(temp_tp);
	mpz_t temp_1;
        mpz_init(temp_1);

	// Same concept as GCD, loop while the mod of two vars is not 0.
	// Eventually, the numbers would reach the form: r*r' + t*t' = gcd(r,t)
	while (mpz_sgn(rp)!=0) { 
		// q = r/r'
		mpz_fdiv_q(q,r,rp); 
		
		// define temp r and temp r'
		mpz_set(temp_r, r); //temp r = r 
		mpz_set(temp_rp, rp); // temp r' = r'
		
		// (r, r') <- (r', r-q*r')
		mpz_set(r, temp_rp); // r = r'
		// r' = temp_r-q*r'
		mpz_mul(temp_1, q, temp_rp);
		mpz_sub(rp, temp_r, temp_1);

		// define temp t and t'
                mpz_set(temp_t, t); // temp t = t
                mpz_set(temp_tp, tp); // temp t' = t'
		// (t, t') <- (t', t-q*t)
		mpz_set(t, temp_tp); // t = t'
		// t' = (temp_t-q)*t'
                mpz_mul(temp_1, q, temp_tp); //  t' time q
                mpz_sub(tp, temp_t, temp_1); // t' = t - temp2
	}
	// if there's no inverse, set o to 0
	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_set_ui(o, 0);
		return;
	}
	if (mpz_sgn(t)==-1) {
		mpz_add(t, t, n);
	}
	mpz_set(o, t);
	mpz_clears(r, temp_r,rp,temp_rp,t,temp_t,tp,temp_tp,q,temp_1,NULL);
}

// does modular exponentiation
// at the end, o = a ^ d (mod n)
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
	// v=1
	mpz_t v;
        mpz_init(v);
        mpz_set_ui(v, 1);
	// p = a
	mpz_t p;
        mpz_init(p);
        mpz_set(p, a);
	// copy of d
	mpz_t dd;
        mpz_init(dd);
        mpz_set(dd, d);
	// copy of n
	mpz_t nn;
        mpz_init(nn);
        mpz_set(nn, n);
	// while d is greater than 0
	while (mpz_sgn(dd) > 0) {
		if (mpz_odd_p(dd) != 0) {
			mpz_mul(v, v, p);
			mpz_mod(v, v, nn);
		}
		mpz_mul(p, p, p);
                mpz_mod(p, p, nn);
		mpz_div_ui(dd,dd,2);
	}
	mpz_set(o, v);
	mpz_clears(v,dd,nn,p,NULL);
}

// use the Miller-Rabin primality testing to check if a number is prime
bool is_prime(mpz_t n, uint64_t iters) {
	extern gmp_randstate_t state;
	// r = n-1
	mpz_t r;
        mpz_init(r);
        mpz_sub_ui(r, n, 1);
	// s = 0 
	mpz_t s;
        mpz_init(s);
	// while r is even, divides it by 2 and add 1 to s
	// end result: n-1 = 2^s*r
	while (mpz_even_p(r) != 0) {
		mpz_div_ui(r, r, 2);
		mpz_add_ui(s,s,1);
	}
	mpz_t a;
        mpz_init(a);
        mpz_t temp;
        mpz_init(temp);
	for (uint64_t i=1; i<iters; i+=1) {
		// special case when n=4 because the ranfe is from 2 to 2
		if (mpz_cmp_ui(n, 4) > 0) {  
			mpz_sub_ui(temp, n, 4);
			mpz_urandomm(a, state, temp);
			mpz_add_ui(a, a, 2);
		}
		else if (mpz_cmp_ui(n,4) == 0) {
		//	mpz_set_ui(a, rand()%2);
		//	mpz_add_ui(a,a,2);
			mpz_set_ui(a,2);
		}
		else {
			return true;
		}

		mpz_t y;
                mpz_init(y);
		pow_mod(y, a, r, n);
		mpz_t n_minus_1;
		mpz_init(n_minus_1);
		mpz_sub_ui(n_minus_1, n, 1);	
		// checks that the power mod of a^r mod n is not equal to 1 or n-1
		if (mpz_cmp_ui(y,1) != 0 & mpz_cmp(y, n_minus_1) != 0) {
			mpz_t j;
	                mpz_init(j);
			mpz_set_ui(j, 1); // j =1
			mpz_t s_minus_1;
	                mpz_init(s_minus_1); 
        	        mpz_sub_ui(s_minus_1, s, 1); // s-1
			while (mpz_cmp(j, s_minus_1) <= 0 & mpz_cmp(y, n_minus_1) != 0) {
				mpz_set_ui(temp,2);
				pow_mod(y,y,temp,n); // power mod of y^2 mod n
				if (mpz_cmp_ui(y,1) == 0) {
					mpz_clears(s_minus_1,j,y,n_minus_1,r,s,a,temp,NULL);
					return false;
				}
				mpz_add_ui(j,j,1);
			}
			// ensures y is not equak to n-1
			if (mpz_cmp(y,n_minus_1) != 0) {
				mpz_clears(s_minus_1,j,y,n_minus_1,r,s,a,temp,NULL);
				return false;
                        }
			mpz_clear(s_minus_1);
			mpz_clear(j);
			
		}
		mpz_clear(y);
		mpz_clear(n_minus_1);
	}
	mpz_clear(r);
	mpz_clear(s);
	mpz_clear(a);
	mpz_clear(temp);
	return true;
}

// generates random numbers and tests if they are prime
// saves prime numbers with at least /bits/ bits long to p
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
		mpz_t r;
                mpz_init(r);
		// while the number is not prime, generate a new number
		while (1)  {
			// range is from 2^(bits-1) to 2^bits-1
			mpz_rrandomb(p,state,bits);
			// prime testing: can't be even or divided by any other prime number
			if (mpz_even_p(p) != 0) {
				continue;
		   	}
			mpz_mod_ui(r, p, 5);
			if (mpz_cmp_ui(r,0) == 0) {
				continue;
			}
			mpz_mod_ui(r,p,3); 
			if (mpz_cmp_ui(r,0) == 0) {
                                continue;
                        }
			mpz_mod_ui(r,p,7);
                        if (mpz_cmp_ui(r,0) == 0) {
                                continue;
                        }
			mpz_mod_ui(r,p,11);
                        if (mpz_cmp_ui(r,0) == 0) {
                                continue;
                        }
			// if number is prime, stop the loop
			if (is_prime(p,iters) == true) {
				break;
			}

		}      
	mpz_clear(r);
}


