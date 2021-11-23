#include "VDSAIState.h"

//////////
// VDAsaiKinematic
//////////
VDAsaiKinematic::VDAsaiKinematic() {}

void VDAsaiKinematic::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &VDAsaiKinematic::set_velocity);
  ClassDB::bind_method(D_METHOD("get_velocity"), &VDAsaiKinematic::get_velocity);
  ClassDB::bind_method(D_METHOD("set_position", "position"), &VDAsaiKinematic::set_position);
  ClassDB::bind_method(D_METHOD("get_position"), &VDAsaiKinematic::get_position);
  ClassDB::bind_method(D_METHOD("set_orientation", "orientation"), &VDAsaiKinematic::set_orientation);
  ClassDB::bind_method(D_METHOD("get_orientation"), &VDAsaiKinematic::get_orientation);
  ClassDB::bind_method(D_METHOD("set_max_velocity", "max_velocity"), &VDAsaiKinematic::set_max_velocity);
  ClassDB::bind_method(D_METHOD("get_max_velocity"), &VDAsaiKinematic::get_max_velocity);
  ClassDB::bind_method(D_METHOD("set_max_force", "max_force"), &VDAsaiKinematic::set_max_force);
  ClassDB::bind_method(D_METHOD("get_max_force"), &VDAsaiKinematic::get_max_force);
  ClassDB::bind_method(D_METHOD("set_mass", "mass"), &VDAsaiKinematic::set_mass);
  ClassDB::bind_method(D_METHOD("get_mass"), &VDAsaiKinematic::get_mass);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "orientation"), "set_orientation", "get_orientation");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_velocity"), "set_max_velocity", "get_max_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_force"), "set_max_force", "get_max_force");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "mass"), "set_mass", "get_mass");
}

void VDAsaiKinematic::set_velocity(Vector3 velocity) {
  this->velocity = velocity;
}

Vector3 VDAsaiKinematic::get_velocity() const {
  return velocity;
}

void VDAsaiKinematic::set_position(Vector3 position) {
  this->position = position;
}

Vector3 VDAsaiKinematic::get_position() const {
  return position;
}

void VDAsaiKinematic::set_orientation(float orientation) {
  this->orientation = orientation;
}
float VDAsaiKinematic::get_orientation() const {
  return orientation;
}

void VDAsaiKinematic::set_max_velocity(float max_velocity) {
  this->max_velocity = max_velocity;
}

float VDAsaiKinematic::get_max_velocity() const {
  return max_velocity;
}

void VDAsaiKinematic::set_max_force(float max_force) {
  this->max_force = max_force;
}

float VDAsaiKinematic::get_max_force() const {
  return max_force;
}

void VDAsaiKinematic::set_mass(float mass) {
  this->mass = mass;
}

float VDAsaiKinematic::get_mass() const {
  return mass;
}
//////////
// VDAsaiSteeringData
//////////
VDAsaiSteeringData::VDAsaiSteeringData() {}

void VDAsaiSteeringData::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &VDAsaiSteeringData::set_velocity);
  ClassDB::bind_method(D_METHOD("get_velocity"), &VDAsaiSteeringData::get_velocity);
  ClassDB::bind_method(D_METHOD("set_orientation", "orientation"), &VDAsaiSteeringData::set_orientation);
  ClassDB::bind_method(D_METHOD("get_orientation"), &VDAsaiSteeringData::get_orientation);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "orientation"), "set_orientation", "get_orientation");
}

void VDAsaiSteeringData::set_velocity(Vector3 velocity) {
  this->velocity = velocity;
}

Vector3 VDAsaiSteeringData::get_velocity() const {
  return velocity;
}

void VDAsaiSteeringData::set_orientation(float orientation) {
  this->orientation = orientation;
}

float VDAsaiSteeringData::get_orientation() const {
  return orientation;
}
//////////
// VDAsaiState
//////////
const Vector3 VDAsaiState::ZERO_VELOCITY = Vector3(0, 0, 0);

VDAsaiState::VDAsaiState() {}

void VDAsaiState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_kinematic", "context"), &VDAsaiState::_get_kinematic);
  ClassDB::bind_method(D_METHOD("_get_steering", "kinematic", "context", "delta"), &VDAsaiState::_get_steering);
  ClassDB::bind_method(D_METHOD("_apply_steering", "steering", "context", "delta"), &VDAsaiState::_apply_steering);

  ClassDB::bind_method(D_METHOD("set_kinematic_param_key", "param_key"), &VDAsaiState::set_kinematic_param_key);
  ClassDB::bind_method(D_METHOD("get_kinematic_param_key"), &VDAsaiState::get_kinematic_param_key);

  BIND_VMETHOD(MethodInfo("_get_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_steering", PropertyInfo(Variant::OBJECT, "kinematic", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiKinematic"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("_apply_steering", PropertyInfo(Variant::OBJECT, "steering", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiSteeringData"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "kinematic_param_key"), "set_kinematic_param_key", "get_kinematic_param_key");
}

Ref<VDAsaiKinematic> VDAsaiState::_get_kinematic(Ref<VDAcContext> context) {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(kinematic_param_key)) {
    kinematic = context->get_context_value(kinematic_param_key);
  }
  return kinematic;
}

Ref<VDAsaiSteeringData> VDAsaiState::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  return Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
}

void VDAsaiState::_apply_steering(Ref<VDAsaiSteeringData> steering, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiKinematic> kinematic = call("_get_kinematic", context);
  if(kinematic.is_valid()) {
    kinematic->set_velocity(steering->get_velocity());
    kinematic->set_orientation(steering->get_orientation());
  }
}

bool VDAsaiState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) {
  Ref<VDAsaiKinematic> kinematic = call("_get_kinematic", context);
  if(kinematic.is_valid()) {
    Ref<VDAsaiSteeringData> steering = call("_get_steering", kinematic, context, delta);
    Vector3 steering_force = steering->get_velocity();
    float mass = kinematic->get_mass();
    float max_velocity = kinematic->get_max_velocity();
    Vector3 acceleration = steering_force / mass;
    Vector3 velocity = kinematic->get_velocity();
    velocity = velocity + acceleration * delta;
    velocity = truncate(velocity, max_velocity);
    // TODO: Update orientation
    //orientation = (float)(atan2(velocity.y, velocity.x) * RAD2DEG);
    steering->set_velocity(velocity);
    call("_apply_steering", steering, context, delta);
    return true;
  }
  return false;
}

void VDAsaiState::set_kinematic_param_key(StringName param_key) {
  this->kinematic_param_key = param_key;
}

StringName VDAsaiState::get_kinematic_param_key() const {
  return kinematic_param_key;
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
  Ref<VDAsaiKinematic> kinematic = call("_get_kinematic", context);
  if(kinematic.is_valid()) {
    Vector3 velocity = ZERO_VELOCITY;
    float orientation = 0.0;
    for(int i = 0; i < behaviors.size(); i++) {
      Ref<VDAsaiState> behavior = behaviors[i];
      Ref<VDAsaiSteeringData> result = behavior->call("_get_steering", kinematic, context, delta);
      velocity += result->get_velocity();
      orientation += result->get_orientation();
    }
    Ref<VDAsaiSteeringData> steering = Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
    call("_apply_steering", steering, context, delta);
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

