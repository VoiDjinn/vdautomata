#include "VDHsmState.h"

//////////
// VDAhsmState
//////////
VDAhsmState::VDAhsmState() {}

void VDAhsmState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("transit", "target", "context"), &VDAhsmState::transit);
    ClassDB::bind_method(D_METHOD("get_viable_transition", "context", "event"), &VDAhsmState::get_viable_transition, DEFVAL(NULL));

    ClassDB::bind_method(D_METHOD("create_transition", "from", "to", "name", "trigger", "conditions"), &VDAhsmState::create_transition_open, DEFVAL(""), DEFVAL(NULL), DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("add_transition", "transition"), &VDAhsmState::add_transition);
    ClassDB::bind_method(D_METHOD("remove_transition", "transition"), &VDAhsmState::remove_transition);
    ClassDB::bind_method(D_METHOD("set_transitions", "transitions"), &VDAhsmState::set_transitions_open);
    ClassDB::bind_method(D_METHOD("get_transitions"), &VDAhsmState::get_transitions_open);

    ClassDB::bind_method(D_METHOD("set_default_substate", "substate"), &VDAhsmState::set_default_substate);
    ClassDB::bind_method(D_METHOD("get_default_substate"), &VDAhsmState::get_default_substate);
    ClassDB::bind_method(D_METHOD("set_has_regions", "has"), &VDAhsmState::set_has_regions);
    ClassDB::bind_method(D_METHOD("has_regions"), &VDAhsmState::has_regions);

    //internal
    ClassDB::bind_method(D_METHOD("_handle_context_param_updated", "param", "new_value", "old_value"), &VDAhsmState::_handle_context_param_updated);
    ClassDB::bind_method(D_METHOD("_handle_context_params_updated"), &VDAhsmState::_handle_context_params_updated);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "substates"), "set_substates", "get_substates");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transitions"), "set_transitions", "get_transitions");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "default_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), "set_default_substate", "get_default_substate");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_regions"), "set_has_regions", "has_regions");

    ADD_SIGNAL(MethodInfo("transition_done", PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition"), PropertyInfo(Variant::OBJECT, "new_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "old_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "VDAcEvent")));
    ADD_SIGNAL(MethodInfo("transition_failed", PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition"), PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "VDAcEvent")));
    ADD_SIGNAL(MethodInfo("faulted"));
}

//TODO: add mechanism to get transtion or remove(?)
bool VDAhsmState::transit(Ref<VDAcState> target, Ref<VDAcContext> context)
{
    Ref<VDAcStateData> data = context->get_state_data(this);
    Ref<VDAcState> current_state = data->get_current_state();
    if(target.is_valid()) {
        if(is_debug_mode()) print_line(to_state_string() + ": Transit to state - " + target->to_state_string());
        current_state->exit(context);
        data->set_current_state(target);
        //if(target_transition.is_valid() && target_transition->get_transition_delegate().is_valid()) {
        //TODO: check out how callv works / or other way for delegates
        //target_transition->get_transition_delegate()->call_funcv();
        //}
        target->enter(context);
        return true;
    }
    return false;
}

Ref<VDAhsmTransition> VDAhsmState::get_viable_transition(Ref<VDAcContext> context, Ref<VDAcEvent> event)
{
    Ref<VDAcStateData> data = context->get_state_data(this);
    Ref<VDAcState> current_state = data->get_current_state();
    if(is_debug_mode()) print_line(to_state_string() + ": Try transit on state - " + current_state->to_state_string());
    Ref<VDAhsmTransition> target_transition;
    Vector<Ref<VDAhsmTransition>> current_transitions = state_map[current_state];
    if(is_debug_mode()) print_line(to_state_string() + ": Current transitions - " + itos(current_transitions.size()));
    for(int i = 0; i < current_transitions.size(); i++) {
        Ref<VDAhsmTransition> transition = current_transitions[i];
        if(is_debug_mode()) print_line(to_state_string() + ": Check viable transition - "+ transition->to_transition_string());
        Ref<VDAcEvent> target_event = transition->get_triggering_event();
        bool viable_for_transit = transition->is_automatic_transition() || (target_event.is_null() || target_event == event);
        if(viable_for_transit) {
            if(is_debug_mode()) print_line(to_state_string() + ": Found viable transition. Check if transit is applicable.");
            if(transition->can_transit(context)) {
                return transition;
            }
        }
    }
    return target_transition;
}

bool VDAhsmState::tick ( Ref<VDAcContext> context ) {
    call("_on_tick", context);
    if(has_regions()) {
        Vector<Ref<VDAcState>> substates = get_substates();
        for(int i = 0; i < substates.size(); i++) {
            Ref<VDAcState> substate = substates[i];
            if(substate.is_valid()) {
                substate->tick(context);
            }
        }
        return true;
    }
    else {
        Ref<VDAcStateData> data = context->get_state_data(this);
        Ref<VDAcState> current_state = data->get_current_state();
        if(current_state.is_valid()) {
            return current_state->tick(context);
        }
    }
    return false;
}

bool VDAhsmState::has_tick() {
    return true;
}

bool VDAhsmState::dispatch(Ref<VDAcEvent> event, Ref<VDAcContext> context)
{
    //if(context->get_queued_events().find(event) == -1) {
    //context->add_queued_event(event);
    bool event_consumed = false;
    if(has_regions()) {
        if(is_debug_mode()) print_line(to_state_string() + ": Has regions. Dispatch on substates.");
        for(int i = 0; i < substates.size(); i++) {
            Ref<VDAcState> substate = substates[i];
            if(substate->dispatch(event, context)) {
                event_consumed = true;
            }
        }
    }
    else {
        if(is_debug_mode()) print_line(to_state_string() + ": No regions. Check for applicable transitions.");
        Ref<VDAhsmTransition> transition = get_viable_transition(context, event);
        if(transition.is_valid()) {
            if(is_debug_mode()) print_line(to_state_string() + ": Found valid transition (" + transition->to_transition_string() + "). Transit to state - " + transition->get_to_state()->to_state_string());
            transition->transit(context);
            //context->remove_queued_event(event);
            return true;
        }
    }
    //}
    return event_consumed;
}

void VDAhsmState::init(Ref<VDAcContext> context)
{
    call ( "_on_init", context );
    if(is_debug_mode()) print_line(to_state_string() + ": Initialized.");
    Ref<VDAcStateData> data = Ref<VDAcStateData>(memnew(VDAcStateData));
    data->set_data_context(context);
    data->set_owning_state(this);
    context->set_state_data(this, data);
    Vector<Ref<VDAcState>> substates = get_substates();
    for(int i = 0; i < substates.size(); i++) {
        Ref<VDAcState> state = substates[i];
        if(is_debug_mode()) print_line(to_state_string() + ": Initialize substate - " + state->to_state_string());
        state->init(context);
    }
    call ( "_after_init", context );
}

void VDAhsmState::enter(Ref<VDAcContext> context) {
    call("_on_enter", context);
    if(is_debug_mode()) print_line(to_state_string() + ": Entered.");
    if(has_regions()) {
        if(is_debug_mode()) print_line(to_state_string() + ": Has regions.");
        for(int i = 0; i < substates.size(); i++) {
            Ref<VDAcState> substate = substates[i];
            if(substate.is_valid()) {
                if(is_debug_mode()) print_line(to_state_string() + ": Enter substate - " + substate->to_state_string());
                if(is_debug_mode()) {
                    print_line(to_state_string() + ": (Prev) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
                }
                substate->enter(context);
                if(is_debug_mode()) {
                    print_line(to_state_string() + ": (After) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
                }
            }
        }
    }
    else {
        Vector<Variant> binds;
        binds.push_back(context);
        context->connect("param_updated", this, "_handle_context_param_updated", binds);
        context->connect("params_updated", this, "_handle_context_params_updated", binds);
        Ref<VDAcStateData> data = context->get_state_data(this);
        Ref<VDAcState> current_state = data->get_current_state();
        if(current_state.is_null() && default_substate.is_valid()) {
            if(is_debug_mode()) print_line(to_state_string() + ": Set default substate as target state - " + default_substate->to_state_string());
            data->set_current_state(default_substate);
            current_state = default_substate;
        }
        if(current_state.is_valid()) {
            if(is_debug_mode()) print_line(to_state_string() + ": Enter current substate - " + current_state->to_state_string());
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (Prev) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
            }
            current_state->enter(context);
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (After) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
            }
        }
    }
    call("_after_enter", context);
}

void VDAhsmState::update(Ref<VDAcContext> context) {
    call("_on_update", context);
    if(has_regions()) {
        for(int i = 0; i < substates.size(); i++) {
            Ref<VDAcState> substate = substates[i];
            substate->update(context);
        }
    }
    else {
        Ref<VDAcStateData> data = context->get_state_data(this);
        Ref<VDAcState> current_state = data->get_current_state();
        if(current_state.is_valid()) {
            current_state->update(context);
        }
    }
    call("_after_update", context);
}

void VDAhsmState::exit(Ref<VDAcContext> context) {
    call("_before_exit", context);
    if(is_debug_mode()) print_line(to_state_string() + ": Exited.");
    if(has_regions()) {
        for(int i = 0; i < substates.size(); i++) {
            Ref<VDAcState> substate = substates[i];
            if(is_debug_mode()) print_line(to_state_string() + ": Exit substate - " + substate->to_state_string());
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (Prev) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
            }
            substate->exit(context);
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (After) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
            }
        }
    }
    else {
        Ref<VDAcStateData> data = context->get_state_data(this);
        Ref<VDAcState> current_state = data->get_current_state();
        if(current_state.is_valid()) {
            if(is_debug_mode()) print_line(to_state_string() + ": Exit current substate - " + current_state->to_state_string());
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (Prev) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
            }
            current_state->exit(context);
            if(is_debug_mode()) {
                print_line(to_state_string() + ": (After) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
            }
            data->set_current_state(nullptr);
        }
        context->disconnect("param_updated", this, "_handle_context_param_updated");
        context->disconnect("params_updated", this, "_handle_context_params_updated");
    }
    call("_on_exit", context);
}

void VDAhsmState::deinit(Ref<VDAcContext> context)
{
    call ( "_before_deinit", context );
    if(is_debug_mode()) print_line(to_state_string() + ": Deinitialized.");
    //Ref<VDAcStateData> data = Ref<VDAcStateData>(memnew(VDAcStateData));
    //data->set_data_context(context);
    //data->set_owning_state(this);
    //context->set_state_data(this, data);
    Vector<Ref<VDAcState>> substates = get_substates();
    for(int i = 0; i < substates.size(); i++) {
        Ref<VDAcState> state = substates[i];
        if(is_debug_mode()) print_line(to_state_string() + ": Deinitialize substate - " + state->to_state_string());
        state->deinit(context);
    }
    call ( "_on_deinit", context );
}

bool VDAhsmState::has_substates() const
{
    return true;
}

void VDAhsmState::restore_history_deep() {}

void VDAhsmState::restore_history_shallow() {}

Ref<VDAhsmTransition> VDAhsmState::create_transition(Ref<VDAcState> from, Ref<VDAcState> to, String name, Ref<VDAcEvent> trigger, HashMap<Variant, Variant, VariantHasher> conditions)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Try creating transition from (" + from->to_state_string() + ") to ("+ to->to_state_string() + ").");
    Ref<VDAhsmTransition> new_transition = Ref<VDAhsmTransition>(memnew(VDAhsmTransition));
    bool has_from_state = this->substates.find(from) >= 0;
    bool proceed = true;
    if(has_from_state && state_map.has(from)) {
        if(is_debug_mode()) print_line(to_state_string() + ": From state already in state map. Lookup if transition already exists.");
        Vector<Ref<VDAhsmTransition>> state_transitions = state_map[from];
        for(int i = 0; i < state_transitions.size(); i++) {
            Ref<VDAhsmTransition> current_transition = state_transitions[i];
            if(current_transition->get_to_state() == to) {
                if(is_debug_mode()) print_line(to_state_string() + ": Transition already establised between the given states.");
                proceed = false;
                break;
            }
        }
    }
    if(proceed) {
        if(!has_from_state) {
            if(is_debug_mode()) print_line(to_state_string() + ": From state not contained. Add it.");
            this->add_substate(from);
        }
        if(this->substates.find(to) == -1) {
            if(is_debug_mode()) print_line(to_state_string() + ": To state not contained. Add it.");
            this->add_substate(to);
        }
        new_transition = Ref<VDAhsmTransition>(memnew(VDAhsmTransition));
        new_transition->set_transition_name(name);
        new_transition->set_from_state(from);
        new_transition->set_to_state(to);
        new_transition->set_triggering_event(trigger);
        new_transition->set_conditions(conditions);
        if(is_debug_mode()) print_line(to_state_string() + ": Transition(" + new_transition->to_transition_string() + ") created. Add it.");
        this->add_transition(new_transition);
    }
    //else {
    //ERR_FAIL_MSG("Transition between given states already exists.");
    //}
    return new_transition;
}

