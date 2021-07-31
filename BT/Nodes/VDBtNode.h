#ifndef VDBTNODE_H
#define VDBTNODE_H

#include "core/reference.h"
#include "../VDBtState.h"

class VDAbtNode : public Reference {
    GDCLASS(VDAbtNode, Reference);

public:
    enum BTState {
        RUNNING,
        SUCCESS,
        FAIL
    };

protected:
    static void _bind_methods();

    String param_key_btstate = "VDBT/BTStateName/State";
    bool is_active = true;
    Ref<VDAbtState> root;

    virtual VDAbtNode::BTState _on_tick(Ref<VDAcContext> context);

    virtual VDAbtNode::BTState _succeed(Ref<VDAcContext> context);
    virtual VDAbtNode::BTState _fail(Ref<VDAcContext> context);
    virtual void _run(Ref<VDAcContext> context);

    virtual bool _succeeded(Ref<VDAcContext> context);
    virtual bool _failed(Ref<VDAcContext> context);
    virtual bool _running(Ref<VDAcContext> context);

public:
    VDAbtNode();

    virtual VDAbtNode::BTState tick(Ref<VDAcContext> context);
};

VARIANT_ENUM_CAST(VDAbtNode::BTState);

#endif
