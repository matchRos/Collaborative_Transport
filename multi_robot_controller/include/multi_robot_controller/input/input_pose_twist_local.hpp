
#include<multi_robot_controller/input/input_base.hpp>
#include<multi_robot_controller/necessary_param_exeption.hpp>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>

/** \addtogroup Input 
 *  @{
 */


class InputPoseTwistLocal: public InputBase
{   
    public:
        InputPoseTwistLocal():InputBase()
        {;}
        InputPoseTwistLocal(ros::NodeHandle &nh,std::string topic_name_pose,std::string topic_name_twist):InputBase(nh)
        {
            this->subscribe(topic_name_pose,topic_name_twist);
           
        }
        inline InputPoseTwistLocal(ros::NodeHandle &nh):InputBase(nh)
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
            this->subscribe(topic_pose,topic_twist);
        }

        inline InputPoseTwistLocal(ros::NodeHandle &nh,ros::NodeHandle &nh_param):InputBase(nh)
        {

            std::string topic_pose;
            if(!nh_param.getParam("topic_pose",topic_pose))
            {
                throw NecessaryParamException(nh_param.resolveName("topic_pose")); 
            }


            std::string topic_twist;
            if(!nh_param.getParam("topic_twist",topic_twist))
            {
                throw NecessaryParamException(nh_param.resolveName("topic_twist")); 
            }

            this->subscribe(topic_pose,topic_twist);  
        };


    private:
        ros::Subscriber sub_pose_;
        ros::Subscriber sub_twist_;

        inline void subscribe(std::string topic_name_pose,std::string topic_name_twist)
        {
            this->sub_pose_=this->nh_.subscribe(topic_name_pose,10,&InputPoseTwistLocal::setPose,this);
            this->sub_twist_=this->nh_.subscribe(topic_name_twist,10,&InputPoseTwistLocal::set,this);
        }

        inline void set(geometry_msgs::TwistStamped msg)
        {this->setAngVel(msg);this->setLinVel(msg);}
       
        inline void setPose(geometry_msgs::PoseStamped msg) 
        {tf::poseMsgToTF(msg.pose,this->pose_);this->time_=msg.header.stamp;}
        
        inline void setLinVel(geometry_msgs::TwistStamped msg) 
        {
            tf::vector3MsgToTF(msg.twist.linear,this->lin_vel_);
            this->lin_vel_=tf::Transform(this->pose_.getRotation(),tf::Vector3(0.0,0.0,0.0))*this->lin_vel_;
        }
       
        inline void setAngVel(geometry_msgs::TwistStamped msg) 
        {tf::vector3MsgToTF(msg.twist.angular,this->ang_vel_);}
};