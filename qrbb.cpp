#include "qrbb.hpp"



Qrbb::Qrbb() = default;

Qrbb::Qrbb(const float & qrSize, const zbar::Symbol & symbol):
	qrSizeInCm(qrSize),
	bl(cv::Point(symbol.get_location_x(1),symbol.get_location_y(1))),
	tl(cv::Point(symbol.get_location_x(0),symbol.get_location_y(0))),
	tr(cv::Point(symbol.get_location_x(3),symbol.get_location_y(3))),
	br(cv::Point(symbol.get_location_x(2),symbol.get_location_y(2)))
{}

void Qrbb::draw(cv::Mat & frame) const{
	cv::putText(frame, "distance: " + std::to_string(getDistanceInCm()) + " cm", cv::Point(0,25), cv::FONT_HERSHEY_DUPLEX, 0.7, CV_RGB(0, 0, 255), 2);
	cv::putText(frame, "rotation: " + std::to_string(getHorizontalDegree()) + "°", cv::Point(0,50), cv::FONT_HERSHEY_DUPLEX, 0.7, CV_RGB(0, 0, 255), 2);
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