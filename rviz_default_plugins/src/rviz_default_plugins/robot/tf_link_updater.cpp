// Copyright (c) 2009, Willow Garage, Inc.
// Copyright (c) 2018, Bosch Software Innovations GmbH.
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


#include "rviz_default_plugins/robot/tf_link_updater.hpp"

#include <string>

#include <OgreVector.h>

#include "rviz_common/frame_manager_iface.hpp"

namespace rviz_default_plugins
{
namespace robot
{

using rviz_common::properties::StatusProperty;

TFLinkUpdater::TFLinkUpdater(
  rviz_common::FrameManagerIface * frame_manager,
  const StatusCallback & status_cb,
  const std::string & tf_prefix)
: frame_manager_(frame_manager),
  status_callback_(status_cb),
  tf_prefix_(tf_prefix)
{}

bool TFLinkUpdater::getLinkTransforms(
  const std::string & _link_name,
  Ogre::Vector3 & visual_position,
  Ogre::Quaternion & visual_orientation,
  Ogre::Vector3 & collision_position,
  Ogre::Quaternion & collision_orientation) const
{
  // Replacing tf::resolve. We know that the name has no leading "/". If we assume that the
  // tf_prefix_ has no leading "/", this is what should happen
  std::string link_name = _link_name;
  if (!tf_prefix_.empty()) {
    link_name = tf_prefix_ + "/" + link_name;
  }

  Ogre::Vector3 position;
  Ogre::Quaternion orientation;
  if (!frame_manager_->getTransform(link_name, position, orientation)) {
    std::string error_message(
      "No transform from [" + link_name + "] to [" + frame_manager_->getFixedFrame() + "]");
    setLinkStatus(StatusProperty::Error, link_name, error_message);
    return false;
  }

  setLinkStatus(StatusProperty::Ok, link_name, "Transform OK");

  // Collision/visual transforms are the same in this case
  visual_position = position;
  visual_orientation = orientation;
  collision_position = position;
  collision_orientation = orientation;

  return true;
}

void TFLinkUpdater::setLinkStatus(
  StatusLevel level, const std::string & link_name, const std::string & text) const
{
  if (status_callback_) {
    status_callback_(level, link_name, text);
  }
}

}  // namespace robot
}  // namespace rviz_default_plugins
