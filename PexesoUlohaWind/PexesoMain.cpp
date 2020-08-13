#define _CRT_SECURE_NO_WARNINGS // kvoli warningom v time

#include <windows.h>


#include <vector>
#include <time.h>  
#include <stdio.h>

#include "PexesoKarta.h"
#include "UdajTabulka.h"

#define SIRKA_OBRAZKA 150
#define VYSKA_OBRAZKA 150
#define VYSKA_CONTROLS 20


void zakriObrazky();
void zamiesajKarty();
void inicializujKarty(HWND);
void otocKartu(PexesoKarta*);
void nastavInterface(HWND);
void skriKartu(PexesoKarta*);
bool skontrolujZhodu(PexesoKarta*, PexesoKarta*);
PexesoKarta* dajKartuNaXY(int, int);

void zapisSkore(char[]);
void nacitajTabulku();
void zoradTabulku();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


bool bolaZmenaVtabulke;
int indexPrvej;
int indexDruhej;
int pocetZobrazenych;
int pocetUhadnutychParov;
char szWinName[] = "Pexeso Zaùko";
char menoHraca[20] = "Hr·Ë";



std::vector<PexesoKarta*>* pexeso;
std::vector<UdajTabulka*>* tabulka;

HBITMAP vrch; //obrazok vrchu karty pokemon lopta
HWND MenoBox;


