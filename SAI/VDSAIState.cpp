#include "VDSAIState.h"
#include "core/math/math_funcs.h"

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
  ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &VDAsaiKinematic::set_orientation);
  ClassDB::bind_method(D_METHOD("get_rotation"), &VDAsaiKinematic::get_orientation);

  ClassDB::bind_method(D_METHOD("set_max_velocity", "max_velocity"), &VDAsaiKinematic::set_max_velocity);
  ClassDB::bind_method(D_METHOD("get_max_velocity"), &VDAsaiKinematic::get_max_velocity);
  ClassDB::bind_method(D_METHOD("set_max_force", "max_force"), &VDAsaiKinematic::set_max_force);
  ClassDB::bind_method(D_METHOD("get_max_force"), &VDAsaiKinematic::get_max_force);
  ClassDB::bind_method(D_METHOD("set_max_rotation", "max_rotation"), &VDAsaiKinematic::set_max_force);
  ClassDB::bind_method(D_METHOD("get_max_rotation"), &VDAsaiKinematic::get_max_force);
  ClassDB::bind_method(D_METHOD("set_max_orientating_force", "max_force"), &VDAsaiKinematic::set_max_orientating_force);
  ClassDB::bind_method(D_METHOD("get_max_orientating_force"), &VDAsaiKinematic::get_max_orientating_force);
  ClassDB::bind_method(D_METHOD("set_mass", "mass"), &VDAsaiKinematic::set_mass);
  ClassDB::bind_method(D_METHOD("get_mass"), &VDAsaiKinematic::get_mass);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "velocity"), "set_velocity", "get_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "orientation"), "set_orientation", "get_orientation");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation"), "set_rotation", "get_rotation");

  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_velocity"), "set_max_velocity", "get_max_velocity");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_force"), "set_max_force", "get_max_force");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_rotation"), "set_max_rotation", "get_max_rotation");
  ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_orientating_force"), "set_max_orientating_force", "get_max_orientating_force");
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

void VDAsaiKinematic::set_rotation(float rotation) {
  this->rotation = rotation;
}

float VDAsaiKinematic::get_rotation() const {
  return rotation;
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

void VDAsaiKinematic::set_max_rotation(float max_rotation) {
  this->max_rotation = max_rotation;
}

float VDAsaiKinematic::get_max_rotation() const {
  return max_rotation;
}

void VDAsaiKinematic::set_max_orientating_force(float max_force) {
  max_orientating_force = max_force;
}

float VDAsaiKinematic::get_max_orientating_force() const {
  return max_orientating_force;
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

VDAsaiState::VDAsaiState() {
  this->set_listening_to_updates(false);
}

void VDAsaiState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_get_kinematic", "context"), &VDAsaiState::_get_kinematic);
  ClassDB::bind_method(D_METHOD("_get_steering", "kinematic", "context", "delta"), &VDAsaiState::_get_steering);
  ClassDB::bind_method(D_METHOD("_apply_steering", "displacement", "orientation", "context", "delta"), &VDAsaiState::_apply_steering);

  //ClassDB::bind_method(D_METHOD("map_to_range", "rotation"), &VDAsaiState::map_to_range);
  ClassDB::bind_method(D_METHOD("set_kinematic_key", "key"), &VDAsaiState::set_kinematic_key);
  ClassDB::bind_method(D_METHOD("get_kinematic_key"), &VDAsaiState::get_kinematic_key);

  BIND_VMETHOD(MethodInfo("_get_kinematic", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
  BIND_VMETHOD(MethodInfo("_get_steering", PropertyInfo(Variant::OBJECT, "kinematic", PROPERTY_HINT_RESOURCE_TYPE, "VDAsaiKinematic"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));
  BIND_VMETHOD(MethodInfo("_apply_steering", PropertyInfo(Variant::VECTOR3, "displacement"), PropertyInfo(Variant::REAL, "orientation"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::REAL, "delta")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "kinematic_key"), "set_kinematic_key", "get_kinematic_key");
}

Ref<VDAsaiKinematic> VDAsaiState::_get_kinematic(Ref<VDAcContext> context) {
  Ref<VDAsaiKinematic> kinematic;
  if(context->has_context_param(kinematic_key)) {
    kinematic = context->get_context_value(kinematic_key);
  }
  return kinematic;
}

Ref<VDAsaiSteeringData> VDAsaiState::_get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  return Ref<VDAsaiSteeringData>(memnew(VDAsaiSteeringData()));
}

void VDAsaiState::_apply_steering(Vector3 displacement, float orientation, Ref<VDAcContext> context, float delta) {
    /*
     * For example:
    transform.Translate(displacement, Space.World);
    transform.rotation = new Quaternion();
    transform.Rotate(Vector3.up, orientation);
    */
}

float VDAsaiState::map_to_range(float rotation) {
  float mapped_rotation = Math::fmod(rotation, 360.0f);
  if(Math::abs(mapped_rotation) > 180.0f) {
    if(mapped_rotation < 0.0f)
      mapped_rotation += 360.0f;
    else
      mapped_rotation -= 360.0f;
  }
  return mapped_rotation;
}

bool VDAsaiState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) {
  Ref<VDAsaiKinematic> kinematic = call("_get_kinematic", context);
  if(kinematic.is_valid()) {
    apply_previous_steering(kinematic, context, delta);
    // TODO: reset steerings for combining behaviors
    Ref<VDAsaiSteeringData> steering = call("_get_steering", kinematic, context, delta);
    update_kinematic(kinematic, steering, delta);
    return true;
  }
  return false;
}

void VDAsaiState::apply_previous_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) {
  Vector3 velocity = kinematic->get_velocity();
  Vector3 displacement = velocity * delta;
  float rotation = kinematic->get_rotation();
  float orientation = kinematic->get_orientation() + rotation * delta;
  if(orientation < 0.0f)
    orientation += 360.0f;
  else if(orientation > 360.0f)
    orientation -= 360.0f;
  call("_apply_steering", displacement, orientation, context, delta);
}

void VDAsaiState::update_kinematic(Ref<VDAsaiKinematic> kinematic, Ref<VDAsaiSteeringData> steering, float delta) {
  Vector3 velocity = kinematic->get_velocity();
  float rotation = 0.0f;
  float max_speed = kinematic->get_max_velocity();
  velocity += steering->get_velocity() * delta;
  rotation += steering->get_orientation() * delta;
  if(velocity.length() > max_speed) {
    velocity.normalize();
    velocity = velocity * max_speed;
  }
  if(steering->get_orientation() == 0.0f) {
    rotation = 0.0f;
  }
  if(steering->get_velocity().length_squared() == 0.0f) {
    velocity = ZERO_VELOCITY;
  }
  kinematic->set_velocity(velocity);
  kinematic->set_rotation(rotation);
}

void VDAsaiState::set_kinematic_key(StringName key) {
  this->kinematic_key = key;
}

StringName VDAsaiState::get_kinematic_key() const {
  return kinematic_key;
}
