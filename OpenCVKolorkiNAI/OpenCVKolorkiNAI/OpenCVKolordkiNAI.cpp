#include <sstream>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> //prostokat



using namespace cv;
using namespace std;
//pocz�tkowa warto�� minimalna i maksymalna filtr�w HSV.
//b�d� zmieniane za pomoc� potencjometru(okno suwakiHSV).
int Odcien_MIN = 0;
int Odcien_MAX = 256;
int Nasycenie_MIN = 0;
int Nasycenie_MAX = 256;
int Wartosc_MIN = 0;
int Wartosc_MAX = 256;

//domy�lna szeroko�� i wysoko��.
const int Wysokosc_ramy = 480;
const int Szerokosc_ramy = 640;

//maksymalna liczba obiekt�w , kt�re maj� by� wykrywane w ramce.
const int MAX_liczba_obiektow = 50;
//minimalna i maksymalna powierzchnia obiektu.
const int MIN_powierzchnia_obiektu = 20 * 20;
const int MAX_powierzchnia_obiektu = Wysokosc_ramy * Szerokosc_ramy / 1.5;   //480 * 426,666

//nazwy kt�re b�d� wy�wietlane na g�rze ka�dego okna.

const string okno = "Oryginalny Obraz";
const string okno1 = "Obraz HSV";
const string okno2 = "Obraz programowalny";
const string suwakiHSV = "suwakiHSV";

bool Ruszenie_myszy;
//u�ywane do wy�wietlania okna debugowania.
bool Tryb_kalibracji;
//u�ywane do wy�wietlania na ekranie prostok�ta , jak u�ytkownik kliknie i przeci�gnie mysz.
bool Przeciaganie_myszy;

bool Prostokat;
//�ledzenie klikni�cia poz�tkowego i aktualnej pozycji myszy.
cv::Point Poczatkowe_klikniecie, Obecna_pozycja_myszy;
//rejon kt�ry u�ytkownik wybra�.
cv::Rect prostokatROI;
//warto�� HSV z regionu kt�ry wybra� u�ytkownik, przechowywane w osobnych wektorach aby by�a mo�liwo�� �atwego ich posortowania
vector<int> Odcien_ROI, Nasycenie_ROI, Wartosc_ROI;

void przesuwanie(int, void*)
{
	//B�dzie uzupe�niane.
}

//Tworzenie okna potencjometru (suwakiHSV).
void tworzenieSuwakowHSV() {

	namedWindow(suwakiHSV, 0);
	//tworzenie pami�ci do przechowywania nazw w potencjometrze.
	char Nazwa_suwakow[50];

	sprintf(Nazwa_suwakow, "Odcien_MIN", Odcien_MIN);
	sprintf(Nazwa_suwakow, "Odcien_MAX", Odcien_MAX);
	sprintf(Nazwa_suwakow, "Nasycenie_MIN", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Nasycenie_MAX", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Wartosc_MIN", Wartosc_MIN);
	sprintf(Nazwa_suwakow, "wartosc_MAX", Wartosc_MIN);

	//tworzenie potencjonometru i wstawienie ich do okna
	//3 parametry : adres ziennej kt�ra si� zmienia
	//Maksymalna warto�� (np. Odcien_MAX)
	//przesuwanie - funkcja kt�ra jest wy�owywana kiedy suwaki s� przesuwane

	createTrackbar("Odcien_MIN", suwakiHSV, &Odcien_MIN, 255, przesuwanie);
	createTrackbar("Odcien_MAX", suwakiHSV, &Odcien_MAX, 255, przesuwanie);
	createTrackbar("Nasycenie_MIN", suwakiHSV, &Nasycenie_MIN, 255, przesuwanie);
	createTrackbar("Nasycenie_MAX", suwakiHSV, &Nasycenie_MAX, 255, przesuwanie);
	createTrackbar("Wartosc_MIN", suwakiHSV, &Wartosc_MIN, 255, przesuwanie);
	createTrackbar("Wartosc_MAX", suwakiHSV, &Wartosc_MAX, 255, przesuwanie);
}

