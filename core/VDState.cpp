#include "./VDState.h"
////////////////////
/// VDAcState
////////////////////
VDAcState::VDAcState() {
  int id = 1;
  while(StringName::search("State" + itos(id))) id++;
  state_ident = StringName("State" + itos(id));
}

void VDAcState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("tick", "context", "structure"), &VDAcState::tick);
  ClassDB::bind_method(D_METHOD("init", "context", "structure"), &VDAcState::init);
  ClassDB::bind_method(D_METHOD("enter", "context", "structure"), &VDAcState::enter);
  ClassDB::bind_method(D_METHOD("update", "context", "structure", "param", "new_value", "old_value"), &VDAcState::update);
  ClassDB::bind_method(D_METHOD("exit", "context", "structure"), &VDAcState::exit);
  ClassDB::bind_method(D_METHOD("deinit", "context", "structure"), &VDAcState::deinit);
  ClassDB::bind_method(D_METHOD("_on_tick", "context", "structure"), &VDAcState::_on_tick);
  ClassDB::bind_method(D_METHOD("_pre_init", "context", "structure"), &VDAcState::_pre_init);
  ClassDB::bind_method(D_METHOD("_on_init", "context", "structure"), &VDAcState::_on_init);
  ClassDB::bind_method(D_METHOD("_pre_enter", "context", "structure"), &VDAcState::_pre_enter);
  ClassDB::bind_method(D_METHOD("_on_enter", "context", "structure"), &VDAcState::_on_enter);
  ClassDB::bind_method(D_METHOD("_on_update", "context", "structure", "param", "new_value", "old_value"), &VDAcState::_on_update);
  ClassDB::bind_method(D_METHOD("_pre_exit", "context", "structure"), &VDAcState::_pre_exit);
  ClassDB::bind_method(D_METHOD("_on_exit", "context", "structure"), &VDAcState::_on_exit);
  ClassDB::bind_method(D_METHOD("_pre_deinit", "context", "structure"), &VDAcState::_pre_deinit);
  ClassDB::bind_method(D_METHOD("_on_deinit", "context", "structure"), &VDAcState::_on_deinit);

  ClassDB::bind_method(D_METHOD("_handle_context_param_updated", "param", "new_value", "old_value", "context", "structure"), &VDAcState::_handle_context_param_updated);
  ClassDB::bind_method(D_METHOD("get_tree"), &VDAcState::get_tree);

  ClassDB::bind_method(D_METHOD("set_state_ident", "ident"), &VDAcState::set_state_ident);
  ClassDB::bind_method(D_METHOD("get_state_ident"), &VDAcState::get_state_ident);
  ClassDB::bind_method(D_METHOD("set_state_name", "name"), &VDAcState::set_state_name);
  ClassDB::bind_method(D_METHOD("get_state_name"), &VDAcState::get_state_name);
  ClassDB::bind_method(D_METHOD("to_state_string"), &VDAcState::to_state_string);

  ClassDB::bind_method(D_METHOD("set_has_tick", "has_tick"), &VDAcState::set_has_tick);
  ClassDB::bind_method(D_METHOD("has_tick"), &VDAcState::has_tick);
  ClassDB::bind_method(D_METHOD("set_listening_to_updates", "listening"), &VDAcState::set_listening_to_updates);
  ClassDB::bind_method(D_METHOD("is_listening_to_updates"), &VDAcState::is_listening_to_updates);
  ClassDB::bind_method(D_METHOD("set_debug_mode", "mode"), &VDAcState::set_debug_mode);
  ClassDB::bind_method(D_METHOD("is_debug_mode"), &VDAcState::is_debug_mode);

  // State-Flow (virtual methods)
  BIND_VMETHOD(MethodInfo("_on_tick", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_pre_init", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_on_init", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_pre_enter", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_on_enter", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_on_update", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure"), PropertyInfo(Variant::NIL, "param"), PropertyInfo(Variant::NIL, "new_value"), PropertyInfo(Variant::NIL, "old_value")));
  BIND_VMETHOD(MethodInfo("_pre_exit", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_on_exit", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_pre_deinit", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
  BIND_VMETHOD(MethodInfo("_on_deinit", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));

  // Param context workflow (virtual methods)
  BIND_VMETHOD(MethodInfo("_handle_context_param_updated", PropertyInfo(Variant::NIL, "param"), PropertyInfo(Variant::NIL, "new_value"), PropertyInfo(Variant::NIL, "old_value"), PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "state_ident", PROPERTY_HINT_NONE, ""/*, PROPERTY_USAGE_NOEDITOR */), "set_state_ident", "get_state_ident");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "state_name"), "set_state_name", "get_state_name");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_tick"), "set_has_tick", "has_tick");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "listening_to_updates"), "set_listening_to_updates", "is_listening_to_updates");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_mode"), "set_debug_mode", "is_debug_mode");

  ADD_SIGNAL(MethodInfo("state_name_changed", PropertyInfo(Variant::STRING, "new_name"), PropertyInfo(Variant::STRING, "old_name")));
  ADD_SIGNAL(MethodInfo("has_tick_changed", PropertyInfo(Variant::BOOL, "new_mode")));
  ADD_SIGNAL(MethodInfo("is_listening_to_updates_changed", PropertyInfo(Variant::BOOL, "new_mode")));
}

