#ifndef VDASAIARRIVELEAVE_H
#define VDASAIARRIVELEAVE_H

#include "SeekFlee.h"
//////////////////////////
// VDAsaiBehaviorArrive //
//////////////////////////
class VDAsaiBehaviorArrive : public VDAsaiBehaviorSeek {
	GDCLASS(VDAsaiBehaviorArrive, VDAsaiBehaviorSeek);
protected:
    static void _bind_methods();

	StringName target_radius_context_key;
	StringName slowing_radius_context_key;
	StringName time_to_reach_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;

	float _get_target_radius(Ref<VDAcContext> context) const;
	float _get_slowing_radius(Ref<VDAcContext> context) const;
	float _get_time_to_reach(Ref<VDAcContext> context) const;
public:
	VDAsaiBehaviorArrive();

	void set_target_radius_key(StringName key);
	StringName get_target_radius_key() const;
	void set_slowing_radius_key(StringName key);
	StringName get_slowing_radius_key() const;
	void set_time_to_reach_key(StringName key);
	StringName get_time_to_reach_key() const;
};
/////////////////////////
// VDAsaiBehaviorLeave //
/////////////////////////
class VDAsaiBehaviorLeave : public VDAsaiBehaviorFlee {
	GDCLASS(VDAsaiBehaviorLeave, VDAsaiBehaviorFlee);
protected:
    static void _bind_methods();

	StringName escape_radius_context_key;
	StringName danger_radius_context_key;
	StringName time_to_reach_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;

	float _get_escape_radius(Ref<VDAcContext> context) const;
	float _get_danger_radius(Ref<VDAcContext> context) const;
	float _get_time_to_reach(Ref<VDAcContext> context) const;
public:
	VDAsaiBehaviorLeave();

	void set_escape_radius_key(StringName key);
	StringName get_escape_radius_key() const;
	void set_danger_radius_key(StringName key);
	StringName get_danger_radius_key() const;
	void set_time_to_reach_key(StringName key);
	StringName get_time_to_reach_key() const;
};
#endif
