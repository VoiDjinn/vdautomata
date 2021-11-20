#ifndef VDSAIDEFAULTBEHAVIOR_H
#define VDSAIDEFAULTBEHAVIOR_H

#include "../VDSAIState.h"

class VDAsaiDBArrive : public VDAsaiState {
    GDCLASS(VDAsaiDBArrive, VDAsaiState);
protected:
    static void _bind_methods();
public:
    VDAsaiDBArrive();
};

#endif
