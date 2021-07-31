#ifndef VDSAIDEFAULTBEHAVIOR_H
#define VDSAIDEFAULTBEHAVIOR_H

#include "../VDSAIState.h"

class VDAsaiDBArrive : public VDAsaiBehaviorState {
    GDCLASS(VDAsaiDBArrive, VDAsaiBehaviorState);
protected:
    static void _bind_methods();
public:
    VDAsaiDBArrive();
};

#endif