bool VDAcState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  if(bhas_tick) return call("_on_tick", context, structure);
  return false;
}

void VDAcState::init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  // TODO: reduce to only container based states
  call("_pre_init", context, structure);
  Ref<VDAcStateData> data = Ref<VDAcStateData> (memnew(VDAcStateData));
  data->set_data_context(context);
  data->set_owning_structure(structure);
  data->set_current_structure(structure);
  context->set_state_data(structure->get_automata_path(), data);
  call("_on_init", context, structure);
}

void VDAcState::enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_enter", context, structure);
  context->add_active_structure(structure);
  if(bis_listening_to_updates) {
    Vector<Variant> binds;
    binds.push_back(context);
    binds.push_back(structure);
    context->connect("context_param_updated", this, "_handle_context_param_updated", binds);
    // TODO: connect with params_changed -> DEFVAL for Variant issue
  }
  call("_on_enter", context, structure);
}
void VDAcState::update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param, Variant new_value, Variant old_value) {
  call("_on_update", context, structure, param, new_value, old_value);
}

void VDAcState::exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_exit", context, structure);
  context->remove_active_structure(structure);
  if(bis_listening_to_updates) context->disconnect("context_param_updated", this, "_handle_context_param_updated");
  call("_on_exit", context, structure);
}

void VDAcState::deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_deinit", context, structure);
  context->remove_state_data(structure->get_automata_path());
  call("_on_deinit", context, structure);
}

bool VDAcState::_on_tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  return true;
}
void VDAcState::_pre_init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_on_init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_pre_enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_on_enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_on_update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param, Variant new_value, Variant old_value) {}
void VDAcState::_pre_exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_on_exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_pre_deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}
void VDAcState::_on_deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {}

void VDAcState::_handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("update", context, structure, param, new_value, old_value);
}

SceneTree * VDAcState::get_tree() const {
  return SceneTree::get_singleton();
}

void VDAcState::set_state_ident(StringName ident) {
  state_ident = ident;
}

StringName VDAcState::get_state_ident() const {
  return state_ident;
}

void VDAcState::set_state_name(String name) {
  if(state_name != name) {
    String old_name = state_name;
    state_name = name;
    emit_signal("state_name_changed", name, old_name);
    property_list_changed_notify();
  }
}

String VDAcState::get_state_name() const {
  return state_name;
}

String VDAcState::to_state_string() const {
  String class_string = state_name;
  if(!class_string.empty()) {
    class_string = class_string + ":" + itos(get_instance_id());
  } else {
    class_string = "[" + get_class() + ":" + itos(get_instance_id()) + "]";
  }
  return class_string;
}

