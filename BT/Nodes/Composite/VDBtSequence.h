#ifndef VDBTSEQUENCE_H
#define VDBTSEQUENCE_H

#include "./VDBtComposite.h"

class VDAbtSequence : public VDAbtComposite {
    GDCLASS(VDAbtSequence, VDAbtComposite);

protected:
    static void _bind_methods();

    virtual VDAbtNode::BTState _on_tick ( Ref<VDAcContext> context ) override;

    Array children;

public:
    VDAbtSequence();
};

#endif
