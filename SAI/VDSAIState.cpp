#include "VDSAIState.h"

//////////
// VDAsaiSteeringForce
//////////
VDAsaiSteeringForce::VDAsaiSteeringForce() {}

void VDAsaiSteeringForce::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &VDAsaiSteeringForce::set_velocity);
  ClassDB::bind_method(D_METHOD("get_velocity"), &VDAsaiSteeringForce::get_velocity);
  ClassDB::bind_method(D_METHOD("set_max_velocity", "max_velocity"), &VDAsaiSteeringForce::set_max_velocity);
  ClassDB::bind_method(D_METHOD("get_max_velocity"), &VDAsaiSteeringForce::get_max_velocity);
  ClassDB::bind_method(D_METHOD("set_position", "position"), &VDAsaiSteeringForce::set_position);
  ClassDB::bind_method(D_METHOD("get_position"), &VDAsaiSteeringForce::get_position);
  ClassDB::bind_method(D_METHOD("set_mass", "mass"), &VDAsaiSteeringForce::set_mass);
  ClassDB::bind_method(D_METHOD("get_mass"), &VDAsaiSteeringForce::get_mass);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_velocity"), "set_max_velocity", "get_max_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "mass"), "set_mass", "get_mass");
}

void VDAsaiSteeringForce::set_velocity(Vector3 velocity) {
  this->velocity = velocity;
}

Vector3 VDAsaiSteeringForce::get_velocity() const {
  return velocity;
}

void VDAsaiSteeringForce::set_max_velocity(float max_velocity) {
  this->max_velocity = max_velocity;
}

float VDAsaiSteeringForce::get_max_velocity() const {
  return max_velocity;
}

void VDAsaiSteeringForce::set_position(Vector3 position) {
  this->position = position;
}

Vector3 VDAsaiSteeringForce::get_position() const {
  return position;
}

void VDAsaiSteeringForce::set_mass(float mass) {
  this->mass = mass;
}

float VDAsaiSteeringForce::get_mass() const {
  return mass;
}
//////////
// VDAsaiState
//////////
VDAsaiState::VDAsaiState() {}

void VDAsaiState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_steering_force", "context"), &VDAsaiState::get_steering_force);
  ClassDB::bind_method(D_METHOD("_on_calculation", "steering_force", "context", "delta"), &VDAsaiState::_on_calculation);
  ClassDB::bind_method(D_METHOD("_apply_velocity", "new_velocity", "context", "delta"), &VDAsaiState::_apply_velocity);

  ClassDB::bind_method(D_METHOD("set_steering_force_param_key", "param_key"), &VDAsaiState::set_steering_force_param_key);
  ClassDB::bind_method(D_METHOD("get_steering_force_param_key"), &VDAsaiState::get_steering_force_param_key);

  BIND_VMETHOD(MethodInfo("get_steering_force", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_on_calculation", PropertyInfo(Variant::OBJECT, "steering_force", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiSteeringForce"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("_apply_velocity", PropertyInfo(Variant::VECTOR3, "new_velocity"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "steering_force_param_key"), "set_steering_force_param_key", "get_steering_force_param_key");
}

Ref<VDAsaiSteeringForce> VDAsaiState::get_steering_force(Ref<VDAcContext> context) {
  Ref<VDAsaiSteeringForce> steering_force;
  if(context->has_context_param(steering_force_param_key)) {
    steering_force = context->get_context_value(steering_force_param_key);
  }
  return steering_force;
}

Vector3 VDAsaiState::_on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta) {
  return Vector3(0, 0, 0);
}

void VDAsaiState::_apply_velocity(Vector3 new_velocity, Vector3 old_velocity, Ref<VDAcContext> context, float delta) {}

bool VDAsaiState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) {
  Ref<VDAsaiSteeringForce> steering_force = call("get_steering_force", context);
  if(steering_force.is_valid()) {
    Vector3 old_velocity = steering_force->get_velocity();
    Vector3 steering = call("_on_calculation", steering_force, context, delta);
    steering = truncate(steering, steering_force->get_max_velocity());
    steering = steering / steering_force->get_mass();
    Vector3 new_velocity = truncate(steering_force->get_velocity() + steering, steering_force->get_max_velocity());
    steering_force->set_velocity(new_velocity);

    call("_apply_velocity", new_velocity, old_velocity, context, delta);
    return true;
  }
  return false;
}

void VDAsaiState::set_steering_force_param_key(StringName param_key) {
  this->steering_force_param_key = param_key;
}

StringName VDAsaiState::get_steering_force_param_key() const {
  return steering_force_param_key;
}
//////////
// VDAsaiCombinedBehavior
//////////
VDAsaiCombinedBehavior::VDAsaiCombinedBehavior() {}

void VDAsaiCombinedBehavior::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_behaviors", "new_behaviors"), &VDAsaiCombinedBehavior::set_behaviors_open);
  ClassDB::bind_method(D_METHOD("get_behaviors"), &VDAsaiCombinedBehavior::get_behaviors_open);

  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "behaviors", PROPERTY_HINT_TYPE_STRING, "17/17:VDAsaiState"), "set_behaviors", "get_behaviors");
}

