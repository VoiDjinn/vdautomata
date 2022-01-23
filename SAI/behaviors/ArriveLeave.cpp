#include "./ArriveLeave.h"
//////////////////////////
// VDAsaiBehaviorArrive //
//////////////////////////
VDAsaiBehaviorArrive::VDAsaiBehaviorArrive() {}

void VDAsaiBehaviorArrive::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_radius", "context"), &VDAsaiBehaviorArrive::_get_target_radius);
  ClassDB::bind_method(D_METHOD("_get_slowing_radius", "context"), &VDAsaiBehaviorArrive::_get_slowing_radius);
  ClassDB::bind_method(D_METHOD("_get_time_to_reach", "context"), &VDAsaiBehaviorArrive::_get_time_to_reach);

  ClassDB::bind_method(D_METHOD("set_target_radius_key", "key"), &VDAsaiBehaviorArrive::set_target_radius_key);
  ClassDB::bind_method(D_METHOD("get_target_radius_key"), &VDAsaiBehaviorArrive::get_target_radius_key);
  ClassDB::bind_method(D_METHOD("set_slowing_radius_key", "key"), &VDAsaiBehaviorArrive::set_slowing_radius_key);
  ClassDB::bind_method(D_METHOD("get_slowing_radius_key"), &VDAsaiBehaviorArrive::get_slowing_radius_key);
  ClassDB::bind_method(D_METHOD("set_time_to_reach_key", "key"), &VDAsaiBehaviorArrive::set_time_to_reach_key);
  ClassDB::bind_method(D_METHOD("get_time_to_reach_key"), &VDAsaiBehaviorArrive::get_time_to_reach_key);

  BIND_VMETHOD(MethodInfo("_get_target_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_slowing_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_time_to_reach", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_radius_key"), "set_target_radius_key", "get_target_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "slowing_radius_key"), "set_slowing_radius_key", "get_slowing_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "time_to_reach_key"), "set_time_to_reach_key", "get_time_to_reach_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorArrive::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 target_pos = call("_get_target_position", context);
  float target_radius = call("_get_target_radius", context);
  float slowing_radius = call("_get_slowing_radius", context);
  float max_speed = kinematic->get_max_velocity();
  Vector3 current_pos = kinematic->get_velocity();
  Vector3 direction = target_pos - current_pos;
  float distance = direction.length();
  float target_speed;
  if(distance < target_radius) return steering;
  if(distance > slowing_radius) target_speed = max_speed;
  else target_speed = max_speed * distance / slowing_radius;
  Vector3 current_velocity = kinematic->get_velocity();
  float time_to_reach = call("_get_time_to_reach", context);
  float max_force = kinematic->get_max_force();
  Vector3 desired_velocity = direction;
  desired_velocity.normalize();
  desired_velocity *= target_speed;
  Vector3 velocity = desired_velocity - current_velocity;
  velocity = velocity / time_to_reach;
  if(velocity.length() > max_force) {
    velocity.normalize();
    velocity *= max_force;
  }
  steering->set_velocity(velocity);
  return steering;
}

float VDAsaiBehaviorArrive::_get_target_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(target_radius_context_key)) {
    float target_radius = context->get_context_value(target_radius_context_key);
    return target_radius;
  }
  return 1.0;
}

float VDAsaiBehaviorArrive::_get_slowing_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(slowing_radius_context_key)) {
    float slowing_radius = context->get_context_value(slowing_radius_context_key);
    return slowing_radius;
  }
  return 2.0;
}

float VDAsaiBehaviorArrive::_get_time_to_reach(Ref<VDAcContext> context) const {
  if(context->has_context_param(time_to_reach_context_key)) {
    float time_to_reach = context->get_context_value(time_to_reach_context_key);
    return time_to_reach;
  }
  return 0.1;
}

void VDAsaiBehaviorArrive::set_target_radius_key(StringName key) {
  target_radius_context_key = key;
}

StringName VDAsaiBehaviorArrive::get_target_radius_key() const {
  return target_radius_context_key;
}

void VDAsaiBehaviorArrive::set_slowing_radius_key(StringName key) {
  slowing_radius_context_key = key;
}

StringName VDAsaiBehaviorArrive::get_slowing_radius_key() const {
  return slowing_radius_context_key;
}

void VDAsaiBehaviorArrive::set_time_to_reach_key(StringName key) {
  time_to_reach_context_key = key;
}

