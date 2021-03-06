#include <multi_robot_controller/feed_forward/msg_orientation_feed_forward.h>


template<class T>
MsgOrientationFeedForward<T>::MsgOrientationFeedForward(ros::NodeHandle &nh,std::string topic):RosOrientationFeedForwardBase(nh)                                                         
{
    this->ori_sub_=this->nh_.subscribe(topic,10,&MsgOrientationFeedForward::callbackOrientation,this);
    
}

template<class T>
void MsgOrientationFeedForward<T>::callbackOrientation(T msg)
{
   this->current_orientation_ =msg;
}
template<class T>
void MsgOrientationFeedForward<T>::update(const ros::TimerEvent&)
{
    if(this->enable_)
    {
        Eigen::Quaterniond quat;
        convertMsg(quat,this->current_orientation_);
        this->updateOrientation(quat);
        geometry_msgs::PoseStamped pose;
        pose.header.frame_id=tf::resolve(this->tf_prefix_,this->ee_frame_id_);
        Pose forward=this->getPose();
        convertMsg(pose,forward);
        this->pose_pub_.publish(pose);

    }
   
}
template<class T>
bool MsgOrientationFeedForward<T>::init()
{
    ros::NodeHandle nh("~");
    RosOrientationFeedForwardBase::init();
    T init=*ros::topic::waitForMessage<T>(this->topic_,this->nh_);
    Orientation ori;
    convertMsg(ori,init);
    this->setInitial(ori);
   
}

template class MsgOrientationFeedForward<geometry_msgs::TransformStamped>;
template class MsgOrientationFeedForward<geometry_msgs::PoseStamped>;
template class MsgOrientationFeedForward<geometry_msgs::Pose>;
template class MsgOrientationFeedForward<std_msgs::Float64>;