#ifndef VDSAIPURSUEEVADE_H
#define VDSAIPURSUEEVADE_H

#include "../VDSAIState.h"

class VDAsaiBehaviorPursue : public VDAsaiState {
    GDCLASS(VDAsaiBehaviorPursue, VDAsaiState);
protected:
    static void _bind_methods();

	StringName target_kinematic_context_key;
	StringName max_prediction_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	Ref<VDAsaiKinematic> _get_target_kinematic(Ref<VDAcContext> context) const;
	float _get_max_prediction(Ref<VDAcContext> context) const;
public:
    VDAsaiBehaviorPursue();

	void set_target_kinematic_key(StringName key);
	StringName get_target_kinematic_key() const;
	void set_max_prediction_key(StringName key);
	StringName get_max_prediction_key() const;
};

class VDAsaiBehaviorEvade : public VDAsaiState {
    GDCLASS(VDAsaiBehaviorEvade, VDAsaiState);
protected:
    static void _bind_methods();

	StringName danger_kinematic_context_key;
	StringName max_prediction_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	Ref<VDAsaiKinematic> _get_danger_kinematic(Ref<VDAcContext> context) const;
	float _get_max_prediction(Ref<VDAcContext> context) const;
public:
    VDAsaiBehaviorEvade();

	void set_danger_kinematic_key(StringName key);
	StringName get_danger_kinematic_key() const;
	void set_max_prediction_key(StringName key);
	StringName get_max_prediction_key() const;
};

#endif