const int WM_KRESLIPEXESO = WM_USER;
const int WM_VYPOCETSURADNIC = WM_USER + 1;
const int WM_ZMENTITULOK = WM_USER + 2;
const int WM_NOVAHRA = WM_USER + 3;
const int WM_PRIPOCITAJ = WM_USER + 3;
const int WM_KONTROLA_ZHODY = WM_USER + 4;
const int WM_ZOBRAZ_TABULKU = WM_USER + 5;
int APIENTRY WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, PSTR lpszArgs, int nWinMode)
{
	HWND            hwnd;
	MSG             msg;
	WNDCLASSEX      wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;// Implicitny styl
	wndclass.lpfnWndProc = WndProc;      // Oknova funkcia
	wndclass.hInstance = hThisInst;    // Popisovac tejto instancie
	wndclass.lpszClassName = szWinName;    // Nazov oknovej triedy
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Druh ikony
	wndclass.hIconSm = 0;            // Druh malej ikony
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);     // Druh kurzora
	wndclass.lpszMenuName = NULL;                            // Ziadne menu
	wndclass.cbClsExtra = 0;            // nie su potrebne
	wndclass.cbWndExtra = 0;            // ziadne extra informacie
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);    // Biele pozadie

	RegisterClassEx(&wndclass);            // Registracia okna
										   // Po registracii vytvorenie okna
	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		szWinName,           // Nazov oknovej triedy
		"PEXESO MAREK ZAçKO",  // Titulok okna
		WS_OVERLAPPED | WS_SYSMENU, // Styl okna // uzivatel nemoze menit velkost okna
		CW_USEDEFAULT,       // Pociatocna x-poloha
		CW_USEDEFAULT,       // Pociatocna y-poloha
		600 + 10,       // Pociatocna sirka
		600 + VYSKA_CONTROLS + 35,       // Pociatocna vyska
		HWND_DESKTOP,        // NULL, popisovac rodicovskeho okna (ziadne)
		NULL,                // popisovac menu okna (ziadne)
		hThisInst,           // popisovac instancie programu
		NULL);                  // parametre tvorby

	ShowWindow(hwnd, nWinMode);        // Zobrazenie okna
	UpdateWindow(hwnd);


	// Slucka sprav
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// Tuto funkciu vola Windows - prenasa do nej spravy z fronty sprav
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT        rect;
	static int casovacOtocenie;

	static int pocitadloSekund;
	static bool hraStart;


	switch (message) {

	case WM_CREATE:
	{
		tabulka = new std::vector<UdajTabulka*>();

		bolaZmenaVtabulke = true;
		casovacOtocenie = 0;
		indexPrvej = indexDruhej = -1;
		hraStart = false;

		pocitadloSekund = 0;
		pocetUhadnutychParov = 0;
		pocetZobrazenych = 0;

		nastavInterface(hwnd);
		inicializujKarty(hwnd);

		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 1:  // Tlacidlo Uloz Meno

			GetWindowText(MenoBox, &menoHraca[0], 20);
			SendMessage(hwnd, WM_ZMENTITULOK, 0, 0);

			break;
		case 2: // Tlacidlo zobraz tabulku
			if (bolaZmenaVtabulke) {
				nacitajTabulku();
				zoradTabulku();
			}
			SendMessage(hwnd, WM_ZOBRAZ_TABULKU, 0, 0);
			break;
		case 3: // Tlacidlo start

			SendMessage(hwnd, WM_NOVAHRA, 0, 0);
			break;
		default:
			break;
		}

		break;
	}

	// z kazdeho udaju v tabulke vytvori jeden velky retazec a ten sa vypise v MessageBox 
	// tu uz je tabulka usporiadana
	case WM_ZOBRAZ_TABULKU:
	{

		std::string tabulkaText;
		char pomocny[100];
		char finalnyText[5000];
		int poradie = 1;
		for each (auto udaj in *tabulka)
		{
			
			sprintf(pomocny, "%3d.     %10s    %3d secs       %s\n", poradie, udaj->meno, udaj->casHry, udaj->cas);
			tabulkaText.append(pomocny);
			poradie++;
		}
		strcpy(finalnyText, tabulkaText.c_str());
		MessageBox(hwnd, finalnyText, "TABULKA", MB_OK);


		bolaZmenaVtabulke = false;
		break;
	}

	case WM_LBUTTONDOWN:
	{
		if (casovacOtocenie == 0) { // dalsiu kartu mozes otocit ked nie su otocene 2, ked casovac pre zobrazenie 2 kariet skoncil

			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			GetClientRect(hwnd, &rect);

			POINT bodKliku = { xPos, yPos };
			bool vNutro = PtInRect(&rect, bodKliku);

			if (hraStart && vNutro  && yPos > VYSKA_CONTROLS) { 
				PexesoKarta* karta = dajKartuNaXY(xPos, yPos);
				if (karta->zobrazena == false) {

					if (indexDruhej == -1 && indexPrvej == -1) { // ziadna karta neotocena
						indexPrvej = std::find((*pexeso).begin(), (*pexeso).end(), karta) - (*pexeso).begin();
						otocKartu(karta);
						break;
					}

					if (indexPrvej != -1 && indexDruhej == -1) { // jedna karta otocena
						indexDruhej = std::find((*pexeso).begin(), (*pexeso).end(), karta) - (*pexeso).begin();
						otocKartu(karta);
						if (pocetZobrazenych == 2) {
							SetTimer(hwnd, 2, 1000, NULL); // casovac ked su zobrazene 2, zhoda sa kontroluje v case pre casovac
						}
						break;
					}
				}
			}
			else if (!hraStart && vNutro && yPos > VYSKA_CONTROLS) {
				MessageBox(hwnd, "Hra nebola odstartovana - klikni na START", "CHYBA", MB_OK);


			}
		}
		break;
	}

	case WM_ZMENTITULOK:
	{
		char titulokOkna[50];
		wsprintf(titulokOkna, "    HRA»: %s   »AS: %d", menoHraca, pocitadloSekund);
		SetWindowText(hwnd, titulokOkna);
		break;
	}

	case WM_NOVAHRA:

		KillTimer(hwnd, 2);
		KillTimer(hwnd, 1);


		indexPrvej = -1;
		indexDruhej = -1;
		casovacOtocenie = 0;
		pocetUhadnutychParov = 0;
		pocetZobrazenych = 0;
		pocitadloSekund = 0;

		zamiesajKarty();
		hraStart = true;
		zakriObrazky(); // otoc karty na vrch
		SetTimer(hwnd, 1, 1000, NULL); // HRA ZACALA


		break;


	case WM_TIMER:
		switch (wParam)
		{
		case 1: // Timer pre Hru
			pocitadloSekund++;
			SendMessage(hwnd, WM_ZMENTITULOK, 0, 0);
			break;

		case 2: // Timer ked su otocene 2 karty
			casovacOtocenie++;

			if (casovacOtocenie == 1) { 
				KillTimer(hwnd, 2);

				if (! skontrolujZhodu((*pexeso)[indexPrvej], (*pexeso)[indexDruhej])) {

					otocKartu((*pexeso)[indexPrvej]);
					otocKartu((*pexeso)[indexDruhej]);
				}
				else { // BOLA ZHODA
					skriKartu((*pexeso)[indexPrvej]);
					skriKartu((*pexeso)[indexDruhej]);
					pocetZobrazenych = 0;
					pocetUhadnutychParov++;

					if (pocetUhadnutychParov == 8) {
						KillTimer(hwnd, 1);

						time_t teraz;
						struct tm* timeinfo;
						time(&teraz);
						timeinfo = localtime(&teraz);
						char cas[50];
						strftime(cas, sizeof(cas), "%e.%B.%G   %R:%S", timeinfo);


						char buffer[150];
						wsprintf(buffer, "VYHRAL SI , TVOJE SKORE BOLO ZAPISANE DO TABULKY: \n MENO:    %s \n »AS hry:    %d\n\n Aktualny datum a cas:  %s ", menoHraca, pocitadloSekund, cas);
						char textNaZapisanie[50];
						wsprintf(textNaZapisanie, "\n%s\n%d\n%s", menoHraca, pocitadloSekund, cas);
						zapisSkore(textNaZapisanie);
						SetWindowText(hwnd, "PEXESO MAREK ZAçKO");
						MessageBox(hwnd, buffer, "VYHRA", MB_OK);
						hraStart = false;
						zakriObrazky();
					}

				}
				casovacOtocenie = 0;
				indexPrvej = -1;
				indexDruhej = -1;
			}
		default:
			break;
		}
	case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:

		for each (auto var in *tabulka)
		{
			delete var;
		}
		delete tabulka;

		for each (auto var in *pexeso)
		{
			delete var;
		}
		delete pexeso;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


