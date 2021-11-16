#ifndef VDBTSTATE_H
#define VDBTSTATE_H

#include "../core/VDState.h"

class VDAbtState : public VDAcParentState {
    GDCLASS(VDAbtState, VDAcParentState);

protected:
    static void _bind_methods();

    String param_key_btstate = "VDBT/BTStateName/State";
    bool is_active = true;

    virtual bool _succeed(Ref<VDAcContext> context);
    virtual bool _fail(Ref<VDAcContext> context);
    virtual void _run(Ref<VDAcContext> context);

    virtual bool _succeeded(Ref<VDAcContext> context);
    virtual bool _failed(Ref<VDAcContext> context);
    virtual bool _running(Ref<VDAcContext> context);

public:
    VDAbtState();

    virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
};

#endif

//Sources:
// 1. - https://blog.zhaytam.com/2020/01/07/behavior-trees-introduction/
