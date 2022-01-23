#ifndef VDSAISEEKFLEE_H
#define VDSAISEEKFLEE_H

#include "../VDSAIState.h"

class VDAsaiBehaviorSeek : public VDAsaiState {
    GDCLASS(VDAsaiBehaviorSeek, VDAsaiState);
protected:
    static void _bind_methods();

	StringName target_position_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	Vector3 _get_target_position(Ref<VDAcContext> context) const;
public:
    VDAsaiBehaviorSeek();

	void set_target_position_key(StringName key);
	StringName get_target_position_key() const;
};

class VDAsaiBehaviorFlee : public VDAsaiState {
    GDCLASS(VDAsaiBehaviorFlee, VDAsaiState);
protected:
    static void _bind_methods();

	StringName danger_position_context_key;

	Ref<VDAsaiSteeringData> _get_steering(Ref<VDAsaiKinematic> kinematic, Ref<VDAcContext> context, float delta) override;
	Vector3 _get_danger_position(Ref<VDAcContext> context) const;
public:
    VDAsaiBehaviorFlee();

	void set_danger_position_key(StringName key);
	StringName get_danger_position_key() const;
};

#endif
