#include "VDSAITargetBehaviors.h"

//////////
// VDAsaiTargetBehavior
//////////
VDAsaiTargetBehavior::VDAsaiTargetBehavior() {}

void VDAsaiTargetBehavior::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_position", "context"), &VDAsaiTargetBehavior::_get_target_position);
  ClassDB::bind_method(D_METHOD("set_target_position_param_key", "param_key"), &VDAsaiTargetBehavior::set_target_position_param_key);
  ClassDB::bind_method(D_METHOD("get_target_position_param_key"), &VDAsaiTargetBehavior::get_target_position_param_key);
  ClassDB::bind_method(D_METHOD("set_direction_type", "type"), &VDAsaiTargetBehavior::set_direction_type);
  ClassDB::bind_method(D_METHOD("get_direction_type"), &VDAsaiTargetBehavior::get_direction_type);

  BIND_VMETHOD(MethodInfo("_get_target_position", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_position_param_key"), "set_target_position_param_key", "get_target_position_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "direction_type", PROPERTY_HINT_ENUM, "TOWARDS,AWAY"), "set_direction_type", "get_direction_type");

  BIND_ENUM_CONSTANT(TOWARDS);
  BIND_ENUM_CONSTANT(AWAY);
}

Ref<VDAsaiSteeringData> VDAsaiTargetBehavior::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering_data = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 target = call("_get_target_position", context);
  Vector3 current = kinematic->get_position();
  Vector3 desired;
  if(direction_type == TOWARDS) desired = target - current;
  else desired = current - target;
  desired.normalize();
  float max_velocity = kinematic->get_max_velocity();
  desired *= max_velocity;
  Vector3 velocity = kinematic->get_velocity();
  Vector3 steering_force = desired - velocity;
  steering_force /= max_velocity;
  steering_force *= kinematic->get_max_force();
  steering_data->set_velocity(steering_force);
  return steering_data;
}

Vector3 VDAsaiTargetBehavior::_get_target_position(Ref<VDAcContext> context) const {
  Vector3 target_position = ZERO_VELOCITY;
  if(context->has_context_param(target_position_param_key)) {
    return context->get_context_value(target_position_param_key);
  }
  return target_position;
}

void VDAsaiTargetBehavior::set_direction_type(VDAsaiTargetBehavior::DirectionType type) {
  direction_type = type;
}

VDAsaiTargetBehavior::DirectionType VDAsaiTargetBehavior::get_direction_type() const {
  return direction_type;
}

void VDAsaiTargetBehavior::set_target_position_param_key(StringName param_key) {
  target_position_param_key = param_key;
}

StringName VDAsaiTargetBehavior::get_target_position_param_key() const {
  return target_position_param_key;
}
//////////
// VDAsaiFacingBehavior
//////////
const Vector3 VDAsaiFacingBehavior::ZERO_FACING = Vector3(1, 0, 0);
const Vector3 VDAsaiFacingBehavior::UP_AXIS = Vector3(0, 1, 0);

VDAsaiFacingBehavior::VDAsaiFacingBehavior() {}

Ref<VDAsaiSteeringData> VDAsaiFacingBehavior::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  return steering;
}
//////////
// VDAsaiNearingBehavior
//////////
VDAsaiNearingBehavior::VDAsaiNearingBehavior() {}

