#ifndef VDBTCOMPOSITE_H
#define VDBTCOMPOSITE_H

#include "../VDBtNode.h"

class VDAbtComposite : public VDAbtNode {
    GDCLASS(VDAbtComposite, VDAbtNode);

protected:
    static void _bind_methods();

public:
    VDAbtComposite();
};

#endif