void VDAcState::set_has_tick(bool has_tick) {
  if(bhas_tick != has_tick) {
    bhas_tick = has_tick;
    emit_signal("has_tick_changed", has_tick);
  }
}

bool VDAcState::has_tick() const {
  return bhas_tick;
}

void VDAcState::set_listening_to_updates(bool listening) {
  if(bis_listening_to_updates != listening) {
    bis_listening_to_updates = listening;
    emit_signal("is_listening_to_updates_changed", listening);
  }
}

bool VDAcState::is_listening_to_updates() const {
  return bis_listening_to_updates;
}

void VDAcState::set_debug_mode(bool mode) {
  bis_debug_mode = mode;
}

bool VDAcState::is_debug_mode() const {
  return bis_debug_mode;
}
////////////////////
// VDAcParentState
////////////////////
VDAcParentState::VDAcParentState() {}

void VDAcParentState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_substate_added", "substate"), &VDAcParentState::_on_substate_added);
  ClassDB::bind_method(D_METHOD("_on_substate_removed", "substate"), &VDAcParentState::_on_substate_removed);
  ClassDB::bind_method(D_METHOD("add_substate", "substate", "pos"), &VDAcParentState::add_substate, DEFVAL(-1));
  ClassDB::bind_method(D_METHOD("remove_substate", "substate"), &VDAcParentState::remove_substate);
  ClassDB::bind_method(D_METHOD("set_substates", "new_substates"), &VDAcParentState::set_substates_open);
  ClassDB::bind_method(D_METHOD("get_substates"), &VDAcParentState::get_substates_open);
  ClassDB::bind_method(D_METHOD("is_substate_unique", "substate"), &VDAcParentState::is_substate_unique);
  ClassDB::bind_method(D_METHOD("has_substates"), &VDAcParentState::has_substates);
  //ClassDB::bind_method ( D_METHOD ( "get_structure_from_context", "context" ), &VDAcState::get_structure_from_context );

  // Substate workflow (virtual methods)
  BIND_VMETHOD(MethodInfo("_on_substate_added", PropertyInfo(Variant::OBJECT, "substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  BIND_VMETHOD(MethodInfo("_on_substate_removed", PropertyInfo(Variant::OBJECT, "substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));

  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "substates", PROPERTY_HINT_TYPE_STRING, "17/17:VDAcState"), "set_substates", "get_substates");

  ADD_SIGNAL(MethodInfo("substate_added", PropertyInfo(Variant::OBJECT, "added_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  ADD_SIGNAL(MethodInfo("substate_removed", PropertyInfo(Variant::OBJECT, "removed_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  ADD_SIGNAL(MethodInfo("substates_changed"));
}

bool VDAcParentState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  if(bhas_tick) {
    call("_on_tick", context, structure);
    HashMap<StringName, Ref<VDAcStateStructure>> substate_structures = structure->get_children_structures();
    List<StringName> keys = structure->get_children_structure_paths();
    for(int i = 0; i < keys.size(); i++) {
      Ref<VDAcStateStructure> substate_structure = substate_structures[keys[i]];
      Ref<VDAcState> substate = substate_structure->get_owning_state();
      substate->call("tick", context, substate_structure);
    }
    return true;
  } else return false;
}

void VDAcParentState::init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_init", context, structure);
  Ref<VDAcStateStructure> empty;
  Ref<VDAcStateData> data = Ref<VDAcStateData> (memnew(VDAcStateData));
  data->set_data_context(context);
  data->set_owning_structure(structure);
  data->set_current_structure(empty);
  context->set_state_data(structure->get_automata_path(), data);
  List<StringName> keys = structure->get_children_structure_paths();
  HashMap< StringName, Ref< VDAcStateStructure>> child_structures = structure->get_children_structures();
  for(int i = 0; i < keys.size(); i++) {
    Ref<VDAcStateStructure> substructure = child_structures[keys[i]];
    Ref<VDAcState> state = substructure->get_owning_state();
    state->call("init", context, substructure);
  }
  call("_on_init", context, structure);
}

