#include "cv_connection.hpp"

#include <opencv2/opencv.hpp>
//#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <string>

#include <cv_bridge/cv_bridge.h>
#include <opencv2/imgproc/imgproc.hpp>
	
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>

OpenCVConnector::OpenCVConnector(std::string topic_name,size_t csiPort,uint32_t cameraIdx) : it(nh), counter(0),csiPort(csiPort),cameraIdx(cameraIdx)	{
   pub = it.advertise(topic_name, 1);
   ROStime = ros::Time::now();
   ROStimemain = ros::Time::now();
}

void OpenCVConnector::WriteToOpenCV(unsigned char* buffer, int width, int height) {
	// This would take a lot of time:  mat_img and cvtColor take 100% of CPU!
	// create a cv::Mat from a dwImageNvMedia rgbaImage
    cv::Mat mat_img(cv::Size(width, height), CV_8UC4, buffer);
    //cv::Mat converted;//=new cv::Mat();
    cv::cvtColor( mat_img  , mat_img,cv::COLOR_RGBA2RGB);   //=COLOR_BGRA2BGR
	
    //cv_bridge::CvImage img_bridge;
    //sensor_msgs::Image img_msg; // >> message to be sent
		
    std_msgs::Header header; // empty header
    header.seq = counter; // user defined counter
    header.stamp = ros::Time::now(); // time
	 	
    //img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, mat_img);
    //img_bridge.toImageMsg(img_msg); // from cv_bridge to sensor_msgs::Image
	
    pub.publish(  cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, mat_img).toImageMsg()  ); 
	//counter ++;
	
	 
	/*std::cerr << "  Port: "<<csiPort<<"  Camera: "<<cameraIdx<<" FPS: " << 1.0/(ros::Time::now().toSec() - ROStimemain.toSec())<<std::endl;
	ROStimemain = ros::Time::now(); */
}

void OpenCVConnector::WriteToOpenCV_GPU(unsigned char* buffer, int width, int height) {

	cv::cuda::GpuMat d_test(cv::Size(width, height), CV_8UC3, src);
	//cv::cuda::GpuMat d_dst;
	cv::cuda::cvtColor(d_test, d_dst, cv::COLOR_RGB2BGR);
	cv::Mat h_test; // = d_test;
	d_dst.download(h_test);// Get from GPU memory
	
    std_msgs::Header header; // empty header
    header.seq = counter; // user defined counter
    header.stamp = ros::Time::now(); // time

    pub.publish(  cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, h_test).toImageMsg()  ); 
	//counter ++;
	 
	/*std::cerr << "  Port: "<<csiPort<<"  Camera: "<<cameraIdx<<" FPS: " << 1.0/(ros::Time::now().toSec() - ROStimemain.toSec())<<std::endl;
	ROStimemain = ros::Time::now(); */
}

void OpenCVConnector::showFPS() {
	std::cerr << "  Port: "<<csiPort<<"  Camera: "<<cameraIdx<<" FPS: " << 1.0/(ros::Time::now().toSec() - ROStime.toSec())<<std::endl;
	ROStime = ros::Time::now();
}
	
	