void NacisnijIPrzeciagnij_prostokat(int zdarzenie, int x, int y, int flagi, void* param) {
	//je�eli tryb kalibracji jest prawd� , b�dziemy korzysta� z myszy aby zmieni� warto�ci HSV
	//je�eli tryb kalibracji jest fa�szywy , ko�ystamy z suwak�w HSV 
	if (Tryb_kalibracji = true) {
		//obs�uga do kana�u wieo jest przekazywana jako parametr i wrzucenie do Mat jako wska�nika
		Mat* kanalVideo = (Mat*)param;

		if (zdarzenie == CV_EVENT_LBUTTONDOWN && Przeciaganie_myszy == false)
		{
			//�ledzenie punktu pocz�tkowego klikni�cia.
			Poczatkowe_klikniecie = cv::Point(x, y);
			//u�ytkownik rozpocz�� przeci�ganie myszy.
			Przeciaganie_myszy = true;
		}

		//Przeci�ganie myszy przez u�ytkownika.
		if (zdarzenie == CV_EVENT_MOUSEMOVE && Przeciaganie_myszy == true)
		{
			//�ledzenie aktualnej pozycji myszy.
			Obecna_pozycja_myszy = cv::Point(x, y);
			//u�ytkownik przeni�s� myszy klikaj�� i przeci�gaj�c
			Ruszenie_myszy = true;
		}
		//u�ytkownik zwolni� lewy przycisk myszy.
		if (zdarzenie == CV_EVENT_LBUTTONUP && Przeciaganie_myszy == true)
		{
			//ustaw wybrany obszar kt�ry wybra� u�ytkownik za pomoc� prostok�tu.
			prostokatROI = Rect(Poczatkowe_klikniecie, Obecna_pozycja_myszy);

			//resetowanie zmiennych logicznych.
			Przeciaganie_myszy = false;
			Ruszenie_myszy = false;
			Prostokat = true;
		}

		if (zdarzenie == CV_EVENT_RBUTTONDOWN) {
			//Gdy u�ytkownik kliknie rawy przycisk myszy 
			//Resetuje warto�ci HSV
			Odcien_MIN = 0;
			Odcien_MAX = 255;
			Nasycenie_MIN = 0;
			Nasycenie_MAX = 255;
			Wartosc_MIN = 0;
			Wartosc_MAX = 255;
		}
	}
}

//Zapisanie warto�ci HSV wybranego obszaru do wektora
void zapiszWartosci_HSV(cv::Mat rama, cv::Mat rama_hsv) {


	if (Ruszenie_myszy == true) {
		//je�eli mysz jest wci�ni�ta , rysujemy prostok�t na ekranie i klikni�cie
		rectangle(rama, Poczatkowe_klikniecie, cv::Point(Obecna_pozycja_myszy.x, Obecna_pozycja_myszy.y), cv::Scalar(0, 255, 0), 1, 8, 0);

	}
	
	if (Ruszenie_myszy == false && Prostokat == true) {
		//czyszczenie poprzednich warto�ci wektora
		if (Odcien_ROI.size() > 0) Odcien_ROI.clear();
		if (Nasycenie_ROI.size() > 0) Nasycenie_ROI.clear();
		if (Wartosc_ROI.size() > 0) Wartosc_ROI.clear();
		//je�eli uzytkownik przeci�gnie tylko linie to nie znajdzie z wybranego obszaru wysoko�ci ani szeroko�ci i wy�liwetli b��d
		if (prostokatROI.width < 1 || prostokatROI.height < 1) cout << "Prosz� narysowa� prostok�t , nie linie " << endl;
		else {
			for (int k = prostokatROI.x; k < prostokatROI.x + prostokatROI.width; k++) {
				for (int p = prostokatROI.y; p < prostokatROI.y + prostokatROI.height; p++) {
					//zapisanie warto�ci HSV w tym momencie.
					Odcien_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[0]);
					Nasycenie_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[1]);
					Wartosc_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[2]);
				}
			}
		}

		//resetowanie wybranego prostok�ta , wi�c w razie potrzeby u�ytkownik mo�e wybra� inny region.
		Prostokat = false;
		//ustawienie minimalnej i maksymalnej warto�ci od minimalnego i maksymalnego elementu ka�dej tablicy. 
		if (Odcien_ROI.size() > 0) {

			Odcien_MIN = *std::min_element(Odcien_ROI.begin(), Odcien_ROI.end());
			Odcien_MAX = *std::max_element(Odcien_ROI.begin(), Odcien_ROI.end());

			cout << "Wartosc 'MIN' Odcienia:  " << Odcien_MIN << endl;
			cout << "Wartosc 'MAX' Odcienia:  " << Odcien_MAX << endl;
		}

		if (Nasycenie_ROI.size() > 0) {

			Nasycenie_MIN = *std::min_element(Nasycenie_ROI.begin(), Nasycenie_ROI.end());
			Nasycenie_MAX = *std::max_element(Nasycenie_ROI.begin(), Nasycenie_ROI.end());

			cout << "Wartosc 'MIN' Nasycenia:  " << Nasycenie_MIN << endl;
			cout << "Wartosc 'MAX' Nasycenia:  " << Nasycenie_MAX << endl;
		}

		if (Wartosc_ROI.size() > 0) {

			Wartosc_MIN = *std::min_element(Wartosc_ROI.begin(), Wartosc_ROI.end());
			Wartosc_MAX = *std::max_element(Wartosc_ROI.begin(), Wartosc_ROI.end());

			cout << "Wartosc 'MIN' Wartosci:  " << Wartosc_MIN << endl;
			cout << "Wartosc 'MAX' Wartosci:  " << Wartosc_MAX << endl;
		}


	}


}

