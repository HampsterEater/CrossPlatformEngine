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

#include "Prime.h"

Bunt Prime::generate(Bunt bits){
	Bunt keyMax, keyMin;
	keyMax = exp(2, bits+3);
	keyMin = exp(2, bits);
	
	Bunt p = (Bunt)rand()%keyMax;

	while(p < keyMin){
		p = p*(Bunt)rand()%keyMax + rand();
	}
	if(p%2 == 0)
		p++;
	for(Bunt mod = 0; !isPrime(p, bits); ){
		mod += 2;
		if(mod % 4 == 0)
			p += mod;
		else
			p -= mod;
	}

	return p;
}

void Prime::seed(){
	srand((unsigned int)time(NULL));
}

bool Prime::FermatPrimeTest(Bunt n, Bunt tester){
	if(gcd(n, tester) != 1)
		return false;
	Bunt value = modexp(tester, n-1, n);
	if(value == 1)
		return true;
	return false;
}

bool Prime::isPrime(Bunt n, Bunt bits){
	Bunt keyMax, keyMin, tester;
	keyMax = exp(2, bits);
	keyMin = exp(2, bits-3);
	bool prime = true;

	for(int i = 0; i < 1 && prime; i++){
		tester = (Bunt)rand()%keyMax;
		while(tester < keyMin){
			tester = tester*(Bunt)rand()%keyMax + rand();
			tester %= n-1;
		}
		prime = FermatPrimeTest(n, tester);
	}
	return prime;
}

Bunt Prime::exp(Bunt b, Bunt e){
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