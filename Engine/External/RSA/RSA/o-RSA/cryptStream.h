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

#pragma once
#include <string>
#include <fstream>
#include "bigInt\BigIntegerLibrary.hh"

#include "..\..\..\..\Memory.h"
using namespace std;


#define Bunt BigUnsigned
#define Bint BigInteger

class cryptStream
{
public:
	cryptStream();
	cryptStream(string inputfile, string outputfile);
	~cryptStream(void);

	void open(string inputfile, string outputfile);
	void encrypt(Bunt mod, Bunt pub);
	void decrypt(Bunt mod, Bunt pri);
private:
	void charToCrypt(char* m, int numChars, char* block, unsigned long size);
	char cryptToChar(char* block, unsigned long size);
	Bunt calcBytes(Bunt);

	Bunt n, e, d;
	string ifilename, ofilename;
};