Ref<VDAhsmTransition> VDAhsmState::create_transition_open(Ref<VDAcState> from, Ref<VDAcState> to, String name, Ref<VDAcEvent> trigger, Dictionary conditions)
{
    HashMap<Variant, Variant, VariantHasher> converted_conditions;
    for(int i = 0; i < conditions.size(); i++) {
        Variant key = conditions.get_key_at_index(i);
        Variant value = conditions.get_value_at_index(i);
        converted_conditions[key] = value;
    }
    return this->create_transition(from, to, name, trigger, converted_conditions);
}

void VDAhsmState::add_transition(Ref<VDAhsmTransition> transition) {
    if(is_debug_mode()) print_line(to_state_string() + ": Adding transition - " + transition->to_transition_string());
    if(substates.find(transition->get_from_state()) >= 0 && substates.find(transition->get_to_state()) >= 0) {
        bool contained = transitions.find(transition) >= 0;
        if(is_debug_mode()) print_line(to_state_string() + ": States available (Elms: " + itos(transitions.size()) + "). Is transition already contained ? - " + (contained ? "Yes" : "No"));
        if(!contained) {
            transitions.push_back(transition);
            Ref<VDAcState> from_state = transition->get_from_state();
            Vector<Ref<VDAhsmTransition>> state_transitions = state_map[from_state];
            state_transitions.push_back(transition);
            state_map[from_state] = state_transitions;
            if(is_debug_mode()) print_line((to_state_string() + ": Added. Size now " + itos(transitions.size())));
        } else {
            ERR_FAIL_MSG("Transition is already available for the given states.");
        }
        //Ref<VDAhsmNode> from_state = subnodes[from];
        //Ref<VDAhsmNode> to_state = subnodes[to];
        //if(!from_state->get_transitions().has(trigger)) {
        //from_state->get_transitions().set(trigger, to_state);
        //}
    } else {
        ERR_FAIL_MSG("States of transition have to be substates of this state machine.");
    }
}

