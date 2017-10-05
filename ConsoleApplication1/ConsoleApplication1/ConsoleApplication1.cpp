#include "stdafx.h"

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <windows.h>
#include <time.h>

#include "opencv2/video/background_segm.hpp"
#include <stdio.h>
#include <string>

using namespace cv;
using namespace std;


Mat imgOriginal;

double myRound( double fValue )
{
    return fValue < 0 ? ceil( fValue - 0.5 )
        : floor( fValue + 0.5 );
}


int * detectColor( int x, int y, int outerA, int outerZ, int innerA, int innerZ)
{
	cout << "Wykrywam kolor..." << endl;

	static int resultArr [6];

	Mat image=imgOriginal.clone();
	Vec3b rgb=image.at<Vec3b>(y,x);
	int B=rgb.val[0];
	int G=rgb.val[1];
	int R=rgb.val[2];
	
	Mat HSV;
	Mat RGB=image(Rect(x,y,1,1));
	cvtColor(RGB, HSV,CV_BGR2HSV);
	
    Vec3b hsv=HSV.at<Vec3b>(0,0);
    int H=hsv.val[0];
    int S=hsv.val[1];
    int V=hsv.val[2];
	
	Mat rectRGB,
		rectHSV;

	Vec3b rectHSVSingle;

	int minH = 179,
		minS = 255,
		minV = 255,
		
		maxH = 0,
		maxS = 0,
		maxV = 0;

	
	for (int i = outerA; i <= outerZ; i++)
	{

		for (int j = innerA; j <= innerZ; j++)
		{

			rectRGB = image(Rect(i, j, 1, 1));
			cvtColor(rectRGB , rectHSV, CV_BGR2HSV);
			rectHSVSingle = rectHSV.at<Vec3b>(0,0);

			int singleH = rectHSVSingle.val[0];
			int singleS = rectHSVSingle.val[1];
			int singleV = rectHSVSingle.val[2];

			
			// Znajdz MINMALNE wartosci dla HSV
			if (minH > singleH)
				minH = singleH;
			
			if (minS > singleS)
				minS = singleS;
			
			if (minV > singleV)
				minV = singleV;

			// Znajdz MAKSYMALNE wartosci dla HSV
			if (maxH < singleH)
				maxH = singleH;

			if (maxS < singleS)
				maxS = singleS;

			if (maxV < singleV)
				maxV = singleV;

		}
	}	

	cout << "H: " << H << " S: " << S << " V: " << V << endl;
	cout << "minH: " << minH << " minS: " << minS << " minV: " << minV << endl;
	cout << "maxH: " << maxH << " maxS: " << maxS << " maxV: " << maxV << endl;

	resultArr[0] = minH;
	resultArr[1] = maxH;
	resultArr[2] = minS;
	resultArr[3] = maxS;
	resultArr[4] = minV;
	resultArr[5] = maxV;
	
	
	return resultArr;
}


int getConfigY()
{
	return /*800*/ 230;
}

// szerokosc okna z Menu
int getMenuX()
{
	return 504;
}

// wysokosc okna z Menu
int getMenuY()
{
	return /*129*/ 171;
}

// szerokosc pojedynczej opcji z Menu
double getMenuOptX()
{
	return getMenuX() / 3.15;
}

// wysokosc pojedynczej opcji z Menu
double getMenuOptY()
{
	return getMenuY() / /*4.3*/ 5.7;
}

int switchCalibration = 1,
	calibSize = 2,
	sizeRatio = 1;

bool startCalibration = false,
	changedCalibSize = false,
	canSaveImage = false,
	showFps = false,
	doFlip = true;

void mouseClickButton(int event, int x, int y, int flags, void* userdata)
{
	//// x = 3.15 / y = 8.7
	
	double menuY = (getConfigY() - getMenuY());

	// gdy myszka jest na wysokosci przyciskow  FPS i Flip
	if (y <= getMenuOptY())
	{
		// gdy myszka jest na przycisku FPS
		if (x >= getMenuOptX() * 0.85 && x <= getMenuOptX() * 1.5)
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));
			
			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				
				if (showFps)
					showFps = false;
				else showFps = true;
			}
		}

		// gdy myszka jest na przycisku Flip
		else if (x >= getMenuOptX() * 1.6 && x <= getMenuOptX() * 2.35)
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));

			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				
				if (doFlip)
					doFlip = false;
				else doFlip = true;
			}

		}

	}

	// gdy myszka jest na wysokosci przyciskow wyboru trybu
	else if (y >= menuY && y <= menuY + getMenuOptY())
	{
						
		double drawCalibOptRightEdge = getMenuOptX(),
			selectCalibOptRightEdge = drawCalibOptRightEdge + 10,
			drawAndSelectOptRightEdge = drawCalibOptRightEdge + selectCalibOptRightEdge + 20;
		
		// gdy myszka jest na jednym z przyciskow, a nie miedzy nimi
		////x <= drawCalibOptRightEdge || (x >= drawAndSelectOptRightEdge && x <= drawAndSelectOptRightEdge + drawCalibOptRightEdge) || x >= drawAndSelectOptRightEdge * 2
		if (x <= drawCalibOptRightEdge || (x >= selectCalibOptRightEdge && x <= selectCalibOptRightEdge + drawCalibOptRightEdge) || x >= selectCalibOptRightEdge * 2)
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));
			
			// gdy uzytkownik kliknie lewym przyciskiem myszy
			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				
				// jesli kliknieto przycisk "kalibracja rysowania"
				if (x <= drawCalibOptRightEdge)
				{
					switchCalibration = 0;
					sizeRatio = 0;
					////setTrackbarPos("Kalibruj", "Oryginalny", 0);
					cout << "kalibruj rysowanie" << endl;
				}
				
				// jesli kliknieto przycisk "kalibracja wybierania"
				////x >= drawAndSelectOptRightEdge && x <= drawAndSelectOptRightEdge + drawCalibOptRightEdge
				else if (x >= selectCalibOptRightEdge && x <= selectCalibOptRightEdge + drawCalibOptRightEdge)
				{
					switchCalibration = 2;
					sizeRatio = 2;
					////setTrackbarPos("Kalibruj", "Oryginalny", 2);
					cout << "kalibruj wybieranie" << endl;
				}

				// jesli kliknieto przycisk "rysuj lub kalibruj"
				////x >= drawAndSelectOptRightEdge * 2
				else if (x >= selectCalibOptRightEdge * 2)
				{
					switchCalibration = 1;
					sizeRatio = 1;
					////setTrackbarPos("Kalibruj", "Oryginalny", 1);
					cout << "rysuj lub wybieraj" << endl;
				}
			}
		}
	}

	// gdy myszka jest na wysokosci przycisku "zapisz"
	else if (y >= menuY + getMenuOptY() * 2 + getMenuOptY() / 2 && y <= menuY + getMenuOptY() * 3 + getMenuOptY() / 3 && x >= getMenuOptX() * 2.35 && x <= getMenuOptX() * 3)
	////else if (y >= menuY + getMenuOptY() && y <= menuY + (getMenuOptY() * 2) && x >= (getMenuOptX() * /*1.5*/ 2.5) - 15 && x <= (getMenuOptX() * 2.5) + 35)
	{
		// zmien ikone kursora myszki na zwykla (strzalka)
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));

		if (event == 1)
		{
			// zmien ikone kursora myszki na reke (dlon)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
			
			// umozliw zapisanie obrazka
			canSaveImage = true;
		}
	}
	
	// gdy myszka jest na wysokosci przycisku uruchamiajacego kalibracje
	//// y >= getMenuOptY() * 2 && y <= getMenuOptY() * 3 && x <= getMenuOptX() * 2 + 10
	else if (y >= menuY + (getMenuOptY() * 4) + (getMenuOptY() / 2)/* && y <= getMenuOptY() * 3*/ && x <= getMenuOptX() * 2 + 10)
	{
		// zmien ikone kursora myszki na zwykla (strzalka)
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));

		if (event == 1)
		{
			// zmien ikone kursora myszki na reke (dlon)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
			
			// umozliw rozpoczecie kalibracji
			startCalibration = true;
			cout << "Rozpocznij kalibracje" << endl;
		}
	}

	// gdy myszka jest na wysokosci przyciskow regulujacych rozmiar okienka kalibracji
	else if (y >= menuY + (getMenuOptY() * 3) + (getMenuOptY() / 4) && y <= menuY + (getMenuOptY() * 3) + (getMenuOptY() / 4)+ getMenuOptY())
	{
		// przycisk - rozmiar okienka 1
		//// x >= getMenuOptX() / 4 && x <= getMenuOptX() / 4 + getMenuOptX() / 2
		if (x >= getMenuOptX() / 8 && x <= getMenuOptX() / 8 + getMenuOptX() / 2)	
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));

			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				calibSize = 1;
				changedCalibSize = false;
				cout << "rozmiar okienka 1" << endl;
			}
		}

		// przycisk - rozmiar okienka 2
		//// x >= getMenuOptX() + getMenuOptX() / 4 && x <= getMenuOptX() * 2 - getMenuOptX() / 4
		else if (x >= (getMenuOptX() * 3) / 4 && x <= (getMenuOptX() * 3) / 4 + getMenuOptX() / 2)
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));
			
			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				calibSize = 2;
				changedCalibSize = false;
				cout << "rozmiar okienka 2" << endl;
			}
		}

		// przycisk - rozmiar okienka 3
		//// x >= getMenuOptX() * 2 + getMenuOptX() / 2 - getMenuOptX() / 12 && x <= getMenuOptX() * 3 - getMenuOptX() / 10
		else if (x >= (getMenuOptX() / 8) * 3 + (getMenuOptX() / 2) * 2 && x <=(getMenuOptX() / 8) * 3 + (getMenuOptX() / 2) * 3)
		{
			// zmien ikone kursora myszki na zwykla (strzalka)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32512)));
			
			if (event == 1)
			{
				// zmien ikone kursora myszki na reke (dlon)
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
				calibSize = 3;
				changedCalibSize = false;
				cout << "rozmiar okienka 3" << endl;
			}
		}

	}
}

