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

#include "cryptStream.h"


cryptStream::cryptStream(void)
{
}

cryptStream::cryptStream(string inputfile, string outputfile){
	ifilename = inputfile;
	ofilename = outputfile;
}

void cryptStream::open(string inputfile, string outputfile){
	ifilename = inputfile;
	ofilename = outputfile;
}

cryptStream::~cryptStream(void)
{
}

void cryptStream::encrypt(Bunt mod, Bunt pub){
	n = mod;
	e = pub;
	int bufferSize = 512;

	unsigned long bSize = calcBytes(mod).toUnsignedLong();
	char* block = new char[bSize*bufferSize];
	ifstream ifile;
	ofstream ofile;
	ifile.open(ifilename, ios_base::in | ios_base::binary);
	ofile.open(ofilename, ios_base::binary | ios_base::out);
	if(!ifile.is_open() || !ofile.is_open())
		throw string("File failed to open");

	char* temp = new char[bufferSize];
	ifile.read(temp, bufferSize);
	while(!ifile.eof()){
		charToCrypt(temp, bufferSize, block, bSize);
		ofile.write(block, bSize*bufferSize);
		ifile.read(temp, bufferSize);
	}
	int bytes = ifile.gcount();
	charToCrypt(temp, bytes, block, bSize);
	ofile.write(block, bSize*bytes);

	ifile.close();
	ofile.close();
}

void cryptStream::decrypt(Bunt mod, Bunt pri){
	n = mod;
	d = pri;

	unsigned long bSize = calcBytes(mod).toUnsignedLong();
	char* block = new char[bSize];
	ifstream ifile;
	ofstream ofile;
	ifile.open(ifilename, ios_base::binary | ios_base::in);
	ofile.open(ofilename, ios_base::binary | ios_base::out);
	if(!ifile.is_open() || !ofile.is_open())
		throw string("File failed to open");

	char temp;
	ifile.read(block, bSize);
	while(!ifile.eof()){
		temp = cryptToChar(block, bSize);
		ofile.write(&temp, 1);
		ifile.read(block,bSize);
	}

	ifile.close();
	ofile.close();
}

Bunt cryptStream::calcBytes(Bunt x){
	if (x == 0)
		return 1;
	Bunt bits = 1;
	Bunt comp = 1;
	while(x >= comp){
		bits++;
		comp *= 2;
	}
	bits--;
	return (bits-1)/8 + 1;
}

//m = c^d(mod n)
char cryptStream::cryptToChar(char* block, unsigned long size){
	Bunt c = 0;
	for(unsigned long i = 0; i < size; i++){
		c *= 256;
		c += (unsigned int)(unsigned char)*(block+i);
	}
	char x = (char)(modexp(c, d, n).toInt());
	return x;
}

//c = m^e(mod n)
void cryptStream::charToCrypt(char* m, int numChars, char* block, unsigned long size){
	for(int j = 0; j < numChars; j++){
		Bint temp((int)*(m+j));
		Bunt x = modexp(temp, e, n);
		for(unsigned long i = 0; i < size; i ++){
			*(block+size*j+i) = 0;
		}

		Bunt rem;
		for(unsigned long i = 1; x > 0; i++){
			rem = x%256;
			*(block+(j+1)*size-i) = (char)rem.toUnsignedInt();
			if(rem > x)
				x = 0;
			else
				x = (x-rem)/256;
		}
	}
}