void VDAhsmState::remove_transition(Ref<VDAhsmTransition> transition)
{
    if(transitions.find(transition) >= 0) {
        transitions.erase(transition);
        Vector<Ref<VDAhsmTransition>> state_transitions = state_map[transition->get_from_state()];
        state_transitions.erase(transition);
        state_map[transition->get_from_state()] = state_transitions;
    }
}

void VDAhsmState::set_transitions(Vector<Ref<VDAhsmTransition>> transitions)
{
    while( this->transitions.size() > 0) {
        Ref<VDAhsmTransition> transition = this->transitions[0];
        this->remove_transition(transition);
    }
    for(int i = 0; i < transitions.size(); i++) {
        Ref<VDAhsmTransition> new_transition = transitions[i];
        this->add_transition(new_transition);
    }
}

Vector<Ref<VDAhsmTransition>> VDAhsmState::get_transitions() const
{
    return this->transitions;
}

void VDAhsmState::set_transitions_open(Array transitions)
{
    Vector<Ref<VDAhsmTransition>> new_transitions;
    for(int i = 0; i < transitions.size(); i++) {
        Ref<VDAhsmTransition> transition = transitions[i];
        new_transitions.push_back(transition);
    }
    this->set_transitions(new_transitions);
}

Array VDAhsmState::get_transitions_open() const
{
    Array open_transitions;
    for(int i = 0; i < transitions.size(); i++) {
        Ref<VDAhsmTransition> transition = transitions[i];
        open_transitions.push_back(transition);
    }
    return open_transitions;
}