string intNaString(int numer) {
	//zamina int Na String
	std::stringstream nn;
	nn << numer;
	return nn.str();
}

void narysujObiekt(int x, int y, Mat &rama , vector< vector<Point> > kontury, vector<Vec4i> hierarchia) {
	
	//Rysowanie kontur wok� wybranego koloru / obiektu

	int max = 0; int i_cont = -1;

	for (int i = 0; i < kontury.size(); i ++)
		if (abs(contourArea(Mat(kontury[i]))) > max)
		{
			max = abs(contourArea(Mat(kontury[i])));
			drawContours(rama, kontury, i, Scalar(255, 0, 0), 3, 8, hierarchia, 0, Point());
		}



	//Wypisanie na ekranie pozycji zaznaczonego obiektu.
	putText(rama, intNaString(x) + "," + intNaString(y), Point(x, y + 30), 1, 1, Scalar(255, 0, 0), 2);


}

void TransformacjeMorph(Mat &thresh) {

	//tworznie elementu strukturalnego kt�ry b�dzie u�ywany do rozszerzania 'dilate' i 'erodowania' obrazu.
	//wybrany element to 3px na 3px prostokat
	Mat ErodowacElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//Rozszerzenie wi�kszego elementu aby by�o pewne �e obiekt jest dobrze widoczny.
	Mat RozszerzacElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, ErodowacElement);
	erode(thresh, thresh, ErodowacElement);

	dilate(thresh, thresh, RozszerzacElement);
	dilate(thresh, thresh, RozszerzacElement);

}

