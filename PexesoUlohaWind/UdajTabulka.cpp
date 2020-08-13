#include "UdajTabulka.h"



bool UdajTabulka::operator>(const UdajTabulka & pravy) const
{
	return this->casHry > pravy.casHry;
}

UdajTabulka::UdajTabulka(char* pmeno, int pcasHry, char* pcas)
{
	this->meno = pmeno;
	this->casHry = pcasHry;
	this->cas = pcas;
}


UdajTabulka::~UdajTabulka()
{
	delete[] meno;
	delete[] cas;
}
