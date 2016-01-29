#include <sstream>
#include <string>
#include <iostream>
#include <numeric>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> //prostokat



using namespace cv;
using namespace std;
//pocz¹tkowa wartoœæ minimalna i maksymalna filtrów HSV.
//bêd¹ zmieniane za pomoc¹ potencjometru(okno suwakiHSV).
int Barwa_MIN = 0;
int Barwa_MAX = 256;
int Nasycenie_MIN = 0;
int Nasycenie_MAX = 256;
int Wartosc_MIN = 0;
int Wartosc_MAX = 256;

//pocz¹tkowa wartoœæ filtru BGR

int Czerwony = 0;
int Zielony = 0;
int Niebieski = 0;

//domyœlna szerokoœæ i wysokoœæ ramy (okna)
const int Wysokosc_ramy = 480;
const int Szerokosc_ramy = 640;

//maksymalna liczba obiektów , które maj¹ byæ wykrywane w ramce.
const int MAX_liczba_obiektow = 50;
//minimalna i maksymalna powierzchnia obiektu.
const int MIN_powierzchnia_obiektu = 10 * 10;
const int MAX_powierzchnia_obiektu = Wysokosc_ramy * Szerokosc_ramy / 1.5;   //480 * 426,666

//nazwy które bêd¹ wyœwietlane na górze ka¿dego okna.

const string okno = "Oryginalny Obraz";
const string okno1 = "Obraz HSV";
const string okno2 = "Obraz programowalny";
const string suwakiHSV = "suwakiHSV";

bool Ruszenie_myszy;
//u¿ywane do wyœwietlania okna debugowania.
bool Tryb_kalibracji;
//u¿ywane do wyœwietlania na ekranie prostok¹ta , jak u¿ytkownik kliknie i przeci¹gnie mysz.
bool Przeciaganie_myszy;

bool Prostokat;
//œledzenie klikniêcia poz¹tkowego i aktualnej pozycji myszy.
cv::Point Poczatkowe_klikniecie, Obecna_pozycja_myszy;
//rejon który u¿ytkownik wybra³.
cv::Rect prostokatROI;
//wartoœæ HSV z regionu który wybra³ u¿ytkownik, przechowywane w osobnych wektorach aby by³a mo¿liwoœæ ³atwego ich posortowania
vector<int> Barwa_ROI, Nasycenie_ROI, Wartosc_ROI;
//wartoœæ BGR dla kontur œledzonego obiektu
vector<int> Czerwony_ROI, Zielony_ROI, Niebieski_ROI;

string intNaString(int numer) {
	//zamina int Na String
	std::stringstream nn;
	nn << numer;
	return nn.str();
}

