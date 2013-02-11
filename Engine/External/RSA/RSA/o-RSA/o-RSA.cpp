#include "o-RSA.h"
//This file is part of o-RSA.
//
//o-RSA is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//o-RSA is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License
//along with o-RSA.  If not, see <http://www.gnu.org/licenses/>.

void o_RSA::generateKeys(string filename, Bunt keysize){
	Bunt e, n;
	Bint d;
	time_t elapsed;

	generate(d, e, n, keysize, elapsed);
	ofstream file;
	file.open(filename + ".pub");
	file << n << endl;
	file << e << endl;
	file.close();
	file.open(filename + ".pri");
	file << n << endl;
	file << d << endl;
	file.close();
}

void o_RSA::generateKeys(Bint& d, Bunt& e, Bunt& n, Bunt keysize){
	time_t elapsed;
	generate(d, e, n, keysize, elapsed);
}

void o_RSA::generate(Bint& d, Bunt& e, Bunt& n, Bunt bits, time_t& elapsed){
	Bunt p, q, ETF;
	elapsed = time(NULL);

	//Generate the primes used to make the keys
	Prime::seed();
	p = Prime::generate(bits);
	q = Prime::generate(bits);

	//The modulus used with the public and private keys
	n = p*q;

	//Compute Euler's Totient function to help generate the keys
	ETF = (p-1)*(q-1);
	
	//Find a public key that has a low hamming weight (fewer 1's in binary)
	srand((unsigned int)time(NULL));
	e = 0;
	while(e < exp(2, bits/8)){
		e = 0;
		for(Bunt i = 1; i < ETF/2; i*=2){
			if(rand()%100 == 1){
				e += i;
			}
		}
	}
	//The public key must be coprime to Euler's Totient function
	while(gcd(ETF, e) != 1){
		e++;
	}

	//Generate the private key
	//The private key must be the multiplicative inverse of e mod ETF
	Bint x = e;
	Bint y = ETF;

	Bint ax = 1;
	Bint ay = 0;
	Bint bx = 0;
	Bint by = 1;

	while(x != 0){
		if(x <= y){
			Bint m = y/x;
			y -= m*x;
			ay -= ax*m;
			by -= bx*m;
		}else{
			swap(x, y);
			swap(ax, ay);
			swap(bx, by);
		}
	}
	d = ay%ETF;
	elapsed = time(NULL) - elapsed;
}

Bunt o_RSA::exp(Bunt b, Bunt e){
	Bunt result = b;
	for(Bunt i = 1; i < e;){
		if(i*2 < e){
			result *= result;
			i *= 2;
		}else{
			result *= b;
			i++;
		}
	}
	return result;
}