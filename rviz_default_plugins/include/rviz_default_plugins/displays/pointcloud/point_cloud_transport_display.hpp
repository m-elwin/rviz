// Copyright (c) 2023, Open Source Robotics Foundation, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#ifndef RVIZ_DEFAULT_PLUGINS__DISPLAYS__POINTCLOUD__POINT_CLOUD_TRANSPORT_DISPLAY_HPP_
#define RVIZ_DEFAULT_PLUGINS__DISPLAYS__POINTCLOUD__POINT_CLOUD_TRANSPORT_DISPLAY_HPP_

#include <memory>

#include "get_transport_from_topic.hpp"
#include "point_cloud_transport/point_cloud_transport.hpp"
#include "point_cloud_transport/subscriber_filter.hpp"
#include "rviz_common/ros_topic_display.hpp"

namespace rviz_default_plugins
{
namespace displays
{

template<class MessageType>
class PointCloud2TransportDisplay : public rviz_common::_RosTopicDisplay
{
// No Q_OBJECT macro here, moc does not support Q_OBJECT in a templated class.

public:
/// Convenience typedef so subclasses don't have to use
/// the long templated class name to refer to their super class.
  typedef PointCloud2TransportDisplay<MessageType> PC2RDClass;

  PointCloud2TransportDisplay()
  : messages_received_(0)
  {
    QString message_type = QString::fromStdString(rosidl_generator_traits::name<MessageType>());
    topic_property_->setMessageType(message_type);
    topic_property_->setDescription(message_type + " topic to subscribe to.");
  }

/**
* When overriding this method, the onInitialize() method of this superclass has to be called.
* Otherwise, the ros node will not be initialized.
*/
  void onInitialize() override
  {
    _RosTopicDisplay::onInitialize();
  }

  ~PointCloud2TransportDisplay() override
  {
    unsubscribe();
  }

  void reset() override
  {
    Display::reset();
    messages_received_ = 0;
  }

  void setTopic(const QString & topic, const QString & datatype) override
  {
    (void) datatype;
    topic_property_->setString(topic);
  }

protected:
  void updateTopic() override
  {
    resetSubscription();
  }

  virtual void subscribe()
  {
    if (!isEnabled()) {
      return;
    }

    if (topic_property_->isEmpty()) {
      setStatus(
        rviz_common::properties::StatusProperty::Error, "Topic",
        QString("Error subscribing: Empty topic name"));
      return;
    }

    try {
      subscription_ = std::make_shared<point_cloud_transport::SubscriberFilter>();
      subscription_->subscribe(
        rviz_ros_node_.lock()->get_raw_node(),
        getPointCloud2BaseTopicFromTopic(topic_property_->getTopicStd()),
        getPointCloud2TransportFromTopic(topic_property_->getTopicStd()),
        qos_profile.get_rmw_qos_profile());
      subscription_start_time_ = rviz_ros_node_.lock()->get_raw_node()->now();
      subscription_callback_ = subscription_->registerCallback(
        std::bind(
          &PointCloud2TransportDisplay<MessageType>::incomingMessage, this, std::placeholders::_1));
      setStatus(rviz_common::properties::StatusProperty::Ok, "Topic", "OK");
    } catch (rclcpp::exceptions::InvalidTopicNameError & e) {
      setStatus(
        rviz_common::properties::StatusProperty::Error, "Topic",
        QString("Error subscribing: ") + e.what());
    }
  }

  void transformerChangedCallback() override
  {
    resetSubscription();
  }

  void resetSubscription()
  {
    unsubscribe();
    reset();
    subscribe();
    context_->queueRender();
  }

  virtual void unsubscribe()
  {
    subscription_.reset();
  }

  void onEnable() override
  {
    subscribe();
  }

  void onDisable() override
  {
    unsubscribe();
    reset();
  }

/// Incoming message callback.
/**
* Checks if the message pointer
* is valid, increments messages_received_, then calls
* processMessage().
*/
  void incomingMessage(const typename MessageType::ConstSharedPtr msg)
  {
    if (!msg) {
      return;
    }

    ++messages_received_;
    QString topic_str = QString::number(messages_received_) + " messages received";
    // Append topic subscription frequency if we can lock rviz_ros_node_.
    std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> node_interface =
      rviz_ros_node_.lock();
    if (node_interface != nullptr) {
      const double duration =
        (node_interface->get_raw_node()->now() - subscription_start_time_).seconds();
      const double subscription_frequency =
        static_cast<double>(messages_received_) / duration;
      topic_str += " at " + QString::number(subscription_frequency, 'f', 1) + " hz.";
    }
    setStatus(
      rviz_common::properties::StatusProperty::Ok,
      "Topic",
      topic_str);

    processMessage(msg);
  }


/// Implement this to process the contents of a message.
/**
* This is called by incomingMessage().
*/
  virtual void processMessage(typename MessageType::ConstSharedPtr msg) = 0;

  uint32_t messages_received_;
  rclcpp::Time subscription_start_time_;

  std::shared_ptr<point_cloud_transport::SubscriberFilter> subscription_;
  message_filters::Connection subscription_callback_;
};

}  //  end namespace displays
}  // end namespace rviz_default_plugins


#endif  // RVIZ_DEFAULT_PLUGINS__DISPLAYS__POINTCLOUD__POINT_CLOUD_TRANSPORT_DISPLAY_HPP_