/*bool*/ /*Mat*/ void notification(string type, int x, int y)
{
	string mainTxt;

	int canvasX = x - imgOriginal.cols / 5,
		canvasY = y - imgOriginal.rows / 2.5;

	if (type == "clear")
	{
		mainTxt = " Zapisac obraz przed wyczyszczeniem?";
	}
	else if (type == "exit")
	{
		mainTxt = " Zapisac obraz przed wyjsciem?";
	}
	
	Mat clearImgCanvas = Mat::zeros(70, 400, CV_8UC3),
		confirmClearAndSave = Mat::zeros(30, 80, CV_8UC3),
		confirmClearNoSave = Mat::zeros(30, 80, CV_8UC3),
		cancelClear = Mat::zeros(30, 90, CV_8UC3);

	rectangle(clearImgCanvas, Point(0, 0), Point(clearImgCanvas.cols, clearImgCanvas.rows), Scalar(0, 220, 255), -1, 4);
	rectangle(confirmClearAndSave, Point(0, 0), Point(confirmClearAndSave.cols, confirmClearAndSave.rows), Scalar(0, 255, 0), -1, 4);
	rectangle(confirmClearNoSave, Point(0, 0), Point(confirmClearNoSave.cols, confirmClearNoSave.rows), Scalar(255, 0, 0), -1, 4);
	rectangle(cancelClear, Point(0, 0), Point(cancelClear.cols, cancelClear.rows), Scalar(0, 0, 255), -1, 4);
			
	putText(clearImgCanvas, mainTxt, cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
	putText(confirmClearAndSave, "  Tak", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
	putText(confirmClearNoSave, "  Nie", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
	putText(cancelClear, " Anuluj", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);

	//cout << "Canvas X / Y: " << canvasX << '/' << canvasY << endl;

	if (/*x != NULL && y != NULL*/ (canvasX >= 0 && canvasX <= /*400*/clearImgCanvas.cols) && (canvasY >= 0 && canvasY <= /*70*/clearImgCanvas.rows))
	{
		// Oznacz srodek znacznika okregiem, aby moc latwiej trafic w przyciski na komunikacie
		circle(clearImgCanvas, Point ((int)canvasX, (int)canvasY), 5, Scalar(0, 0, 0), 2, 8, 0);
	}

	confirmClearAndSave.copyTo(clearImgCanvas(cv::Rect(clearImgCanvas.cols / 8, clearImgCanvas.rows / 2, confirmClearAndSave.cols, confirmClearAndSave.rows)));
	confirmClearNoSave.copyTo(clearImgCanvas(cv::Rect(clearImgCanvas.cols / 8 + confirmClearAndSave.cols + 20, clearImgCanvas.rows / 2, confirmClearNoSave.cols, confirmClearNoSave.rows)));
	cancelClear.copyTo(clearImgCanvas(cv::Rect(clearImgCanvas.cols / 8 + confirmClearAndSave.cols + confirmClearNoSave.cols + 40, clearImgCanvas.rows / 2, cancelClear.cols, cancelClear.rows)));
	clearImgCanvas.copyTo(imgOriginal(cv::Rect(imgOriginal.cols / 5, imgOriginal.rows / 2.5, clearImgCanvas.cols, clearImgCanvas.rows)));

	////return clearImgCanvas;
	////return true;
}

void saveTheImage(Mat image)
{
	time_t now = time(0); 
	char buf[80]; 
				
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);

	strftime(buf, sizeof(buf), "%Y-%m-%d_%H.%M.%S", &timeinfo);

	string date = buf;

	////cout << result << endl;
				
	cout << "Zapisano obrazek " << date << endl;
	imwrite("Obrazek " + date + " .png", image);
	putText( imgOriginal, "Zapisano obrazek", cvPoint (10, 100),FONT_HERSHEY_SIMPLEX, 1.1, Scalar(0,255,255) ,2);
}

//// Pomiar czasu
clock_t start, stop, middleTime;
double gestureTime;
////

int _tmain(int argc, char** argv[])
{
	//////////////////////////
	//// zmienne typu STRING

	string pointerInfo,
		notificationType = "";
	
	///////////////////////
	//// zmienne typu INT

		
		// single mins/maxs
	int	minimumHue = 0,
		maximumHue = 0,
		minimumSaturation = 0,
		maximumSaturation = 0,
		minimumValue = 0,
		maximumValue = 0,

		// total mins/maxs
		totalMinHue = 179,
		totalMaxHue = 0,
		totalMinSaturation = 255,
		totalMaxSaturation = 0,
		totalMinValue = 255,
		totalMaxValue = 0,

		hsvArrIterator = 0,

		sumMinHue = 0,
		sumMaxHue = 0,
		sumMinSaturation = 0,
		sumMaxSaturation = 0,
		sumMinValue = 0,
		sumMaxValue = 0,

		// dla paskow
		dCalcMinH = 0,
		dCalcMaxH = 179,
		dCalcMinS = 0,
		dCalcMaxS = 255,
		dCalcMinV = 0,
		dCalcMaxV = 255,
		
		sCalcMinH = 0,
		sCalcMaxH = 179,
		sCalcMinS = 0,
		sCalcMaxS = 255,
		sCalcMinV = 0,
		sCalcMaxV = 255,
	
		//// Poprzednia wartosc Hue dla petli
		prevH = 0,

		// Zmienne do kolorow HSV dla rysowania
		dLowH = 0, //0;
		dHighH = 179, //179;	
		dLowS = 0, //0;
		dHighS = 255, //255;
		dLowV = 0, //120; //0;
		dHighV = 255, //255;

		// Zmienne do kolorow HSV dla zaznaczania
		sLowH = 0, //0;
		sHighH = 179, //179;	
		sLowS = 0, //0;
		sHighS = 255, //255;
		sLowV = 0, //120; //0;
		sHighV = 255, //255;
		
		// poczatkowa grubosc linii
		grub = 2,

		// menu konfiguracyjne
		configY = getConfigY(),

		// Obrazki pod przyciski dla trybow kalibracji
		menuX = getMenuX() /*505*/,
		menuY = getMenuY() /*260*/,
		
		pFromX = (640 / 2) - 25 /*135*/ /*95*/,
		pFromY = 155 /*100*/,
		pToX =  (640 / 2) + 25 /*185*/ /*170*/,
		pToY = 205 /*210*/,
	
		// Licznik paskow kalibracji
		calibrationbarCounter = 0,
		////switchCalibration = getSwitchCalibration() /*1*/,
		choosePointer = 1,
		gestureTime = 0,

		switchedInstruction = 1;
	
	//////////////////////////
	//// zmienne typu DOUBLE

	double	posX = -1,
			posY = -1,
			menuOptX = getMenuOptX() /*menuX / 3.15 /*160*/,
			menuOptY = getMenuOptY() /*menuY / 8.7 /*30*/,
			iLastX = -1,
			iLastY = -1,

			iLastXX = -1,
			iLastYY = -1;
	////////////////////////
	//// zmienne typu BOOL

	bool canDraw = false,
		canSelect = false,
		couldntDraw = false,

		gotMaxLeftRectangle = false,	
		showedMax = false,
		clockStarted = false,
		clockStopped = false,
		noClock = false,
		repeatCalibration = false,
		
		notificationShowed = false;

	//////////////////////////
	//// zmienne typu Scalar
	
	Scalar drawColor = Scalar(0,0,255),
		pointerInfoColor;

	///////////////////////
	//// zmienne typu MAT

		// Znaczniki: rysowanie, wybor
	Mat drawPointer,
		selectPointer,
		testImage = imread("testowyKolor.png"),

		dLowHsv,
		dHighHsv,
		dCentralHsv,
		
		sLowHsv,
		sHighHsv,
		sCentralHsv,
	
		hsvColorsCanvas = Mat::zeros( 240, 160, CV_8UC3 ),
		cDLowHsv = Mat::zeros(75, 75, CV_8UC3),
		cDCentralHsv = Mat::zeros(75, 75, CV_8UC3),
		cDHighHsv = Mat::zeros(75, 75, CV_8UC3),
		cSLowHsv = Mat::zeros(75, 75, CV_8UC3),
		cSCentralHsv = Mat::zeros(75, 75, CV_8UC3),
		cSHighHsv = Mat::zeros(75, 75, CV_8UC3),

		
		// nowe okno instrukcji
		////instruction = Mat::zeros(250, 500, CV_8UC3),

		// cale okno konfiguracji
		configurationCanvas =  Mat::zeros(/*800*/ configY, menuX, CV_8UC3),

		modesCanvas = Mat::zeros (menuY , menuX, CV_8UC3 ),
		drawCalibration = Mat::zeros( (int)menuOptY, (int)menuOptX, CV_8UC3 ),
		drawAndSelect = Mat::zeros( (int)menuOptY, (int)menuOptX, CV_8UC3 ),
		selectCalibration = Mat::zeros( (int)menuOptY,(int) menuOptX, CV_8UC3 ),		
		
		// przycisk uruchomienia kalibracji
		runCalibration = Mat::zeros( (int)menuOptY, (int)menuOptX * 2 + 10, CV_8UC3 ),
		
		// regulacja rozmiaru okienka kalibracji
		littleCalibrationSpot = Mat::zeros( (int)menuOptY, (int)menuOptX / 2, CV_8UC3 ),
		middleCalibrationSpot = Mat::zeros( (int)menuOptY, (int)menuOptX / 2, CV_8UC3 ),
		bigCalibrationSpot = Mat::zeros( (int)menuOptY, (int)menuOptX / 2, CV_8UC3 ),

		// separatory sekcji: kalibracja, rysowanie/wybieranie 
		verticalSeparator = Mat::zeros(menuY, 8, CV_8UC3),
		horizontalSeparator = Mat::zeros(16, menuX, CV_8UC3),

		// opcje
		optionsCanvas = Mat::zeros(/*40*/ getConfigY() - menuY - horizontalSeparator.rows, /*imgOriginal.cols*/ menuX, CV_8UC3),
		switchFps = Mat::zeros(/*20*/ menuOptY, /*50*/ (menuOptX / 3) * 2, CV_8UC3),
		switchFlip = Mat::zeros(/*40*/ menuOptY, (menuOptX / 4) * 3, CV_8UC3),
		
		saveImage = Mat::zeros( (int) (menuOptY / 5) * 4 , (int)((menuOptX / 3) * 2), CV_8UC3 ),

		imgColorPanel0,
		imgColorPanel,

		imgTmp,
		imgLines;
	

	// 0 - dla normalnej kamerki; 1 - dla telefonu
	VideoCapture cap(0); 

    // Sprawdzenie, czy kamerka jest podlaczona
	if ( !cap.isOpened() )  
    {
         cout << "Brak dostepu do kamerki" << endl;
         return -1;
    }

	 	
	// Barwy HSV - po kalibracji obu znacznikow
	namedWindow("Ustawione barwy HSV", CV_WINDOW_AUTOSIZE);

	// tlo dla calego okna konfiguracji
	rectangle(configurationCanvas, Point(0, 0), Point(configurationCanvas.cols, configurationCanvas.rows), Scalar(255, 255, 255), -1, 4);
	
	// dodaj opcje do okna
	rectangle(optionsCanvas, Point(0, 0), Point(optionsCanvas.cols, optionsCanvas.rows), Scalar(120, 120, 120), -1, 4);

	// przelacz wyswietlanie FPS
	rectangle(switchFps, Point(0, 0), Point(switchFps.cols, switchFps.rows), Scalar(0, 0, 0), -1, 4);

	// przelacz obrot obrazu
	rectangle(switchFlip, Point(0, 0), Point(switchFlip.cols, switchFlip.rows), Scalar(0, 0, 0), -1, 4);

	// zmiana koloru tla "przyciskow"
	rectangle(modesCanvas, Point(0, 0), Point(modesCanvas.cols, modesCanvas.rows), Scalar(255, 255, 255), -1, 4);

	// zmiana koloru "przyciskow"
	rectangle(drawCalibration, Point(0, 0), Point(drawCalibration.cols, drawCalibration.rows), Scalar(0, 120, 0), -1, 4);
	rectangle(selectCalibration, Point(0, 0), Point(selectCalibration.cols, selectCalibration.rows), Scalar(0, 0, 120), -1, 4);
	rectangle(drawAndSelect, Point(0, 0), Point(drawAndSelect.cols, drawAndSelect.rows), Scalar(120, 0, 0), -1, 4);
	rectangle(saveImage, Point(0, 0), Point(saveImage.cols, saveImage.rows), Scalar(0, 0, 255), -1, 4);
	rectangle(runCalibration, Point(0, 0), Point(modesCanvas.cols, selectCalibration.rows * 2), Scalar(0, 255, 0), -1, 4);

	rectangle(verticalSeparator, Point(0, 0), Point(verticalSeparator.cols, verticalSeparator.rows), Scalar(0, 0, 0), -1, 4);
	
	// zmiana koloru okienek do kalibracji
	rectangle(littleCalibrationSpot, Point(0, 0), Point(littleCalibrationSpot.cols, littleCalibrationSpot.rows), Scalar(255, 0, 0), -1, 4);
	rectangle(middleCalibrationSpot, Point(0, 0), Point(middleCalibrationSpot.cols, middleCalibrationSpot.rows), Scalar(255, 0, 0), -1, 4);
	rectangle(bigCalibrationSpot, Point(0, 0), Point(bigCalibrationSpot.cols, bigCalibrationSpot.rows), Scalar(255, 0, 0), -1, 4);

	// Podpisy "przyciskow"
	putText(drawCalibration, "Kalibruj rysowanie", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
	putText(drawAndSelect, "Rysuj / Wybieraj", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
	putText(selectCalibration, "Kalibruj wybieranie", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

	putText(saveImage, "ZAPISZ Obraz", cvPoint(5, 15), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

	putText(runCalibration, "          Rozpocznij kalibracje", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

	putText(littleCalibrationSpot, "male", cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
	putText(middleCalibrationSpot, "Srednie", cvPoint(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
	putText(bigCalibrationSpot, "DUZE", cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

	putText(modesCanvas, "Rozmiar okienka kalibracji:", cvPoint(5, 85), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 1);

	putText(switchFps, "Licznik FPS", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
	putText(switchFlip, "Odwroc obraz", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

	// pokaz "przyciski" z podpisami
	drawCalibration.copyTo(modesCanvas(cv::Rect(0, 0, drawCalibration.cols, drawCalibration.rows)));
	selectCalibration.copyTo(modesCanvas(cv::Rect(drawCalibration.cols + 10, 0, selectCalibration.cols, selectCalibration.rows)));
	drawAndSelect.copyTo(modesCanvas(cv::Rect(selectCalibration.cols + selectCalibration.cols + 20, 0, drawAndSelect.cols, drawAndSelect.rows)));

	saveImage.copyTo(modesCanvas(cv::Rect(drawCalibration.cols + selectCalibration.cols + (drawAndSelect.cols / 3), drawAndSelect.rows * 2.5 /*+ ((int)drawAndSelect.rows / 2)*/, saveImage.cols, saveImage.rows)));

	runCalibration.copyTo(modesCanvas(cv::Rect(0, selectCalibration.rows * 4.5, runCalibration.cols, runCalibration.rows)));

	littleCalibrationSpot.copyTo(modesCanvas(cv::Rect( 20, /*selectCalibration.rows * 2.5*/ selectCalibration.rows * 3 + selectCalibration.rows / 4, littleCalibrationSpot.cols, littleCalibrationSpot.rows)));
	middleCalibrationSpot.copyTo(modesCanvas(cv::Rect(littleCalibrationSpot.cols + 40, selectCalibration.rows * 3 + selectCalibration.rows / 4, middleCalibrationSpot.cols, middleCalibrationSpot.rows)));
	bigCalibrationSpot.copyTo(modesCanvas(cv::Rect(littleCalibrationSpot.cols + middleCalibrationSpot.cols + 60, selectCalibration.rows * 3 + selectCalibration.rows / 4, bigCalibrationSpot.cols, bigCalibrationSpot.rows)));

	switchFps.copyTo(optionsCanvas(cv::Rect(menuOptX * /*0.25*/ 0.85, 0, switchFps.cols, switchFps.rows)));
	switchFlip.copyTo(optionsCanvas(cv::Rect(menuOptX * /*1.75*/ 1.6, 0, switchFlip.cols, switchFlip.rows)));
		
	verticalSeparator.copyTo(modesCanvas(cv::Rect(runCalibration.cols + 1, 0, verticalSeparator.cols, verticalSeparator.rows)));
	horizontalSeparator.copyTo(configurationCanvas(cv::Rect(0, getConfigY() - menuY - horizontalSeparator.rows, horizontalSeparator.cols, horizontalSeparator.rows)));

	modesCanvas.copyTo(configurationCanvas(cv::Rect(0, configurationCanvas.rows - modesCanvas.rows, modesCanvas.cols, modesCanvas.rows)));
		
	// dodaje opcje do menu trybow
	optionsCanvas.copyTo(configurationCanvas(cv::Rect(0, 0, optionsCanvas.cols, optionsCanvas.rows)));

	// pokaz menu
	imshow("Menu", /*modesCanvas*/ configurationCanvas);	
	
	imgColorPanel = imread("malenewGUI.png" /*"newGUI.png"*/);
	

    if(!imgColorPanel.data)                              
    {
        cout <<  "Nie odnaleziono obrazka z menu" << std::endl;
		
        return -1;
    } 


	cap.read(imgTmp);

	imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );
	
	// Okno z wlacznikiem kalibracji	////
	namedWindow("Kalibracja znacznika RYS", WINDOW_AUTOSIZE);
	namedWindow("Kalibracja znacznika WYB", WINDOW_AUTOSIZE);

	// Pokaz instrukcje
	namedWindow("Instrukcja", CV_WINDOW_AUTOSIZE);

	// Suwaki do kalibracji znacznika rysowania
	createTrackbar("dLowH", "Kalibracja znacznika RYS", &dCalcMinH, 179); 
	createTrackbar("dHighH", "Kalibracja znacznika RYS", &dCalcMaxH, 179);
	createTrackbar("dLowS", "Kalibracja znacznika RYS", &dCalcMinS, 255); 
	createTrackbar("dHighS", "Kalibracja znacznika RYS", &dCalcMaxS, 255);
	createTrackbar("dLowV", "Kalibracja znacznika RYS", &dCalcMinV, 255);
	createTrackbar("dHighV", "Kalibracja znacznika RYS", &dCalcMaxV, 255);

	// Suwaki do kalibracji znacznika wyboru
	createTrackbar("sLowH", "Kalibracja znacznika WYB", &sCalcMinH, 179); 
	createTrackbar("sHighH", "Kalibracja znacznika WYB", &sCalcMaxH, 179);
	createTrackbar("sLowS", "Kalibracja znacznika WYB", &sCalcMinS, 255); 
	createTrackbar("sHighS", "Kalibracja znacznika WYB", &sCalcMaxS, 255);
	createTrackbar("sLowV", "Kalibracja znacznika WYB", &sCalcMinV, 255);
	createTrackbar("sHighV", "Kalibracja znacznika WYB", &sCalcMaxV, 255);

	

	// wykrywaj akcje myszki w Menu
	setMouseCallback("Menu", mouseClickButton, NULL);

    while (true)
    {
		//// zacznij mierzyc czas wykonania 1 klatki obrazu
		clock_t frameStart = clock();

        bool bSuccess = cap.read(imgOriginal);

        if (!bSuccess) 
        {
             cout << "nie mozna odczytac strumienia" << endl;
             ////break;
        }
		
		if (doFlip)
		{
			// Obroc obraz, gdy chcesz rysowaæ widzac siebie
			flip(imgOriginal,imgOriginal, 1);
		}
				
		Mat imgHSV, imgHSV2;

		// Zmiana przestrzeni barw?
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); 
		cvtColor(imgOriginal, imgHSV2, COLOR_BGR2HSV); 

		Mat imgThresholded, imgThresholded2;
				
		// znacznik rysowania
		inRange(imgHSV, Scalar(dCalcMinH,dCalcMinS,dCalcMinV), Scalar( dCalcMaxH,dCalcMaxS,dCalcMaxV), drawPointer); 
		// znacznik wyboru
		inRange(imgHSV2, Scalar(sCalcMinH, sCalcMinS, sCalcMinV), Scalar( sCalcMaxH, sCalcMaxS, sCalcMaxV), selectPointer);
		
		
		// ustaw obrazy binarne dla znacznikow
		imgThresholded = drawPointer;
		imgThresholded2 = selectPointer;


		//erozja i dylatacja tresholdu  >>>>>>>>>>>>>>>>>> zamiast 5, ustawic 3 i wtedy wiecej FPS
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

		erode(imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
		dilate( imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

		//erozja i dylatacja znacznika rysowania
		erode(drawPointer, drawPointer, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
		dilate( drawPointer, drawPointer, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
		
		//erozja i dylatacja znacznika wybierania
		erode(selectPointer, selectPointer, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
		dilate( selectPointer, selectPointer, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

		
		// Kalibracja
		if (switchCalibration != 1)
		{
			couldntDraw = true;
			canDraw = false;
			canSelect = false;

			if (switchCalibration == 0)
			{
				pointerInfo = "Znacznik RYSOWANIA";
				pointerInfoColor = Scalar(0, 255, 0);

				if (switchedInstruction != switchCalibration)
				{
					destroyWindow("Instrukcja");
					switchedInstruction = switchCalibration;
				}

				namedWindow("Instrukcja", CV_WINDOW_AUTOSIZE);
				Mat instruction = Mat::zeros(250, 500, CV_8UC3);			
				putText(instruction, "1. Umiesc znacznik w ZIELONYM polu", cvPoint(0, 30), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);				
				putText(instruction, "2. Wybierz odpowiednia wielkosc dla okienka", cvPoint(0, 60), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "   kalibracji (zmiana wielkosci ZIELONEGO pola)", cvPoint(0, 90), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "3. Uruchom kalibracje, aby ustawic kolor dla", cvPoint(0, 120), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "   znacznika RYSOWANIA", cvPoint(0, 150), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				

				imshow("Instrukcja", instruction);
			}

			else if (switchCalibration == 2)
			{
				pointerInfo = "Znacznik WYBIERANIA";
				pointerInfoColor = Scalar(0, 0, 255);

				if (switchedInstruction != switchCalibration)
				{
					destroyWindow("Instrukcja");
					switchedInstruction = switchCalibration;
				}

				namedWindow("Instrukcja", CV_WINDOW_AUTOSIZE);
				Mat instruction = Mat::zeros(250, 500, CV_8UC3);
				putText(instruction, "1. Umiesc znacznik w CZERWONYM polu", cvPoint(0, 30), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "2. Wybierz odpowiednia wielkosc dla okienka", cvPoint(0, 60), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, " kalibracji (zmiana wielkosci CZERWONEGO pola)", cvPoint(0, 90), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "3. Uruchom kalibracje, aby ustawic kolor dla", cvPoint(0, 120), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				putText(instruction, "   znacznika WYBIERANIA", cvPoint(0, 150), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
				imshow("Instrukcja", instruction);
			}
			
			if (calibSize == 1 && changedCalibSize == false)
			{
				cout << "maly znacznik" << endl;

				pFromX = (imgOriginal.size().width / 2) - 13,
				pFromY = 167,
				pToX = (imgOriginal.size().width / 2) + 13,
				pToY = 193;

				changedCalibSize = true;
			}
		
			else if (calibSize == 2 && changedCalibSize == false)
			{
				cout << "Sredni Znacznik" << endl;
				
				pFromX = (imgOriginal.size().width / 2) - 25,
				pFromY = 155,
				pToX = (imgOriginal.size().width / 2) + 25,
				pToY = 205;

				changedCalibSize = true;
			}
			
			else if (calibSize == 3 && changedCalibSize == false)
			{
				cout << "DUZY ZNACZNIK" << endl;
				
				pFromX = (imgOriginal.size().width / 2) - 38,
				pFromY = 142,
				pToX = (imgOriginal.size().width / 2) + 38,
				pToY = 218;

				changedCalibSize = true;
			}
			
			// Wykryj kolor w ROI
			else if (startCalibration == true)
			{
				cout << "KLIK" <<endl;

				int dCentralH,
					dCentralS,
					dCentralV,
					sCentralH,
					sCentralS,
					sCentralV;
				
				int * allArr,
					averageMinHue = 0,
					averageMaxHue = 0,
					averageMinSaturation = 0,
					averageMaxSaturation = 0,
					averageMinValue = 0,
					averageMaxValue = 0;

				if (repeatCalibration == true)
				{
					hsvArrIterator = 0,

					sumMinHue = 0,
					sumMaxHue = 0,
					sumMinSaturation = 0,
					sumMaxSaturation = 0,
					sumMinValue = 0,
					sumMaxValue = 0;

					totalMinHue = 179,
					totalMaxHue = 0,
					totalMinSaturation = 255,
					totalMaxSaturation = 0,
					totalMinValue = 255,
					totalMaxValue = 0;

					repeatCalibration = false;
				}

			
				int pointX =  (pToX - pFromX) / 2;
				int pointY = (pToY - pFromY) / 2;
				
				allArr = detectColor(pointX, pointY, pFromX, pToX, pFromY, pToY);

				for (int i = 0; i < 6; i++)
				{
					cout << "Wykryte wartosci HSV: " << allArr[i] << endl;
				}


				minimumHue = allArr[0];
				maximumHue = allArr[1];
				minimumSaturation = allArr[2];
				maximumSaturation = allArr[3];
				minimumValue = allArr[4];
				maximumValue = allArr[5];

				cout << "------------------------" << endl;


				if (totalMinHue > minimumHue)
					totalMinHue = minimumHue;
				
				if (totalMaxHue < maximumHue)
					totalMaxHue = maximumHue;
				
				if (totalMinSaturation > minimumSaturation)
					totalMinSaturation = minimumSaturation;
				
				if (totalMaxSaturation < maximumSaturation)
					totalMaxSaturation = maximumSaturation;
				
				if (totalMinValue > minimumValue)
					totalMinValue = minimumValue;

				if (totalMaxValue < maximumValue)
					totalMaxValue = maximumValue;

						
				// srednia wartosci
				sumMinHue += minimumHue;
				sumMaxHue += maximumHue;
				sumMinSaturation += minimumSaturation;
				sumMaxSaturation += maximumSaturation;
				sumMinValue += minimumValue;
				sumMaxValue += maximumValue;
						
				averageMinHue = sumMinHue / (hsvArrIterator + 1);
				averageMaxHue = sumMaxHue / (hsvArrIterator + 1);
				averageMinSaturation = sumMinSaturation / (hsvArrIterator + 1);
				averageMaxSaturation = sumMaxSaturation / (hsvArrIterator + 1);
				averageMinValue = sumMinValue / (hsvArrIterator + 1);
				averageMaxValue = sumMaxValue / (hsvArrIterator + 1);


				// totalne minimalne/maksymalne
				if (switchCalibration == 0)
				{
					
					dCalcMinH = totalMinHue;
					dCalcMaxH = totalMaxHue;
					dCalcMinS = totalMinSaturation;
					dCalcMaxS = totalMaxSaturation;
					dCalcMinV = totalMinValue;
					dCalcMaxV = totalMaxValue;

					
					cvtColor(cDLowHsv, dLowHsv, CV_BGR2HSV);
					cvtColor(cDCentralHsv, dCentralHsv, CV_BGR2HSV);
					cvtColor(cDHighHsv, dHighHsv, CV_BGR2HSV);


					dCentralH = (dCalcMaxH - dCalcMinH) / 2,
					dCentralS = (dCalcMaxS - dCalcMinS) / 2, 
					dCentralV = (dCalcMaxV - dCalcMinV) / 2;

					rectangle(dLowHsv, Point(0, 0), Point(75, 75), Scalar(dCalcMinH, dCalcMinS, dCalcMinV), -1, 4);
					rectangle(dHighHsv, Point(0, 0), Point(75, 75), Scalar(dCalcMaxH, dCalcMaxS, dCalcMaxV), -1, 4);
					rectangle(dCentralHsv, Point(0, 0), Point(75, 75), Scalar(dCentralH, dCentralS, dCentralV), -1, 4);

					cout << (hsvArrIterator + 1) << ">> [RYS] Ustawione wartiosci mini HSV: " << dCalcMinH << '/' << dCalcMinS << '/' << dCalcMinV << endl;
					cout << (hsvArrIterator + 1) << ">> [RYS] Ustawione wartiosci MAXI HSV: " << dCalcMaxH << '/' << dCalcMaxS << '/' << dCalcMaxV << endl;

					setTrackbarPos("dLowH", "Kalibracja znacznika RYS", dCalcMinH);
					setTrackbarPos("dHighH", "Kalibracja znacznika RYS", dCalcMaxH);
					setTrackbarPos("dLowS", "Kalibracja znacznika RYS", dCalcMinS);
					setTrackbarPos("dHighS", "Kalibracja znacznika RYS", dCalcMaxS);
					setTrackbarPos("dLowV", "Kalibracja znacznika RYS", dCalcMinV);
					setTrackbarPos("dHighV", "Kalibracja znacznika RYS", dCalcMaxV);					
				
					dLowHsv.copyTo(hsvColorsCanvas(cv::Rect(0, 0, dLowHsv.cols, dLowHsv.rows)));
					dCentralHsv.copyTo(hsvColorsCanvas(cv::Rect(0, 80, dCentralHsv.cols, dCentralHsv.rows)));
					dHighHsv.copyTo(hsvColorsCanvas(cv::Rect(0, 160, dHighHsv.cols, dHighHsv.rows)));

					imshow("Ustawione barwy HSV", hsvColorsCanvas);
					
				}
				else if (switchCalibration == 2)
				{
					
					sCalcMinH = totalMinHue;
					sCalcMaxH = totalMaxHue;
					sCalcMinS = totalMinSaturation;
					sCalcMaxS = totalMaxSaturation;
					sCalcMinV = totalMinValue;
					sCalcMaxV = totalMaxValue;

					cvtColor(cSLowHsv, sLowHsv, CV_BGR2HSV);
					cvtColor(cSCentralHsv, sCentralHsv, CV_BGR2HSV);
					cvtColor(cSHighHsv, sHighHsv, CV_BGR2HSV);

					sCentralH = (sCalcMaxH - sCalcMinH) / 2,
					sCentralS = (sCalcMaxS - sCalcMinS) / 2, 
					sCentralV = (sCalcMaxV - sCalcMinV) / 2;

					rectangle(sLowHsv, Point(0, 0), Point(75, 75), Scalar(sCalcMinH, sCalcMinS, sCalcMinV), -1, 4);
					rectangle(sHighHsv, Point(0, 0), Point(75, 75), Scalar(sCalcMaxH, sCalcMaxS, sCalcMaxV), -1, 4);
					rectangle(sCentralHsv, Point(0, 0), Point(75, 75), Scalar(sCentralH, sCentralS, sCentralV), -1, 4);

					cout << (hsvArrIterator + 1) << ">> [WYB] Ustawione wartiosci mini HSV: " << sCalcMinH << '/' << sCalcMinS << '/' << sCalcMinV << endl;
					cout << (hsvArrIterator + 1) << ">> [WYB] Ustawione wartiosci MAXI HSV: " << sCalcMaxH << '/' << sCalcMaxS << '/' << sCalcMaxV << endl;
					
					setTrackbarPos("sLowH", "Kalibracja znacznika WYB", sCalcMinH);
					setTrackbarPos("sHighH", "Kalibracja znacznika WYB", sCalcMaxH);
					setTrackbarPos("sLowS", "Kalibracja znacznika WYB", sCalcMinS);
					setTrackbarPos("sHighS", "Kalibracja znacznika WYB", sCalcMaxS);
					setTrackbarPos("sLowV", "Kalibracja znacznika WYB", sCalcMinV);
					setTrackbarPos("sHighV", "Kalibracja znacznika WYB", sCalcMaxV);

					sLowHsv.copyTo(hsvColorsCanvas(cv::Rect(85, 0, sLowHsv.cols, sLowHsv.rows)));
					sCentralHsv.copyTo(hsvColorsCanvas(cv::Rect(85, 80, sCentralHsv.cols, sCentralHsv.rows)));
					sHighHsv.copyTo(hsvColorsCanvas(cv::Rect(85, 160, sHighHsv.cols, sHighHsv.rows)));

					imshow("Ustawione barwy HSV", hsvColorsCanvas);	
				}

				
				cout << endl << "------------------------" << endl;

				if (hsvArrIterator == 3)
				{
	
					cout << '[' << (hsvArrIterator + 1) << ']' << " ZAKONCZONO KALIBRACJE! \n Ostateczne wartosci H/S/V: " << endl;
					cout << "[WYB] mini HSV: " << sCalcMinH << '/' << sCalcMinS << '/' << sCalcMinV << endl;
					cout << "[WYB] MAXI HSV: " << sCalcMaxH << '/' << sCalcMaxS << '/' << sCalcMaxV << endl;
					
					setTrackbarPos("dLowH", "Kalibracja znacznika RYS", dCalcMinH);
					setTrackbarPos("dHighH", "Kalibracja znacznika RYS", dCalcMaxH);
					setTrackbarPos("dLowS", "Kalibracja znacznika RYS", dCalcMinS);
					setTrackbarPos("dHighS", "Kalibracja znacznika RYS", dCalcMaxS);
					setTrackbarPos("dLowV", "Kalibracja znacznika RYS", dCalcMinV);
					setTrackbarPos("dHighV", "Kalibracja znacznika RYS", dCalcMaxV);
					
					setTrackbarPos("sLowH", "Kalibracja znacznika WYB", sCalcMinH);
					setTrackbarPos("sHighH", "Kalibracja znacznika WYB", sCalcMaxH);
					setTrackbarPos("sLowS", "Kalibracja znacznika WYB", sCalcMinS);
					setTrackbarPos("sHighS", "Kalibracja znacznika WYB", sCalcMaxS);
					setTrackbarPos("sLowV", "Kalibracja znacznika WYB", sCalcMinV);
					setTrackbarPos("sHighV", "Kalibracja znacznika WYB", sCalcMaxV);

					repeatCalibration = true;
				}
				
				hsvArrIterator++;
			}

			// Mniejsze ROI do umieszczenia w nim znacznika + napis informujacy, ktory znacznik trzeba tam umiescic
			putText( imgOriginal, pointerInfo, cvPoint(185, 250),FONT_HERSHEY_SIMPLEX, 0.8, pointerInfoColor, 2);
			rectangle(imgOriginal, Point( pFromX, pFromY ), Point( pToX, pToY), pointerInfoColor, +5, 4 );
			
			if (switchCalibration == 0)
				rectangle(imgThresholded, Point( pFromX, pFromY ), Point( pToX, pToY ), Scalar( 255, 255, 255 ), +5, 4 );
			
			else if (switchCalibration == 2)
				rectangle(imgThresholded2, Point( pFromX, pFromY ), Point( pToX, pToY ), Scalar( 255, 255, 255 ), +5, 4 );
			
			startCalibration = false;
		}
		
		else if (switchCalibration == 1)
		{
			if(canSaveImage == true)
			{
				saveTheImage(imgLines);
				canSaveImage = false;
			}
			
			if (countNonZero(drawPointer) < 10 && countNonZero(selectPointer) < 10)
			{
				cout << "BRAK ZNACZNIKOW" << endl;

				canDraw = false;
				canSelect = false;

				if (clockStopped == true && noClock == true)
				{
					noClock = false;
				
					start = 0;
					middleTime = 0;
					clockStarted = false;
				}

				couldntDraw = true;
			}

			else if (countNonZero(drawPointer) < 10 && countNonZero(selectPointer) > 10)
			{
				canDraw = false;
				canSelect = true;
			
				imgColorPanel.copyTo(imgOriginal(cv::Rect(0,0,imgColorPanel.cols, imgColorPanel.rows)));

				if (clockStopped == true && noClock == true)
				{
					noClock = false;
					start = 0;
					middleTime = 0;
					clockStarted = false;
				}

				couldntDraw = true;
			}
		
			else if (countNonZero(drawPointer) > 10 && countNonZero(selectPointer) < 10)
			{
				canDraw = true;
				canSelect = false;

				if (clockStopped == true && noClock == true)
				{
					noClock = false;
				
					start = 0;
					middleTime = 0;
					clockStarted = false;
				}
			}

			else if (countNonZero(drawPointer) > 10 && countNonZero(selectPointer) > 10)
			{
			
				if (clockStarted == false)
				{
					start = clock();
					middleTime = start;
					clockStarted = true;

					noClock = true;
				}
			
				canDraw = false;
				canSelect = false;

				if (clockStarted == true)
				{
					middleTime = clock();
				}
			
				// po uplywie 2 sekund od momentu obecnosci 2 znacznikow w polu kamery - pokaz menu
				if (clockStarted == true && ((double)(middleTime - start) / CLOCKS_PER_SEC) > 2)
				{
					stop = clock();
					gestureTime = (stop - start) / CLOCKS_PER_SEC;
			
					// Dodaj panel manu na gorze okna
					imgColorPanel.copyTo(imgOriginal(cv::Rect(0,0,imgColorPanel.cols, imgColorPanel.rows)));

					clockStopped = true;

					canSelect = true;
				}

				couldntDraw = true;
			}

			if (switchedInstruction != switchCalibration)
			{
				destroyWindow("Instrukcja");
				switchedInstruction = switchCalibration;
			}

			namedWindow("Instrukcja", CV_WINDOW_AUTOSIZE);
			Mat instruction = Mat::zeros(250, 500, CV_8UC3);			
			putText(instruction, " - znacznik RYSOWANIA sluzy do rysowania", cvPoint(0, 30), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, " - znacznik WYBIERANIA sluzy do wyboru opcji:", cvPoint(0, 60), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, "   1. Koloru linii", cvPoint(0, 90), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, "   2. Grubosci linii", cvPoint(0, 120), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, "   3. Wyczyszczenia obrazu", cvPoint(0, 150), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, "   4. Wyjscia z programu", cvPoint(0, 180), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			putText(instruction, " - przycisk ZAPISZ sluzy do zapisania rysunku", cvPoint(0, 210), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(255, 255, 255), 1);
			imshow("Instrukcja", instruction);
		}

		//momenty znacznika ZAZNACZANIA
		Moments sMoments = moments(selectPointer);

		double sM01 = sMoments.m01;
		double sM10 = sMoments.m10;
		double sArea = sMoments.m00;

		//momenty znacznika RYSOWANIA
		Moments dMoments = moments(drawPointer);

		double dM01 = dMoments.m01;
		double dM10 = dMoments.m10;
		double dArea = dMoments.m00;

			
		if (sArea > 10000 && canSelect)
		{
			posX = sM10 / sArea;
			posY = sM01 / sArea;        
        
			CvPoint cvpoint = cvPoint(180, 125); 
		
			// Oznacz srodek znacznika okregiem (aby moc latwiej trafic w przyciski w menu)
			circle(imgOriginal, Point ((int)posX, (int)posY), 5, Scalar(255, 255, 255), 2, 8, 0);

			////cout << "x / y: " << posX << '/' << posY << endl;

			if(posX  > 406 && posX < 472 && posY > 0 && posY < 55 /*&&/ canSelect*/)  
			{
				drawColor = Scalar(0, 0, 255);
				putText( imgOriginal, "Kolor CZERWONY", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, drawColor, 3);
			}

			else if(posX  > 488 && posX < 555 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				drawColor = Scalar(0, 255, 0);
				putText( imgOriginal, "Kolor ZIELONY", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, drawColor, 3);
			}

			else if(posX  > 571 && posX < 636 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				drawColor = Scalar(255, 0, 0);
				putText( imgOriginal, "Kolor NIEBIESKI", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, drawColor, 3);
			}
			
			else if(posX  > 333 && posX < 364 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				grub=5;
				putText( imgOriginal, "Grubosc 5", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255), 3);
			}
			
			else if(posX  > 284 && posX < 310 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				grub=4;
				putText( imgOriginal, "Grubosc 4", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255), 3);
			}
			
			else if(posX  > 240 && posX < 261 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				grub=3;
				putText( imgOriginal, "Grubosc 3", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255), 3);
			}
			
			else if(posX  > 201 && posX < 217 && posY > 0 && posY < 55 /*&& canSelect*/) 
			{
				grub=2;
				putText( imgOriginal, "Grubosc 2", cvpoint,FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255) ,3);
			}

			else if (posX > 65 && posX < 125 && posY > 0 && posY < 55)
			{
				cout << "Wyczysc obraz." << endl;
				putText( imgOriginal, "WYCZYSC obraz", cvPoint (30, 125),FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255) ,3);

				// Wyczysc obraz
				notificationType = "clear";
				////notificationShowed = notification("clear");
				////imgLines = 0;
			}
			
			else if (posX > 1 && posX < 62 && posY > 0 && posY < 55)
			{
				cout << "Zakoncz program." << endl;
				
				putText( imgOriginal, "WYJDZ z programu", cvPoint (30, 125),FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,255) ,3);

				// Wyjscie z programu.
				notificationType = "exit";
				////notificationShowed = notification("exit");
				////break;
			}


			iLastX = posX;
			iLastY = posY;
		}


		else if (dArea > 10000 && canDraw && switchCalibration == 1)
		{ 
			posX = dM10 / dArea;
			posY = dM01 / dArea;

			if (couldntDraw == true)
			{
				iLastXX = posX;
				iLastYY = posY;

				couldntDraw = false;
			}
			
			if (iLastXX >= 0 && iLastYY >= 0 && posX >= 0 && posY >= 0)
			{
				// Rysuj                /TO/                 /FROM/ 
				line(imgLines, Point((int)posX, (int)posY), Point((int)iLastXX, (int)iLastYY), drawColor, grub, 4);

				// Oznacz srodek znacznika okregiem (aby moc dokladnie widziec skad rysuje)
				circle(imgOriginal, Point ((int)posX, (int)posY), 3, Scalar(255, 255, 255), 2, 8, 0);
			}

			iLastXX = posX;
			iLastYY = posY;
		}

		
		// Polacz obraz z kamerki z rysunkiem
		imgOriginal = imgOriginal + imgLines;

		double thresWidth,
			thresHeight,
			thres2Width,
			thres2Height;
	
		// zmien rozmiar obrazow binarnych (aspect ratio)
		switch (sizeRatio)
		{
			case 0 : thresWidth = (imgThresholded.cols / 2 - 9) * 1.5;
					thresHeight = (imgThresholded.rows / 2) * 1.5;
					
					thres2Width = (imgThresholded2.cols / 2 - 9) / 2;
					thres2Height = (imgThresholded2.rows / 2) / 1.5;

					break;
			case 1 : thresWidth = imgThresholded.cols / 2 - 9;
					thresHeight = imgThresholded.rows / 2;
					
					thres2Width = imgThresholded2.cols / 2 - 9;
					thres2Height = imgThresholded2.rows / 2;

					break;
			case 2 : thresWidth = (imgThresholded.cols / 2 - 9) / 2;
					thresHeight = (imgThresholded.rows / 2) / 1.5;
					
					thres2Width = (imgThresholded2.cols / 2 - 9) * 1.5;
					thres2Height = (imgThresholded2.rows / 2) * 1.5;

					break;
			default : cout << "Nieznany Aspect Ratio..." << endl;
					break;
		}
		
		if (notificationType != "")
		{
			notification(notificationType, posX, posY);

			// (czyszczenie lub wyjscie) + zapis
			if ((posX >= 180 && posX <= 255) && (posY >= 230 && posY <= 255))
			{
				// zapisz obraz przed czyszczeniem/zamknieciem
				saveTheImage(imgLines);

				if (notificationType == "clear")
					imgLines = 0;
				else if (notificationType == "exit")
					break;

				cout << "============ zapisano => " << endl;
				notificationType = "";
			}

			// (czyszczenie lub wyjscie) bez zapisu
			else if ((posX >= 280 && posX <= 355 ) && (posY >= 230 && posY <= 255))
			{
				if (notificationType == "clear")
					imgLines = 0;
				else if (notificationType == "exit")
					break;

				cout << "============ NIE zapisano =>  " << endl;
				notificationType = "";
			}

			// anulowanie
			else if ((posX >=385 && posX <= 460) && (posY >= 230 && posY <= 255))
			{
				cout << "============ ANULOWANO" << endl;
				notificationType = "";
			}
		}

		int expectedWindowHeight,
			thresholdInfoHeight = 30;
			// rozmiar ekranu
			/*scrX = GetSystemMetrics(SM_CXSCREEN),
			scrY = GetSystemMetrics(SM_CYSCREEN)*/

		if (thresWidth < thres2Height)
			expectedWindowHeight = thres2Height;
		else if (thresWidth >= thres2Height)
			expectedWindowHeight = thresHeight;

		// zmiana rozmiarow okien binarnych - w zaleznosci o trybu pracy programu (kalibracja lub rysowanie/wybieranie)
		resize(imgThresholded, imgThresholded, Size ((int)thresWidth, (int)thresHeight));
		resize(imgThresholded2, imgThresholded2, Size ((int)thres2Width, (int)thres2Height));
					
		// (niejawna) konwersja obrazow binarnych z CV_8U na CV_8UC3 - aby mozna je bylo umiescic w 1 oknie w formie czworokatow
		cvtColor(imgThresholded, imgThresholded, CV_GRAY2RGB);
		cvtColor(imgThresholded2, imgThresholded2, CV_GRAY2RGB);
	
		// zakoncz pomiar 1 klatki
		clock_t frameStop = clock();
		double frameTime = (double)(frameStop - frameStart) / CLOCKS_PER_SEC;

		int framePerSec = (int) (1 / frameTime);

		string fps = to_string (framePerSec) + " FPS";

		Mat drawThresholdInfo = Mat::zeros(thresholdInfoHeight, imgThresholded.cols, CV_8UC3),
			selectThresholdInfo = Mat::zeros(thresholdInfoHeight, imgThresholded2.cols, CV_8UC3),
			thresholdsCanvas = Mat::zeros(expectedWindowHeight + thresholdInfoHeight + 1, imgOriginal.cols - 10, CV_8UC3);

		rectangle(thresholdsCanvas, Point(0, 0), Point(thresholdsCanvas.cols, thresholdsCanvas.rows), Scalar(0, 255, 0), -1, 4);
			
		putText(drawThresholdInfo, "Obraz bin RYS:", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
		putText(selectThresholdInfo, "Obraz bin WYB:", cvPoint(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

		drawThresholdInfo.copyTo(thresholdsCanvas(cv::Rect(0, 0, drawThresholdInfo.cols, drawThresholdInfo.rows)));
		selectThresholdInfo.copyTo(thresholdsCanvas(cv::Rect(drawThresholdInfo.cols + 5, 0, selectThresholdInfo.cols, selectThresholdInfo.rows)));
		imgThresholded.copyTo(thresholdsCanvas(cv::Rect(0, drawThresholdInfo.rows + 1, imgThresholded.cols, imgThresholded.rows)));
		imgThresholded2.copyTo(thresholdsCanvas(cv::Rect(imgThresholded.cols + 5, selectThresholdInfo.rows + 1, imgThresholded2.cols, imgThresholded2.rows)));

		if (showFps)
		{
			// pokaz ilosc FPS w lewym dolnym rogu okna
			putText(imgOriginal, fps, cvPoint(10, 460), FONT_HERSHEY_SIMPLEX, 0.65, Scalar(0, 0, 255), 2);
		}

		imshow("Kamera", imgOriginal);
		imshow("Obrazy binarne", thresholdsCanvas);

		// Czekaj 1 milisekunde na wcisniecie klawisza ESCAPE
		if (waitKey(1) == 27)
		{
			cout << "nacisnieto esc" << endl;
			break; 
		}

    }
	cap.release();
	destroyAllWindows();
	return 0;
}