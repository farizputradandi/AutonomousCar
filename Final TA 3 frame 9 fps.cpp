//include from lib 
//u can include folder inside these directory file /usr/local/include
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <raspicam_cv.h> 

//pre installed (u dont need to install this, and this isn't included inside /usr/local/include dir)
#include <iostream>
#include <wiringPi.h>  
#include <wiringSerial.h>
#include <chrono>
#include <ctime>  
 

 


using namespace std;
using namespace cv;
using namespace raspicam;


Mat frame,original,Matrix, framePers, frameGray,frameThresh,frameEdge,frameFinal,frameFinalDuplicate,frameFinalDuplicate1;
Mat ROILane, ROILaneEnd;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result, laneEnd;
int dest;
RaspiCam_Cv Camera;  

Point2f Source[] = {Point2f(0,180),Point2f(40,130),Point2f(360,130), Point2f(400,180)};
Point2f Destination[] = {Point2f(100,0),Point2f(100,240),Point2f(300,240), Point2f(300,0)};

//to print text
stringstream textResult;


vector<int> histrogramLane;
vector<int> histrogramLaneEnd;

//machine learning var
CascadeClassifier Stop_Cascade;
CascadeClassifier Traffic_Cascade;

Mat frame_stop, RoI_Stop, gray_Stop;
Mat frame_Traffic, RoI_Traffic, gray_Traffic;

vector<Rect> Stop;
vector<Rect> Traffic;

int stopdetected=0, Trafficdetected=0;

 

void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,400 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0)); //set fps, ex: 100 will cause great number of fps

}

void Capture()
{
    Camera.grab();
    Camera.retrieve(original);  
    flip(original,original,0);
    cvtColor(original, frame, COLOR_BGR2RGB); 
    cvtColor(original, frame_stop, COLOR_BGR2RGB);
    cvtColor(original, frame_Traffic, COLOR_BGR2RGB);
}

//create point of interset
void Perspective()
{
	line(original,Source[0], Source[1], Scalar(0,0,255), 1);
	line(original,Source[1], Source[2], Scalar(0,0,255), 1);
	line(original,Source[2], Source[3], Scalar(0,0,255), 1);
	line(original,Source[3], Source[0], Scalar(0,0,255), 1); 
	
	line(original,Destination[0], Destination[1], Scalar(0,255,0), 2);
	line(original,Destination[1], Destination[2], Scalar(0,255,0), 2);
	line(original,Destination[2], Destination[3], Scalar(0,255,0), 2);
	line(original,Destination[3], Destination[0], Scalar(0,255,0), 2);
	
	
	Matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePers, Matrix, Size(400,240));
	 
}

void Threshold()
{
	cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	
	inRange(frameGray, 100, 255, frameThresh); // higher thresshold = little tolerance to detect white
	
	Canny(framePers,frameEdge, 50,300 , 3, false);
	
	
	add(frameThresh, frameEdge, frameFinal);
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);   //used in histrogram function only
	cvtColor(frameFinal, frameFinalDuplicate1, COLOR_RGB2BGR);   //used in histrogram function only
	
}

void Histrogram()
{
    histrogramLane.resize(400);
    histrogramLane.clear();
     
    //Rect r = Rect(80,140,1,100);
    //rectangle(frameFinal, r, Scalar(0,0,255), 0.5);

    //ine(frameFinal, Point2f(90, 0), Point2f(90, 240), Scalar(0, 0,255), 2);    
    //line(frameFinal, Point2f(365, 0), Point2f(365, 240), Scalar(0, 0,255), 2);    
     for(int i=0; i<400; i++)       //frame.size().width = 400
    {
	ROILane = frameFinalDuplicate(Rect(i,140,1,100));

	divide(255, ROILane, ROILane); // 255 =max possible intensity
	histrogramLane.push_back((int)(sum(ROILane)[0])); 
    }
    
    histrogramLaneEnd.resize(400);
    histrogramLaneEnd.clear();
    
     for(int i=0; i<400; i++)       //frame.size().width = 400
    {
	ROILaneEnd = frameFinalDuplicate1(Rect(i,0,1,240));
	divide(255, ROILaneEnd, ROILaneEnd); // 255 =max possible intensity
	histrogramLaneEnd.push_back((int)(sum(ROILaneEnd)[0])); 
    }
    
    laneEnd = sum(histrogramLane)[0];
    cout<<"Lane End = "<<laneEnd<<endl;      
    
   
}