void VDAcParentState::enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_enter", context, structure);
  List<StringName> keys = structure->get_children_structure_paths();
  HashMap< StringName, Ref< VDAcStateStructure > > sub_structures = structure->get_children_structures();
  for(int i = 0; i < keys.size(); i++) {
    Ref<VDAcStateStructure> substructure = sub_structures[keys[i]];
    Ref<VDAcState> substate = substructure->get_owning_state();
    if(substate.is_valid()) {
      substate->call("enter", context, substructure);
    }
  }
  call("_on_enter", context, structure);
}

void VDAcParentState::update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param, Variant new_value, Variant old_value) {
  call("_on_update", context, structure, param, new_value, old_value);
  List<StringName> keys = structure->get_children_structure_paths();
  HashMap< StringName, Ref< VDAcStateStructure > > sub_structures = structure->get_children_structures();
  for(int i = 0; i < keys.size(); i++) {
    Ref<VDAcStateStructure> substructure = sub_structures[keys[i]];
    Ref<VDAcState> substate = substructure->get_owning_state();
    substate->call("update", context, substructure, param, new_value, old_value);
  }
}

void VDAcParentState::exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_exit", context, structure);
  List<StringName> keys = structure->get_children_structure_paths();
  HashMap<StringName, Ref<VDAcStateStructure>> sub_structures = structure->get_children_structures();
  for(int i = 0; i < keys.size(); i++) {
    Ref<VDAcStateStructure> substructure = sub_structures[keys[i]];
    Ref<VDAcState> substate = substructure->get_owning_state();
    substate->call("exit", context, substructure);
  }
  call("_on_exit", context, structure);
}

void VDAcParentState::deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_deinit", context, structure);
  List<StringName> keys = structure->get_children_structure_paths();
  HashMap< StringName, Ref< VDAcStateStructure > > sub_structures = structure->get_children_structures();
  for(int i = 0; i < keys.size(); i++) {
    Ref<VDAcStateStructure> sub_structure = sub_structures[keys[i]];
    Ref<VDAcState> state = sub_structure->get_owning_state();
    state->call("deinit", context, sub_structure);
  }
  call("_on_deinit", context, structure);
}

void VDAcParentState::_on_substate_added(Ref<VDAcState> substate) {}
void VDAcParentState::_on_substate_removed(Ref<VDAcState> substate) {}

void VDAcParentState::add_substate(Ref<VDAcState> substate, int pos) {
  ERR_FAIL_COND_MSG(!substate.is_valid(), "Add: Substate is not valid.");
  if(substates.find(substate) < 0) {
    if(pos >= 0) substates.insert(pos, substate);
    else substates.push_back(substate);
    call("_on_substate_added", substate);
    emit_signal("substate_added", substate);
    property_list_changed_notify();
  }
}

void VDAcParentState::remove_substate(Ref<VDAcState> substate) {
  ERR_FAIL_COND_MSG(!substate.is_valid(), "Remove: Substate is not valid.");
  if(substates.find(substate) >= 0) {
    substates.erase(substate);
    call("_on_substate_removed", substate);
    emit_signal("substate_removed", substate);
    property_list_changed_notify();
  }
}

bool VDAcParentState::has_substates() const {
  return substates.size() > 0;
}

void VDAcParentState::set_substates(Vector<Ref<VDAcState>> new_substates) {
  int added = 0;
  int removed = 0;
  for(int i = 0; i < new_substates.size(); i++) {
    Ref<VDAcState> state = new_substates[i];
    if(substates.find(state) < 0) {
      add_substate(state, i);
      added++;
    }
  }
  int state_index;
  for(state_index = 0; state_index < substates.size(); state_index++) {
    Ref<VDAcState> state = substates[state_index];
    if(new_substates.find(state) < 0) {
      remove_substate(state);
      state_index--;
      removed++;
    }
  }
  if(removed > 0 || added > 0) {
    emit_signal("substates_changed");
    property_list_changed_notify();
  }
}

