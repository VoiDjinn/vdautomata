#include "AlignFace.h"
#include "core/math/math_funcs.h"
/////////////////////////
// VDAsaiBehaviorAlign //
/////////////////////////
VDAsaiBehaviorAlign::VDAsaiBehaviorAlign() {}

void VDAsaiBehaviorAlign::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_kinematic", "context"), &VDAsaiBehaviorAlign::_get_target_kinematic);
  ClassDB::bind_method(D_METHOD("_get_target_radius", "context"), &VDAsaiBehaviorAlign::_get_target_radius);
  ClassDB::bind_method(D_METHOD("_get_slowing_radius", "context"), &VDAsaiBehaviorAlign::_get_slowing_radius);
  ClassDB::bind_method(D_METHOD("_get_time_to_reach", "context"), &VDAsaiBehaviorAlign::_get_time_to_reach);

  ClassDB::bind_method(D_METHOD("set_target_kinematic_key", "key"), &VDAsaiBehaviorAlign::set_target_kinematic_key);
  ClassDB::bind_method(D_METHOD("get_target_kinematic_key"), &VDAsaiBehaviorAlign::get_target_kinematic_key);
  ClassDB::bind_method(D_METHOD("set_target_radius_key", "key"), &VDAsaiBehaviorAlign::set_target_radius_key);
  ClassDB::bind_method(D_METHOD("get_target_radius_key"), &VDAsaiBehaviorAlign::get_target_radius_key);
  ClassDB::bind_method(D_METHOD("set_slowing_radius_key", "key"), &VDAsaiBehaviorAlign::set_slowing_radius_key);
  ClassDB::bind_method(D_METHOD("get_slowing_radius_key"), &VDAsaiBehaviorAlign::get_slowing_radius_key);
  ClassDB::bind_method(D_METHOD("set_time_to_reach_key", "key"), &VDAsaiBehaviorAlign::set_time_to_reach_key);
  ClassDB::bind_method(D_METHOD("get_time_to_reach_key"), &VDAsaiBehaviorAlign::get_time_to_reach_key);

  BIND_VMETHOD(MethodInfo("_get_target_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_target_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_slowing_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_time_to_reach", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_kinematic_key"), "set_target_kinematic_key", "get_target_kinematic_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_radius_key"), "set_target_radius_key", "get_target_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "slowing_radius_key"), "set_slowing_radius_key", "get_slowing_radius_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "time_to_reach_key"), "set_time_to_reach_key", "get_time_to_reach_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorAlign::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Ref<VDAsaiKinematic> target_kinematic = call("_get_target_kinematic", context);
  float target_radius = call("_get_target_radius", context);
  float slowing_radius = call("_get_slowing_radius", context);
  float target_orientation = target_kinematic->get_orientation();
  float current_orientation = kinematic->get_orientation();
  float rotation = target_orientation - current_orientation;
  rotation = VDAsaiState::map_to_range(rotation);
  float rotation_size =  Math::abs(rotation);
  if(rotation_size < target_radius) return steering;
  float target_rotation = 0.0;
  float max_rotation = kinematic->get_max_rotation();
  if(target_rotation > slowing_radius) target_rotation = max_rotation;
  else target_rotation = max_rotation * rotation_size / slowing_radius;
  target_rotation *= rotation / rotation_size;
  float time_to_reach = call("_get_time_to_reach", context);
  float current_rotation = kinematic->get_orientation();
  float angular = target_rotation - current_rotation;
  angular /= time_to_reach;
  float angular_accel = Math::abs(angular);
  float max_orientation_force = kinematic->get_max_orientating_force();
  if(angular_accel > max_orientation_force) {
    angular /= angular_accel;
    angular *= max_orientation_force;
  }
  steering->set_orientation(angular);
  return steering;
}

Ref<VDAsaiKinematic> VDAsaiBehaviorAlign::_get_target_kinematic(Ref<VDAcContext> context) const {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(target_kinematic_context_key)) {
    kinematic = context->get_context_value(target_kinematic_context_key);
    return kinematic;
  }
  return kinematic;
}

float VDAsaiBehaviorAlign::_get_target_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(target_radius_context_key)) {
    float target_radius = context->get_context_value(target_radius_context_key);
    return target_radius;
  }
  return 1.0;
}

