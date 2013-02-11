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

#include "bigInt\BigIntegerLibrary.hh"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "..\..\..\..\Memory.h"
using namespace std;

#pragma once
#define Bunt BigUnsigned

class Prime
{
public:
	//Generates a random prime of size between bits and bits+3
	static Bunt generate(Bunt bits);
	//Seeds the random function, should be called before generate
	static void seed();
private:
	//Uses the Fermat Primality test to check primality of n (bits is the order of magnitude of n)
	static bool isPrime(Bunt n, Bunt bits);
	//Standard implementation of Fermat Primality test
	static bool FermatPrimeTest(Bunt n, Bunt tester);
	//Returns b to the power of e
	static Bunt exp(Bunt b, Bunt e);
};

