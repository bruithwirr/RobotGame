#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to robot_msgs__msg__RobotState

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotState {

    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub theta: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub linear_velocity: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub angular_velocity: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub current_load: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub max_load: i8,

}



impl Default for RobotState {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotState::default())
  }
}

impl rosidl_runtime_rs::Message for RobotState {
  type RmwMsg = super::msg::rmw::RobotState;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        x: msg.x,
        y: msg.y,
        theta: msg.theta,
        linear_velocity: msg.linear_velocity,
        angular_velocity: msg.angular_velocity,
        current_load: msg.current_load,
        max_load: msg.max_load,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      x: msg.x,
      y: msg.y,
      theta: msg.theta,
      linear_velocity: msg.linear_velocity,
      angular_velocity: msg.angular_velocity,
      current_load: msg.current_load,
      max_load: msg.max_load,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      x: msg.x,
      y: msg.y,
      theta: msg.theta,
      linear_velocity: msg.linear_velocity,
      angular_velocity: msg.angular_velocity,
      current_load: msg.current_load,
      max_load: msg.max_load,
    }
  }
}