void LaneFinder()
{
     
    
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin() + 170); 
    LeftLanePos = distance(histrogramLane.begin(), LeftPtr); 
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histrogramLane.begin() +230, histrogramLane.end()); 
    RightLanePos = distance(histrogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255,0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0,255,0), 2); 
}


void LaneCenter()
{
    laneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
    frameCenter = 200;
    
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 1);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 1);

    Result = laneCenter-frameCenter;
}

void stop_detection()
{ 
     
	//if(!Stop_Cascade.load("//home//pi//Desktop//c++//cascade_file//90.xml")) 
	if(!Stop_Cascade.load("//home//pi//Desktop//c++//cascade_file//stop90.xml")) 
	{
	    printf("unable to open stop cascade file");
	} 
	
	RoI_Stop = frame_stop(Rect(0,0,400,240)); 
	cvtColor(RoI_Stop, gray_Stop, COLOR_RGB2GRAY); 
	//equalizeHist(gray_Stop, gray_Stop);
	Stop_Cascade.detectMultiScale(gray_Stop, Stop);
	
	for(int i =0; i< Stop.size(); i++){
		Point P1(Stop[i].x, Stop[i].y);
		Point P2(Stop[i].x + (Stop[i].width *1.2), Stop[i].x + Stop[i].height);
		
		rectangle(RoI_Stop, P1, P2, Scalar(0,0,255),2);
		putText(RoI_Stop, "Stop Sign",P1,FONT_HERSHEY_PLAIN,1, Scalar(0,0,255),2); 
		stopdetected=1;
	}
	
}

void traffic_detection()
{ 
	//if(!Stop_Cascade.load("//home//pi//Desktop//c++//cascade_file//Stop_cascade.xml"))
	if(!Traffic_Cascade.load("//home//pi//Desktop//c++//cascade_file//traffic90.xml"))
	{
	    printf("unable to open traffic cascade file");
	} 
	 
	
	
	RoI_Traffic = frame_Traffic(Rect(0,0,400,240)); 
	cvtColor(RoI_Traffic, gray_Traffic, COLOR_RGB2GRAY);
	equalizeHist(gray_Traffic, gray_Traffic);
	Traffic_Cascade.detectMultiScale(gray_Traffic, Traffic);
	
	for(int i =0; i< Traffic.size(); i++){
		Point P1(Traffic[i].x, Traffic[i].y);
		Point P2(Traffic[i].x + (Traffic[i].width *1.2), Traffic[i].x + Traffic[i].height);
		
		rectangle(RoI_Traffic, P1, P2, Scalar(0,0,255),2);
		putText(RoI_Traffic, "Stop Sign",P1,FONT_HERSHEY_PLAIN,1, Scalar(0,0,255),2); 
		Trafficdetected=1;
	}
	
}

