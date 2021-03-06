#include <multi_robot_controller/feed_forward/tf_orientation_feed_forward.h>

TfOrientationFeedForward::TfOrientationFeedForward(ros::NodeHandle &nh):RosOrientationFeedForwardBase(nh)                                   
{
}
void TfOrientationFeedForward::update(const ros::TimerEvent&)
{
    if(this->enable_)
    {
        try{
            geometry_msgs::TransformStamped trafo=tf_buffer_.lookupTransform(   tf::resolve(this->tf_prefix_,this->current_source_frame_),
                                                                                tf::resolve(this->tf_prefix_,this->current_target_frame_),
                                                                                ros::Time(0));
            Orientation quat;
            convertMsg(quat,trafo);
            this->updateOrientation(quat);
            geometry_msgs::PoseStamped pose;
            pose.header.frame_id=tf::resolve(this->tf_prefix_,this->ee_frame_id_);
            Pose forward=this->getPose();
            convertMsg(pose,forward);
            this->pose_pub_.publish(pose);    
        }
        catch(tf2::TransformException &ex) {
                    ROS_WARN("Could NOT find trafo for initial pose lookupfrom %s to %s: %s",
                                                            this->current_source_frame_.c_str(),
                                                            this->current_target_frame_.c_str(), ex.what());
        }
        catch(std::exception &ex)
        {
            ROS_WARN_STREAM(ex.what());
            throw ex;
        }
    }
   
}
bool TfOrientationFeedForward::init()
{
    RosOrientationFeedForwardBase::init();
    ros::NodeHandle nh("~");
    if(!nh.getParam("current_target_frame",this->current_target_frame_)||!nh.getParam("current_source_frame",this->current_source_frame_))
    {
        ROS_WARN("Frames for current orientation contain error!");
    }
    try{
     geometry_msgs::TransformStamped trafo=tf_buffer_.lookupTransform(   tf::resolve(this->tf_prefix_,this->current_source_frame_),
                                                                            tf::resolve(this->tf_prefix_,this->current_target_frame_),
                                                                            ros::Time(0));
        Orientation quat;
        convertMsg(quat,trafo);
        this->setInitial(quat);
    }
    catch(tf2::TransformException &ex) {
        ROS_WARN("Could NOT find trafo for initial pose lookupfrom %s to %s: %s",
                                                this->current_source_frame_.c_str(),
                                                this->current_target_frame_.c_str(), ex.what());
    } 
}