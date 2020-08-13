#pragma once
#include <windows.h>

class PexesoKarta
{
public:
	bool zobrazena;
	PexesoKarta( HBITMAP obrazok);
	PexesoKarta();
	
	void setZobrazena(bool z);
	void setObrazok(HBITMAP obrazok);
	HBITMAP getObrazok();
	HWND getHWND();
	void setHWND(HWND hwnd);
	~PexesoKarta();
	
private:
	HBITMAP obrazokKarta;
	HWND handle;
	
	

};