void VDAhsmState::set_default_substate(Ref<VDAcState> substate)
{
    if(this->substates.find(substate) >= 0) {
        this->default_substate = substate;
    }
    else {
        ERR_FAIL_MSG("Substate to be set as default substate is not a substate of this machine.");
    }
}

Ref<VDAcState> VDAhsmState::get_default_substate() const
{
    return this->default_substate;
}

void VDAhsmState::set_has_regions ( bool has ) {
    this->bhas_regions = has;
}

bool VDAhsmState::has_regions() {
    return this->bhas_regions;
}

void VDAhsmState::_on_substate_added(Ref<VDAcState> state)
{
    Vector<Ref<VDAhsmTransition>> new_transitions;
    state_map.set(state, new_transitions);
    if(default_substate.is_null()) {
        if(is_debug_mode()) print_line(to_state_string() + ": First substate and no default substate set. Set as default substate.");
        this->set_default_substate(state);
    }
}

void VDAhsmState::_on_substate_removed(Ref<VDAcState> state)
{
    if(this->state_map.has(state)) {
        Vector<Ref<VDAhsmTransition>> old_transitions = this->state_map[state];
        for(int i = 0; i < old_transitions.size(); i++) {
            Ref<VDAhsmTransition> old_transition = old_transitions[i];
            this->transitions.erase(old_transition);
        }
        this->state_map.erase(state);
    }
    if(this->default_substate == state) {
        if(this->substates.size() > 0) {
            this->set_default_substate(this->substates[0]);
        } else {
            this->default_substate.unref();
        }
    }
}

