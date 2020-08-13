#pragma once
class UdajTabulka
{
public:
	char* meno;
	int casHry;
	char* cas;
	bool operator>(const UdajTabulka &pravy) const;
	UdajTabulka(char* pmeno,int pcasHry,char* pcas);
	~UdajTabulka();
};