//Tworzenie okna potencjometru (suwakiHSV).
void tworzenieSuwakowHSV() {

	namedWindow(suwakiHSV, 0);
	//tworzenie pamiêci do przechowywania nazw w potencjometrze.
	char Nazwa_suwakow[50];

	sprintf(Nazwa_suwakow, "Barwa_MIN", Barwa_MIN);
	sprintf(Nazwa_suwakow, "Barwa_MAX", Barwa_MAX);
	sprintf(Nazwa_suwakow, "Nasycenie_MIN", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Nasycenie_MAX", Nasycenie_MIN);
	sprintf(Nazwa_suwakow, "Wartosc_MIN", Wartosc_MIN);
	sprintf(Nazwa_suwakow, "wartosc_MAX", Wartosc_MIN);

	//tworzenie potencjonometru i wstawienie ich do okna

	createTrackbar("Barwa_MIN", suwakiHSV, &Barwa_MIN, 255, NULL);
	createTrackbar("Barwa_MAX", suwakiHSV, &Barwa_MAX, 255, NULL);
	createTrackbar("Nasycenie_MIN", suwakiHSV, &Nasycenie_MIN, 255, NULL);
	createTrackbar("Nasycenie_MAX", suwakiHSV, &Nasycenie_MAX, 255, NULL);
	createTrackbar("Wartosc_MIN", suwakiHSV, &Wartosc_MIN, 255, NULL);
	createTrackbar("Wartosc_MAX", suwakiHSV, &Wartosc_MAX, 255, NULL);
}


void NacisnijIPrzeciagnij_prostokat(int zdarzenie, int x, int y, int flagi, void* param) {
	//je¿eli tryb kalibracji jest prawd¹ , bêdziemy korzystaæ z myszy aby zmieniæ wartoœci HSV
	//je¿eli tryb kalibracji jest fa³szywy , ko¿ystamy z suwaków HSV 
	if (Tryb_kalibracji = true) {
		//obs³uga do kana³u wideo jest przekazywana jako parametr i wrzucenie do Mat jako wskaŸnika
		Mat* kanalVideo = (Mat*)param;

		if (zdarzenie == CV_EVENT_LBUTTONDOWN && Przeciaganie_myszy == false)
		{
			//œledzenie punktu pocz¹tkowego klikniêcia.
			Poczatkowe_klikniecie = cv::Point(x, y);
			//u¿ytkownik rozpocz¹³ przeci¹ganie myszy.
			Przeciaganie_myszy = true;
		}

		//Przeci¹ganie myszy przez u¿ytkownika.
		if (zdarzenie == CV_EVENT_MOUSEMOVE && Przeciaganie_myszy == true)
		{
			//œledzenie aktualnej pozycji myszy.
			Obecna_pozycja_myszy = cv::Point(x, y);
			//u¿ytkownik przeniós³ myszy klikaj¹æ i przeci¹gaj¹c
			Ruszenie_myszy = true;
		}
		//u¿ytkownik zwolni³ lewy przycisk myszy.
		if (zdarzenie == CV_EVENT_LBUTTONUP && Przeciaganie_myszy == true)
		{
			//ustaw wybrany obszar który wybra³ u¿ytkownik za pomoc¹ prostok¹tu.
			prostokatROI = Rect(Poczatkowe_klikniecie, Obecna_pozycja_myszy);

			//resetowanie zmiennych logicznych.
			Przeciaganie_myszy = false;
			Ruszenie_myszy = false;
			Prostokat = true;
		}

		if (zdarzenie == CV_EVENT_RBUTTONDOWN) {
			//Gdy u¿ytkownik kliknie prawy przycisk myszy 
			//Resetuje wartoœci HSV
			Barwa_MIN = 0;
			Barwa_MAX = 255;
			Nasycenie_MIN = 0;
			Nasycenie_MAX = 255;
			Wartosc_MIN = 0;
			Wartosc_MAX = 255;
		}
	}
}
//Zapisanie wartoœci BGR do rysowania kontur obiektu
 void zapiszWartosc_BGR(cv::Mat rama, cv::Mat rama_bgr) {

	if (Ruszenie_myszy == false && Prostokat == true) {
		//czyszczenie poprzednich wartoœci wektora
		if (Czerwony_ROI.size() > 0) Czerwony_ROI.clear();
		if (Zielony_ROI.size() > 0) Zielony_ROI.clear();
		if (Niebieski_ROI.size() > 0) Niebieski_ROI.clear();
		else {
			for (int k = prostokatROI.x; k < prostokatROI.x + prostokatROI.width; k++) {
				for (int p = prostokatROI.y; p < prostokatROI.y + prostokatROI.height; p++) {
					//zapisanie wartoœci BGR w tym momencie.
					Czerwony_ROI.push_back((int)rama_bgr.at<cv::Vec3b>(p, k)[2]);
					Zielony_ROI.push_back((int)rama_bgr.at<cv::Vec3b>(p, k)[1]);
					Niebieski_ROI.push_back((int)rama_bgr.at<cv::Vec3b>(p, k)[0]);
				}
			}
		}

		if (Czerwony_ROI.size() > 0) {
			int red_sum = std::accumulate(Czerwony_ROI.begin(), Czerwony_ROI.end(), 0);
			int red_avg = red_sum / Czerwony_ROI.size();
			Czerwony = red_avg;
		}

		if (Zielony_ROI.size() > 0) {

			int green_sum = std::accumulate(Zielony_ROI.begin(), Zielony_ROI.end(), 0);
			int green_avg = green_sum / Zielony_ROI.size();
			Zielony = green_avg;

		}

		if (Niebieski_ROI.size() > 0) {

			int blue_sum = std::accumulate(Niebieski_ROI.begin(), Niebieski_ROI.end(), 0);
			int blue_avg = blue_sum / Niebieski_ROI.size();
			Niebieski = blue_avg;

		}

	}


} 


//Zapisanie wartoœci HSV wybranego obszaru do wektora
void zapiszWartosci_HSV(cv::Mat rama, cv::Mat rama_hsv) {


	if (Ruszenie_myszy == true) {
		//je¿eli mysz jest wciœniêta , rysujemy prostok¹t na ekranie i klikniêcie
		rectangle(rama, Poczatkowe_klikniecie, cv::Point(Obecna_pozycja_myszy.x, Obecna_pozycja_myszy.y), cv::Scalar(0, 0, 255), 1, 8, 0);

	}
	
	if (Ruszenie_myszy == false && Prostokat == true) {
		//czyszczenie poprzednich wartoœci wektora
		if (Barwa_ROI.size() > 0) Barwa_ROI.clear();
		if (Nasycenie_ROI.size() > 0) Nasycenie_ROI.clear();
		if (Wartosc_ROI.size() > 0) Wartosc_ROI.clear();
		//je¿eli uzytkownik przeci¹gnie tylko linie to nie znajdzie z wybranego obszaru wysokoœci ani szerokoœci i wyœliwetli b³¹d
		if (prostokatROI.width < 1 || prostokatROI.height < 1) cout << "Proszê narysowaæ prostok¹t" << endl;
		else {
			for (int k = prostokatROI.x; k < prostokatROI.x + prostokatROI.width; k++) {
				for (int p = prostokatROI.y; p < prostokatROI.y + prostokatROI.height; p++) {
					//zapisanie wartoœci HSV w tym momencie.
					Barwa_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[0]);
					Nasycenie_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[1]);
					Wartosc_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[2]);
				}
			}
		}

		//resetowanie wybranego prostok¹ta , wiêc w razie potrzeby u¿ytkownik mo¿e wybraæ inny region.
		Prostokat = false;
		//ustawienie minimalnej i maksymalnej wartoœci od minimalnego i maksymalnego elementu ka¿dej tablicy. 
		if (Barwa_ROI.size() > 0) {

			Barwa_MIN = *std::min_element(Barwa_ROI.begin(), Barwa_ROI.end());
			Barwa_MAX = *std::max_element(Barwa_ROI.begin(), Barwa_ROI.end());

			cout << "Wartosc 'MIN' Barwy:  " << Barwa_MIN << endl;
			cout << "Wartosc 'MAX' Barwy:  " << Barwa_MAX << endl;
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


void narysujObiekt(int x, int y, Mat &rama , vector< vector<Point> > kontury, vector<Vec4i> hierarchia) {
	
	//Rysowanie kontur wokó³ wybranego koloru / obiektu

	int max = 0;

	for (int i = 0; i < kontury.size(); i ++)
		if (abs(contourArea(Mat(kontury[i]))) > max)
		{
			max = abs(contourArea(Mat(kontury[i])));

			drawContours(rama, kontury, i, Scalar(Czerwony,Zielony,Niebieski), 3, 8, hierarchia, 0, Point());
		}



	//Wypisanie na ekranie pozycji zaznaczonego obiektu.
	putText(rama, intNaString(x) + "," + intNaString(y), Point(x, y + 30), 1, 1, Scalar(0, 0, 255), 2);


}

void TransformacjeMorph(Mat &thresh) {

	//tworznie elementu strukturalnego który bêdzie u¿ywany do rozszerzania 'dilate' i 'erodowania' obrazu.
	//wybrany element to 3px na 3px prostokat
	Mat ErodowacElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//Rozszerzenie wiêkszego elementu aby by³o pewne ¿e obiekt jest dobrze widoczny.
	Mat RozszerzacElement = getStructuringElement(MORPH_RECT, Size(8, 8));


	//funkcja ta powoduje przefiltrowanie naszego obrazka z zastosowaniem filtru erozji
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
	//znalezienie kontur filtrowanego obrazu za pomoc¹ funkcji OpenCV findContours.
	findContours(temp, kontury, hierarchia, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//u¿yci metody moments do znalezienia naszego filtrowanego obiektu.
	double refObszar = 0;
	int najwiekszyIndeks = 0;
	bool obiektZnaleziony = false;

	if (hierarchia.size() > 0) {

		int liczbaObiektow = hierarchia.size();
		//je¿eli znaleziona liczba obiektów jest wiêksza od MAX_liczba_obiektow to mamy g³oœny filtr.
		if (liczbaObiektow < MAX_liczba_obiektow) {

			for (int index = 0; index >= 0; index = hierarchia[index][0]) {

				Moments moment = moments((cv::Mat)kontury[index]);

				double obszar = moment.m00;

				//je¿eli powierzchnia jest mniejsza jak 20px na 20px to jest to prawdopodobnie tylko ha³as.
				//je¿eli obszar jest jest taki sam , jak 3/2 wielkoœci obrazu , prawdopodobnie jest z³y filt.
				//my tylko chcemy obiektu z najwiêkszym obszarem wiêc oszczêdzamy obszar odniesienia
				//powtarzanie i porównywanie go do obszaru kolejnej iteracji.
				if (obszar > MIN_powierzchnia_obiektu && obszar < MAX_powierzchnia_obiektu && obszar > refObszar)
				{
					x = moment.m10 / obszar;
					y = moment.m01 / obszar;

					obiektZnaleziony = true;

					refObszar = obszar;
					//zapisz indeks najwiêkszych kontur. do korzystania z 
					najwiekszyIndeks = index;
				}
				else obiektZnaleziony = false;

			}
			//niech u¿ytkownik wie ¿e znalaz³ obiekt
			if (obiektZnaleziony == true)
			{
				putText(KanalKamery, "Obiekt Znaleziony", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//narysuj lokalizacjê obiektu na ekranie.
				narysujObiekt(x, y, KanalKamery , kontury, hierarchia);

			}
		}
		//w przeciwnym razie wyœwietl ¿e za du¿o ha³asu i nale¿y zoptymalizowaæ filt.
		else putText(KanalKamery, "Zbyt Duzo Halasu !! Zreguluj Filtr", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

	}
}


int main(int argc, char* argv[])
{
	//niektóre zmienne logiczne dla ró¿nych funkcji w ramach tego programu.
	bool sledzObiekt = true;
	bool uzyjTransformacjeMorph = true;


	Tryb_kalibracji = true;
	//MAtrix do przechowywania obrazu BGR
	Mat BGR;
	//Matrix do przechowywania ka¿dej klatki kamery
	Mat KanalKamery;
	//Matrix to przechowywania obrazu HSV
	Mat HSV;
	//Matrix do przechowywania obrazu binarnego prog
	Mat prog;
	//wartoœci x i y o lokaliacji obiektu.
	int x = 0;
	int y = 0;
	//obiekt przechwytywania wideo.
	VideoCapture przejmij;
	//otwarcie przechwytywania obiektu w lokalizaji 0.
	przejmij.open(0);
	//wysokoœæ i szerokoœæ przechwytywanej ramy(obrazu)
	przejmij.set(CV_CAP_PROP_FRAME_WIDTH, Szerokosc_ramy);
	przejmij.set(CV_CAP_PROP_FRAME_HEIGHT, Wysokosc_ramy);
	//tworzenie okna.
	cv::namedWindow(okno);
	//ustaw wywo³anie funkcji myszy w oknie "KanalKamery".
	cv::setMouseCallback(okno, NacisnijIPrzeciagnij_prostokat, &KanalKamery);
	//inicjalizacja ruchu myszy i ustawienie na false.
	Przeciaganie_myszy = false;
	Ruszenie_myszy = false;
	Prostokat = false;
	//rozpoczêcie nieskoñczonej pêtli gdzie obraz z kamery jest kopiowany do KanalKamery matrix.
	//wszystkie nasze dzia³ania zostan¹ przeprowadzone w tej pêtli.
	while (1)
	{
		//sklepienie obrazu do matrix.
		przejmij.read(KanalKamery);
		//ustawienie wartoœci BGR
        BGR = KanalKamery;
        zapiszWartosc_BGR(KanalKamery, BGR);
		//konwersja ramki z BGR do HSV.
		cvtColor(KanalKamery, HSV, COLOR_BGR2HSV);
		//ustawienie wartoœci HSV z wybranego przez u¿ytkownika regionu.
		zapiszWartosci_HSV(KanalKamery, HSV);
		//filtrowanie HSV miêdzy wartoœciami i filtrowanym obiektem.
		inRange(HSV, Scalar(Barwa_MIN, Nasycenie_MIN, Wartosc_MIN), Scalar(Barwa_MAX, Nasycenie_MAX, Wartosc_MAX), prog);
		//wykonywanie operacji morfologicznych na progowym obrazie w celu wyeliminowania zak³uceñ
		//i podkreœlaj¹ filtrowany obiekt
		if (uzyjTransformacjeMorph)
			TransformacjeMorph(prog);

		if (sledzObiekt)
			SledzenieFiltrowanegoObiektu(x, y, prog, KanalKamery);
		
		
		if (Tryb_kalibracji == true)
		{
			//tworzenie suwaków filtrowania HSV.
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
		//opóŸnienie 30 ms, tak ¿e ekran mo¿na odœwie¿yæ.
		if (waitKey(30) == 99) Tryb_kalibracji = !Tryb_kalibracji;
	}

	

	return 0;
}