Vector<Ref<VDAcState>> VDAcParentState::get_substates() const {
  return substates;
}

void VDAcParentState::set_substates_open(Array new_substates) {
  Vector<Ref<VDAcState>> new_substate_vector;
  for(int i = 0; i < new_substates.size(); i++) {
    Ref<VDAcState> substate = new_substates[i];
    if(substate.is_valid()) {
      new_substate_vector.push_back(substate);
    } else {
      // Creating an "empty" default state
      Ref<VDAcState> new_substate = Ref<VDAcState> (memnew(VDAcState));
      new_substate_vector.push_back(new_substate);
    }
  }
  set_substates(new_substate_vector);
}

Array VDAcParentState::get_substates_open() const {
  Array open_substates;
  for(int i = 0; i < substates.size(); i++) {
    Ref<VDAcState> substate = substates[i];
    open_substates.push_back(substate);
  }
  return open_substates;
}

bool VDAcParentState::is_substate_unique(Ref<VDAcState> substate) {
  if(!substate.is_valid()) {
    ERR_PRINT_ONCE("Unique substate: Non-valid substate.");
    return false;
  }
  for(int i = 0; i < substates.size(); i++) {
    Ref<VDAcState> cur_substate = substates[i];
    if(cur_substate != substate && cur_substate->get_state_ident() == substate->get_state_ident()) {
      return false;
    }
  }
  return true;
}
////////////////////
// VDAcStateData
////////////////////
VDAcStateData::VDAcStateData() {}

void VDAcStateData::_bind_methods() {
  ADD_SIGNAL(MethodInfo("current_structure_changed", PropertyInfo(Variant::NIL, "new_structure"), PropertyInfo(Variant::NIL, "old_structure")));
}

void VDAcStateData::set_data_context(Ref<VDAcContext> new_context) {
  context = new_context;
}

Ref<VDAcContext> VDAcStateData::get_data_context() const {
  return context;
}

void VDAcStateData::set_owning_structure(Ref<VDAcStateStructure> structure) {
  owning_structure = structure;
}

Ref<VDAcStateStructure> VDAcStateData::get_owning_structure() const {
  return owning_structure;
}

void VDAcStateData::set_current_structure(Ref<VDAcStateStructure> structure) {
  if(current_structure != structure) {
    Ref<VDAcStateStructure> old_structure = current_structure;
    current_structure = structure;
    emit_signal("current_structure_changed", structure, old_structure);
  }
}

Ref<VDAcStateStructure> VDAcStateData::get_current_structure() const {
  return current_structure;
}
////////////////////
/// VDAcContext
////////////////////
VDAcContext::VDAcContext() {}

VDAcContext::~VDAcContext() {
  active_structures.clear();
//     queued_events.clear();
  queued_structures.clear();
  state_datas.clear();
}

