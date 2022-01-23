#include "SeekFlee.h"
////////////////////////
// VDAsaiBehaviorSeek //
////////////////////////
VDAsaiBehaviorSeek::VDAsaiBehaviorSeek() {}

void VDAsaiBehaviorSeek::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_position", "context"), &VDAsaiBehaviorSeek::_get_target_position);

  ClassDB::bind_method(D_METHOD("set_target_position_key", "key"), &VDAsaiBehaviorSeek::set_target_position_key);
  ClassDB::bind_method(D_METHOD("get_target_position_key"), &VDAsaiBehaviorSeek::get_target_position_key);

  BIND_VMETHOD(MethodInfo("_get_target_position", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_position_key"), "set_target_position_key", "get_target_position_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorSeek::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 targtet = call("_get_target_position", context);
  Vector3 velocity = targtet - kinematic->get_position();
  velocity.normalize();
  velocity = velocity * kinematic->get_max_force();
  steering->set_velocity(velocity);
  return steering;
}

Vector3 VDAsaiBehaviorSeek::_get_target_position(Ref<VDAcContext> context) const {
  if(context->has_context_param(target_position_context_key)) {
    Vector3 target_position = context->get_context_value(target_position_context_key);
    return target_position;
  }
  return ZERO_VELOCITY;
}

void VDAsaiBehaviorSeek::set_target_position_key(StringName key) {
  target_position_context_key = key;
}

StringName VDAsaiBehaviorSeek::get_target_position_key() const {
  return target_position_context_key;
}
////////////////////////
// VDAsaiBehaviorFlee //
////////////////////////
VDAsaiBehaviorFlee::VDAsaiBehaviorFlee() {}

void VDAsaiBehaviorFlee::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_danger_position", "context"), &VDAsaiBehaviorFlee::_get_danger_position);

  ClassDB::bind_method(D_METHOD("set_danger_position_key", "key"), &VDAsaiBehaviorFlee::set_danger_position_key);
  ClassDB::bind_method(D_METHOD("get_danger_position_key"), &VDAsaiBehaviorFlee::get_danger_position_key);

  BIND_VMETHOD(MethodInfo("_get_danger_position", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "danger_position_key"), "set_danger_position_key", "get_danger_position_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorFlee::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 danger = call("_get_danger_position", context);
  Vector3 velocity = kinematic->get_position() - danger;
  velocity.normalize();
  velocity = velocity * kinematic->get_max_force();
  steering->set_velocity(velocity);
  return steering;
}

Vector3 VDAsaiBehaviorFlee::_get_danger_position(Ref<VDAcContext> context) const {
  if(context->has_context_param(danger_position_context_key)) {
    Vector3 danger_position = context->get_context_value(danger_position_context_key);
    return danger_position;
  }
  return ZERO_VELOCITY;
}

void VDAsaiBehaviorFlee::set_danger_position_key(StringName key) {
  danger_position_context_key = key;
}

StringName VDAsaiBehaviorFlee::get_danger_position_key() const {
  return danger_position_context_key;
}
