#ifndef VDASAISTATE_H
#define VDASAISTATE_H

#include "../core/VDState.h"

//////////
// VDAsaiKinematic
//////////
class VDAsaiKinematic : public Resource {
	GDCLASS(VDAsaiKinematic, Resource);
protected:
    static void _bind_methods();

	Vector3 velocity = Vector3(0, 0, 0);
	float orientation = 0.0;
	float max_speed = 1.0;
	Vector3 position = Vector3(0, 0, 0);
	float mass = 1.0;
public:
	VDAsaiKinematic();

	void set_velocity(Vector3 velocity);
	Vector3 get_velocity() const;
	void set_orientation(float orientation);
	float get_orientation() const;
	void set_max_speed(float max_speed);
	float get_max_speed() const;
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
// common functions
protected:
	static const Vector3 ZERO_VELOCITY;

	static Vector3 truncate(Vector3 vector, float max) {
		float scale = max / vector.length();
		scale = MIN(scale, 1.0);
		return vector * scale;
	}
protected:
    static void _bind_methods();

	StringName kinematic_param_key;

	virtual Ref<VDAsaiKinematic> get_kinematic(Ref<VDAcContext> context);
	virtual Vector3 _on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta);
	virtual void _apply_velocity(Vector3 new_velocity, Vector3 old_velocity, Ref<VDAcContext> context, float delta);

public:
	VDAsaiState();

	virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) override;
	void set_kinematic_param_key(StringName param_key);
	StringName get_kinematic_param_key() const;
};
//////////
// VDAsaiCombinedBehavior / MoveManager
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
#endif