void VDAcContext::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_setup"), &VDAcContext::_setup);
  ClassDB::bind_method(D_METHOD("_undo"), &VDAcContext::_undo);
  ClassDB::bind_method(D_METHOD("get_tree"), &VDAcContext::get_tree);

  ClassDB::bind_method(D_METHOD("set_context_params", "params"), &VDAcContext::set_context_params_open);
  ClassDB::bind_method(D_METHOD("set_owning_automata", "automata"), &VDAcContext::set_owning_automata);
  ClassDB::bind_method(D_METHOD("set_blackboard", "blackboard"), &VDAcContext::set_blackboard);
  ClassDB::bind_method(D_METHOD("set_delta", "delta"), &VDAcContext::set_delta);
  ClassDB::bind_method(D_METHOD("get_context_params"), &VDAcContext::get_context_params_open);
  ClassDB::bind_method(D_METHOD("get_owning_automata"), &VDAcContext::get_owning_automata);
  ClassDB::bind_method(D_METHOD("get_blackboard"), &VDAcContext::get_blackboard);
  ClassDB::bind_method(D_METHOD("get_delta"), &VDAcContext::get_delta);

  ClassDB::bind_method(D_METHOD("set_context_value", "key", "value", "notify"), &VDAcContext::set_context_value, DEFVAL(true));
  ClassDB::bind_method(D_METHOD("get_context_value", "key"), &VDAcContext::get_context_value);

  ClassDB::bind_method(D_METHOD("add_active_structure", "structure"), &VDAcContext::add_active_structure);
  ClassDB::bind_method(D_METHOD("remove_active_structure", "structure"), &VDAcContext::remove_active_structure);
  ClassDB::bind_method(D_METHOD("get_active_structures"), &VDAcContext::get_active_structures_open);

  ClassDB::bind_method(D_METHOD("set_state_data", "state_path", "data"), &VDAcContext::set_state_data);
  ClassDB::bind_method(D_METHOD("get_state_data", "state_path"), &VDAcContext::get_state_data);
  ClassDB::bind_method(D_METHOD("get_state_datas"), &VDAcContext::get_state_datas_open);

  BIND_VMETHOD(MethodInfo("_setup"));
  BIND_VMETHOD(MethodInfo("_undo"));

  ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "params"), "set_context_params", "get_context_params");

  ADD_SIGNAL(MethodInfo("context_param_updated", PropertyInfo(Variant::NIL, "param"), PropertyInfo(Variant::NIL, "new_value"), PropertyInfo(Variant::NIL, "old_value")));
  ADD_SIGNAL(MethodInfo("context_params_changed"));
  ADD_SIGNAL(MethodInfo("state_data_changed", PropertyInfo(Variant::OBJECT, "new_data"), PropertyInfo(Variant::OBJECT, "old_data"), PropertyInfo(Variant::STRING, "path")));
  ADD_SIGNAL(MethodInfo("state_data_added", PropertyInfo(Variant::NIL, "data"), PropertyInfo(Variant::STRING, "path")));
  ADD_SIGNAL(MethodInfo("active_structure_added", PropertyInfo(Variant::NIL, "added_state_structure")));
  ADD_SIGNAL(MethodInfo("active_structure_removed", PropertyInfo(Variant::NIL, "removed_state_structure")));
}

void VDAcContext::_setup() {}
void VDAcContext::_undo() {}

SceneTree * VDAcContext::get_tree() const {
  return SceneTree::get_singleton();
}

void VDAcContext::set_context_params(HashMap<Variant, Variant, VariantHasher> new_params) {
  int removed = 0;
  List<Variant> new_entries;
  List<Variant> contained_entries;
  List<Variant> new_keys;
  new_params.get_key_list(&new_keys);
  for(int i = 0; i < new_keys.size(); i++) {
    const Variant &new_key = new_keys[i];
    if(params.has(new_key)) {
      contained_entries.push_back(new_key);
    } else {
      new_entries.push_back(new_key);
    }
  }
  int param_index;
  for(param_index = 0; param_index < param_keys.size(); param_index++) {
    const Variant &key = param_keys[param_index];
    if(!contained_entries.find(key)) {
      param_keys.erase(key);
      params.erase(key);
      param_index--;
      removed++;
    }
  }
  for(int i = 0; i < new_entries.size(); i++) {
    const Variant &new_key = new_entries[i];
    const Variant &new_param = new_params[new_key];
    param_keys.push_back(new_key);
    params.set(new_key, new_param);
  }
  if(removed > 0 || new_entries.size() > 0) {
    emit_signal("context_params_changed");
    property_list_changed_notify();
  }
}

HashMap<Variant, Variant, VariantHasher> VDAcContext::get_context_params() const {
  return params;
}

