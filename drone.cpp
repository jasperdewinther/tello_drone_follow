#include "drone.hpp"


Drone::Drone(){
	std::cout << "Binding port\n";
	m_sock.bind(m_recvPort, m_recvIP);
	m_sock.setBlocking(true);
	std::cout << "Connecting to drone\n";
	executeFunction("command");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	getBattery();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	streamon();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << "starting video thread\n";
	m_videoThread = std::thread(getVideo, this);
	//std::this_thread::sleep_for(std::chrono::seconds(2));
}

Drone::~Drone(){
	std::this_thread::sleep_for(std::chrono::seconds(3));
	land();
	streamoff();
};

void Drone::executeFunction(const std::string & command, const bool & waitForResponse){
	const char * encodedmsg = command.c_str();
	m_sock.send(encodedmsg, strlen(encodedmsg), m_sendIP, m_sendPort);
	if (waitForResponse){
		sf::Packet msg;
		m_sock.receive(msg, m_recvIP, m_recvPort);
		auto data = static_cast<const char*>(msg.getData());
		auto result = std::string(data);
		std::cout << "command: " << command << " received message: " << result << "\n";
	}
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
void Drone::rc(const int & lr, const int & fb, const int & ud, const int & yaw){
	executeFunction("rc " + std::to_string(lr) + " " + std::to_string(fb) + " " + std::to_string(ud) + " " + std::to_string(yaw), false);
}

void Drone::land(){
	executeFunction("land");
}

void getVideo(Drone * d){
  	cv::VideoCapture cap("udp://@0.0.0.0:11111");
	zbar::ImageScanner scanner;
  	scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
	int frameSkip = 3;
	cv::Mat frame;
	cv::namedWindow("output");
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
					d->m_toRotate = bb.getHorizontalDegree();
					d->m_toGoForwardAndBackward = -(100.f-bb.getDistanceInCm());
					d->m_toGoLeftAndRight = -((bb.getCenter().x - (frame.cols/2)) * bb.cmPerPixel());
					d->m_toGoUpAndDown = -((bb.getCenter().y - (frame.rows/2)) * bb.cmPerPixel());
					d->m_valuesSet = true;
				}
			} else {
				d->m_toRotate = 0;
				d->m_toGoForwardAndBackward = 0;
				d->m_toGoLeftAndRight = 0;
				d->m_toGoUpAndDown = 0;
				d->m_valuesSet = false;
			}
	  	}
		cv::imshow("output", frame);
		cv::waitKey(1);
	}
}