// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef ROS_SEC_TEST__ATTACKS__NOOP__COMPONENT_HPP_
#define ROS_SEC_TEST__ATTACKS__NOOP__COMPONENT_HPP_
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_lifecycle/lifecycle_publisher.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

namespace ros_sec_test
{
namespace attacks
{
namespace noop
{

/// Attack node which does not do anything for testing purposes.
/**
 * If you would like to create your own attack, forking this no-op node
 * is a good strategy.
 */
class Component : public rclcpp_lifecycle::LifecycleNode
{
public:
  Component();
  Component(const Component &) = delete;
  Component & operator=(const Component &) = delete;

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_configure(const rclcpp_lifecycle::State & /* state */) final;

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_activate(const rclcpp_lifecycle::State & /* state */) final;

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_deactivate(const rclcpp_lifecycle::State & /* state */) final;

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_cleanup(const rclcpp_lifecycle::State & /* state */) final;

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_shutdown(const rclcpp_lifecycle::State & /* state */ state) final;
};


}  // namespace noop
}  // namespace attacks
}  // namespace ros_sec_test

#endif  // ROS_SEC_TEST__ATTACKS__NOOP__COMPONENT_HPP_
