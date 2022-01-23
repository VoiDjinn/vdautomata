#include "PursueEvade.h"

//////////////////////////
// VDAsaiBehaviorPursue //
//////////////////////////
VDAsaiBehaviorPursue::VDAsaiBehaviorPursue() {}

void VDAsaiBehaviorPursue::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_target_kinematic", "context"), &VDAsaiBehaviorPursue::_get_target_kinematic);
  ClassDB::bind_method(D_METHOD("_get_max_prediction", "context"), &VDAsaiBehaviorPursue::_get_max_prediction);

  ClassDB::bind_method(D_METHOD("set_target_kinematic_key", "key"), &VDAsaiBehaviorPursue::set_target_kinematic_key);
  ClassDB::bind_method(D_METHOD("get_target_kinematic_key"), &VDAsaiBehaviorPursue::get_target_kinematic_key);
  ClassDB::bind_method(D_METHOD("set_max_prediction_key", "key"), &VDAsaiBehaviorPursue::set_max_prediction_key);
  ClassDB::bind_method(D_METHOD("get_max_prediction_key"), &VDAsaiBehaviorPursue::get_max_prediction_key);

  BIND_VMETHOD(MethodInfo("_get_target_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_max_prediction", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "target_kinematic_key"), "set_target_kinematic_key", "get_target_kinematic_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "max_prediction"), "set_max_prediction_key", "get_max_prediction_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorPursue::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Vector3 current_velocity = kinematic->get_velocity();
  Vector3 current_pos = kinematic->get_position();
  Ref<VDAsaiKinematic> target_kinematic = call("_get_target_kinematic", context);
  Vector3 target_velocity = target_kinematic->get_velocity();
  Vector3 target_pos = target_kinematic->get_position();
  float max_prediction = call("_get_max_prediction", context);
  Vector3 direction = target_pos - current_pos;
  float distance = direction.length();
  float speed = current_velocity.length();
  float prediction;
  if(speed <= distance / max_prediction)
    prediction = max_prediction;
  else
    prediction = distance / speed;
  //Vector3 target_pos = targetAux.transform.position;
  target_pos += target_velocity * prediction;

  //TODO: maybe seperate later to call parent method
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 velocity = target_pos - current_pos;
  velocity.normalize();
  velocity = velocity * kinematic->get_max_force();
  steering->set_velocity(velocity);
  return steering;
}

Ref<VDAsaiKinematic> VDAsaiBehaviorPursue::_get_target_kinematic(Ref<VDAcContext> context) const {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(target_kinematic_context_key)) {
    kinematic = context->get_context_value(target_kinematic_context_key);
    return kinematic;
  }
  return kinematic;
}

float VDAsaiBehaviorPursue::_get_max_prediction(Ref<VDAcContext> context) const {
  if(context->has_context_param(max_prediction_context_key)) {
    float prediction = context->get_context_value(max_prediction_context_key);
    return prediction;
  }
  return 1.0;
}

void VDAsaiBehaviorPursue::set_target_kinematic_key(StringName key) {
  target_kinematic_context_key = key;
}

StringName VDAsaiBehaviorPursue::get_target_kinematic_key() const {
  return target_kinematic_context_key;
}

void VDAsaiBehaviorPursue::set_max_prediction_key(StringName key) {
  max_prediction_context_key = key;
}

StringName VDAsaiBehaviorPursue::get_max_prediction_key() const {
  return max_prediction_context_key;
}
////////////////////////
// VDAsaiBehaviorEvade //
////////////////////////
VDAsaiBehaviorEvade::VDAsaiBehaviorEvade() {}

void VDAsaiBehaviorEvade::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_danger_kinematic", "context"), &VDAsaiBehaviorEvade::_get_danger_kinematic);
  ClassDB::bind_method(D_METHOD("_get_max_prediction", "context"), &VDAsaiBehaviorEvade::_get_max_prediction);

  ClassDB::bind_method(D_METHOD("set_danger_kinematic_key", "key"), &VDAsaiBehaviorEvade::set_danger_kinematic_key);
  ClassDB::bind_method(D_METHOD("get_danger_kinematic_key"), &VDAsaiBehaviorEvade::get_danger_kinematic_key);
  ClassDB::bind_method(D_METHOD("set_max_prediction_key", "key"), &VDAsaiBehaviorEvade::set_max_prediction_key);
  ClassDB::bind_method(D_METHOD("get_max_prediction_key"), &VDAsaiBehaviorEvade::get_max_prediction_key);

  BIND_VMETHOD(MethodInfo("_get_danger_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_max_prediction", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "danger_kinematic_key"), "set_danger_kinematic_key", "get_danger_kinematic_key");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "max_prediction"), "set_max_prediction_key", "get_max_prediction_key");
}

Ref<VDAsaiSteeringData> VDAsaiBehaviorEvade::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Vector3 current_velocity = kinematic->get_velocity();
  Vector3 current_pos = kinematic->get_position();
  Ref<VDAsaiKinematic> danger_kinematic = call("_get_danger_kinematic", context);
  Vector3 target_velocity = danger_kinematic->get_velocity();
  Vector3 danger_pos = danger_kinematic->get_position();
  float max_prediction = call("_get_max_prediction", context);
  Vector3 direction = danger_pos - current_pos;
  float distance = direction.length();
  float speed = current_velocity.length();
  float prediction;
  if(speed <= distance / max_prediction)
    prediction = max_prediction;
  else
    prediction = distance / speed;
  //Vector3 danger_pos = targetAux.transform.position;
  danger_pos += target_velocity * prediction;

  //TODO: maybe seperate later to call parent method
  Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
  Vector3 velocity = current_pos- danger_pos;
  velocity.normalize();
  velocity = velocity * kinematic->get_max_force();
  steering->set_velocity(velocity);
  return steering;
}

Ref<VDAsaiKinematic> VDAsaiBehaviorEvade::_get_danger_kinematic(Ref<VDAcContext> context) const {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(danger_kinematic_context_key)) {
    kinematic = context->get_context_value(danger_kinematic_context_key);
    return kinematic;
  }
  return kinematic;
}

float VDAsaiBehaviorEvade::_get_max_prediction(Ref<VDAcContext> context) const {
  if(context->has_context_param(max_prediction_context_key)) {
    float prediction = context->get_context_value(max_prediction_context_key);
    return prediction;
  }
  return 1.0;
}

void VDAsaiBehaviorEvade::set_danger_kinematic_key(StringName key) {
  danger_kinematic_context_key = key;
}

StringName VDAsaiBehaviorEvade::get_danger_kinematic_key() const {
  return danger_kinematic_context_key;
}

void VDAsaiBehaviorEvade::set_max_prediction_key(StringName key) {
  max_prediction_context_key = key;
}

StringName VDAsaiBehaviorEvade::get_max_prediction_key() const {
  return max_prediction_context_key;
}
