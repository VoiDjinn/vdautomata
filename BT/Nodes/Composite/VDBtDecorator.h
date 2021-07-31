#ifndef VDBTDECORATOR_H
#define VDBTDECORATOR_H

#include "./VDBtComposite.h"
#include "../VDBtNode.h"

class VDAbtDecorator : public VDAbtComposite {
    GDCLASS(VDAbtDecorator, VDAbtComposite);

    Ref<VDAbtNode> node;

protected:
    static void _bind_methods();

public:
    VDAbtDecorator();
};

#endif
