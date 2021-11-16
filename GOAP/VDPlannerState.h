#ifndef VDPLANNERSTATE_H
#define VDPLANNERSTATE_H

#include "../core/VDState.h"

class VDAgpState : public VDAcParentState {
    GDCLASS(VDAgpState, VDAcParentState);

    Array state_atoms;
    Array actions;

protected:
    static void _bind_methods();

public:
    VDAgpState();

    Array plan_from_states(Ref<VDAcState> state, Ref<VDAcState> goal_state);
    Array get_path_to_goal(Ref<VDAcContext> context, Ref<VDAcState> goal);
};

#endif
