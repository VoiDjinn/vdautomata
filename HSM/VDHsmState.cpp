#include "VDHsmState.h"
//////////
// VDAhsmOrthogonalState
//////////
VDAhsmOrthogonalState::VDAhsmOrthogonalState() {}

void VDAhsmOrthogonalState::_bind_methods() {}

void VDAhsmOrthogonalState::restore_history_deep() {}
void VDAhsmOrthogonalState::restore_history_shallow() {}
//////////
// VDAhsmCompositeState
//////////
VDAhsmCompositeState::VDAhsmCompositeState() {}

void VDAhsmCompositeState::_bind_methods() {
  ClassDB::bind_method(D_METHOD("add_transition", "transition"), &VDAhsmCompositeState::add_transition);
  ClassDB::bind_method(D_METHOD("remove_transition", "transition"), &VDAhsmCompositeState::remove_transition);
  ClassDB::bind_method(D_METHOD("set_transitions", "new_transitions"), &VDAhsmCompositeState::set_transitions_open);
  ClassDB::bind_method(D_METHOD("get_transitions"), &VDAhsmCompositeState::get_transitions_open);
  ClassDB::bind_method(D_METHOD("get_viable_transition", "context", "structure"), &VDAhsmCompositeState::get_viable_transition);

  ClassDB::bind_method(D_METHOD("set_default_substate", "substate"), &VDAhsmCompositeState::set_default_substate);
  ClassDB::bind_method(D_METHOD("get_default_substate"), &VDAhsmCompositeState::get_default_substate);
  ClassDB::bind_method(D_METHOD("is_valid_state_map_entry", "substate"), &VDAhsmCompositeState::is_valid_state_map_entry);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "default_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), "set_default_substate", "get_default_substate");
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transitions", PROPERTY_HINT_TYPE_STRING, "17/17:VDAhsmTransition"), "set_transitions", "get_transitions");

  ADD_SIGNAL(MethodInfo("transition_done", PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition"), PropertyInfo(Variant::OBJECT, "new_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "old_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  ADD_SIGNAL(MethodInfo("transition_failed", PropertyInfo(Variant::OBJECT, "added_transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition")));
  ADD_SIGNAL(MethodInfo("transition_added", PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition")));
  ADD_SIGNAL(MethodInfo("transition_removed", PropertyInfo(Variant::OBJECT, "removed_transition", PROPERTY_HINT_RESOURCE_TYPE, "VDAhsmTransition")));
  ADD_SIGNAL(MethodInfo("transitions_changed"));
}

bool VDAhsmCompositeState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  if(call("has_tick")) {
    call("_on_tick", context, structure);
    Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
    Ref<VDAcStateStructure> child_structure = data->get_current_structure();
    Ref<VDAcState> current_state = child_structure->get_owning_state();
    if(current_state->call("has_tick")) {
      return current_state->call("tick", context, child_structure);
    }
  }
  return false;
}

void VDAhsmCompositeState::enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_enter", context, structure);
  Vector<Variant> binds;
  binds.push_back(context);
  binds.push_back(structure);
  if(call("is_listening_to_updates")) {
    context->connect("param_updated", this, "_handle_context_param_updated", binds);
    context->connect("params_updated", this, "_handle_context_params_updated", binds);
  }
  Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
  Ref<VDAcStateStructure> current_structure = data->get_current_structure();
  if(current_structure.is_null() && default_substate.is_valid()) {
    Ref<VDAcStateStructure> default_structure = structure->get_child_structure(default_substate->get_state_ident());
    data->set_current_structure(default_structure);
    current_structure = default_structure;
  }
  if(current_structure.is_valid() && current_structure->get_owning_state().is_valid()) {
    current_structure->get_owning_state()->call("enter", context, current_structure);
  }
  call("_on_enter", context, structure);
}

void VDAhsmCompositeState::update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param, Variant new_value, Variant old_value) {
  call("_on_update", context, structure, param, new_value, old_value);
  Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
  Ref<VDAcStateStructure> current_structure = data->get_current_structure();
  Ref<VDAcState> current_state = current_structure->get_owning_state();
  if(current_state.is_valid()) {
    if(current_state->call("is_listening_to_updates")) {
      current_state->call("update", context, current_structure, param, new_value, old_value);
    }
  }
}

void VDAhsmCompositeState::exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("_pre_exit", context, structure);
  Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
  Ref<VDAcStateStructure> current_structure = data->get_current_structure();
  Ref<VDAcState> current_state = current_structure->get_owning_state();
  if(current_state.is_valid()) {
    current_state->call("exit", context, current_structure);
    Ref<VDAcStateStructure> empty;
    data->set_current_structure(empty);
  }
  if(call("is_listening_to_updates")) {
    context->disconnect("param_updated", this, "_handle_context_param_updated");
    context->disconnect("params_updated", this, "_handle_context_params_updated");
  }
  call("_on_exit", context, structure);
}