void VDAsaiNearingBehavior::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_damp_nearing_distance", "nearing_distance", "context", "delta"), &VDAsaiNearingBehavior::_damp_nearing_distance);
  ClassDB::bind_method(D_METHOD("_get_target_radius", "context"), &VDAsaiNearingBehavior::_get_target_radius);
  ClassDB::bind_method(D_METHOD("_get_nearing_tolerance", "context"), &VDAsaiNearingBehavior::_get_nearing_tolerance);
  ClassDB::bind_method(D_METHOD("_get_nearing_damping", "context"), &VDAsaiNearingBehavior::_get_nearing_damping);

  ClassDB::bind_method(D_METHOD("set_target_radius_param_key", "param_key"), &VDAsaiNearingBehavior::set_target_radius_param_key);
  ClassDB::bind_method(D_METHOD("get_target_radius_param_key"), &VDAsaiNearingBehavior::get_target_radius_param_key);
  ClassDB::bind_method(D_METHOD("set_nearing_tolerance_param_key", "param_key"), &VDAsaiNearingBehavior::set_nearing_tolerance_param_key);
  ClassDB::bind_method(D_METHOD("get_nearing_tolerance_param_key"), &VDAsaiNearingBehavior::get_nearing_tolerance_param_key);
  ClassDB::bind_method(D_METHOD("set_nearing_damping_param_key", "param_key"), &VDAsaiNearingBehavior::set_nearing_damping_param_key);
  ClassDB::bind_method(D_METHOD("get_nearing_damping_param_key"), &VDAsaiNearingBehavior::get_nearing_damping_param_key);

  BIND_VMETHOD(MethodInfo("_damp_nearing_distance", PropertyInfo(Variant::REAL, "nearing_distance"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("_get_target_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_nearing_tolerance", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_nearing_damping", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_radius_param_key"), "set_target_radius_param_key", "get_target_radius_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "nearing_tolerance_param_key"), "set_nearing_tolerance_param_key", "get_nearing_tolerance_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "nearing_damping_param_key"), "set_nearing_damping_param_key", "get_nearing_damping_param_key");
}

Ref<VDAsaiSteeringData> VDAsaiNearingBehavior::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));

  Vector3 target_pos = call("_get_target_position", context);
  Vector3 current_pos = kinematic->get_position();
  Vector3 desired_velocity;
  bool is_towards = direction_type == TOWARDS;
  if(is_towards) desired_velocity = target_pos - current_pos;
  else desired_velocity = current_pos - target_pos;
  float target_radius = call("_get_target_radius", context);
  float max_velocity = kinematic->get_max_velocity();
  float distance = desired_velocity.length();
  if(is_towards ? distance >= target_radius : distance <= target_radius) {
    desired_velocity.normalize();
    desired_velocity *= max_velocity;
  } else {
    float tolerance = call("_get_nearing_tolerance", context);
    float factor = 0.0;
    // TODO: fix tolerance calculation
    if(is_towards) factor = distance / (target_radius + tolerance);
    else factor = distance / (target_radius - tolerance);
    float damped_factor = call("_damp_nearing_distance", factor, context);
    desired_velocity *= damped_factor;
  }
  float max_force = kinematic->get_max_force();
  Vector3 current_velocity = kinematic->get_velocity();
  Vector3 steering_force = desired_velocity - current_velocity;
  steering_force /= max_velocity;
  steering_force *= max_force;
  steering->set_velocity(steering_force);
  return steering;
}

float VDAsaiNearingBehavior::_damp_nearing_distance(float nearing_distance, Ref<VDAcContext> context, float delta) {
  float damping = call("get_nearing_damping", context);
  return nearing_distance * damping;
}

float VDAsaiNearingBehavior::_get_target_radius(Ref<VDAcContext> context) const {
  float radius = 1.0;
  if(context->has_context_param(target_radius_param_key)) {
    return context->get_context_value(target_radius_param_key);
  }
  return radius;
}

float VDAsaiNearingBehavior::_get_nearing_tolerance(Ref<VDAcContext> context) const {
  float tolerance = 0.0;
  if(context->has_context_param(nearing_tolerance_param_key)) {
    return context->get_context_value(nearing_tolerance_param_key);
  }
  return tolerance;
}

float VDAsaiNearingBehavior::_get_nearing_damping(Ref<VDAcContext> context) const {
  float damping = 1.0;
  if(context->has_context_param(nearing_damping_param_key)) {
    return context->get_context_value(nearing_damping_param_key);
  }
  return damping;
}

void VDAsaiNearingBehavior::set_target_radius_param_key(StringName param_key) {
  target_radius_param_key = param_key;
}

StringName VDAsaiNearingBehavior::get_target_radius_param_key() const {
  return target_radius_param_key;
}

void VDAsaiNearingBehavior::set_nearing_tolerance_param_key(StringName param_key) {
  nearing_tolerance_param_key = param_key;
}

StringName VDAsaiNearingBehavior::get_nearing_tolerance_param_key() const {
  return nearing_tolerance_param_key;
}

void VDAsaiNearingBehavior::set_nearing_damping_param_key(StringName param_key) {
  nearing_damping_param_key = param_key;
}

StringName VDAsaiNearingBehavior::get_nearing_damping_param_key() const {
  return nearing_damping_param_key;
}

