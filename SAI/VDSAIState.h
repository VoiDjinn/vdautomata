#ifndef VDASAISTATE_H
#define VDASAISTATE_H

#include "../core/VDState.h"

//////////
// VDAsaiSteeringForce
//////////
class VDAsaiSteeringForce : public Resource {
	GDCLASS(VDAsaiSteeringForce, Resource);
protected:
    static void _bind_methods();

	Vector3 velocity;
	float max_velocity;
	Vector3 position;
	float mass;
public:
	VDAsaiSteeringForce();

	void set_velocity(Vector3 velocity);
	Vector3 get_velocity() const;
	void set_max_velocity(float max_velocity);
	float get_max_velocity() const;
	void set_position(Vector3 position);
	Vector3 get_position() const;
	void set_mass(float mass);
	float get_mass() const;
};
//////////
// VDAsaiState
//////////
class VDAsaiState : public VDAcState {
	GDCLASS(VDAsaiState, VDAcState);
protected:
    static void _bind_methods();

	//Variant steering_force_param_key;
	StringName steering_force_param_key;

	virtual Ref<VDAsaiSteeringForce> get_steering_force(Ref<VDAcContext> context);
	virtual Vector3 _on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta);
	virtual void _apply_velocity(Vector3 new_velocity, Vector3 old_velocity, Ref<VDAcContext> context, float delta);

	Vector3 truncate(Vector3 vector, float max) {
		float scale = max / vector.length();
		scale = MIN(scale, 1.0);
		return vector * scale;
	}
public:
	VDAsaiState();

	virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) override;
	void set_steering_force_param_key(StringName param_key);
	StringName get_steering_force_param_key() const;
};
//////////
// VDAsaiCombinedBehavior
//////////
class VDAsaiCombinedBehavior : public VDAsaiState {
	GDCLASS(VDAsaiCombinedBehavior, VDAsaiState);
protected:
    static void _bind_methods();

	Vector<Ref<VDAsaiState>> behaviors;
public:
	VDAsaiCombinedBehavior();

	virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) override;

	void set_behaviors(Vector<Ref<VDAsaiState>> new_behaviors);
	void set_behaviors_open(Array new_behaviors);
	Vector<Ref<VDAsaiState>> get_behaviors() const;
	Array get_behaviors_open() const;
};
//////////
// VDAsaiTargetBehavior
//////////
class VDAsaiTargetBehavior : public VDAsaiState {
	GDCLASS(VDAsaiTargetBehavior, VDAsaiState);
public:
	enum DirectionType {
		TOWARDS,
		AWAY
	};
protected:
    static void _bind_methods();

	StringName target_position_param_key;
	DirectionType direction_type;

	virtual Vector3 get_target_position(Ref<VDAcContext> context) const;
	virtual Vector3 _on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta) override;
public:
	VDAsaiTargetBehavior();

	void set_target_position_param_key(StringName param_key);
	StringName get_target_position_param_key() const;
	void set_direction_type(DirectionType type);
	DirectionType get_direction_type() const;
};

VARIANT_ENUM_CAST(VDAsaiTargetBehavior::DirectionType);
//////////
// VDAsaiNearingBehavior
//////////
class VDAsaiNearingBehavior : public VDAsaiTargetBehavior {
	GDCLASS(VDAsaiNearingBehavior, VDAsaiTargetBehavior);
protected:
    static void _bind_methods();

	StringName radius_param_key;

	virtual float get_radius(Ref<VDAcContext> context) const;
	virtual Vector3 _on_calculation(Ref<VDAsaiSteeringForce> steering_force, Ref<VDAcContext> context, float delta) override;
public:
	VDAsaiNearingBehavior();

	void set_radius_param_key(StringName param_key);
	StringName get_radius_param_key() const;
};
#endif
