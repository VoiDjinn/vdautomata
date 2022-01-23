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
	Vector3 position = Vector3(0, 0, 0);
	float orientation = 0.0;
	float rotation = 0.0;

	float max_velocity = 1.0;
	float max_force = 1.0;
	float max_rotation = 0.0;
	float max_orientating_force = 1.0;
	float mass = 1.0;
public:
	VDAsaiKinematic();

	void set_velocity(Vector3 velocity);
	Vector3 get_velocity() const;
	void set_position(Vector3 position);
	Vector3 get_position() const;
	void set_orientation(float orientation);
	float get_orientation() const;
	void set_rotation(float rotation);
	float get_rotation() const;

	void set_max_velocity(float max_velocity);
	float get_max_velocity() const;
	void set_max_force(float max_force);
	float get_max_force() const;
	void set_max_rotation(float max_rotation);
	float get_max_rotation() const;
	void set_max_orientating_force(float max_force);
	float get_max_orientating_force() const;
	void set_mass(float mass);
	float get_mass() const;
};
//////////
// VDAsaiSteeringData
//////////
class VDAsaiSteeringData : public Reference {
	GDCLASS(VDAsaiSteeringData, Reference);
protected:
    static void _bind_methods();

	Vector3 velocity = Vector3(0, 0, 0);
	float orientation = 0.0;
public:
	VDAsaiSteeringData();

	void set_velocity(Vector3 velocity);
	Vector3 get_velocity() const;
	void set_orientation(float orientation);
	float get_orientation() const;
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
    static void _bind_methods();

	StringName kinematic_key;

	virtual Ref<VDAsaiKinematic> _get_kinematic(Ref<VDAcContext> context);
	virtual Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta);
	virtual void _apply_steering(Vector3 displacement, float orientation, Ref<VDAcContext> context, float delta);

	void apply_previous_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta);
	void update_kinematic(Ref<VDAsaiKinematic> kinematic, Ref<VDAsaiSteeringData> steering, float delta);
public:
	VDAsaiState();

	static float map_to_range(float rotation);

	virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta) override;
	void set_kinematic_key(StringName key);
	StringName get_kinematic_key() const;
};
#endif
