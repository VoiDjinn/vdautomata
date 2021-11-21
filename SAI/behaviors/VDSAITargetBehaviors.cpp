#include "VDSAITargetBehaviors.h"

//////////
// VDAsaiTargetBehavior
//////////
VDAsaiTargetBehavior::VDAsaiTargetBehavior() {}

void VDAsaiTargetBehavior::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_target_position", "context"), &VDAsaiTargetBehavior::get_target_position);
  ClassDB::bind_method(D_METHOD("set_target_position_param_key", "param_key"), &VDAsaiTargetBehavior::set_target_position_param_key);
  ClassDB::bind_method(D_METHOD("get_target_position_param_key"), &VDAsaiTargetBehavior::get_target_position_param_key);
  ClassDB::bind_method(D_METHOD("set_direction_type", "type"), &VDAsaiTargetBehavior::set_direction_type);
  ClassDB::bind_method(D_METHOD("get_direction_type"), &VDAsaiTargetBehavior::get_direction_type);

  BIND_VMETHOD(MethodInfo("get_target_position", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  BIND_ENUM_CONSTANT(TOWARDS);
  BIND_ENUM_CONSTANT(AWAY);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_position_param_key"), "set_target_position_param_key", "get_target_position_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "direction_type", PROPERTY_HINT_ENUM, "TOWARDS,AWAY"), "set_direction_type", "get_direction_type");
}

Vector3 VDAsaiTargetBehavior::_on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Vector3 target_position = call("get_target_position", context);
  Vector3 desired_velocity = (target_position - kinematic->get_position()).normalized() * kinematic->get_max_speed();
  if(direction_type == AWAY) desired_velocity *= -1;
  Vector3 velocity = kinematic->get_velocity();
  Vector3 steering = (desired_velocity - velocity) / kinematic->get_mass();
  return velocity + steering;
}

Vector3 VDAsaiTargetBehavior::get_target_position(Ref<VDAcContext> context) const {
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
// VDAsaiNearingBehavior
//////////
VDAsaiNearingBehavior::VDAsaiNearingBehavior() {}

void VDAsaiNearingBehavior::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_damp_nearing_distance", "nearing_distance", "context", "delta"), &VDAsaiNearingBehavior::_damp_nearing_distance);
  ClassDB::bind_method(D_METHOD("get_target_radius", "context"), &VDAsaiNearingBehavior::get_target_radius);
  ClassDB::bind_method(D_METHOD("get_nearing_tolerance", "context"), &VDAsaiNearingBehavior::get_nearing_tolerance);
  ClassDB::bind_method(D_METHOD("get_nearing_damping", "context"), &VDAsaiNearingBehavior::get_nearing_damping);

  ClassDB::bind_method(D_METHOD("set_target_radius_param_key", "param_key"), &VDAsaiNearingBehavior::set_target_radius_param_key);
  ClassDB::bind_method(D_METHOD("get_target_radius_param_key"), &VDAsaiNearingBehavior::get_target_radius_param_key);
  ClassDB::bind_method(D_METHOD("set_nearing_tolerance_param_key", "param_key"), &VDAsaiNearingBehavior::set_nearing_tolerance_param_key);
  ClassDB::bind_method(D_METHOD("get_nearing_tolerance_param_key"), &VDAsaiNearingBehavior::get_nearing_tolerance_param_key);
  ClassDB::bind_method(D_METHOD("set_nearing_damping_param_key", "param_key"), &VDAsaiNearingBehavior::set_nearing_damping_param_key);
  ClassDB::bind_method(D_METHOD("get_nearing_damping_param_key"), &VDAsaiNearingBehavior::get_nearing_damping_param_key);

  BIND_VMETHOD(MethodInfo("_damp_nearing_distance", PropertyInfo(Variant::REAL, "nearing_distance"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("get_target_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("get_nearing_tolerance", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("get_nearing_damping", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_radius_param_key"), "set_target_radius_param_key", "get_target_radius_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "nearing_tolerance_param_key"), "set_nearing_tolerance_param_key", "get_nearing_tolerance_param_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "nearing_damping_param_key"), "set_nearing_damping_param_key", "get_nearing_damping_param_key");
}

// TODO: move out of target radius (AWAY)
// TODO: nearing_tolerance, for stopping before reaching target (TOWARDS)
Vector3 VDAsaiNearingBehavior::_on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Vector3 steering = ZERO_VELOCITY;
  Vector3 target_position = call("get_target_position", context);
  Vector3 current_position = kinematic->get_position();
  float distance = current_position.distance_to(target_position);
  Vector3 desired_velocity = (target_position - kinematic->get_position()).normalized() * kinematic->get_max_speed();
  if(direction_type == AWAY) desired_velocity *= -1;
  /*
  float nearing_tolerance = call("get_nearing_tolerance", context);
  if(distance <= nearing_tolerance && direction_type == TOWARDS) {
    kinematic->set_velocity(ZERO_VELOCITY);
    return ZERO_VELOCITY;
  } else {
  */
  float target_radius = call("get_target_radius", context);
  if(distance <= target_radius) {
    float damped_distance = call("_damp_nearing_distance", (distance / target_radius), context, delta);
    desired_velocity *= damped_distance;
  }
  //}
  Vector3 velocity = kinematic->get_velocity();
  steering = (desired_velocity - velocity) / kinematic->get_mass();
  return velocity + steering;
}

float VDAsaiNearingBehavior::_damp_nearing_distance(float nearing_distance, Ref<VDAcContext> context, float delta) {
  float damping = call("get_nearing_damping", context);
  return nearing_distance * damping;
}

float VDAsaiNearingBehavior::get_target_radius(Ref<VDAcContext> context) const {
  float radius = 1.0;
  if(context->has_context_param(target_radius_param_key)) {
    return context->get_context_value(target_radius_param_key);
  }
  return radius;
}

float VDAsaiNearingBehavior::get_nearing_tolerance(Ref<VDAcContext> context) const {
  float tolerance = 1.0;
  if(context->has_context_param(nearing_tolerance_param_key)) {
    return context->get_context_value(nearing_tolerance_param_key);
  }
  return tolerance;
}

float VDAsaiNearingBehavior::get_nearing_damping(Ref<VDAcContext> context) const {
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


