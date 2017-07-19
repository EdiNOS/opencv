#include <opencv2/opencv.hpp>

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

using namespace cv;
using namespace std;

int USB = 0;

int initSerial(){
		struct termios tty;
		struct termios tty_old;
		memset (&tty, 0, sizeof tty);

		/* Error Handling */
		if ( tcgetattr ( USB, &tty ) != 0 ) {
		   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		}

		/* Save old tty parameters */
		tty_old = tty;

		/* Set Baud Rate */
		cfsetospeed (&tty, (speed_t)B9600);
		cfsetispeed (&tty, (speed_t)B9600);

		/* Setting other Port Stuff */
		tty.c_cflag     &=  ~PARENB;            // Make 8n1
		tty.c_cflag     &=  ~CSTOPB;
		tty.c_cflag     &=  ~CSIZE;
		tty.c_cflag     |=  CS8;

		tty.c_cflag     &=  ~CRTSCTS;           // no flow control
		tty.c_cc[VMIN]   =  1;                  // read doesn't block
		tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
		tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

		/* Make raw */
		cfmakeraw(&tty);

		/* Flush Port, then applies attributes */
		tcflush( USB, TCIFLUSH );
		if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
		   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
		}
	}

	int sendData(char *cmd){
		int n_written = 0,
		    spot = 0;

		do {
		    n_written = write( USB, &cmd[spot], 1 );
		    spot += n_written;
		} while (cmd[spot-1] != '\r' && n_written > 0);
	}


int main()
{
	VideoCapture cap;
	CascadeClassifier detector;

	USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
	initSerial();

	if(!detector.load("haarcascades/haarcascade_frontalface_alt.xml")) 
		cout << "No se puede abrir clasificador." << endl;

	if(!cap.open(2))
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
				if (rc.x<260){
					sendData("A");
				}else{
					sendData("B");
				}
				if (rc.x>140){
					sendData("C");
				}else{
					sendData("D");
				}
				
		}

		imshow("Deteccion de rostros", imagen);	
		
		if(waitKey(1) >= 0) break;
	}

	return 1;
}

	