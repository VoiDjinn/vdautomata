#ifndef VDBTSELECTOR_H
#define VDBTSELECTOR_H

#include "./VDBtComposite.h"
#include "../VDBtNode.h"

class VDAbtSelector : public VDAbtComposite {
    GDCLASS(VDAbtSelector, VDAbtComposite);

protected:
    static void _bind_methods();

    virtual VDAbtNode::BTState _on_tick ( Ref<VDAcContext> context ) override;

    Array children;

public:
    VDAbtSelector();
};

#endif