void VDAhsmState::_internal_transition(Ref<VDAcContext> context)
{
    Ref<VDAhsmTransition> transition = this->get_viable_transition(context);
    if(transition.is_valid()) {
        if(is_debug_mode()) print_line(to_state_string() + ": Transit to state - " + transition->get_to_state()->to_state_string());
        transition->transit(context);
    }
}

void VDAhsmState::_handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Received changed param value. Context - " + itos(context->get_instance_id()));
    this->_internal_transition(context);
}

void VDAhsmState::_handle_context_params_updated(Ref<VDAcContext> context)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Received changed params. Context - " + itos(context->get_instance_id()));
    this->_internal_transition(context);
}
//////////
// VDAhsmTransition
//////////
VDAhsmTransition::VDAhsmTransition() {}

void VDAhsmTransition::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("transit", "context"), &VDAhsmTransition::transit);
    ClassDB::bind_method(D_METHOD("can_transit", "context"), &VDAhsmTransition::can_transit);

    ClassDB::bind_method(D_METHOD("set_transition_delegate", "delegate"), &VDAhsmTransition::set_triggering_event);
    ClassDB::bind_method(D_METHOD("get_transition_delegate"), &VDAhsmTransition::get_transition_delegate);
    ClassDB::bind_method(D_METHOD("set_triggering_event", "event"), &VDAhsmTransition::set_triggering_event);
    ClassDB::bind_method(D_METHOD("get_triggering_event"), &VDAhsmTransition::get_triggering_event);
    ClassDB::bind_method(D_METHOD("set_from_state", "state"), &VDAhsmTransition::set_from_state);
    ClassDB::bind_method(D_METHOD("get_from_state"), &VDAhsmTransition::get_from_state);
    ClassDB::bind_method(D_METHOD("set_to_state", "state"), &VDAhsmTransition::set_to_state);
    ClassDB::bind_method(D_METHOD("get_to_state"), &VDAhsmTransition::get_to_state);
    ClassDB::bind_method(D_METHOD("set_conditions", "conditions"), &VDAhsmTransition::set_conditions_open);
    ClassDB::bind_method(D_METHOD("get_conditions"), &VDAhsmTransition::get_conditions_open);

    ClassDB::bind_method(D_METHOD("set_is_automatic_transition", "automatic"), &VDAhsmTransition::set_is_automatic_transition);
    ClassDB::bind_method(D_METHOD("is_automatic_transition"), &VDAhsmTransition::is_automatic_transition);

    ClassDB::bind_method ( D_METHOD ( "set_transition_name", "name" ), &VDAhsmTransition::set_transition_name );
    ClassDB::bind_method ( D_METHOD ( "get_transition_name" ), &VDAhsmTransition::get_transition_name );
    ClassDB::bind_method ( D_METHOD ( "to_transition_string" ), &VDAhsmTransition::to_transition_string );

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "transition_delegate"), "set_transition_delegate", "get_transition_delegate");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "triggering_event"), "set_triggering_event", "get_triggering_event");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "from"), "set_from_state", "get_from_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "to"), "set_to_state", "get_to_state");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "conditions"), "set_conditions", "get_conditions");

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "automatic_transition"), "set_is_automatic_transition", "is_automatic_transition");
    ADD_PROPERTY ( PropertyInfo ( Variant::STRING, "transition_name" ), "set_transition_name", "get_transition_name" );
}

