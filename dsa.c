#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>
gmp_randstate_t state;

mpz_t prime, q, alpha, beta, generator;  // key (prime, q, alpha, beta)
const unsigned long int bitsOfPrime = 1023, bitsOfq = 159;

void findGeneratorOfPrime() {
	mpz_t res;
	mpz_inits(generator, res, NULL);
	while(true) {
		mpz_urandomm(generator, state, prime);  ///random < prime 
		mpz_powm(res, generator, prime, prime);
		if (mpz_cmp(res, generator) == 0) {
			break;
		}
	}
	mpz_clear(res);
}

void key_gen() {
	mpz_t remainder, temp, quotient;
	unsigned long int one = 1, zero = 0;
    mpz_init_set_ui(prime, 1);
    mpz_init_set_ui(q, 1);
    mpz_inits(temp, remainder, quotient, alpha, NULL);

    mpz_mul_2exp(prime, prime, bitsOfPrime); // prime = 1 * 2^{1023}
    mpz_nextprime(prime, prime);    
    findGeneratorOfPrime();

    mpz_mul_2exp(q, q, bitsOfq); // prime = 1 * 2^{159}
    mpz_sub_ui(temp, prime, one); // temp = prime - 1
    while(true) {
    	mpz_nextprime(q, q);
    	mpz_cdiv_r(remainder, temp, q);
    	if (mpz_cmp_ui(remainder, zero) == 0) {
    		mpz_cdiv_q(quotient, temp, q);
    		break;
    	}
    }
    // find generator of subgroup using "Lagrange’s theorem"
    mpz_powm(alpha, generator, quotient, prime);
    mpz_powm(temp, alpha, q, q);
    if (mpz_cmp(temp, alpha) == 0) {
		printf("subgroup generator is right!\n");
	}

    mpz_clears(remainder, temp, quotient, NULL);
}




int main() {
	// init random state
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
	
	key_gen();
	
	// clean up
	mpz_clears(prime, q, alpha, beta, NULL);
}