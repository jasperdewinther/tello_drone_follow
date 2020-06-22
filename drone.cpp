#include "drone.hpp"


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
void Drone::move(const int & fb, const int & lr, const int & ud, const int & speed){
	executeFunction("go " + std::to_string(fb) + " " + std::to_string(lr) + " " + std::to_string(ud) + " " + std::to_string(speed));
}
void Drone::land(){
	executeFunction("land");
}

void display(cv::Mat &im, cv::Mat &bbox)
	{
	  int n = bbox.rows;
	  for(int i = 0 ; i < n ; i++)
	  {
	    cv::line(im, cv::Point2i(bbox.at<float>(i,0),bbox.at<float>(i,1)), cv::Point2i(bbox.at<float>((i+1) % n,0), bbox.at<float>((i+1) % n,1)), cv::Scalar(255,0,0), 3);
	  }
	  cv::imshow("Result", im);
	}
void getVideo(Drone * d){
  	cv::VideoCapture cap("udp://@0.0.0.0:11111");
	zbar::ImageScanner scanner;

  	// Configure scanner
  	scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
	int frameSkip = 4;
	cv::Mat frame;
	cv::Mat rectifiedImage;
	cv::Mat roi;
	cv::Ptr<cv::Tracker> tracker;
	cv::Rect2d bb = {-1,-1,-1,-1};
	bool needInit = true;
	while(true){
		for (int i = 0; i < frameSkip; i ++){
			cap.grab();
		}
	  	bool success = cap.read(frame);
	  	if (success){
			cv::imshow("output", frame);
			cv::Mat frameGrey;
			cv::cvtColor(frame, frameGrey, CV_BGR2GRAY);
			zbar::Image image(frame.cols, frame.rows, "Y800", (uchar *)frameGrey.data, frame.cols * frame.rows);
			scanner.scan(image);
			for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol){


				// Print type and data
				std::cout << "Type : " << symbol->get_type_name() << "\n";
				std::cout << "Data : " <<  symbol->get_data() << "\n";
				std::cout << "size : " <<  symbol->get_location_size() << "\n";
				std::cout << "x : " <<  symbol->get_location_x(0) << "\n";
				std::cout << "y : " <<  symbol->get_location_y(0) << "\n";
			}

			if (cv::waitKey(1) == 115){
				//copy = frame.clone();
				std::cout << "starting roi thread\n";
				//std::thread(setRoi, std::ref(x), std::ref(y), std::ref(w), std::ref(h));
				std::cout << "done starting roi thread\n";
            	needInit = true;
			}
			if (bb.x != -1 && bb.y != -1 && bb.width != -1 && bb.height != -1){
				if (needInit){
					std::cout << "initialising tracker\n";
					tracker = cv::TrackerKCF::create();
					tracker->init(frame, bb);
				}
				tracker->update(frame, bb);
				cv::rectangle( frame, bb, cv::Scalar( 255, 0, 0 ), 2, 1 );
			}
	  	}
	}
}