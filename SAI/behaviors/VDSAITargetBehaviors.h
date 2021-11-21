#ifndef VDASAITARGETBEHAVIORS_H
#define VDASAITARGETBEHAVIORS_H

#include "../VDSAIState.h"

//////////
// VDAsaiTargetBehavior / Seek/Flee
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
	virtual Vector3 _on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
public:
	VDAsaiTargetBehavior();

	void set_target_position_param_key(StringName param_key);
	StringName get_target_position_param_key() const;
	void set_direction_type(DirectionType type);
	DirectionType get_direction_type() const;
};

VARIANT_ENUM_CAST(VDAsaiTargetBehavior::DirectionType);
//////////
// VDAsaiFacingBehavior
//////////

//////////
// VDAsaiNearingBehavior / Arrive/Departure
//////////
class VDAsaiNearingBehavior : public VDAsaiTargetBehavior {
	GDCLASS(VDAsaiNearingBehavior, VDAsaiTargetBehavior);
protected:
    static void _bind_methods();

	StringName target_radius_param_key;
	StringName nearing_tolerance_param_key;
	StringName nearing_damping_param_key;

	virtual float get_target_radius(Ref<VDAcContext> context) const;
	virtual float get_nearing_tolerance(Ref<VDAcContext> context) const;
	virtual float get_nearing_damping(Ref<VDAcContext> context) const;
	virtual Vector3 _on_calculation(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	virtual float _damp_nearing_distance(float nearing_distance, Ref<VDAcContext> context, float delta);
public:
	VDAsaiNearingBehavior();

	void set_target_radius_param_key(StringName param_key);
	StringName get_target_radius_param_key() const;
	void set_nearing_tolerance_param_key(StringName param_key);
	StringName get_nearing_tolerance_param_key() const;
	void set_nearing_damping_param_key(StringName param_key);
	StringName get_nearing_damping_param_key() const;
};
#endif