int main(int argc,char **argv)
{
	
//setup 
    Setup(argc, argv, Camera);
    cout<<"Connecting to camera"<<endl;
    if (!Camera.open())
    {
    cout<<"Failed to Connect"<<endl;
    }
    cout<<"Camera Id = "<<Camera.getId()<<endl;
    
    int fd,fa ;  
    if ((fd = serialOpen ("/dev/ttyUSB0", 115200)) < 0)
    {
       // fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
	return 1 ;
    }
    
    if ((fa = serialOpen ("/dev/ttyUSB1", 115200)) < 0)
    {
	//fprintf (stderr, "Unable to open serial esp8266 device: %s\n", strerror (errno)) ;
	return 1 ;
    }
    
    while(1)
    {
	auto start = std::chrono::system_clock::now();
	Capture();
	Perspective();
	Threshold();
	Histrogram();
	LaneFinder();
	LaneCenter();
	stop_detection();
	traffic_detection();
 if(serialDataAvail (fa)){
         
	dest =serialGetchar (fa);
  	cout<<"going to destination"<<dest<<endl;
	
        //send to mblock
	serialPutchar (fd, dest) ;    
	//delay(3000);
	
    }
     if(stopdetected ==1 ){
	serialPutchar (fd, 9) ;  
	cout<<"stop detected "<<endl;
	stopdetected=0;  
	delay(6000);
	 
	    
    }
    if(Trafficdetected==1){
	serialPutchar (fd, 10) ;  
	cout<<"traffic detected "<<endl; 
	Trafficdetected=0;
	delay(500);
	 
	    
    }
      
    if (Result == 0)
    { 
	serialPutchar (fd, 1) ;  
	cout<<"Forward"<<endl;
    }
    
        
    else if (Result >0 && Result <10)
    { 
	serialPutchar (fd, 2) ;  
	cout<<"left3"<<endl;
    }
    
        else if (Result >=10 && Result <20)
    { 
	serialPutchar (fd, 3) ;  
	cout<<"left2"<<endl;
    }
    
        else if (Result >20 && Result <30)
    { 
	serialPutchar (fd, 4) ;  
	cout<<"left1"<<endl;
    }
    
        else if (Result >30)
    { 
	serialPutchar (fd, 41) ;  
	cout<<"left1"<<endl;
    }
    
        else if (Result <0 && Result >-10)
    { 
	serialPutchar (fd, 5) ;  
	cout<<"right3"<<endl;
    }
    
        else if (Result <=-10 && Result >-20)
    { 
	serialPutchar (fd, 6) ;  
	cout<<"right2"<<endl;
    }
    
        else if (Result <-20 && Result >-30)
    { 
	serialPutchar (fd, 7) ;  
	cout<<"right1"<<endl;
    }
    
        else if (Result <-30)
    { 
	serialPutchar (fd, 71) ;  
	cout<<"right1"<<endl;
    }
    if (laneEnd > 10000)
    {  
	serialPutchar (fd, 8) ;  
	cout<<"lane turn"<<endl;
	delay(2000);
	
    }
    
	textResult.str(" ");
	textResult.clear();
	textResult<<"Result = "<<Result;
	putText(original, textResult.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);

//frame show

	namedWindow("original", WINDOW_KEEPRATIO);
	moveWindow("original", 0, 20);
	resizeWindow("original", 360, 240);
	imshow("original", original);
	
	namedWindow("framePers", WINDOW_KEEPRATIO);
	moveWindow("framePers", 355, 20);
	resizeWindow("framePers", 360, 240); 
	imshow("framePers", framePers);
	
	//namedWindow("frame_Traffic", WINDOW_KEEPRATIO);
	//moveWindow("frame_Traffic", 730, 20);
	//resizeWindow("frame_Traffic", 360, 240); 
	//imshow("frame_Traffic", frame_Traffic);
	
	namedWindow("frameThresh", WINDOW_KEEPRATIO);
	moveWindow("frameThresh", 715, 20);
	resizeWindow("frameThresh", 360, 240); 
	imshow("frameThresh", frameThresh);
	
	namedWindow("frameEdge", WINDOW_KEEPRATIO);
	moveWindow("frameEdge", 0, 265);
	resizeWindow("frameEdge", 360, 240); 
	imshow("frameEdge", frameEdge);
	
	namedWindow("frameFinal", WINDOW_KEEPRATIO);
	moveWindow("frameFinal", 355, 265);
	resizeWindow("frameFinal", 360, 240); 
	imshow("frameFinal", frameFinal);
	  
	//namedWindow("stop sign", WINDOW_KEEPRATIO);
	//moveWindow("stop sign", 715, 265);
	//resizeWindow("stop sign", 360, 240); 
	//imshow("stop sign", RoI_Stop);
	  
	
	   
//count fps
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	float t = elapsed_seconds.count();
	int FPS = 1/t;
	cout<<"FPS = "<<FPS<<endl;
	
//destroy windows
	if(waitKey(30)=='q')
	    break;
    }
      
    return 0;
    
}

 