void SledzenieFiltrowanegoObiektu(int &x, int &y, Mat prog, Mat &KanalKamery)
{

	Mat temp;
	prog.copyTo(temp);

	//dwa wektory potrzebne do znalezienia kontur.
	vector< vector<Point > > kontury;
	vector<Vec4i> hierarchia;
	//znalezienie kontur filtrowanego obrazu za pomoc� funkcji OpenCV findContours.
	findContours(temp, kontury, hierarchia, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//u�yci metody moments do znalezienia naszego filtrowanego obiektu.
	double refObszar = 0;
	int najwiekszyIndeks = 0;
	bool obiektZnaleziony = false;

	if (hierarchia.size() > 0) {

		int liczbaObiektow = hierarchia.size();
		//je�eli znaleziona liczba obiekt�w jest wi�ksza od MAX_liczba_obiektow to mamy g�o�ny filtr.
		if (liczbaObiektow < MAX_liczba_obiektow) {

			for (int index = 0; index >= 0; index = hierarchia[index][0]) {

				Moments moment = moments((cv::Mat)kontury[index]);

				double obszar = moment.m00;

				//je�eli powierzchnia jest mniejsza jak 20px na 20px to jest to prawdopodobnie tylko ha�as.
				//je�eli obszar jest jest taki sam , jak 3/2 wielko�ci obrazu , prawdopodobnie jest z�y filt.
				//my tylko chcemy obiektu z najwi�kszym obszarem wi�c oszcz�dzamy obszar odniesienia
				//powtarzanie i por�wnywanie go do obszaru kolejnej iteracji.
				if (obszar > MIN_powierzchnia_obiektu && obszar < MAX_powierzchnia_obiektu && obszar > refObszar)
				{
					x = moment.m10 / obszar;
					y = moment.m01 / obszar;

					obiektZnaleziony = true;

					refObszar = obszar;
					//zapisz indeks najwi�kszych kontur. do korzystania z 
					najwiekszyIndeks = index;
				}
				else obiektZnaleziony = false;

			}
			//niech u�ytkownik wie �e znalaz� obiekt
			if (obiektZnaleziony == true)
			{
				putText(KanalKamery, "SledzenieObiektu", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//narysuj lokalizacj� obiektu na ekranie.
				narysujObiekt(x, y, KanalKamery , kontury, hierarchia);

			}
		}
		//w przeciwnym razie wy�wietl �e za du�o ha�asu i nale�y zoptymalizowa� filt.
		else putText(KanalKamery, "Zbyt Duzo Halasu !! Zreguluj Filtr", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

	}
}


int main(int argc, char* argv[])
{
	//niekt�re zmienne logiczne dla r�nych funkcji w ramach tego programu.
	bool sledzObiekt = true;
	bool uzyjTransformacjeMorph = true;


	Tryb_kalibracji = true;
	//Matrix do przechowywania ka�dej klatki kamery
	Mat KanalKamery;
	//Matrix to przechowywania obrazu HSV
	Mat HSV;
	//Matrix do przechowywania obrazu binarnego prog
	Mat prog;
	//warto�ci x i y o lokaliacji obiektu.
	int x = 0;
	int y = 0;
	//obiekt przechwytywania wideo.
	VideoCapture przejmij;
	//otwarcie przechwytywania obiektu w lokalizaji 0.
	przejmij.open(0);
	//wysoko�� i szeroko�� przechwytywanej ramy(obrazu)
	przejmij.set(CV_CAP_PROP_FRAME_WIDTH, Szerokosc_ramy);
	przejmij.set(CV_CAP_PROP_FRAME_HEIGHT, Wysokosc_ramy);
	//tworzenie okna.
	cv::namedWindow(okno);
	//ustaw wywo�anie funkcji myszy w oknie "KanalKamery".
	//
	cv::setMouseCallback(okno, NacisnijIPrzeciagnij_prostokat, &KanalKamery);
	//inicjalizacja ruchu myszy i ustawienie na false.
	Przeciaganie_myszy = false;
	Ruszenie_myszy = false;
	Prostokat = false;
	//rozpocz�cie niesko�czonej p�tli gdzie obraz z kamery jest kopiowany do KanalKamery matrix.
	//wszystkie nasze dzia�ania zostan� przeprowadzone w tej p�tli.
	while (1)
	{
		//sklepienie obrazu do matrix.
		przejmij.read(KanalKamery);
		//konwersja ramki z BGR do HSV.
		cvtColor(KanalKamery, HSV, COLOR_BGR2HSV);
		//ustawienie warto�ci HSV z wybranego przez u�ytkownika regionu.
		zapiszWartosci_HSV(KanalKamery, HSV);
		//filtrowanie HSV mi�dzy warto�ciami i filtrowanym obiektem.
		inRange(HSV, Scalar(Odcien_MIN, Nasycenie_MIN, Wartosc_MIN), Scalar(Odcien_MAX, Nasycenie_MAX, Wartosc_MAX), prog);
		//wykonywanie operacji morfologicznych na progowym obrazie w celu wyeliminowania zak�uce�
		//i podkre�laj� filtrowany obiekt
		if (uzyjTransformacjeMorph)
			SledzenieFiltrowanegoObiektu(x, y, prog, KanalKamery);

		if (sledzObiekt)
			SledzenieFiltrowanegoObiektu(x, y, prog, KanalKamery);
		
		
		if (Tryb_kalibracji == true)
		{
			//tworzenie suwak�w filtrowania HSV.
			tworzenieSuwakowHSV();
			imshow(okno1, HSV);
			imshow(okno2, prog);
		}
		else
		{
			destroyWindow(okno1);
			destroyWindow(okno2);
			destroyWindow(suwakiHSV);
		}

		imshow(okno, KanalKamery);
		//op�nienie 30 ms, tak �e ekran mo�na od�wie�y�.
		//Obraz nie pojawi si� bez tego polecenia wait(Key())
		//u�ycie polecenia waitKey() aby przechwyci� dane z klawiatury.
        

		if (waitKey(30) == 99) Tryb_kalibracji = !Tryb_kalibracji;
	}

	

	return 0;
}