void VDAhsmCompositeState::add_transition(Ref<VDAhsmTransition> transition) {
  ERR_FAIL_COND_MSG(!transition.is_valid(), "Add: Transition is not valid.");
  ERR_FAIL_COND_MSG(transitions.find(transition) >= 0, "Add: Transition already contained.");
  transitions.push_back(transition);
  update_transition_from_state(transition->get_from_state(), nullptr, transition);
//#ifdef TOOLS_ENABLED
  Vector<Variant> binds;
  binds.push_back(transition);
  transition->connect("from_state_changed", this, "update_transition_from_state", binds);
  emit_signal("transition_added", transition);
  property_list_changed_notify();
//#endif
}

void VDAhsmCompositeState::remove_transition(Ref<VDAhsmTransition> transition) {
  ERR_FAIL_COND_MSG(transition.is_valid(), "Remove: Transition is not valid.");
  ERR_FAIL_COND_MSG(transitions.find(transition) < 0, "Remove: Transition not contained.");
  if(transitions.find(transition) >= 0) {
    transitions.erase(transition);
    if(transition->get_from_state().is_valid()) {
      Vector<Ref<VDAhsmTransition>> state_transitions = state_map[transition->get_from_state()->get_state_ident()];
      state_transitions.erase(transition);
      state_map[transition->get_from_state()->get_state_ident()] = state_transitions;
    }
//#ifdef TOOLS_ENABLED
    transition->disconnect("from_state_changed", this, "update_transition_from_state");
    emit_signal("transition_removed", transition);
    property_list_changed_notify();
//#endif
  }
}

void VDAhsmCompositeState::set_transitions(Vector<Ref<VDAhsmTransition>> new_transitions) {
  int removed = 0;
  List<Ref<VDAhsmTransition>> new_entries;
  List<Ref<VDAhsmTransition>> contained_entries;
  for(int i = 0; i < new_transitions.size(); i++) {
    Ref<VDAhsmTransition> transition = new_transitions[i];
    if(transitions.find(transition) >= 0) {
      contained_entries.push_back(transition);
    } else {
      new_entries.push_back(transition);
    }
  }
  int transition_index;
  for(transition_index = 0; transition_index < transitions.size(); transition_index++) {
    Ref<VDAhsmTransition> transition = transitions[transition_index];
    if(!contained_entries.find(transition)) {
      remove_transition(transition);
      transition_index--;
      removed++;
    }
  }
  for(int i = 0; i < new_entries.size(); i++) {
    Ref<VDAhsmTransition> entry = new_entries[i];
    add_transition(entry);
  }
  if(removed > 0 || new_entries.size() > 0) {
    emit_signal("transitions_changed");
    property_list_changed_notify();
  }
}

Vector<Ref<VDAhsmTransition>> VDAhsmCompositeState::get_transitions() const {
  return transitions;
}

void VDAhsmCompositeState::set_transitions_open(Array new_transitions) {
  Vector<Ref<VDAhsmTransition>> vtransitions;
  for(int i = 0; i < new_transitions.size(); i++) {
    Ref<VDAhsmTransition> transition = new_transitions[i];
    if(transition.is_valid()) {
      vtransitions.push_back(transition);
    } else {
      // Creating an "empty" default transition
      Ref<VDAhsmTransition> new_transition = Ref<VDAhsmTransition> (memnew(VDAhsmTransition));
      vtransitions.push_back(new_transition);
    }
  }
  set_transitions(vtransitions);
}

