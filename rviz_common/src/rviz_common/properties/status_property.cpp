// Copyright (c) 2012, Willow Garage, Inc.
// Copyright (c) 2017, Open Source Robotics Foundation, Inc.
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


#include <QApplication>
#include <QColor>
#include <QPalette>

#include "rviz_common/load_resource.hpp"
#include "rviz_common/properties/property_tree_model.hpp"
#include "rviz_common/properties/status_property.hpp"

namespace rviz_common
{
namespace properties
{

QColor StatusProperty::status_colors_[3] = {QColor(), QColor(192, 128, 0), QColor(192, 32, 32)};
QString StatusProperty::status_words_[3] = {"Ok", "Warn", "Error"};

StatusProperty::StatusProperty(
  const QString & name,
  const QString & text,
  StatusProperty::Level level,
  Property * parent)
: Property(name, text, text, parent),
  level_(level)
{
  setShouldBeSaved(false);
  status_icons_[0] = loadPixmap("package://rviz_common/icons/ok.png");
  status_icons_[1] = loadPixmap("package://rviz_common/icons/warning.png");
  status_icons_[2] = loadPixmap("package://rviz_common/icons/error.png");

  if (!status_colors_[0].isValid()) {  // initialize default text color once
    status_colors_[0] = QApplication::palette().color(QPalette::Text);
  }
}

bool StatusProperty::setValue(const QVariant & new_value)
{
  setDescription(new_value.toString() );
  return Property::setValue(new_value);
}

QVariant StatusProperty::getViewData(int column, int role) const
{
  if ( (getViewFlags(column) & Qt::ItemIsEnabled) && column == 0 && role == Qt::ForegroundRole) {
    return statusColor(level_);
  }
  if (column == 0 && role == Qt::DecorationRole) {
    return statusIcon(level_);
  }
  return Property::getViewData(column, role);
}

Qt::ItemFlags StatusProperty::getViewFlags(int column) const
{
  return Property::getViewFlags(column);
}

// static function
QColor StatusProperty::statusColor(StatusProperty::Level level)
{
  return status_colors_[static_cast<int>(level)];
}

// static function
QIcon StatusProperty::statusIcon(StatusProperty::Level level) const
{
  return status_icons_[level];
}

// static function
QString StatusProperty::statusWord(StatusProperty::Level level)
{
  return status_words_[static_cast<int>(level)];
}


void StatusProperty::setLevel(StatusProperty::Level level)
{
  if (level_ != level) {
    level_ = level;
    if (model_) {
      model_->emitDataChanged(this);
    }
  }
}

StatusProperty::Level StatusProperty::getLevel() const
{
  return level_;
}

}  // namespace properties
}  // namespace rviz_common