// zoradi tabulku podla casu hry,pre prehladnost kodu vyuziva operator > definovany v triede UdajTabulka
void zoradTabulku()
{
	// BubbleSort
	for (int c = 0; c < (tabulka->size()); c++)
	{
		for (int d = 0; d < tabulka->size() - c - 1; d++)
		{
			if ((*(*tabulka)[d]) > (*(*tabulka)[d + 1]))
			{
				UdajTabulka* docasny = (*tabulka)[d];
				(*tabulka)[d] = (*tabulka)[d + 1];
				(*tabulka)[d + 1] = docasny;
			}
		}
	}
}

// zobrazi zakrite vsetky karty , zobrazeny je len vrch(pokemon lopta)
void zakriObrazky()
{
	for each (auto karta in *pexeso)
	{
		ShowWindow(karta->getHWND(), SW_SHOW);
		karta->zobrazena = false;
		SendMessage(karta->getHWND(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vrch);
	}
}

// miesanie kariet, staci len vymienat obrazky
void zamiesajKarty()
{
	srand(time(NULL));
	for (int i = 0; i < 100; i++)
	{
		int a = rand() % 15;
		int b = rand() % 15;
		HBITMAP obrazokNaSwap;
		obrazokNaSwap = (*pexeso)[a]->getObrazok();
		(*pexeso)[a]->setObrazok((*pexeso)[b]->getObrazok());
		(*pexeso)[b]->setObrazok(obrazokNaSwap);

	}
}
// inicializuje karty,nacita obrazky, tu sa karty nemiesaju  ,  karty su ulozene vo vectore pexeso
void inicializujKarty(HWND hwnd) {
	pexeso = new std::vector<PexesoKarta*>();

	HBITMAP pikachu = (HBITMAP)LoadImage(NULL, "obrazky/obrazok.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP bulba = (HBITMAP)LoadImage(NULL, "obrazky/obrazok2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP charm = (HBITMAP)LoadImage(NULL, "obrazky/obrazok3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP turt = (HBITMAP)LoadImage(NULL, "obrazky/obrazok4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP magik = (HBITMAP)LoadImage(NULL, "obrazky/obrazok5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP drak = (HBITMAP)LoadImage(NULL, "obrazky/obrazok6.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP gengar = (HBITMAP)LoadImage(NULL, "obrazky/obrazok7.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP ryba = (HBITMAP)LoadImage(NULL, "obrazky/obrazok8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	vrch = (HBITMAP)LoadImage(NULL, "obrazky/vrch.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	std::vector<HBITMAP> obrazky;
	obrazky.push_back(pikachu);
	obrazky.push_back(bulba);
	obrazky.push_back(charm);
	obrazky.push_back(turt);
	obrazky.push_back(magik);
	obrazky.push_back(drak);
	obrazky.push_back(gengar);
	obrazky.push_back(ryba);


	for (int i = 0; i < 8; i++) {
		PexesoKarta* karta = new PexesoKarta(obrazky[i]);
		PexesoKarta* karta2 = new PexesoKarta(obrazky[i]);

		pexeso->push_back(karta);
		pexeso->push_back(karta2);
	}
	int pocitadlo = 0;
	for (int i = 0; i < 4; i++) { // incializacia vrchu kariet
		for (int j = 0; j < 4; j++) {

			HWND handle = CreateWindow("STATIC", "", SS_BITMAP | WS_CHILD | WS_VISIBLE, (j*SIRKA_OBRAZKA), (i*VYSKA_OBRAZKA) + VYSKA_CONTROLS, SIRKA_OBRAZKA, VYSKA_OBRAZKA, hwnd, NULL, NULL, NULL);
			SendMessage(handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vrch);
			(*pexeso)[pocitadlo]->setHWND(handle);
			pocitadlo++;
		}
	}
}

// vytvori uzivatelsky interface
void nastavInterface(HWND hwnd) {
	MenoBox = CreateWindow("EDIT",
		"Hr·Ë",
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT,       // Pociatocna x-poloha
		CW_USEDEFAULT,       // Pociatocna y-poloha
		180, 20,
		hwnd, NULL, NULL, NULL);

	CreateWindow("BUTTON", "Uloû meno", WS_VISIBLE | WS_CHILD | WS_BORDER, 182, 1, 99, 20, hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("BUTTON", "Zobraz Tabuæku", WS_VISIBLE | WS_CHILD | WS_BORDER, 283, 1, 150, 20, hwnd, (HMENU)2, NULL, NULL);
	CreateWindow("BUTTON", "START", WS_VISIBLE | WS_CHILD | WS_BORDER, 480, 1, 50, 20, hwnd, (HMENU)3, NULL, NULL);
}

// skrije kartu, pouzivane pri uhadnuti p·ru
void skriKartu(PexesoKarta* karta) {

	karta->zobrazena = true;
	ShowWindow(karta->getHWND(), SW_HIDE);


}
//vrati kartu na pozicii x,y v klientskej ploche
PexesoKarta * dajKartuNaXY(int x, int y)
{
	PexesoKarta* karta = nullptr;
	int stlpec = x / VYSKA_OBRAZKA;
	int riadok = y / SIRKA_OBRAZKA;
	int index = ((riadok * 4) + stlpec);
	karta = (*pexeso)[index];
	return karta;
}
// Zapise hracove skore do suboru
void zapisSkore(char retazec[])
{
	FILE* subor = fopen("Tabulka.txt", "a");
	fputs(retazec, subor);
	fclose(subor);
	bolaZmenaVtabulke = true;

}

// Nacita tabulku do vectora tabulka
void nacitajTabulku()
{

	FILE* subor = fopen("Tabulka.txt", "r");
	if (subor == nullptr) { // subor neexistuje (prve spustenie)
		FILE* pomocny = fopen("Tabulka.txt", "w"); // len na vytvorenie suboru
		fclose(pomocny);
		subor = fopen("Tabulka.txt", "r");
	}


	for each (auto var in *tabulka)
	{
		delete var;
	}
	tabulka->clear();


	int casHraINT;
	int pocitadloRiadkov = 0;
	char prazdnyRiadok[10];
	if (!feof(subor)) {
		fgets(prazdnyRiadok, 10, subor);
	}
	while (!feof(subor)) {
		char* meno = new char[20];
		char casHra[20];
		char* cas = new char[100];
		fgets(meno, 30, subor);
		fgets(casHra, 20, subor);
		casHraINT = atoi(casHra);
		fgets(cas, 50, subor);
		meno[strlen(meno) - 1] = '\0';
		cas[strlen(cas) - 1] = '\0';

		UdajTabulka* udaj = new UdajTabulka(meno, casHraINT, cas);
		tabulka->push_back(udaj);

	}

	fclose(subor);
}
// otoci konkretnu kartu, ked je momentalne zobrazeny vrch karty tak zobrazi jej obrazok a naopak..
void otocKartu(PexesoKarta* karta) {
	if (karta->zobrazena == true) {
		karta->zobrazena = false;
		SendMessage(karta->getHWND(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vrch);
		pocetZobrazenych--;
		return;

	}
	if (karta->zobrazena == false) {
		karta->zobrazena = true;
		SendMessage(karta->getHWND(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)karta->getObrazok());
		pocetZobrazenych++;
	}


}
//kontrola zhody 2 kariet 
bool skontrolujZhodu(PexesoKarta* prva, PexesoKarta* druha) {
	if (prva->getObrazok() == druha->getObrazok())
		return true;

	return false;
}