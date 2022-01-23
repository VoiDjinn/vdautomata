#ifndef VDASAIALIGNFACING_H
#define VDASAIALIGNFACING_H

#include "../VDSAIState.h"

/////////////////////////
// VDAsaiBehaviorAlign //
/////////////////////////
class VDAsaiBehaviorAlign : public VDAsaiState {
	GDCLASS(VDAsaiBehaviorAlign, VDAsaiState);
protected:
    static void _bind_methods();

	StringName target_kinematic_context_key;
	StringName target_radius_context_key;
	StringName slowing_radius_context_key;
	StringName time_to_reach_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;

	Ref<VDAsaiKinematic> _get_target_kinematic(Ref<VDAcContext> context) const;
	float _get_target_radius(Ref<VDAcContext> context) const;
	float _get_slowing_radius(Ref<VDAcContext> context) const;
	float _get_time_to_reach(Ref<VDAcContext> context) const;
public:
	VDAsaiBehaviorAlign();

	void set_target_kinematic_key(StringName key);
	StringName get_target_kinematic_key() const;
	void set_target_radius_key(StringName key);
	StringName get_target_radius_key() const;
	void set_slowing_radius_key(StringName key);
	StringName get_slowing_radius_key() const;
	void set_time_to_reach_key(StringName key);
	StringName get_time_to_reach_key() const;
};
////////////////////////
// VDAsaiBehaviorFace //
////////////////////////
class VDAsaiBehaviorFace : public VDAsaiBehaviorAlign {
	GDCLASS(VDAsaiBehaviorFace, VDAsaiBehaviorAlign);
protected:
    static void _bind_methods();

	StringName target_position_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	Vector3 _get_target_position(Ref<VDAcContext> context) const;
public:
	VDAsaiBehaviorFace();

	void set_target_position_key(StringName key);
	StringName get_target_position_key() const;
};
#endif
