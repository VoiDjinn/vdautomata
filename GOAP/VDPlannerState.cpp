#include "./VDPlannerState.h"

VDAgpState::VDAgpState() {}

void VDAgpState::_bind_methods() {}

Array VDAgpState::plan_from_states ( Ref<VDAcState> state, Ref<VDAcState> goal_state ) {
    Array actions;
    return actions;
}

Array VDAgpState::get_path_to_goal ( Ref<VDAcContext> context, Ref<VDAcState> goal ) {
    Array path;
    return path;
}
