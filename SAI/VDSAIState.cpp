#include "VDSAIState.h"

//////////
// VDAsaiKinematic
//////////
VDAsaiKinematic::VDAsaiKinematic() {}

void VDAsaiKinematic::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &VDAsaiKinematic::set_velocity);
  ClassDB::bind_method(D_METHOD("get_velocity"), &VDAsaiKinematic::get_velocity);
  ClassDB::bind_method(D_METHOD("set_orientation", "orientation"), &VDAsaiKinematic::set_orientation);
  ClassDB::bind_method(D_METHOD("get_orientation"), &VDAsaiKinematic::get_orientation);
  ClassDB::bind_method(D_METHOD("set_max_speed", "max_speed"), &VDAsaiKinematic::set_max_speed);
  ClassDB::bind_method(D_METHOD("get_max_speed"), &VDAsaiKinematic::get_max_speed);
  ClassDB::bind_method(D_METHOD("set_position", "position"), &VDAsaiKinematic::set_position);
  ClassDB::bind_method(D_METHOD("get_position"), &VDAsaiKinematic::get_position);
  ClassDB::bind_method(D_METHOD("set_mass", "mass"), &VDAsaiKinematic::set_mass);
  ClassDB::bind_method(D_METHOD("get_mass"), &VDAsaiKinematic::get_mass);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "orientation"), "set_orientation", "get_orientation");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_speed"), "set_max_speed", "get_max_speed");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "mass"), "set_mass", "get_mass");
}

void VDAsaiKinematic::set_velocity(Vector3 velocity) {
  this->velocity = velocity;
}

Vector3 VDAsaiKinematic::get_velocity() const {
  return velocity;
}

void VDAsaiKinematic::set_orientation(float orientation) {
  this->orientation = orientation;
}
float VDAsaiKinematic::get_orientation() const {
  return orientation;
}

void VDAsaiKinematic::set_max_speed(float max_speed) {
  this->max_speed = max_speed;
}

float VDAsaiKinematic::get_max_speed() const {
  return max_speed;
}

void VDAsaiKinematic::set_position(Vector3 position) {
  this->position = position;
}

Vector3 VDAsaiKinematic::get_position() const {
  return position;
}

void VDAsaiKinematic::set_mass(float mass) {
  this->mass = mass;
}

float VDAsaiKinematic::get_mass() const {
  return mass;
}
//////////
// VDAsaiSteeringResults
//////////
VDAsaiSteeringResults::VDAsaiSteeringResults() {}

void VDAsaiSteeringResults::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &VDAsaiSteeringResults::set_velocity);
  ClassDB::bind_method(D_METHOD("get_velocity"), &VDAsaiSteeringResults::get_velocity);
  ClassDB::bind_method(D_METHOD("set_orientation", "orientation"), &VDAsaiSteeringResults::set_orientation);
  ClassDB::bind_method(D_METHOD("get_orientation"), &VDAsaiSteeringResults::get_orientation);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "orientation"), "set_orientation", "get_orientation");
}

void VDAsaiSteeringResults::set_velocity(Vector3 velocity) {
  this->velocity = velocity;
}

Vector3 VDAsaiSteeringResults::get_velocity() const {
  return velocity;
}

void VDAsaiSteeringResults::set_orientation(float orientation) {
  this->orientation = orientation;
}

float VDAsaiSteeringResults::get_orientation() const {
  return orientation;
}
//////////
// VDAsaiState
//////////
const Vector3 VDAsaiState::ZERO_VELOCITY = Vector3(0, 0, 0);

VDAsaiState::VDAsaiState() {}

void VDAsaiState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_kinematic", "context"), &VDAsaiState::get_kinematic);
  ClassDB::bind_method(D_METHOD("_on_calculation", "kinematic", "context", "delta"), &VDAsaiState::_on_calculation);
  ClassDB::bind_method(D_METHOD("_apply_velocity", "kinematic", "context", "delta"), &VDAsaiState::_apply_steering);

  ClassDB::bind_method(D_METHOD("set_kinematic_param_key", "param_key"), &VDAsaiState::set_kinematic_param_key);
  ClassDB::bind_method(D_METHOD("get_kinematic_param_key"), &VDAsaiState::get_kinematic_param_key);

  BIND_VMETHOD(MethodInfo("get_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_on_calculation", PropertyInfo(Variant::OBJECT, "kinematic", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiKinematic"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("_apply_steering", PropertyInfo(Variant::OBJECT, "kinematic", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiKinematic"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "kinematic_param_key"), "set_kinematic_param_key", "get_kinematic_param_key");
}

Ref<VDAsaiKinematic> VDAsaiState::get_kinematic(Ref<VDAcContext> context) {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(kinematic_param_key)) {
    kinematic = context->get_context_value(kinematic_param_key);
  }
  return kinematic;
}

Ref<VDAsaiSteeringResults> VDAsaiState::_on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Ref<VDAsaiSteeringResults> results = Ref<VDAsaiSteeringResults>(memnew(VDAsaiSteeringResults()));
  results->set_velocity(ZERO_VELOCITY);
  results->set_orientation(0);
  return results;
}

void VDAsaiState::_apply_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {}

bool VDAsaiState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) {
  Ref<VDAsaiKinematic> kinematic = call("get_kinematic", context);
  if(kinematic.is_valid()) {
    Ref<VDAsaiSteeringResults> results = call("_on_calculation", kinematic, context, delta);
    Vector3 new_velocity = kinematic->get_velocity() + results->get_velocity();
    float new_orientation = kinematic->get_orientation() + results->get_orientation();
    kinematic->set_velocity(new_velocity);
    kinematic->set_orientation(new_orientation);
    call("_apply_velocity", kinematic, context, delta);
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
  Ref<VDAsaiKinematic> kinematic = call("get_kinematic", context);
  if(kinematic.is_valid()) {
    Vector3 velocity = kinematic->get_velocity();
    float orientation = kinematic->get_orientation();
    for(int i = 0; i < behaviors.size(); i++) {
      Ref<VDAsaiState> behavior = behaviors[i];
      Ref<VDAsaiSteeringResults> results = behavior->call("_on_calculation", kinematic, context, delta);
      velocity += results->get_velocity();
      orientation += results->get_orientation();
    }
    // TODO: truncate and / mass
    kinematic->set_velocity(velocity);
    kinematic->set_orientation(orientation);
    call("_apply_steering", kinematic, context, delta);
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