Array VDAhsmCompositeState::get_transitions_open() const {
  Array open_transitions;
  for(int i = 0; i < transitions.size(); i++) {
    Ref<VDAhsmTransition> transition = transitions[i];
    open_transitions.push_back(transition);
  }
  return open_transitions;
}

Ref<VDAhsmTransition> VDAhsmCompositeState::get_viable_transition(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
  Ref<VDAcStateStructure> current_structure = data->get_current_structure();
  Ref<VDAcState> current_state = current_structure->get_owning_state();
  Ref<VDAhsmTransition> target_transition;
  Vector<Ref<VDAhsmTransition>> current_transitions = state_map[current_state->get_state_ident()];
  for(int i = 0; i < current_transitions.size(); i++) {
    Ref<VDAhsmTransition> transition = current_transitions[i];
    if(transition->call("_can_transit", context, current_structure)) {
      target_transition = transition;
      break;
    }
  }
  return target_transition;
}

void VDAhsmCompositeState::add_state_map_entry(StringName entry_name, Ref<VDAcState> substate) {
  Vector<Ref<VDAhsmTransition>> new_transitions;
  state_map.set(entry_name, new_transitions);
}

void VDAhsmCompositeState::remove_state_map_entry(StringName entry_name, Ref<VDAcState> substate) {
  if(state_map.has(entry_name)) {
    Vector<Ref<VDAhsmTransition>> old_transitions = state_map[entry_name];
    for(int i = 0; i < old_transitions.size(); i++) {
      Ref<VDAhsmTransition> old_transition = old_transitions[i];
      transitions.erase(old_transition);
    }
    state_map.erase(entry_name);
  }
  if(default_substate == substate || substates.size() == 0) {
    set_default_substate(nullptr);
  }
}

void VDAhsmCompositeState::update_transition_from_state(Ref<VDAcState> new_state, Ref<VDAcState> old_state, Ref<VDAhsmTransition> transition) {
  if(old_state.is_valid() && state_map.has(old_state->get_state_ident())) {
    Vector<Ref<VDAhsmTransition>> state_transitions = state_map[old_state->get_state_ident()];
    if(state_transitions.find(transition) >= 0) {
      state_transitions.erase(transition);
      state_map[old_state->get_state_ident()] = state_transitions;
    }
  }
  ERR_FAIL_COND_MSG(!new_state.is_valid(), "Update: No valid from state set.");
  if(substates.find(new_state) < 0) {
    add_substate(new_state);
    WARN_PRINT("Update: Non-contained from state added to substates.");
  }
  StringName state_name = new_state->get_state_ident();
  Vector<Ref<VDAhsmTransition>> state_transitions;
  if(state_map.has(state_name)) {
    state_transitions = state_map[state_name];
  }
  state_transitions.push_back(transition);
  state_map.set(state_name, state_transitions);
  property_list_changed_notify();
}

void VDAhsmCompositeState::set_default_substate(Ref<VDAcState> substate) {
  if(substate != default_substate) {
    if(substate.is_valid() && substates.find(substate) < 0) {
      add_substate(substate);
      WARN_PRINT("Set default substate: Added substate to list.");
    }
    Ref<VDAcState> old_state = default_substate;
    default_substate = substate;
    emit_signal("default_substate_changed", substate, old_state);
    property_list_changed_notify();
  }
}

Ref<VDAcState> VDAhsmCompositeState::get_default_substate() const {
  return default_substate;
}

bool VDAhsmCompositeState::is_valid_state_map_entry(Ref<VDAcState> substate) {
  if(!substate.is_valid()) {
    WARN_PRINT("Non-valid substate. State map will not be changed.");
    return false;
  }
  if(!is_substate_unique(substate)) {
    WARN_PRINT("Non-unique substate. State map will not be changed.");
    return false;
  }
  return true;
}

void VDAhsmCompositeState::_on_substate_added(Ref<VDAcState> substate) {
  Vector<Variant> binds;
  binds.push_back(substate);
  if(is_valid_state_map_entry(substate)) {
    add_state_map_entry(substate->get_state_ident(), substate);
    if(default_substate.is_null()) {
      set_default_substate(substate);
      property_list_changed_notify();
    }
  }
}

