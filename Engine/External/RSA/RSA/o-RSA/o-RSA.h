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

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include "bigInt\BigIntegerLibrary.hh"
#include "Prime.h"
#include "..\..\..\..\Memory.h"
using namespace std;

#pragma once
#define Bint BigInteger
#define Bunt BigUnsigned

class o_RSA
{
public:
	//Generates a public and private key pair and outputs them to <file>.pub and <file>.pri respectively
	static void generateKeys(string file, Bunt keysize);
	//Generates a public and private key pair and returns them
	static void generateKeys(Bint& pri, Bunt& pub, Bunt& mod, Bunt keysize);
private:
	//Generates private and public key pairs for the interfaces
	static void generate(Bint& d, Bunt& e, Bunt& n, Bunt keysize, time_t& elapsed);
	//Returns b to the power of e
	static Bunt exp(Bunt b, Bunt e);
};

