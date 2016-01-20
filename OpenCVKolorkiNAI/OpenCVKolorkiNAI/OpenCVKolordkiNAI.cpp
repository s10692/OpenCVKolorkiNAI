#include <sstream>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> //prostokat



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

const int MAX_liczba_obiektow = 50;

const int MIN_powierzchnia_obiektu = 20 * 20;
const int MAX_powierzchnia_obiektu = Wysokosc_ramy * Szerokosc_ramy / 1.5;


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
	createTrackbar("Odcien_MAX", suwakiHSV, &Odcien_MAX, 255, przesuwanie);
	createTrackbar("Nasycenie_MIN", suwakiHSV, &Nasycenie_MIN, 255, przesuwanie);
	createTrackbar("Nasycenie_MAX", suwakiHSV, &Nasycenie_MAX, 255, przesuwanie);
	createTrackbar("Wartosc_MIN", suwakiHSV, &Wartosc_MIN, 255, przesuwanie);
	createTrackbar("Wartosc_MAX", suwakiHSV, &Wartosc_MAX, 255, przesuwanie);
}

void NacisnijIPrzeciagnij_prostokat(int zdarzenie, int x, int y, int flagi, void* param) {

	if (Tryb_kalibracji = true) {

		Mat* kanalVideo = (Mat*)param;

		if (zdarzenie == CV_EVENT_LBUTTONDOWN && Przeciaganie_myszy == false)
		{
			Poczatkowe_klikniecie = cv::Point(x, y);

			Przeciaganie_myszy = true;
		}

		if (zdarzenie == CV_EVENT_MOUSEMOVE && Przeciaganie_myszy == true)
		{
			Obecna_pozycja_myszy = cv::Point(x, y);

			Ruszenie_myszy = true;
		}

		if (zdarzenie == CV_EVENT_LBUTTONUP && Przeciaganie_myszy == true)
		{
			prostokatROI = Rect(Poczatkowe_klikniecie, Obecna_pozycja_myszy);


			Przeciaganie_myszy = false;
			Ruszenie_myszy = false;
			Prostokat = true;
		}

		if (zdarzenie == CV_EVENT_RBUTTONDOWN) {

			Odcien_MIN = 0;
			Odcien_MAX = 255;
			Nasycenie_MIN = 0;
			Nasycenie_MAX = 255;
			Wartosc_MIN = 0;
			Wartosc_MAX = 255;
		}
	}
}
void zapiszWartosci_HSV(cv::Mat rama, cv::Mat rama_hsv) {
	
	if (Ruszenie_myszy == false && Prostokat == true) {

		if (Odcien_ROI.size() > 0) Odcien_ROI.clear();
		if (Nasycenie_ROI.size() > 0) Nasycenie_ROI.clear();
		if (Wartosc_ROI.size() > 0) Wartosc_ROI.clear();

		if (prostokatROI.width < 1 || prostokatROI.height < 1) cout << "Proszê narysowaæ prostok¹t , nie linie " << endl;
		else {
			for (int k = prostokatROI.x; k < prostokatROI.x + prostokatROI.width; k++) {
				for (int p = prostokatROI.y; p < prostokatROI.y + prostokatROI.height; p++) {

					Odcien_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[0]);
					Nasycenie_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[1]);
					Wartosc_ROI.push_back((int)rama_hsv.at<cv::Vec3b>(p, k)[2]);
				}
			}
		}


		Prostokat = false;

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


	if (Ruszenie_myszy == true) {

		rectangle(rama, Poczatkowe_klikniecie, cv::Point(Obecna_pozycja_myszy.x, Obecna_pozycja_myszy.y), cv::Scalar(0, 255, 0), 1, 8, 0);

		}

}

string intNaString(int numer) {

	std::stringstream nn;
	nn << numer;
	return nn.str();
}

void narysujObiekt(int x, int y, Mat &rama , vector< vector<Point> > kontury, vector<Vec4i> hierarchia) {
	
	int max = 0; int i_cont = -1;

	for (int i = 0; i < kontury.size(); i ++)
		if (abs(contourArea(Mat(kontury[i]))) > max)
		{
			max = abs(contourArea(Mat(kontury[i])));
		}
	if (i_cont >= 0)
		for (int i = 0; i< kontury.size(); i++)
			drawContours(rama, kontury, i, Scalar(255, 0, 179), 2, 8, hierarchia, 0, Point());
	putText(rama, intNaString(x) + "," + intNaString(y), Point(x, y + 30), 1, 1, Scalar(255, 0, 0), 2);

}

void TransformacjeMorph(Mat &thresh) {


	Mat ErodowacElement = getStructuringElement(MORPH_RECT, Size(3, 3));

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

	vector< vector<Point > > kontury;
	vector<Vec4i> hierarchia;

	findContours(temp, kontury, hierarchia, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	double refObszar = 0;
	int najwiekszyIndeks = 0;
	bool obiektZnaleziony = false;

	if (hierarchia.size() > 0) {

		int liczbaObiektow = hierarchia.size();

		if (liczbaObiektow < MAX_liczba_obiektow) {

			for (int index = 0; index >= 0; index = hierarchia[index][0]) {

				Moments moment = moments((cv::Mat)kontury[index]);

				double obszar = moment.m00;

				if (obszar > MIN_powierzchnia_obiektu && obszar < MAX_powierzchnia_obiektu && obszar > refObszar)
				{
					x = moment.m10 / obszar;
					y = moment.m01 / obszar;

					obiektZnaleziony = true;

					refObszar = obszar;

					najwiekszyIndeks = index;
				}
				else obiektZnaleziony = false;

			}

			if (obiektZnaleziony == true)
			{
				putText(KanalKamery, "SledzenieObiektu", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);

				narysujObiekt(x, y, KanalKamery , kontury, hierarchia);

			}
		}

		else putText(KanalKamery, "Zbyt Duzo Halasu !! Zreguluj Filtr", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

	}
}


int main(int argc, char* argv[])
{

	bool sledzObiekt = true;
	bool uzyjTransformacjeMorph = true;


	Tryb_kalibracji = true;

	Mat KanalKamery;

	Mat HSV;

	Mat prog;

	int x = 0;
	int y = 0;

	VideoCapture przejmij;

	przejmij.open(0);

	przejmij.set(CV_CAP_PROP_FRAME_WIDTH, Szerokosc_ramy);
	przejmij.set(CV_CAP_PROP_FRAME_HEIGHT, Wysokosc_ramy);

	cv::namedWindow(okno);

	cv::setMouseCallback(okno, NacisnijIPrzeciagnij_prostokat, &KanalKamery);

	Przeciaganie_myszy = false;
	Ruszenie_myszy = false;
	Prostokat = false;

	while (1)
	{

		przejmij.read(KanalKamery);

		cvtColor(KanalKamery, HSV, COLOR_BGR2HSV);

		zapiszWartosci_HSV(KanalKamery, HSV);
		
		inRange(HSV, Scalar(Odcien_MIN, Nasycenie_MIN, Wartosc_MIN), Scalar(Odcien_MAX, Nasycenie_MAX, Wartosc_MAX), prog);

		if (uzyjTransformacjeMorph)
			SledzenieFiltrowanegoObiektu(x, y, prog, KanalKamery);

		if (Tryb_kalibracji == true)
		{

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


		if (waitKey(30) == 99) Tryb_kalibracji = !Tryb_kalibracji;
	}

	

	return 0;
}