bool VDAsaiCombinedBehavior::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) {
  Ref<VDAsaiSteeringForce> steering_force = call("get_steering_force", context);
  if(steering_force.is_valid()) {
    Vector3 old_velocity = steering_force->get_velocity();
    Vector3 steering = Vector3(0, 0, 0);
    for(int i = 0; i < behaviors.size(); i++) {
      Ref<VDAsaiState> behavior = behaviors[i];
      Vector3 behavior_steering = behavior->call("_on_calculation", steering_force, context, delta);
      steering = steering + behavior_steering;
    }
    steering = truncate(steering, steering_force->get_max_velocity());
    steering = steering / steering_force->get_mass();
    Vector3 new_velocity = truncate(steering_force->get_velocity() + steering, steering_force->get_max_velocity());
    steering_force->set_velocity(new_velocity);
    call("_apply_velocity", new_velocity, old_velocity, context, delta);
    return true;
  }
  return false;
}

void VDAsaiCombinedBehavior::set_behaviors(Vector<Ref<VDAsaiState>> new_behaviors) {
  int added = 0;
  int removed = 0;
  for(int i = 0; i < new_behaviors.size(); i++) {
    Ref<VDAsaiState> behavior = new_behaviors[i];
    if(behaviors.find(behavior) < 0) {
      behaviors.push_back(behavior);
      added++;
    }
  }
  int behavior_index;
  for(behavior_index = 0; behavior_index < behaviors.size(); behavior_index++) {
    Ref<VDAsaiState> behavior = behaviors[behavior_index];
    if(new_behaviors.find(behavior) < 0) {
      behaviors.erase(behavior);
      behavior_index--;
      removed++;
    }
  }
  if(removed > 0 || added > 0) {
    property_list_changed_notify();
  }
}

void VDAsaiCombinedBehavior::set_behaviors_open(Array new_behaviors) {
  Vector<Ref<VDAsaiState>> new_behavior_vector;
  for(int i = 0; i < new_behaviors.size(); i++) {
    Ref<VDAsaiState> behavior = new_behaviors[i];
    if(behavior.is_valid()) {
      new_behaviors.push_back(behavior);
    } else {
      // Creating an "empty" default state
      Ref<VDAsaiState> new_behavior = Ref<VDAsaiState> (memnew(VDAsaiState));
      new_behaviors.push_back(new_behavior);
    }
  }
  set_behaviors(new_behavior_vector);
}

Vector<Ref<VDAsaiState>> VDAsaiCombinedBehavior::get_behaviors() const {
  return behaviors;
}

Array VDAsaiCombinedBehavior::get_behaviors_open() const {
  Array open_behavior;
  for(int i = 0; i < behaviors.size(); i++) {
    Ref<VDAsaiState> behavior = behaviors[i];
    open_behavior.push_back(behavior);
  }
  return open_behavior;
}
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

Vector3 VDAsaiTargetBehavior::_on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta) {
  Vector3 target = call("get_target_position", context);
  Vector3 desired = target - steering_force->get_position();
  desired.normalize();
  desired *= steering_force->get_max_velocity();
  Vector3 steering = desired - steering_force->get_velocity();
  if(direction_type == AWAY) steering *= -1;
  return steering;
}

Vector3 VDAsaiTargetBehavior::get_target_position(Ref<VDAcContext> context) const {
  Vector3 target_position = Vector3(0, 0, 0);
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
  ClassDB::bind_method(D_METHOD("get_radius", "context"), &VDAsaiNearingBehavior::get_radius);
  ClassDB::bind_method(D_METHOD("set_radius_param_key", "param_key"), &VDAsaiNearingBehavior::set_radius_param_key);
  ClassDB::bind_method(D_METHOD("get_radius_param_key"), &VDAsaiNearingBehavior::get_radius_param_key);

  BIND_VMETHOD(MethodInfo("get_radius", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "radius_param_key"), "set_radius_param_key", "get_radius_param_key");
}

Vector3 VDAsaiNearingBehavior::_on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta) {
  Vector3 steering = Vector3(0, 0, 0);
  Vector3 target_position = call("get_target_position", context);
  float slowing_radius = call("get_radius", context);
  Vector3 desired = target_position - steering_force->get_position();
  float distance = desired.length();
  if(distance < slowing_radius) {
    desired = desired.normalized() * steering_force->get_max_velocity() * (distance / slowing_radius);
  } else {
    desired = desired.normalized() * steering_force->get_max_velocity();
  }
  steering = desired - steering_force->get_velocity();
  if(direction_type == AWAY) steering *= -1;
  return steering;
}

float VDAsaiNearingBehavior::get_radius(Ref<VDAcContext> context) const {
  float radius = 1.0;
  if(context->has_context_param(radius_param_key)) {
    return context->get_context_value(radius_param_key);
  }
  return radius;
}

void VDAsaiNearingBehavior::set_radius_param_key(StringName param_key) {
  radius_param_key = param_key;
}

StringName VDAsaiNearingBehavior::get_radius_param_key() const {
  return radius_param_key;
}
