#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>
gmp_randstate_t state;

mpz_t prime, q, alpha, beta, privateKey, hashValue, ephemeralKey, r, s;  // key (prime, q, alpha, beta)

const unsigned long int bitsOfPrime = 1023, bitsOfq = 159;

char* sha = "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";

void findBeta() {
    mpz_inits(privateKey, beta, NULL);
    mpz_urandomm(privateKey, state, q); 
    mpz_powm(beta, alpha, privateKey, prime); 
}

void findAlpha() {
    mpz_t res;
    mpz_inits(alpha, res, NULL);

    while(true) {
        mpz_urandomm(alpha, state, q);  
        mpz_powm(res, alpha, q, q);  
        if (mpz_cmp(res, alpha) == 0) { // test generator
            break;
        }
    }
    mpz_clear(res);
}

void findPrime() {
    mpz_t multiplier, remainder;
    mpz_inits(multiplier, remainder, NULL);
    
    mpz_set_ui(multiplier, 1); 
    mpz_mul_2exp(multiplier, multiplier, bitsOfPrime); // multi: 1 * 2 ^ {1023}
    mpz_cdiv_r(remainder, multiplier, q);
    mpz_sub(multiplier, multiplier, remainder);

    // mpz_mul_2exp(prime, prime, bitsOfPrime); // prime = 1 * 2 ^ {1023}
    // mpz_nextprime(prime, prime); 

    while(true) {
        mpz_add_ui(prime, multiplier, 1);
        if (mpz_probab_prime_p(prime, 10) != 0) {
            break;
        } else {
            mpz_add(multiplier, multiplier, q);
        }
    }
    mpz_clears(multiplier, remainder, NULL);

}

void ephemeralKeyGen() {
    mpz_inits(ephemeralKey, NULL);
    mpz_urandomm(ephemeralKey, state, q); 
}

void key_gen() {
    
    mpz_init_set_ui(prime, 1);
    mpz_init_set_ui(q, 1);

    // find q
    mpz_mul_2exp(q, q, bitsOfq);   // q = 1 * 2^{159}
    mpz_nextprime(q, q);    // get q 
    
    // find prime
    findPrime();

    // find generator of subgroup
    findAlpha();
    findBeta();

    // print
    gmp_printf("prime: \n%Zd\n", prime);
    gmp_printf("q: \n%Zd\n", q);
    gmp_printf("alpha: \n%Zd\n", alpha);
    gmp_printf("beta: \n%Zd\n", beta);
    gmp_printf("private key: \n%Zd\n", privateKey);

}
// gen (r, s)
void sign() {
    mpz_t invertKey, temp;
    mpz_inits(r, s, invertKey, temp, NULL);
    mpz_init_set_str(hashValue, sha, 16);
    ephemeralKeyGen();

    //gen r
    mpz_powm(r, alpha, ephemeralKey, prime);
    mpz_mod(r, r, q);  

    // gen invert key
    mpz_invert(invertKey, ephemeralKey, q);

    // gen s
    mpz_mul(temp, privateKey, r);
    mpz_add(temp, temp, hashValue);
    mpz_mul(temp, invertKey, temp);
    mpz_mod(s, temp, q);


    gmp_printf("r: \n%Zd\n", r);
    gmp_printf("s: \n%Zd\n", s);

    mpz_clears(invertKey, temp, NULL);
}

void verify() {
    
}
int main() {
	// init random state
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
	
	key_gen();
	sign();
    verify();
	// clean up
	mpz_clears(prime, q, alpha, beta, privateKey, hashValue, ephemeralKey, r, s, NULL);
}