float VDAsaiBehaviorAlign::_get_slowing_radius(Ref<VDAcContext> context) const {
  if(context->has_context_param(slowing_radius_context_key)) {
    float slowing_radius = context->get_context_value(slowing_radius_context_key);
    return slowing_radius;
  }
  return 2.0;
}

float VDAsaiBehaviorAlign::_get_time_to_reach(Ref<VDAcContext> context) const {
  if(context->has_context_param(time_to_reach_context_key)) {
    float time_to_reach = context->get_context_value(time_to_reach_context_key);
    return time_to_reach;
  }
  return 0.1;
}

void VDAsaiBehaviorAlign::set_target_kinematic_key(StringName key) {
  target_kinematic_context_key = key;
}

StringName VDAsaiBehaviorAlign::get_target_kinematic_key() const {
  return target_kinematic_context_key;
}

void VDAsaiBehaviorAlign::set_target_radius_key(StringName key) {
  target_radius_context_key = key;
}

StringName VDAsaiBehaviorAlign::get_target_radius_key() const {
  return target_radius_context_key;
}

void VDAsaiBehaviorAlign::set_slowing_radius_key(StringName key) {
  slowing_radius_context_key = key;
}

StringName VDAsaiBehaviorAlign::get_slowing_radius_key() const {
  return slowing_radius_context_key;
}

void VDAsaiBehaviorAlign::set_time_to_reach_key(StringName key) {
  time_to_reach_context_key = key;
}

StringName VDAsaiBehaviorAlign::get_time_to_reach_key() const {
  return time_to_reach_context_key;
}
////////////////////////
// VDAsaiBehaviorFace //
////////////////////////
VDAsaiBehaviorFace::VDAsaiBehaviorFace() {}

void VDAsaiBehaviorFace::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_position", "context"), &VDAsaiBehaviorFace::_get_target_position);

  ClassDB::bind_method(D_METHOD("set_target_position_key", "key"), &VDAsaiBehaviorFace::set_target_position_key);
  ClassDB::bind_method(D_METHOD("get_target_position_key"), &VDAsaiBehaviorFace::get_target_position_key);

  BIND_VMETHOD(MethodInfo("_get_target_position", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_position_key"), "set_target_position_key", "get_target_position_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorFace::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {

  Ref<VDAsaiKinematic> target_kinematic = call("_get_target_kinematic", context);
  float target_orientation = target_kinematic->get_orientation();
  Vector3 target_pos = call("_get_target_position", context);
  Vector3 current_pos = kinematic->get_position();
  Vector3 direction = target_pos - current_pos;
  if(direction.length() > 0.0f) {
    target_orientation = Math::atan2(direction.x, direction.z);
    target_orientation = Math::rad2deg(target_orientation);
  }
  // TODO: implement via parent call
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  float target_radius = call("_get_target_radius", context);
  float slowing_radius = call("_get_slowing_radius", context);
  float current_orientation = kinematic->get_orientation();
  float rotation = target_orientation - current_orientation;
  rotation = VDAsaiState::map_to_range(rotation);
  float rotation_size =  Math::abs(rotation);
  if(rotation_size < target_radius) return steering;
  float target_rotation = 0.0;
  float max_rotation = kinematic->get_max_rotation();
  if(target_rotation > slowing_radius) target_rotation = max_rotation;
  else target_rotation = max_rotation * rotation_size / slowing_radius;
  target_rotation *= rotation / rotation_size;
  float time_to_reach = call("_get_time_to_reach", context);
  float current_rotation = kinematic->get_rotation();
  float angular = target_rotation - current_rotation;
  angular /= time_to_reach;
  float angular_accel = Math::abs(angular);
  float max_orientation_force = kinematic->get_max_orientating_force();
  if(angular_accel > max_orientation_force) {
    angular /= angular_accel;
    angular *= max_orientation_force;
  }
  steering->set_orientation(angular);
  return steering;
}

Vector3 VDAsaiBehaviorFace::_get_target_position(Ref<VDAcContext> context) const {
  if(context->has_context_param(target_position_context_key)) {
    Vector3 target_position = context->get_context_value(target_position_context_key);
    return target_position;
  }
  return ZERO_VELOCITY;
}

void VDAsaiBehaviorFace::set_target_position_key(StringName key) {
  target_position_context_key = key;
}

StringName VDAsaiBehaviorFace::get_target_position_key() const {
  return target_position_context_key;
}
