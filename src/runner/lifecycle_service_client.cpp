// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "lifecycle_msgs/msg/state.hpp"
#include "lifecycle_msgs/msg/transition.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "lifecycle_msgs/srv/get_state.hpp"

#include "rclcpp/rclcpp.hpp"

#include "rcutils/logging_macros.h"

#include "ros_sec_test/runner/lifecycle_service_client.hpp"

using namespace std::chrono_literals;

using ChangeStateSrv = lifecycle_msgs::srv::ChangeState;
using GetStateSrv = lifecycle_msgs::srv::GetState;

template<typename FutureT, typename WaitTimeT>
std::future_status
wait_for_result(
  FutureT & future,
  WaitTimeT time_to_wait)
{
  auto end = std::chrono::steady_clock::now() + time_to_wait;
  std::chrono::milliseconds wait_period(100);
  std::future_status status = std::future_status::timeout;
  do {
    auto now = std::chrono::steady_clock::now();
    auto time_left = end - now;
    if (time_left <= std::chrono::seconds(0)) {break;}
    status = future.wait_for((time_left < wait_period) ? time_left : wait_period);
  } while (rclcpp::ok() && status != std::future_status::ready);
  return status;
}

template<typename Request, class Rep, class Period>
typename rclcpp::Client<Request>::SharedResponse call_once_ready(
  rclcpp::Node * node,
  rclcpp::Client<Request> * client,
  typename rclcpp::Client<Request>::SharedRequest request,
  const std::chrono::duration<Rep, Period> & time_out
)
{
  if (!client->wait_for_service(time_out)) {
    RCLCPP_ERROR(node->get_logger(), "Service %s is not available.",
      client->get_service_name());
    return typename rclcpp::Client<Request>::SharedResponse();
  }
  auto future_result = client->async_send_request(request);
  auto future_status = wait_for_result(future_result, time_out);
  if (future_status != std::future_status::ready) {
    RCLCPP_ERROR(
      node->get_logger(), "Server time out while calling service %s",
      node->get_name());
    return typename rclcpp::Client<Request>::SharedResponse();
  }
  if (future_result.get()) {
    return future_result.get();
  } else {
    RCLCPP_ERROR(node->get_logger(), "Failed to call service %s", node->get_name());
    return typename rclcpp::Client<Request>::SharedResponse();
  }
}

static std::string build_service_name(
  const std::string & target_node_name,
  const std::string & topic_name)
{
  std::ostringstream ss;
  ss << "/";
  ss << target_node_name;
  ss << "/";
  ss << topic_name;
  return ss.str();
}

static std::string build_change_state_service_name(const std::string & target_node_name)
{
  return build_service_name(target_node_name, "change_state");
}

static std::string build_get_state_service_name(const std::string & target_node_name)
{
  return build_service_name(target_node_name, "get_state");
}

LifecycleServiceClient::LifecycleServiceClient(
  rclcpp::Node * parent_node,
  const std::string & target_node_name)
: target_node_name_(target_node_name),
  parent_node_(parent_node),
  client_change_state_(parent_node->create_client<ChangeStateSrv>(build_change_state_service_name(
      target_node_name))),
  client_get_state_(parent_node->create_client<GetStateSrv>(build_get_state_service_name(
      target_node_name)))
{}

unsigned
LifecycleServiceClient::get_state(std::chrono::seconds time_out)
{
  auto request = std::make_shared<GetStateSrv::Request>();
  auto result = call_once_ready(parent_node_, client_get_state_.get(), request, time_out);
  return result->current_state.id;
}

bool
LifecycleServiceClient::change_state(
  rclcpp_lifecycle::Transition transition,
  std::chrono::seconds time_out)
{
  auto request = std::make_shared<ChangeStateSrv::Request>();
  request->transition.id = transition.id();
  auto result = call_once_ready(parent_node_, client_change_state_.get(), request, time_out);
  return !!result;
}

bool LifecycleServiceClient::activate()
{
  return change_state(rclcpp_lifecycle::Transition(lifecycle_msgs::msg::Transition::
           TRANSITION_CONFIGURE));
}

bool LifecycleServiceClient::configure()
{
  return change_state(rclcpp_lifecycle::Transition(lifecycle_msgs::msg::Transition::
           TRANSITION_CONFIGURE));
}

bool LifecycleServiceClient::shutdown()
{
  return change_state(rclcpp_lifecycle::Transition(lifecycle_msgs::msg::Transition::
           TRANSITION_CONFIGURE));
}
