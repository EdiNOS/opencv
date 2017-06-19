#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap;
	CascadeClassifier detector;

	if(!detector.load("haarcascades/haarcascade_frontalface_alt.xml")) 
		cout << "No se puede abrir clasificador." << endl;

	if(!cap.open(0))
		cout << "No se puede acceder a la webcam." << endl;

	while(true)
	{	
		Mat dest, gray, imagen;

		cap >> imagen;

		cvtColor(imagen, gray, CV_BGR2GRAY);
		equalizeHist(gray, dest);

		vector<Rect> rect;
		detector.detectMultiScale(dest, rect, 1.2, 3, 0, Size(60,60));

		for(Rect rc : rect)
		{
			rectangle(imagen, 
				Point(rc.x, rc.y), 
				Point(rc.x + rc.width, rc.y + rc.height), 
				CV_RGB(0,255,0), 2);
		}

		imshow("Deteccion de rostros", imagen);	
		
		if(waitKey(1) >= 0) break;
	}

	return 1;
}