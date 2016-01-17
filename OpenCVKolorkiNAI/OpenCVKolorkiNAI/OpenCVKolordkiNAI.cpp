#include <sstream>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

int Odcien_MIN = 0;
int Odcien_MAX = 256;
int Nasycenie_MIN = 0;
int Nasycenie_MAX = 256;
int Wartosc_MIN = 0;
int Wartosc_MAX = 256;


const int Wysokosc_ramy = 480;
const int Szerokosc_ramy = 640;

const int MAX_wykryte_obiekty = 50;

const int MIN_powierzchnia_obiektu = 15 * 15;
const int MAX_powierzchnia_obiektu = Wysokosc_ramy * Szerokosc_ramy / 2;


const string okno = "Oryginalny Obraz";
const string okno1 = "Obraz HSV";
const string okno2 = "Obraz programowalny";
const string okno3 = "Obraz po przekszta³ceniach";
const string suwakiHSV = "suwakiHSV";

bool Ruszenie_myszy;
bool Tryb_kalibracji;
bool Przeciaganie_myszy;

bool Prostokat;

cv::Point Poczatkowe_klikniecie, Obecna_pozycja_myszy;
cv::Rect prostokatROI;
vector<int> Odcien_ROI, Nasycenie_ROI, Wartosc_ROI;

void przesuwanie(int, void*)
{
	//Bêdzie uzupe³niane
}


void tworzenieSuwakowHSV() {

	namedWindow(suwakiHSV, 0);

	char Nazwa_suwakow[50];

	sprintf(Nazwa_suwakow, "Odcien_MIN", Odcien_MIN);
	sprintf(Nazwa_suwakow, "Odcien_MAX", Odcien_MAX);
	sprintf(Nazwa_suwakow, "Nasycenie_MIN", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Nasycenie_MAX", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Wartosc_MIN", Wartosc_MIN);
	sprintf(Nazwa_suwakow, "wartosc_MAX", Wartosc_MIN);



	createTrackbar("Odcien_MIN", suwakiHSV, &Odcien_MIN, 255, przesuwanie);
	createTrackbar("Odcien_MAX", suwakiHSV, &Odcien_MIN, 255, przesuwanie);
	createTrackbar("Nasycenie_MIN", suwakiHSV, &Nasycenie_MIN, 255, przesuwanie);
	createTrackbar("Nasycenie_MIN", suwakiHSV, &Nasycenie_MIN, 255, przesuwanie);
	createTrackbar("Wartosc_MIN", suwakiHSV, &Wartosc_MIN, 255, przesuwanie);
	createTrackbar("Wartosc_MIN", suwakiHSV, &Wartosc_MIN, 255, przesuwanie);





	
}