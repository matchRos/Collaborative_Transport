#include <multi_robot_controller/feed_forward/ros_orientation_feed_forward_base.h>

RosOrientationFeedForwardBase::RosOrientationFeedForwardBase(ros::NodeHandle &nh):  nh_(nh),                                                                        
                                                                                    tf_listener_(tf_buffer_),
                                                                                    enable_(false)             
{
    ros::NodeHandle priv("~");
    this->tf_buffer_.setUsingDedicatedThread(true);
    this->init_service_=priv.advertiseService("enable",&RosOrientationFeedForwardBase::initServiceCallback,this);
    this->enable_service_=priv.advertiseService("disable",&RosOrientationFeedForwardBase::disableServiceCallback,this);
}

bool RosOrientationFeedForwardBase::init()
{
    double rate;
    ros::NodeHandle nh("~");
    
    nh.getParam("tf_prefix",this->tf_prefix_);
    
    bool lookup_offset=false;
    if(nh.getParam("lookup_offset",lookup_offset))
    {
        std::string off_source_frame,off_target_frame;
        if(!nh.getParam("off_target_frame",off_target_frame)||!nh.getParam("off_source_frame",off_source_frame))
        {
            ROS_WARN("Frames for offset lookup contain error!");
        }
        else
        {         
            try{
                geometry_msgs::TransformStamped offset_trafo=tf_buffer_.lookupTransform(tf::resolve(this->tf_prefix_,off_source_frame),
                                                                                    tf::resolve(this->tf_prefix_,off_target_frame),
                                                                                    ros::Time(0),
                                                                                    ros::Duration(10.0));
                Pose offset;
                convertMsg(offset,offset_trafo);
                this->setOffset(offset);
                ROS_INFO_STREAM("Set offset: \n"<<offset);
            }
            catch(tf2::TransformException &ex) {
                        ROS_WARN("Could NOT find trafo for offset lookup from %s to %s: %s",
                                                                off_source_frame.c_str(),
                                                                off_target_frame.c_str(), ex.what());
            }
        }
    }
    else
    {
        ROS_WARN("Did not load offset for feed forward!");
    }
    bool lookup_initial_pose;
    if(nh.getParam("lookup_initial_pose",lookup_initial_pose))
    {
        std::string target_frame,source_frame;
        if(!nh.getParam("ee_target_frame",target_frame)||!nh.getParam("ee_source_frame",source_frame))
        {
            ROS_WARN("Frames for offset lookup contain error!");
        }
        else
        {
            try
            {
                geometry_msgs::TransformStamped trafo=tf_buffer_.lookupTransform(tf::resolve(this->tf_prefix_,source_frame),
                                                                                tf::resolve(this->tf_prefix_,target_frame),
                                                                                ros::Time(0),
                                                                                ros::Duration(10.0));
                Pose initial;
                convertMsg(initial,trafo);
                this->setDesiredPose(initial);
                ROS_INFO_STREAM("Set initial pose: \n"<<initial);
            }
            catch(tf2::TransformException &ex) {
                  ROS_WARN("Could NOT find trafo for initial lookup from %s to %s: %s",
                                                                source_frame.c_str(),
                                                                target_frame.c_str(), ex.what());
            }
        }
    }
    else
    {
        ROS_WARN("Did not load initial pose for feed forward!");
    }
    if(!nh.getParam("ee_pose_frame",this->ee_frame_id_))
    {
        ROS_WARN("Could not load frame id for feed forward pose!");
        return false;
    }
    std::string pose_topic;
    if(nh.getParam("pose_topic",pose_topic))
    {
        this->pose_pub_=this->nh_.advertise<geometry_msgs::PoseStamped>(pose_topic,10);
    }

    if(nh.getParam("update_rate",rate))
    {
        this->update_timer_=this->nh_.createTimer(ros::Rate(rate),&RosOrientationFeedForwardBase::update,this);
    }
    else
    {
       throw NecessaryParamException("update_rate");
    }
    return true;
    
}
bool RosOrientationFeedForwardBase::initServiceCallback(std_srvs::EmptyRequest &req,std_srvs::EmptyResponse &res)
{
    this->init();
    this->enable_=true;
    return true;
}
bool RosOrientationFeedForwardBase::disableServiceCallback(std_srvs::EmptyRequest &req,std_srvs::EmptyResponse &res)
{
    this->enable_=false;
    return true;
}

