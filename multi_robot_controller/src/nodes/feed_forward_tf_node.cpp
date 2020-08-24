#include<ros/ros.h>
#include<multi_robot_controller/feed_forward/tf_orientation_feed_forward.h>



int main(int argc, char** argv)
{
    ros::init(argc,argv,"forward_publisher");
    ros::NodeHandle nh;
    TfOrientationFeedForward feed(nh);
    ros::Duration(1.0).sleep();
    feed.init();
    ros::spin();
}