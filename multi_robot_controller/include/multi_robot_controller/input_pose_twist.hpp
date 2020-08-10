
#include<multi_robot_controller/input_base.hpp>
#include<multi_robot_controller/necessary_param_exeption.hpp>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>

class InputPoseTwist: public InputBase
{   
    public:
        InputPoseTwist(ros::NodeHandle nh,std::string topic_name_pose,std::string topic_name_twist)
        {
           
            this->sub_pose_=nh.subscribe(topic_name_pose,10,&InputPoseTwist::setPose,this);
            this->sub_twist_=nh.subscribe(topic_name_twist,10,&InputPoseTwist::set,this);
        }
        inline InputPoseTwist(ros::NodeHandle nh)
        {
            ros::NodeHandle priv("~");
            std::string topic_pose;
            if(!priv.getParam("topic_pose",topic_pose))
            {
              throw NecessaryParamException(priv.resolveName("topic_pose")); 
            }
            std::string topic_twist;
            if(!priv.getParam("topic_twist",topic_twist))
            {
                throw NecessaryParamException(priv.resolveName("topic_twist")); 
            }
            InputPoseTwist(nh,topic_pose,topic_twist);
        }
    private:
        ros::Subscriber sub_pose_;
        ros::Subscriber sub_twist_;

        inline void set(geometry_msgs::Twist msg)
        {this->setAngVel(msg);this->setLinVel(msg);}
       
        inline void setPose(geometry_msgs::PoseStamped msg) 
        {tf::poseMsgToTF(msg.pose,this->pose_);}
        
        inline void setLinVel(geometry_msgs::Twist msg) 
        {tf::vector3MsgToTF(msg.linear,this->lin_vel_);}
       
        inline void setAngVel(geometry_msgs::Twist msg) 
        {tf::vector3MsgToTF(msg.angular,this->ang_vel_);}
};