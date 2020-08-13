#include "PexesoKarta.h"



PexesoKarta::PexesoKarta( HBITMAP obrazok)
{
	
	this->obrazokKarta = obrazok;
	this->zobrazena = false;
}

PexesoKarta::PexesoKarta()
{
}


void PexesoKarta::setZobrazena(bool z)
{
	this->zobrazena = z;
}

void PexesoKarta::setObrazok(HBITMAP obrazok)
{
	this->obrazokKarta = obrazok;
}

HBITMAP PexesoKarta::getObrazok()
{
	return this->obrazokKarta;
}

HWND PexesoKarta::getHWND()
{
	return handle;
}

void PexesoKarta::setHWND(HWND hwnd)
{
	handle = hwnd;
}


PexesoKarta::~PexesoKarta()
{
}
