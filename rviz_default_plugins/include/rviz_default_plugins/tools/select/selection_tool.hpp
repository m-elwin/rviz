// Copyright (c) 2008, Willow Garage, Inc.
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


#ifndef RVIZ_DEFAULT_PLUGINS__TOOLS__SELECT__SELECTION_TOOL_HPP_
#define RVIZ_DEFAULT_PLUGINS__TOOLS__SELECT__SELECTION_TOOL_HPP_

#include <vector>

#include "rviz_common/tool.hpp"
#include "rviz_common/interaction/forwards.hpp"

#include "rviz_default_plugins/visibility_control.hpp"

namespace Ogre
{
class Viewport;
}

namespace rviz_default_plugins
{
namespace tools
{

class MoveTool;

class RVIZ_DEFAULT_PLUGINS_PUBLIC SelectionTool : public rviz_common::Tool
{
public:
  SelectionTool();
  virtual ~SelectionTool();

  virtual void onInitialize();

  virtual void activate();
  virtual void deactivate();

  virtual int processMouseEvent(rviz_common::ViewportMouseEvent & event);
  virtual int processKeyEvent(QKeyEvent * event, rviz_common::RenderPanel * panel);

  virtual void update(float wall_dt, float ros_dt);

private:
  MoveTool * move_tool_;

  bool selecting_;
  int sel_start_x_;
  int sel_start_y_;

  rviz_common::interaction::M_Picked highlight_;

  bool moving_;
};

}  // namespace tools
}  // namespace rviz_default_plugins

#endif  // RVIZ_DEFAULT_PLUGINS__TOOLS__SELECT__SELECTION_TOOL_HPP_