void VDAhsmCompositeState::_on_substate_removed(Ref<VDAcState> substate) {
  remove_state_map_entry(substate->get_state_ident(), substate);
}

void VDAhsmCompositeState::_handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("update", context, structure, param, new_value, old_value);
  _internal_transition(context, structure);
}

void VDAhsmCompositeState::_handle_context_params_updated(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  call("update", context, structure);
  _internal_transition(context, structure);
}

void VDAhsmCompositeState::_internal_transition(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  Ref<VDAhsmTransition> transition = get_viable_transition(context, structure);
  if(transition.is_valid()) {
    transition->transit(context, structure);
  }
}
//////////
// VDAhsmTransition
//////////
VDAhsmTransition::VDAhsmTransition() {}

void VDAhsmTransition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("transit", "context", "structure"), &VDAhsmTransition::transit);
  ClassDB::bind_method(D_METHOD("_can_transit", "context", "structure"), &VDAhsmTransition::_can_transit);

  ClassDB::bind_method(D_METHOD("set_transition_delegate", "delegate"), &VDAhsmTransition::set_transition_delegate);
  ClassDB::bind_method(D_METHOD("get_transition_delegate"), &VDAhsmTransition::get_transition_delegate);
  ClassDB::bind_method(D_METHOD("set_from_state", "state"), &VDAhsmTransition::set_from_state);
  ClassDB::bind_method(D_METHOD("get_from_state"), &VDAhsmTransition::get_from_state);
  ClassDB::bind_method(D_METHOD("set_to_state", "state"), &VDAhsmTransition::set_to_state);
  ClassDB::bind_method(D_METHOD("get_to_state"), &VDAhsmTransition::get_to_state);
  ClassDB::bind_method(D_METHOD("set_conditions", "conditions"), &VDAhsmTransition::set_conditions_open);
  ClassDB::bind_method(D_METHOD("get_conditions"), &VDAhsmTransition::get_conditions_open);


  ClassDB::bind_method(D_METHOD("set_transition_name", "name"), &VDAhsmTransition::set_transition_name);
  ClassDB::bind_method(D_METHOD("get_transition_name"), &VDAhsmTransition::get_transition_name);
  ClassDB::bind_method(D_METHOD("to_transition_string"), &VDAhsmTransition::to_transition_string);

  BIND_VMETHOD(MethodInfo("_can_transit", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "from", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), "set_from_state", "get_from_state");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "to", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), "set_to_state", "get_to_state");
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "conditions", PROPERTY_HINT_TYPE_STRING, "17/17:VDAcCondition"), "set_conditions", "get_conditions");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "transition_name"), "set_transition_name", "get_transition_name");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "transition_delegate", PROPERTY_HINT_RESOURCE_TYPE, "FuncRef"), "set_transition_delegate", "get_transition_delegate");

  ADD_SIGNAL(MethodInfo("from_state_changed", PropertyInfo(Variant::OBJECT, "new_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "old_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  ADD_SIGNAL(MethodInfo("to_state_changed", PropertyInfo(Variant::OBJECT, "new_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "old_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
  ADD_SIGNAL(MethodInfo("conditions_changed"));
}

bool VDAhsmTransition::transit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) {
  Ref<VDAcStateData> data = context->get_state_data(structure->get_automata_path());
  Ref<VDAcStateStructure> children_structure = data->get_current_structure();
  Ref<VDAcState> current_state = children_structure->get_owning_state();
  if(current_state.is_valid()) {
    current_state->call("exit", context, children_structure);
  }
  /*
  if(transition_delegate.is_valid()) {
      transition_delegate->call_func(context);
  }
  */
  if(to.is_valid()) {
    Ref<VDAcStateStructure> to_structure = structure->get_child_structure(to->get_state_ident());
    data->set_current_structure(to_structure);
    to->call("enter", context, to_structure);
    return true;
  }
  return false;
}

bool VDAhsmTransition::_can_transit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) const {
  bool can_transit = true;
  for(int i = 0; i < conditions.size(); i++) {
    Ref<VDAcCondition> condition = conditions[i];
    if(!condition->call("_pass", context)) {
      can_transit = false;
      break;
    }
  }
  return can_transit;
}

void VDAhsmTransition::set_transition_delegate(Ref<FuncRef> delegate) {
  transition_delegate = delegate;
  property_list_changed_notify();
}

Ref<FuncRef> VDAhsmTransition::get_transition_delegate() const {
  return transition_delegate;
}

void VDAhsmTransition::set_from_state(Ref<VDAcState> state) {
  from = state;
  property_list_changed_notify();
}

Ref<VDAcState> VDAhsmTransition::get_from_state() const {
  return from;
}

void VDAhsmTransition::set_to_state(Ref<VDAcState> state) {
  to = state;
  property_list_changed_notify();
}

Ref<VDAcState> VDAhsmTransition::get_to_state() const {
  return to;
}

void VDAhsmTransition::set_conditions(Vector<Ref<VDAcCondition>> new_conditions) {
  int removed = 0;
  List<Ref<VDAcCondition>> new_entries;
  List<Ref<VDAcCondition>> contained_entries;
  for(int i = 0; i < new_conditions.size(); i++) {
    Ref<VDAcCondition> condition = new_conditions[i];
    if(conditions.find(condition) >= 0) {
      contained_entries.push_back(condition);
    } else {
      new_entries.push_back(condition);
    }
  }
  int condition_index;
  for(condition_index = 0; condition_index < conditions.size(); condition_index++) {
    Ref<VDAcCondition> condition = conditions[condition_index];
    if(!contained_entries.find(condition)) {
      conditions.erase(condition);
      condition_index--;
      removed++;
    }
  }
  for(int i = 0; i < new_entries.size(); i++) {
    Ref<VDAcCondition> entry = new_entries[i];
    conditions.push_back(entry);
  }
  if(removed > 0 || new_entries.size() > 0) {
    emit_signal("conditions_changed");
    property_list_changed_notify();
  }
}

void VDAhsmTransition::set_conditions_open(Array conditions) {
  Vector<Ref<VDAcCondition>> vconditions;
  for(int i = 0; i < conditions.size(); i++) {
    Ref<VDAcCondition> condition = conditions[i];
    vconditions.push_back(condition);
  }
  set_conditions(vconditions);
}

Vector<Ref<VDAcCondition>> VDAhsmTransition::get_conditions() const {
  return conditions;
}

Array VDAhsmTransition::get_conditions_open() {
  Array oconditions;
  for(int i = 0; i < conditions.size(); i++) {
    Ref<VDAcCondition> condition = conditions[i];
    oconditions.append(condition);
  }
  return oconditions;
}

void VDAhsmTransition::set_transition_name(String name) {
  transition_name = name;
}

String VDAhsmTransition::get_transition_name() const {
  String name = "";
  if(!transition_name.empty()) {
    name = transition_name;
  } else {
    name = get_class();
  }
  return name;
}

String VDAhsmTransition::to_transition_string() const {
  String class_string = "";
  if(!transition_name.empty()) {
    class_string = transition_name + ":" + itos(get_instance_id());
  } else {
    class_string = "[" + get_class() + ":" + itos(get_instance_id()) + "]";
  }
  return class_string;
}
//////////
// VDAhsmNode
//////////
VDAhsmNode::VDAhsmNode() {}

void VDAhsmNode::_bind_methods() {}

void VDAhsmNode::add_transition(Ref<VDAcState> state, int trigger) {
  if(!transitions.has(trigger)) {
    transitions.set(trigger, state);
  }
}

HashMap<int, Ref<VDAcState>> VDAhsmNode::get_transitions() const {
  return transitions;
}

void VDAhsmNode::set_node_state(Ref<VDAcState> state) {
  node_state = state;
}

Ref<VDAcState> VDAhsmNode::get_node_state() const {
  return node_state;
}

void VDAhsmNode::set_owning_state(Ref<VDAcState> state) {
  owning_state = state;
}

Ref<VDAcState> VDAhsmNode::get_owning_state() const {
  return owning_state;
}

void VDAhsmNode::set_node_name(String name) {
  node_name = name;
}

String VDAhsmNode::get_node_name() const {
  return node_name;
}