StringName VDAsaiBehaviorArrive::get_time_to_reach_key() const {
  return time_to_reach_context_key;
}
/////////////////////////
// VDAsaiBehaviorLeave //
/////////////////////////
VDAsaiBehaviorLeave::VDAsaiBehaviorLeave() {}

void VDAsaiBehaviorLeave::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_escape_radius", "context"), &VDAsaiBehaviorLeave::_get_escape_radius);
  ClassDB::bind_method(D_METHOD("_get_danger_radius", "context"), &VDAsaiBehaviorLeave::_get_danger_radius);
  ClassDB::bind_method(D_METHOD("_get_time_to_reach", "context"), &VDAsaiBehaviorLeave::_get_time_to_reach);

  ClassDB::bind_method(D_METHOD("set_escape_radius_key", "key"), &VDAsaiBehaviorLeave::set_escape_radius_key);
  ClassDB::bind_method(D_METHOD("get_escape_radius_key"), &VDAsaiBehaviorLeave::get_escape_radius_key);
  ClassDB::bind_method(D_METHOD("set_danger_radius_key", "key"), &VDAsaiBehaviorLeave::set_danger_radius_key);
  ClassDB::bind_method(D_METHOD("get_danger_radius_key"), &VDAsaiBehaviorLeave::get_danger_radius_key);
  ClassDB::bind_method(D_METHOD("set_time_to_reach_key", "key"), &VDAsaiBehaviorLeave::set_time_to_reach_key);
  ClassDB::bind_method(D_METHOD("get_time_to_reach_key"), &VDAsaiBehaviorLeave::get_time_to_reach_key);

  BIND_VMETHOD(MethodInfo("_get_escape_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_danger_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_time_to_reach", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "escape_radius_key"), "set_escape_radius_key", "get_escape_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "danger_radius_key"), "set_danger_radius_key", "get_danger_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "time_to_reach_key"), "set_time_to_reach_key", "get_time_to_reach_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorLeave::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 danger_pos = call("_get_danger_position", context);
  float escape_radius = call("_get_escape_radius", context);
  float danger_radius = call("_get_danger_radius", context);
  Vector3 current_pos = kinematic->get_position();
  float max_speed = kinematic->get_max_velocity();
  Vector3 direction = current_pos - danger_pos;
  float distance = direction.length();
  if(distance > danger_radius) return steering;
  float reduce;
  if(distance < escape_radius) reduce = 0.0;
  else reduce = distance / danger_radius * max_speed;
  float targetSpeed = max_speed - reduce;
  float time_to_reach = call("_get_time_to_reach", context);
  Vector3 current_velocity = kinematic->get_velocity();
  float max_force = kinematic->get_max_force();
  Vector3 desiredVelocity = direction;
  desiredVelocity.normalize();
  desiredVelocity *= targetSpeed;
  Vector3 velocity = desiredVelocity - current_velocity;
  velocity /= time_to_reach;
  if(velocity.length() > max_force) {
    velocity.normalize();
    velocity *= max_force;
  }
  steering->set_velocity(velocity);
  return steering;
}

float VDAsaiBehaviorLeave::_get_escape_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(escape_radius_context_key)) {
    float target_radius = context->get_context_value(escape_radius_context_key);
    return target_radius;
  }
  return 1.0;
}

float VDAsaiBehaviorLeave::_get_danger_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(danger_radius_context_key)) {
    float slowing_radius = context->get_context_value(danger_radius_context_key);
    return slowing_radius;
  }
  return 2.0;
}

float VDAsaiBehaviorLeave::_get_time_to_reach(Ref<VDAcContext> context) const {
  if(context->has_context_param(time_to_reach_context_key)) {
    float time_to_reach = context->get_context_value(time_to_reach_context_key);
    return time_to_reach;
  }
  return 0.1;
}

void VDAsaiBehaviorLeave::set_escape_radius_key(StringName key) {
  escape_radius_context_key = key;
}

StringName VDAsaiBehaviorLeave::get_escape_radius_key() const {
  return escape_radius_context_key;
}

void VDAsaiBehaviorLeave::set_danger_radius_key(StringName key) {
  danger_radius_context_key = key;
}

StringName VDAsaiBehaviorLeave::get_danger_radius_key() const {
  return danger_radius_context_key;
}

void VDAsaiBehaviorLeave::set_time_to_reach_key(StringName key) {
  time_to_reach_context_key = key;
}

StringName VDAsaiBehaviorLeave::get_time_to_reach_key() const {
  return time_to_reach_context_key;
}