void VDAcContext::set_context_params_open(Dictionary new_params) {
  Array new_param_keys = new_params.keys();
  HashMap<Variant, Variant, VariantHasher> hash_params;
  for(int i = 0; i < new_param_keys.size(); i++) {
    const Variant &key = new_param_keys[i];
    const Variant &value = new_params[key];
    hash_params[key] = value;
  }
  set_context_params(hash_params);
}

Dictionary VDAcContext::get_context_params_open() const {
  Dictionary dict;
  List<Variant> keys;
  params.get_key_list(&keys);
  for(int i = 0; i < keys.size(); i++) {
    const Variant &key = keys[i];
    const Variant &value = params[key];
    dict[key] = value;
  }
  return dict;
}

void VDAcContext::set_context_value(const Variant &key, const Variant &value, bool notify) {
  Variant old_value;
  if(!param_keys.find(key)) {
    param_keys.push_back(key);
  } else {
    old_value = params[key];
  }
  if(value != old_value) {
    params[key] = value;
    if(notify) emit_signal("context_param_updated", key, value, old_value);
  }
}

Variant VDAcContext::get_context_value(const Variant &key) const {
  return params[key];
}

bool VDAcContext::has_context_param(const Variant &key) {
  return param_keys.find(key);
}

void VDAcContext::set_owning_automata(Ref<VDAcAutomata> automata) {
  owning_automata = automata;
}

Ref<VDAcAutomata> VDAcContext::get_owning_automata() const {
  return owning_automata;
}

void VDAcContext::set_blackboard(Ref<VDAcBlackboard> blackboard) {
  this->blackboard = blackboard;
}

Ref<VDAcBlackboard> VDAcContext::get_blackboard() const {
  return blackboard;
}

void VDAcContext::set_delta(float delta) {
  this->delta = delta;
}

float VDAcContext::get_delta() const {
  return delta;
}

void VDAcContext::add_active_structure(Ref<VDAcStateStructure> structure) {
  int idx = active_structures.find(structure);
  if(idx < 0) {
    active_structures.push_back(structure);
    emit_signal("active_structure_added", structure);
  }
}

void VDAcContext::remove_active_structure(Ref<VDAcStateStructure> structure) {
  int state_index = active_structures.find(structure);
  if(state_index >= 0) {
    active_structures.remove(state_index);
    emit_signal("active_structure_removed", structure);
  }
}

// TODO: rework
void VDAcContext::set_active_structures(Vector<Ref<VDAcStateStructure>> structures) {
  active_structures = structures;
}

Vector<Ref<VDAcStateStructure>> VDAcContext::get_active_structures() const {
  return active_structures;
}

Array VDAcContext::get_active_structures_open() const {
  Array open_state_structures;
  for(int i = 0; i < active_structures.size(); i++) {
    Ref<VDAcStateStructure> state = active_structures[i];
    open_state_structures.push_back(state);
  }
  return open_state_structures;
}

void VDAcContext::set_state_data(StringName state_path, Ref<VDAcStateData> data) {
  if(state_datas.has(state_path)) {
    Ref<VDAcStateData> old_data = state_datas[state_path];
    if(old_data != data) {
      state_datas.set(state_path, data);
      emit_signal("state_data_changed", data, old_data, state_path);
    }
  } else {
    state_datas.set(state_path, data);
    emit_signal("state_data_added", data, state_path);
  }
}

Ref<VDAcStateData> VDAcContext::get_state_data(StringName state_path) const {
  Ref<VDAcStateData> data;
  if(state_datas.has(state_path)) {
    data = state_datas.get(state_path);
  }
  return data;
}

void VDAcContext::remove_state_data(StringName state_path) {
  if(state_datas.has(state_path)) {
    state_datas.erase(state_path);
  }
}

HashMap<StringName, Ref<VDAcStateData>> VDAcContext::get_state_datas() const {
  return state_datas;
}

Dictionary VDAcContext::get_state_datas_open() const {
  Dictionary datas;
  List<StringName> paths;
  state_datas.get_key_list(&paths);
  for(int i = 0; i < paths.size(); i++) {
    StringName id = paths[i];
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



