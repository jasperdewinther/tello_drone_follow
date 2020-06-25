#include "drone.hpp"

Qrbb::Qrbb() = default;

Qrbb::Qrbb(const float & qrSize, const zbar::Symbol & symbol):
	qrSizeInCm(qrSize),
	bl(cv::Point(symbol.get_location_x(1),symbol.get_location_y(1))),
	tl(cv::Point(symbol.get_location_x(0),symbol.get_location_y(0))),
	tr(cv::Point(symbol.get_location_x(3),symbol.get_location_y(3))),
	br(cv::Point(symbol.get_location_x(2),symbol.get_location_y(2)))
{}

void Qrbb::draw(cv::Mat & frame) const{
	cv::line(frame, tl, tr, cv::Scalar(0,0,255), 3, 8, 0);
	cv::putText(frame, "tl", tl, cv::FONT_HERSHEY_DUPLEX, 3.0, CV_RGB(0, 0, 255), 2);
	cv::line(frame, tr, br, cv::Scalar(0,0,255), 3, 8, 0);
	cv::putText(frame, "tr", tr, cv::FONT_HERSHEY_DUPLEX, 3.0, CV_RGB(0, 0, 255), 2);
	cv::line(frame, bl, br, cv::Scalar(0,0,255), 3, 8, 0);
	cv::putText(frame, "bl", bl, cv::FONT_HERSHEY_DUPLEX, 3.0, CV_RGB(0, 0, 255), 2);
	cv::line(frame, tl, bl, cv::Scalar(0,0,255), 3, 8, 0);
	cv::putText(frame, "br", br, cv::FONT_HERSHEY_DUPLEX, 3.0, CV_RGB(0, 0, 255), 2);
}

float Qrbb::getHorizontalDegree() const{
	//calculate distance to left side of the qr code and right side of qr code
	//these distances will be used to calculate the angle of the qr code compared to the camera
	float leftSize = abs(tl.y - bl.y);
	float rightSize = abs(tr.y - br.y);

	float corner = 23.23/(360/leftSize);
	float distanceInCmLeft = qrSizeInCm/tan(corner * 3.14159265 / 180.0 );

	corner = 23.23/(360/rightSize);
	float distanceInCmRight = qrSizeInCm/tan(corner * 3.14159265 / 180.0 );

	float A = distanceInCmRight;
	float B = distanceInCmLeft;
	float C = qrSizeInCm;

	return atan((pow(A,2)-pow(B,2)-pow(C,2))/-(2*B*C)) / 3.14159265 * 180.0;
}
float Qrbb::getVerticalDegree() const{
	float topSize = abs(tl.x - tr.x);
	float bottomSize = abs(bl.x - br.x);
	return abs(topSize - bottomSize);
}
float Qrbb::getDistanceInCm() const{
	//calculate distance based on height and width and take the average
	//assumes camera FOV is 82.6 and resolution is 1280x720
	float average = (abs(tl.y - bl.y)+abs(tr.y - br.y))/2.f;
	float corner = 23.23/(360/average);
	float distanceInCmHeight = qrSizeInCm/tan(corner * 3.14159265 / 180.0 );

	average = (abs(tl.x - tr.x)+abs(bl.x - br.x))/2.f;
	corner = 41.3/(640/average);
	float distanceInCmWidth = qrSizeInCm/tan(corner * 3.14159265 / 180.0 );
	return (distanceInCmHeight+distanceInCmWidth)/2;
}
float Qrbb::cmPerPixel() const{
	return qrSizeInCm / ((abs(tl.y - bl.y)+abs(tr.y - br.y)+abs(tl.x - tr.x)+abs(bl.x - br.x))/4.f);
}
cv::Point Qrbb::getCenter() const{
	int averagex = static_cast<int>((tl.x+bl.x+tr.x+br.x)/4.f);
	int averagey = static_cast<int>((tl.y+bl.y+tr.y+br.y)/4.f);
	return cv::Point(averagex, averagey);
}
Drone::Drone(){
	std::cout << "Binding port\n";
	m_sock.bind(m_recvPort, m_recvIP);
	m_sock.setBlocking(true);
	std::cout << "Connecting to drone\n";
	executeFunction("command");
	getBattery();
	streamon();
	std::cout << "starting video thread\n";
	m_videoThread = std::thread(getVideo, this);
	//std::this_thread::sleep_for(std::chrono::seconds(2));
}

Drone::~Drone(){
	std::this_thread::sleep_for(std::chrono::seconds(3));
	land();
	streamoff();
};

void Drone::executeFunction(const std::string & command){
	const char * encodedmsg = command.c_str();
	m_sock.send(encodedmsg, strlen(encodedmsg), m_sendIP, m_sendPort);
	sf::Packet msg;
	m_sock.receive(msg, m_recvIP, m_recvPort);
	auto data = static_cast<const char*>(msg.getData());
	auto result = std::string(data);
	std::cout << "command: " << command << " received message: " << result << "\n";
}
void Drone::streamon(){
	executeFunction("streamon");
}
void Drone::streamoff(){
	executeFunction("streamoff");
}
void Drone::getBattery(){
	executeFunction("battery?");
}
void Drone::takeOff(){
	executeFunction("takeoff");
}
void Drone::rotate(const int & rotation){
	if (rotation >= 0 ){
		executeFunction("cw " + std::to_string(rotation%360));
	} else {
		executeFunction("ccw " + std::to_string(-(rotation%360)));
	}
}
void Drone::move(const int & fb, const int & lr, const int & ud, const int & speed){
	executeFunction("go " + std::to_string(fb) + " " + std::to_string(lr) + " " + std::to_string(ud) + " " + std::to_string(speed));
}
void Drone::land(){
	executeFunction("land");
}

void getVideo(Drone * d){
  	cv::VideoCapture cap("udp://@0.0.0.0:11111");
	zbar::ImageScanner scanner;
  	scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
	int frameSkip = 4;
	cv::Mat frame;
	cv::namedWindow("outputfuckingshowyourself");
	while(true){
		for (int i = 0; i < frameSkip; i ++){
			cap.grab();
		}
	  	bool success = cap.read(frame);
	  	if (success){
			//std::cout << "success\n";
			cv::Mat frameGrey;
			cv::cvtColor(frame, frameGrey, CV_BGR2GRAY);
			zbar::Image image(frame.cols, frame.rows, "Y800", (uchar *)frameGrey.data, frame.cols * frame.rows);
			scanner.scan(image);
			Qrbb bb;
			if (image.get_symbols().get_size() == 1){
				for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol){
					bb = Qrbb(17, *symbol);
					bb.draw(frame);
					const std::lock_guard<std::mutex> lock(d->m_writeMutex);
					d->m_toRotate = static_cast<int>(bb.getHorizontalDegree());
					d->m_toGoForwardAndBackward = -(200-bb.getDistanceInCm());
					d->m_toGoLeftAndRight = -((bb.getCenter().x - (frame.cols/2)) * bb.cmPerPixel());
					d->m_toGoUpAndDown = -((bb.getCenter().y - (frame.rows/2)) * bb.cmPerPixel());
				}
			} else {
				d->m_toRotate = 0;
				d->m_toGoForwardAndBackward = 0;
				d->m_toGoLeftAndRight = 0;
				d->m_toGoUpAndDown = 0;
			}
	  	}
		cv::imshow("outputfuckingshowyourself", frame);
		cv::waitKey(1);
	}
}