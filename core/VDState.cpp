#include "./VDState.h"
////////////////////
/// VDAcState
////////////////////
VDAcState::VDAcState() {}

void VDAcState::_bind_methods() {
    ClassDB::bind_method ( D_METHOD ( "tick", "context" ), &VDAcState::tick );
    ClassDB::bind_method ( D_METHOD ( "has_tick" ), &VDAcState::has_tick );
    ClassDB::bind_method(D_METHOD("dispatch", "event", "context"), &VDAcState::dispatch);
    ClassDB::bind_method ( D_METHOD ( "init", "context" ), &VDAcState::init );
    ClassDB::bind_method ( D_METHOD ( "enter", "context" ), &VDAcState::enter );
    ClassDB::bind_method ( D_METHOD ( "update", "context" ), &VDAcState::update );
    ClassDB::bind_method ( D_METHOD ( "exit", "context" ), &VDAcState::exit );
    ClassDB::bind_method ( D_METHOD ( "deinit", "context" ), &VDAcState::deinit );

    ClassDB::bind_method(D_METHOD("add_substate", "substate"), &VDAcState::add_substate);
    ClassDB::bind_method(D_METHOD("remove_substate", "substate"), &VDAcState::remove_substate);
    ClassDB::bind_method(D_METHOD("has_substates"), &VDAcState::has_substates);
    ClassDB::bind_method(D_METHOD("set_substates", "substates"), &VDAcState::set_substates_open);
    ClassDB::bind_method(D_METHOD("get_substates"), &VDAcState::get_substates_open);

    ClassDB::bind_method ( D_METHOD ( "set_state_name", "name" ), &VDAcState::set_state_name );
    ClassDB::bind_method ( D_METHOD ( "get_state_name" ), &VDAcState::get_state_name );

    //ClassDB::bind_method ( D_METHOD ( "get_structure_from_context", "context" ), &VDAcState::get_structure_from_context );

    ClassDB::bind_method ( D_METHOD ( "to_state_string" ), &VDAcState::to_state_string );
    ClassDB::bind_method ( D_METHOD ( "set_debug_mode", "mode" ), &VDAcState::set_debug_mode );
    ClassDB::bind_method ( D_METHOD ( "is_debug_mode" ), &VDAcState::is_debug_mode );

    BIND_VMETHOD ( MethodInfo ( "_on_substate_added", PropertyInfo ( Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_substate_removed", PropertyInfo ( Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState" ) ) );

    BIND_VMETHOD ( MethodInfo ( "_on_tick", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_init", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_after_init", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_enter", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_after_enter", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_update", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_after_update", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_before_exit", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_exit", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_before_deinit", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_deinit", PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
    BIND_VMETHOD ( MethodInfo ( "_on_event", PropertyInfo ( Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "VDAcEvent" ), PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );

    ADD_PROPERTY ( PropertyInfo ( Variant::STRING, "state_name" ), "set_state_name", "get_state_name" );
    ADD_PROPERTY ( PropertyInfo ( Variant::BOOL, "debug_mode" ), "set_debug_mode", "is_debug_mode" );

    ADD_SIGNAL(MethodInfo("substate_added", PropertyInfo(Variant::OBJECT, "substate")));
    ADD_SIGNAL(MethodInfo("substate_removed", PropertyInfo(Variant::OBJECT, "substate")));
}

bool VDAcState::tick ( Ref<VDAcContext> context ) {
    return call ( "_on_tick", context );
}

bool VDAcState::dispatch ( Ref<VDAcEvent> event, Ref<VDAcContext> context ) {
    if(is_debug_mode() && event.is_valid()) {
        print_line(to_state_string() + ": Process from event - " + event->to_string());
    }
    return event->process_from(context, this);
}

bool VDAcState::process ( Ref<VDAcEvent> event, Ref<VDAcContext> context ) {
    bool result = call("_on_event", event, context);
    return result;
}

void VDAcState::init ( Ref<VDAcContext> context ) {
    call ( "_on_init", context );
    Ref<VDAcStateData> data = Ref<VDAcStateData>(memnew(VDAcStateData));
    data->set_data_context(context);
    data->set_owning_state(this);
    context->set_state_data(this, data);
    if(is_debug_mode()) {
        print_line(to_state_string() + ": Initialized.");
    }
    call ( "_after_init", context );
}

void VDAcState::enter ( Ref<VDAcContext> context ) {
    call ( "_on_enter", context );
    if(is_debug_mode()) {
        print_line(to_state_string() + ": (Prev) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
    }
    context->add_active_state(this);
    if(is_debug_mode()) {
        print_line(to_state_string() + ": (After) Leaf state entered. Added to active states. Num: " + itos(context->get_active_states().size()));
    }
    call ( "_after_enter", context );
}
void VDAcState::update ( Ref<VDAcContext> context ) {
    call ( "_on_update", context );
    call ( "_after_update", context );
}

void VDAcState::exit ( Ref<VDAcContext> context ) {
    call ( "_before_exit", context );
    if(is_debug_mode()) {
        print_line(to_state_string() + ": (Prev) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
    }
    context->remove_active_state(this);
    if(is_debug_mode()) {
        print_line(to_state_string() + ": (After) Leaf state exited. Removed from active states. Num: " + itos(context->get_active_states().size()));
    }
    call ( "_on_exit", context );
}

void VDAcState::deinit(Ref<VDAcContext> context)
{
    call ( "_before_deinit", context );
    //Ref<VDAcStateData> data = Ref<VDAcStateData>(memnew(VDAcStateData));
    //data->set_data_context(context);
    //data->set_owning_state(this);
    //context->set_state_data(this, data);
    if(is_debug_mode()) {
        print_line(to_state_string() + ": Deinitialized.");
    }
    call ( "_on_deinit", context );
}

void VDAcState::add_substate(Ref<VDAcState> substate) {
    if(is_debug_mode()) print_line(to_state_string() + ": Try adding substate - "+ substate->to_state_string());
    if(substate.is_valid() && substates.find(substate) < 0) {
        if(is_debug_mode()) print_line(to_state_string() + ": Valid substate. Push in.");
        substates.push_back(substate);
        call("_on_substate_added", substate);
        emit_signal("substate_added", substate);
    }
    //packing state into node and adding to graph
    //Ref<VDAhsmNode> subnode = Ref<VDAhsmNode>(memnew(VDAhsmNode()));
    //subnode->set_node_state(substate);
    //subnode->set_owning_state(this);
    //subnodes[state_key] = subnode;
    //List<String> new_keys;
    //subnodes.get_key_list(&new_keys);
    //subnode_keys = new_keys;
}

void VDAcState::remove_substate(Ref<VDAcState> substate)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Try removing substate - " + substate->to_state_string());
    if(substate.is_valid() && this->substates.find(substate) > 0) {
        this->substates.erase(substate);
        call("_on_substate_removed", substate);
        emit_signal("substate_removed", substate);
    }
}

bool VDAcState::has_substates() const {
    return substates.size() > 0;
}

void VDAcState::set_substates(Vector<Ref<VDAcState>> substates)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Setting substates. Clean previous substates.");
    while(this->substates.size() > 0) {
        Ref<VDAcState> substate = this->substates[0];
        this->remove_substate(substate);
    }
    for(int i = 0; i < substates.size(); i++) {
        Ref<VDAcState> new_substate = substates[i];
        this->add_substate(new_substate);
    }
}

Vector<Ref<VDAcState>> VDAcState::get_substates() const {
    return substates;
}

void VDAcState::set_substates_open(Array substates)
{
    if(is_debug_mode()) print_line(to_state_string() + ": Setting substates (public).");
    Vector<Ref<VDAcState>> new_substates;
    for(int i = 0; i < substates.size(); i++) {
        Ref<VDAcState> substate = substates[i];
        if(substate.is_valid()) {
            if(is_debug_mode()) print_line(to_state_string() + ": Pushing substate (public) - " + substate->to_state_string());
            new_substates.push_back(substate);
        }
    }
    this->set_substates(new_substates);
}

Array VDAcState::get_substates_open() const
{
    Array open_substates;
    for(int i = 0; i < substates.size(); i++) {
        Ref<VDAcState> substate = substates[i];
        open_substates.push_back(substate);
    }
    return open_substates;
}

bool VDAcState::_on_tick ( Ref<VDAcContext> context ) {
    return true;
}
void VDAcState::_on_init ( Ref<VDAcContext> context ) {}
void VDAcState::_after_init ( Ref<VDAcContext> context ) {}
void VDAcState::_on_enter ( Ref<VDAcContext> context ) {}
void VDAcState::_after_enter ( Ref<VDAcContext> context ) {}
void VDAcState::_on_update ( Ref<VDAcContext> context ) {}
void VDAcState::_after_update ( Ref<VDAcContext> context ) {}
void VDAcState::_before_exit ( Ref<VDAcContext> context ) {}
void VDAcState::_on_exit ( Ref<VDAcContext> context ) {}
void VDAcState::_before_deinit(Ref<VDAcContext> context) {}
void VDAcState::_on_deinit(Ref<VDAcContext> context) {}

void VDAcState::_on_substate_added(Ref<VDAcState> state) {}
void VDAcState::_on_substate_removed(Ref<VDAcState> state) {}

void VDAcState::set_state_name ( String name ) {
    this->state_name = name;
}

String VDAcState::get_state_name() const {
    String name = "";
    if(!this->state_name.empty()) {
        name = this->state_name;
    }
    else {
        name = this->get_class();
    }
    return name;
}

String VDAcState::to_state_string() const {
    String class_string = "";
    if ( !state_name.empty() ) {
        class_string = state_name + ":" + itos ( get_instance_id() );
    } else {
        class_string = "[" + get_class() + ":" + itos ( get_instance_id() ) + "]";
    }
    return class_string;
}

void VDAcState::set_debug_mode(bool mode)
{
    this->debug_mode = mode;
}

bool VDAcState::is_debug_mode() const
{
    return this->debug_mode;
}
////////////////////
// VDAcStateData
////////////////////
VDAcStateData::VDAcStateData() {}

void VDAcStateData::_bind_methods() {}

void VDAcStateData::set_data_context ( Ref<VDAcContext> context ) {
    this->context = context;
}

Ref<VDAcContext> VDAcStateData::get_data_context() const {
    return this->context;
}

void VDAcStateData::set_owning_state ( Ref<VDAcState> state ) {
    this->owning_state = state;
}

Ref<VDAcState> VDAcStateData::get_owning_state() const {
    return this->owning_state;
}

void VDAcStateData::set_current_state ( Ref<VDAcState> state ) {
    this->current_state = state;
}

Ref<VDAcState> VDAcStateData::get_current_state() const {
    return this->current_state;
}
////////////////////
/// VDAcEvent
////////////////////
VDAcEvent::VDAcEvent() {}

void VDAcEvent::_bind_methods() {
    ClassDB::bind_method(D_METHOD ("set_event_name", "name"), &VDAcEvent::set_event_name);
    ClassDB::bind_method(D_METHOD ("get_event_name"), &VDAcEvent::get_event_name);

    ADD_PROPERTY(PropertyInfo ("event_name"), "set_event_name", "get_event_name");
}

bool VDAcEvent::process_from (Ref<VDAcContext> context, Ref<VDAcState> state ) {
    return state->process(this, context);
}

void VDAcEvent::set_event_name(String name) {
    this->event_name = name;
}

String VDAcEvent::get_event_name() const {
    return this->event_name;
}
////////////////////
/// VDAcContext
////////////////////
VDAcContext::VDAcContext() {}

VDAcContext::~VDAcContext() {
    active_states.clear();
//     queued_events.clear();
    queued_states.clear();
    state_datas.clear();
}

void VDAcContext::_bind_methods() {
    ClassDB::bind_method ( D_METHOD ( "set_owner", "owner" ), &VDAcContext::set_owner );
    ClassDB::bind_method ( D_METHOD ( "set_context_params", "params" ), &VDAcContext::set_context_params_open );
    ClassDB::bind_method ( D_METHOD ( "set_owning_automata", "automata" ), &VDAcContext::set_owning_automata );
    ClassDB::bind_method ( D_METHOD ( "set_blackboard", "blackboard" ), &VDAcContext::set_blackboard );
    ClassDB::bind_method ( D_METHOD ( "set_delta", "delta" ), &VDAcContext::set_delta );

    ClassDB::bind_method ( D_METHOD ( "get_owner" ), &VDAcContext::get_owner );
    ClassDB::bind_method ( D_METHOD ( "get_context_params" ), &VDAcContext::get_context_params_open );
    ClassDB::bind_method ( D_METHOD ( "get_owning_automata" ), &VDAcContext::get_owning_automata );
    ClassDB::bind_method ( D_METHOD ( "get_blackboard" ), &VDAcContext::get_blackboard );
    ClassDB::bind_method ( D_METHOD ( "get_delta" ), &VDAcContext::get_delta );

    ClassDB::bind_method ( D_METHOD ( "set_context_value", "key", "value" ), &VDAcContext::set_context_value );
    ClassDB::bind_method ( D_METHOD ( "get_context_value", "key" ), &VDAcContext::get_context_value );

    ClassDB::bind_method ( D_METHOD ( "add_active_state", "state" ), &VDAcContext::add_active_state );
    ClassDB::bind_method ( D_METHOD ( "remove_active_state", "state" ), &VDAcContext::remove_active_state );
    ClassDB::bind_method ( D_METHOD ( "get_active_states" ), &VDAcContext::get_active_states_open );

    ClassDB::bind_method ( D_METHOD ( "set_state_data", "state", "data" ), &VDAcContext::set_state_data );
    ClassDB::bind_method ( D_METHOD ( "get_state_data", "state" ), &VDAcContext::get_state_data );
    ClassDB::bind_method ( D_METHOD ( "get_state_datas" ), &VDAcContext::get_state_datas_open );

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "params"), "set_context_params", "get_context_params");

    ADD_SIGNAL(MethodInfo("param_updated", PropertyInfo(Variant::NIL, "param"), PropertyInfo(Variant::NIL, "new_value"),PropertyInfo(Variant::NIL, "old_value")));
    ADD_SIGNAL(MethodInfo("params_updated"));
}

void VDAcContext::set_owner ( Variant owner ) {
    this->owner = owner;
}

Variant VDAcContext::get_owner() const {
    return this->owner;
}

void VDAcContext::set_context_params(HashMap<Variant, Variant, VariantHasher> params)
{
    this->params = params;
    param_keys.clear();
    this->params.get_key_list(&param_keys);
    print_line("Context::"+itos(get_instance_id()) + " - Params updated.");
    emit_signal("params_updated");
}

HashMap<Variant, Variant, VariantHasher> VDAcContext::get_context_params() const
{
    return this->params;
}

void VDAcContext::set_context_params_open(Dictionary params)
{
    HashMap<Variant, Variant, VariantHasher> new_params;
    for(int i = 0; i < param_keys.size(); i++) {
        Variant key = param_keys[i];
        Variant value = params[key];
        new_params[key] = value;
    }
    set_context_params(new_params);
}

Dictionary VDAcContext::get_context_params_open() const
{
    Dictionary dict;
    List<Variant> keys;
    params.get_key_list(&keys);
    for(int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        Variant value = this->params[key];
        dict[key] = value;
    }
    return dict;
}

void VDAcContext::set_context_value ( Variant key, Variant value ) {
    Variant old_value;
    if(!this->param_keys.find(key)) {
        this->param_keys.push_back(key);
    } else {
        old_value = this->params[key];
    }
    if(value != old_value) {
        this->params[key] = value;
        print_line("Context::"+itos(get_instance_id()) + " - Param updated.");
        emit_signal("param_updated", key, value, old_value);
    }
}

Variant VDAcContext::get_context_value ( Variant key ) const {
    return this->params[key];
}

bool VDAcContext::has_context_param(Variant key)
{
    return this->param_keys.find(key);
}

void VDAcContext::set_owning_automata(Ref<VDAcAutomata> automata)
{
    this->owning_automata = automata;
}

Ref<VDAcAutomata> VDAcContext::get_owning_automata() const
{
    return this->owning_automata;
}

void VDAcContext::set_blackboard ( Ref<VDAcBlackboard> blackboard ) {
    this->blackboard = blackboard;
}

Ref<VDAcBlackboard> VDAcContext::get_blackboard() const {
    return this->blackboard;
}

void VDAcContext::set_delta ( float delta ) {
    this->delta = delta;
}

float VDAcContext::get_delta() const {
    return this->delta;
}

//TODO: stop the cycle of adding and removing twice
void VDAcContext::add_active_state ( Ref<VDAcState> state ) {
    int idx = active_states.find ( state );
    print_line(itos(get_instance_id()) + ": Add. State index - " + itos(idx));
    if ( idx < 0 ) {
        active_states.push_back ( state );
    }
}

void VDAcContext::remove_active_state ( Ref<VDAcState> state ) {
    int state_index = this->active_states.find(state);
    print_line(itos(get_instance_id()) + ": Remove. State index - " + itos(state_index));
    if(state_index >= 0) {
        this->active_states.remove(state_index);
    }
}

void VDAcContext::set_active_states ( Vector<Ref<VDAcState>> states ) {
    this->active_states = states;
}

Vector<Ref<VDAcState>> VDAcContext::get_active_states() const {
    return this->active_states;
}

Array VDAcContext::get_active_states_open() const {
    Array open_states;
    for ( int i = 0; i < this->active_states.size(); i++ ) {
        Ref<VDAcState> state = this->active_states[i];
        open_states.push_back ( state );
    }
    return open_states;
}

void VDAcContext::add_queued_event(Ref<VDAcEvent> event)
{
    if(this->queued_events.find(event) == -1) this->queued_events.push_back(event);
}

void VDAcContext::remove_queued_event(Ref<VDAcEvent> event)
{
    this->queued_events.erase(event);
}

Vector<Ref<VDAcEvent>> VDAcContext::get_queued_events() const
{
    return this->queued_events;
}

void VDAcContext::set_state_data ( Ref<VDAcState> state, Ref<VDAcStateData> data ) {
    ERR_FAIL_COND_MSG(state.is_null(), "State cannot be null.");
    this->state_datas.set(state->get_instance_id(), data);
}

Ref<VDAcStateData> VDAcContext::get_state_data ( Ref<VDAcState> state ) const {
    Ref<VDAcStateData> data;
    int state_id = state->get_instance_id();
    if(this->state_datas.has(state_id)) {
        data = this->state_datas.get(state_id);
    }
    return data;
}

HashMap<int, Ref<VDAcStateData>> VDAcContext::get_state_datas() const {
    return this->state_datas;
}

Dictionary VDAcContext::get_state_datas_open() const {
    Dictionary datas;
    List<int> ids;
    state_datas.get_key_list(&ids);
    for(int i = 0; i < ids.size(); i++) {
        int id = ids[i];
        Ref<VDAcStateData> data = state_datas[id];
        datas[id] = data;
    }
    return datas;
}
///////////////////
/// VDAcBlackboard
///////////////////
VDAcBlackboard::VDAcBlackboard() {}

void VDAcBlackboard::_bind_methods() {}
