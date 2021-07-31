#include "./VDNode.h"

VDAcNode::VDAcNode()
{
    Ref<VDAcContext> initial_context = Ref<VDAcContext> ( memnew ( VDAcContext() ) );
    this->set_automata_context(initial_context);
    this->set_process ( false );
    this->set_physics_process ( false );
}

void VDAcNode::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "start" ), &VDAcNode::start );
    ClassDB::bind_method ( D_METHOD ( "stop" ), &VDAcNode::stop );
    ClassDB::bind_method ( D_METHOD ( "is_running" ), &VDAcNode::is_running );

    ClassDB::bind_method ( D_METHOD ( "set_automata", "automata" ), &VDAcNode::set_automata );
    ClassDB::bind_method ( D_METHOD ( "get_automata" ), &VDAcNode::get_automata );
    ClassDB::bind_method ( D_METHOD ( "set_automata_context", "context" ), &VDAcNode::set_automata_context );
    ClassDB::bind_method ( D_METHOD ( "get_automata_context" ), &VDAcNode::get_automata_context );

    ClassDB::bind_method ( D_METHOD ( "set_initial_params", "params" ), &VDAcNode::set_initial_params_open );
    ClassDB::bind_method ( D_METHOD ( "get_initial_params" ), &VDAcNode::get_initial_params_open );
    ClassDB::bind_method ( D_METHOD ( "set_tick_mode", "mode" ), &VDAcNode::set_tick_mode );
    ClassDB::bind_method ( D_METHOD ( "get_tick_mode" ), &VDAcNode::get_tick_mode );
    ClassDB::bind_method ( D_METHOD ( "set_agent_path", "path" ), &VDAcNode::set_owner );
    ClassDB::bind_method ( D_METHOD ( "get_agent_path" ), &VDAcNode::get_owner );
    ClassDB::bind_method ( D_METHOD ( "set_overwrites_context_params", "overwrites" ), &VDAcNode::set_overwrites_context_params );
    ClassDB::bind_method ( D_METHOD ( "is_overwrites_context_params" ), &VDAcNode::is_overwrites_context_params );

    BIND_VMETHOD ( MethodInfo ( "_on_starting" ) );
    BIND_VMETHOD ( MethodInfo ( "_after_starting" ) );
    BIND_VMETHOD ( MethodInfo ( "_on_stopping" ) );
    BIND_VMETHOD ( MethodInfo ( "_after_stopping" ) );

    ADD_PROPERTY ( PropertyInfo ( Variant::OBJECT, "automata", PROPERTY_HINT_RESOURCE_TYPE, "VDAcAutomata" ), "set_automata", "get_automata" );
    ADD_PROPERTY ( PropertyInfo ( Variant::OBJECT, "automata_context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ), "set_automata_context", "get_automata_context" );
    ADD_PROPERTY ( PropertyInfo ( Variant::DICTIONARY, "initial_params" ), "set_initial_params", "get_initial_params" );
    ADD_PROPERTY ( PropertyInfo ( Variant::INT, "tick_mode", PROPERTY_HINT_ENUM, "PROCESS, PHYSICS_PROCESS, ALL, NONE" ), "set_tick_mode", "get_tick_mode" );
    ADD_PROPERTY ( PropertyInfo ( Variant::NODE_PATH, "agent_path"), "set_agent_path", "get_agent_path" );
    ADD_PROPERTY ( PropertyInfo ( Variant::BOOL, "overwrites_context_params"), "set_overwrites_context_params", "is_overwrites_context_params" );
}

void VDAcNode::_notification ( int p_what )
{
    switch ( p_what ) {
    case NOTIFICATION_READY: {
        this->init_context_owning_node();
    }
    break;
    case NOTIFICATION_PROCESS: {
        context->set_delta ( get_process_delta_time() );
        Vector<Ref<VDAcState>> active_states = this->context->get_active_states();
        for ( int i = 0; i < active_states.size(); i++ ) {
            Ref<VDAcState> state = active_states[i];
            state->tick ( context );
        }
    }
    break;
    case NOTIFICATION_PHYSICS_PROCESS: {
        context->set_delta ( get_physics_process_delta_time() );
        Vector<Ref<VDAcState>> active_states = this->context->get_active_states();
        for ( int i = 0; i < active_states.size(); i++ ) {
            Ref<VDAcState> state = active_states[i];
            state->tick ( context );
        }
    }
    break;
    }
}

void VDAcNode::init_context_owning_node()
{
    Node * node_agent = get_node_or_null(np_agent);
    if(node_agent != nullptr) {
        this->context->set_owner(node_agent);
    }
    this->context->set_owner ( this->get_parent() );
}

void VDAcNode::start()
{
    ERR_FAIL_COND_MSG ( this->automata.is_null(), "Automata can't be null." );
    ERR_FAIL_COND_MSG(!this->is_inside_tree(), "Node has to be inside SceneTree before it can be started.");
    ERR_FAIL_COND_MSG ( is_running(), "Can't start node if it is already running." );
    Ref<VDAcState> root = this->automata->get_root_state();
    ERR_FAIL_COND_MSG ( root.is_null(), "Root state can't be null." );
    call ( "_on_starting" );
    this->context->set_owning_automata(this->automata);
    if(is_overwrites_context_params()) {
        context->set_context_params(this->initial_params);
    }
    else {
        List<Variant> param_keys;
        this->initial_params.get_key_list(&param_keys);
        for(int i = 0; i < param_keys.size(); i++) {
            Variant param_key = param_keys[i];
            if(!context->has_context_param(param_key)) {
                Variant param_value = this->initial_params[param_key];
                context->set_context_value(param_key, param_value);
            }
        }
    }
    this->init_context_owning_node();
    root->init ( this->context );
    root->enter ( this->context );
    set_process ( tick_mode == ALL || tick_mode == PROCESS );
    set_physics_process ( tick_mode == ALL || tick_mode == PHYSICS_PROCESS );
    call ( "_after_starting" );
}

void VDAcNode::stop()
{
    ERR_FAIL_COND_MSG ( this->automata.is_null(), "Automata can't be null." );
    if ( is_running() ) {
        Ref<VDAcState> root = this->automata->get_root_state();
        ERR_FAIL_COND_MSG ( root.is_null(), "Root state can't be null." );
        call ( "_on_stopping" );
        root->exit ( this->context );
        root->deinit(this->context);
        set_process ( false );
        set_physics_process ( false );
        call ( "_after_stopping" );
    } else {
        WARN_PRINT ( "Can't stop node if it is not running." );
    }
}

bool VDAcNode::is_running() const
{
    return is_processing() || is_physics_processing();
}

void VDAcNode::_on_starting() {}
void VDAcNode::_after_starting() {}
void VDAcNode::_on_stopping() {}
void VDAcNode::_after_stopping() {}

void VDAcNode::set_automata ( Ref<VDAcAutomata> automata )
{
    ERR_FAIL_COND_MSG ( is_running(), "Can't change automata while automata node is running." )
    this->automata = automata;
    this->context->set_owning_automata ( automata );
}

Ref<VDAcAutomata> VDAcNode::get_automata ( )
{
    return this->automata;
}

void VDAcNode::set_automata_context ( Ref<VDAcContext> context )
{
    ERR_FAIL_COND_MSG ( is_running(), "Can't change automata context while automata node is running." )
    this->context = context;
}

Ref<VDAcContext> VDAcNode::get_automata_context() const
{
    return this->context;
}

void VDAcNode::set_initial_params(HashMap<Variant, Variant, VariantHasher> params)
{
    ERR_FAIL_COND_MSG ( is_running(), "Can't change initial parameters while automata node is running." )
    this->initial_params = params;
    if(is_overwrites_context_params()) {
        this->context->set_context_params(this->initial_params);
    }
    else {
        List<Variant> param_keys;
        this->initial_params.get_key_list(&param_keys);
        for(int i = 0; i < param_keys.size(); i++) {
            Variant param_key = param_keys[i];
            if(!this->context->has_context_param(param_key)) {
                Variant param_value = this->initial_params[param_key];
                this->context->set_context_value(param_key, param_value);
            }
        }
    }
}

HashMap<Variant, Variant, VariantHasher> VDAcNode::get_initial_params() const
{
    return this->initial_params;
}

void VDAcNode::set_initial_params_open(Dictionary params)
{
    ERR_FAIL_COND_MSG ( is_running(), "Can't change initial parameters while automata node is running." )
    HashMap<Variant, Variant, VariantHasher> new_params;
    List<Variant> keys;
    params.get_key_list(&keys);
    for(int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        Variant value = params[key];
        new_params[i] = value;
    }
    set_initial_params(new_params);
}

Dictionary VDAcNode::get_initial_params_open() const
{
    Dictionary dict;
    List<Variant> keys;
    initial_params.get_key_list(&keys);
    for(int i = 0; i < keys.size(); i++) {
        Variant key = keys[i];
        Variant value = initial_params[key];
        dict[key] = value;
    }
    return dict;
}

void VDAcNode::set_tick_mode ( VDAcNode::VDATickMode mode )
{
    ERR_FAIL_COND_MSG ( is_running(), "Can't change tick mode while automata node is running." )
    this->tick_mode = mode;
}

VDAcNode::VDATickMode VDAcNode::get_tick_mode() const
{
    return this->tick_mode;
}

void VDAcNode::set_agent_path(const NodePath& path)
{
    this->np_agent = path;
}

NodePath VDAcNode::get_agent_path() const
{
    return this->np_agent;
}

void VDAcNode::set_overwrites_context_params(bool overwrite)
{
    this->boverwrites_context_params = overwrite;
}

bool VDAcNode::is_overwrites_context_params() const
{
    return this->boverwrites_context_params;
}