bool VDAhsmTransition::transit(Ref<VDAcContext> context)
{
    Ref<VDAcStateData> data = context->get_state_data(from);
    Ref<VDAcState> current_state = data->get_current_state();
    print_line("Transition:: (Prev) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
    if(current_state.is_valid()) current_state->exit(context);
    print_line("Transition:: (After) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
    if(transition_delegate.is_valid()) {
        //TODO: gotta do it smh
        //target_transition->get_transition_delegate()->call_funcv();
    }
    if(to.is_valid()) {
        data->set_current_state(to);
        print_line("Transition:: (Prev) Leaf state entered. Add to active states. Num: " + itos(context->get_active_states().size()));
        to->enter(context);
        print_line("Transition:: (After) Leaf state entered. Add to active states. Num: " + itos(context->get_active_states().size()));
        return true;
    }
    return false;
}

bool VDAhsmTransition::can_transit(Ref<VDAcContext> context) const
{
    HashMap<Variant, Variant, VariantHasher> context_params = context->get_context_params();
    bool can_transit = true;
    for(int i = 0; i < condition_keys.size(); i++) {
        Variant key = condition_keys[i];
        Variant condition = conditions[key];
        if(!context_params.has(key) || context_params[key] != condition) {
            can_transit = false;
            break;
        }
    }
    return can_transit;
}

void VDAhsmTransition::set_transition_delegate(Ref<FuncRef> delegate)
{
    this->transition_delegate = delegate;
}

Ref<FuncRef> VDAhsmTransition::get_transition_delegate() const
{
    return this->transition_delegate;
}

void VDAhsmTransition::set_triggering_event(Ref<VDAcEvent> event)
{
    this->triggering_event = event;
}

Ref<VDAcEvent> VDAhsmTransition::get_triggering_event() const
{
    return triggering_event;
}

void VDAhsmTransition::set_from_state(Ref<VDAcState> state)
{
    this->from = state;
}

Ref<VDAcState> VDAhsmTransition::get_from_state() const
{
    return from;
}

void VDAhsmTransition::set_to_state(Ref<VDAcState> state)
{
    this->to = state;
}

Ref<VDAcState> VDAhsmTransition::get_to_state() const
{
    return to;
}

Vector<Variant> VDAhsmTransition::get_condition_keys() const
{
    return condition_keys;
}

void VDAhsmTransition::set_conditions(HashMap<Variant, Variant, VariantHasher> conditions)
{
    this->conditions = conditions;
    condition_keys.clear();
    List<Variant> keys;
    this->conditions.get_key_list(&keys);
    for(int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        condition_keys.push_back(key);
    }
}

HashMap<Variant, Variant, VariantHasher> VDAhsmTransition::get_conditions() const
{
    return conditions;
}

void VDAhsmTransition::set_conditions_open(Dictionary conditions)
{
    HashMap<Variant, Variant, VariantHasher> new_conditions;
    List<Variant> keys;
    conditions.get_key_list(&keys);
    for(int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        Variant value = conditions[key];
        new_conditions[key] = value;
    }
    set_conditions(new_conditions);
}

Dictionary VDAhsmTransition::get_conditions_open()
{
    Dictionary dict;
    for(int i = 0; i < condition_keys.size(); i++) {
        Variant key = condition_keys[i];
        Variant value = this->conditions[key];
        dict[key] = value;
    }
    return dict;
}

void VDAhsmTransition::set_is_automatic_transition(bool automatic)
{
    this->bis_automatic_transition = automatic;
}

bool VDAhsmTransition::is_automatic_transition() const
{
    return this->bis_automatic_transition;
}

void VDAhsmTransition::set_transition_name ( String name ) {
    this->transition_name = name;
}

String VDAhsmTransition::get_transition_name() const {
    String name = "";
    if(!this->transition_name.empty()) {
        name = this->transition_name;
    }
    else {
        name = this->get_class();
    }
    return name;
}

String VDAhsmTransition::to_transition_string() const {
    String class_string = "";
    if ( !transition_name.empty() ) {
        class_string = transition_name + ":" + itos ( get_instance_id() );
    } else {
        class_string = "[" + get_class() + ":" + itos ( get_instance_id() ) + "]";
    }
    return class_string;
}
//////////
// VDAhsmNode
//////////
VDAhsmNode::VDAhsmNode() {}

void VDAhsmNode::_bind_methods() {}

void VDAhsmNode::add_transition ( Ref<VDAcState> state, int trigger ) {
    if(!this->transitions.has(trigger)) {
        this->transitions.set(trigger, state);
    }
}

HashMap<int, Ref<VDAcState>> VDAhsmNode::get_transitions() const {
    return this->transitions;
}

void VDAhsmNode::set_node_state ( Ref<VDAcState> state ) {
    this->node_state = state;
}

Ref<VDAcState> VDAhsmNode::get_node_state() const {
    return this->node_state;
}

void VDAhsmNode::set_owning_state ( Ref<VDAcState> state ) {
    this->owning_state = state;
}

Ref<VDAcState> VDAhsmNode::get_owning_state() const {
    return this->owning_state;
}

void VDAhsmNode::set_node_name ( String name ) {
    this->node_name = name;
}

String VDAhsmNode::get_node_name() const {
    return this->node_name;
}
