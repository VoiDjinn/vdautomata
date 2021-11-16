#include "./VDNode.h"

VDAcNode::VDAcNode() {
  Ref<VDAcContext> initial_context = call("_create_context");
  set_automata_context(initial_context);
  set_process(false);
  set_physics_process(false);
}

void VDAcNode::_bind_methods() {
  ClassDB::bind_method(D_METHOD("start"), &VDAcNode::start);
  ClassDB::bind_method(D_METHOD("stop"), &VDAcNode::stop);
  ClassDB::bind_method(D_METHOD("is_running"), &VDAcNode::is_running);

  ClassDB::bind_method(D_METHOD("set_automata", "new_automata"), &VDAcNode::set_automata);
  ClassDB::bind_method(D_METHOD("get_automata"), &VDAcNode::get_automata);
  ClassDB::bind_method(D_METHOD("set_automata_context", "new_context"), &VDAcNode::set_automata_context);
  ClassDB::bind_method(D_METHOD("get_automata_context"), &VDAcNode::get_automata_context);

  ClassDB::bind_method(D_METHOD("set_tick_mode", "mode"), &VDAcNode::set_tick_mode);
  ClassDB::bind_method(D_METHOD("get_tick_mode"), &VDAcNode::get_tick_mode);
  ClassDB::bind_method(D_METHOD("set_is_autostarting", "autostart"), &VDAcNode::set_is_autostarting);
  ClassDB::bind_method(D_METHOD("is_autostarting"), &VDAcNode::is_autostarting);

  ClassDB::bind_method(D_METHOD("_pre_start"), &VDAcNode::_pre_start);
  ClassDB::bind_method(D_METHOD("_on_start"), &VDAcNode::_on_start);
  ClassDB::bind_method(D_METHOD("_pre_stop"), &VDAcNode::_pre_stop);
  ClassDB::bind_method(D_METHOD("_on_stop"), &VDAcNode::_on_stop);
  ClassDB::bind_method(D_METHOD("_create_context"), &VDAcNode::_create_context);

  BIND_VMETHOD(MethodInfo("_pre_start"));
  BIND_VMETHOD(MethodInfo("_on_start"));
  BIND_VMETHOD(MethodInfo("_pre_stop"));
  BIND_VMETHOD(MethodInfo("_on_stop"));
  BIND_VMETHOD(MethodInfo("_create_context"));

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "automata", PROPERTY_HINT_RESOURCE_TYPE, "VDAcAutomata"), "set_automata", "get_automata");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "automata_context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), "set_automata_context", "get_automata_context");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "tick_mode", PROPERTY_HINT_ENUM, "PROCESS, PHYSICS_PROCESS, ALL, NONE"), "set_tick_mode", "get_tick_mode");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autostart"), "set_is_autostarting", "is_autostarting");

  ADD_SIGNAL(MethodInfo("automata_changed", PropertyInfo(Variant::OBJECT, "new_automata", PROPERTY_HINT_RESOURCE_TYPE, "VDAcAutomata"), PropertyInfo(Variant::OBJECT, "old_automata", PROPERTY_HINT_RESOURCE_TYPE, "VDAcAutomata")));
  ADD_SIGNAL(MethodInfo("context_changed", PropertyInfo(Variant::OBJECT, "new_context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext"), PropertyInfo(Variant::OBJECT, "old_context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
}

void VDAcNode::_notification(int p_what) {
  switch(p_what) {
    case NOTIFICATION_READY: {
      if(is_autostarting() && !is_running()) {
        start();
      }
    }
    break;
    case NOTIFICATION_PROCESS: {
      context->set_delta(get_process_delta_time());
      Vector<Ref<VDAcStateStructure>> active_state_structures = context->get_active_structures();
      for(int i = 0; i < active_state_structures.size(); i++) {
        Ref<VDAcStateStructure> structure = active_state_structures[i];
        structure->get_owning_state()->call("tick", context, structure);
      }
    }
    break;
    case NOTIFICATION_PHYSICS_PROCESS: {
      context->set_delta(get_physics_process_delta_time());
      Vector<Ref<VDAcStateStructure>> active_state_structures = context->get_active_structures();
      for(int i = 0; i < active_state_structures.size(); i++) {
        Ref<VDAcStateStructure> structure = active_state_structures[i];
        structure->get_owning_state()->call("tick", context, structure);
      }
    }
    break;
  }
}

void VDAcNode::start() {
  ERR_FAIL_COND_MSG(automata.is_null(), "Start: Automata is null.");
  ERR_FAIL_COND_MSG(context.is_null(), "Start: Context is null.");
  ERR_FAIL_COND_MSG(!is_inside_tree(), "Start: Node not inside scene tree.");
  if(!is_running()) {
    Ref<VDAcState> root = automata->get_root_state();
    Ref<VDAcStateStructure> root_structure = automata->get_root_structure();
    ERR_FAIL_COND_MSG(root.is_null(), "Start: Root state is null.");
    context->set_owning_automata(automata);
    context->call_deferred("_setup");
    call_deferred("_pre_start");
    root->call_deferred("init", context, root_structure);
    root->call_deferred("enter", context, root_structure);
    if(root->call("has_tick")) {
      set_process(tick_mode == ALL || tick_mode == PROCESS);
      set_physics_process(tick_mode == ALL || tick_mode == PHYSICS_PROCESS);
    }
    call_deferred("_on_start");
  } else {
    WARN_PRINT("Start: Is already running.");
  }
}

void VDAcNode::stop() {
  ERR_FAIL_COND_MSG(automata.is_null(), "Stop: Automata is null.");
  ERR_FAIL_COND_MSG(context.is_null(), "Stop: Context is null.");
  if(is_running()) {
    Ref<VDAcState> root = automata->get_root_state();
    Ref<VDAcStateStructure> root_structure = automata->get_root_structure();
    ERR_FAIL_COND_MSG(root.is_null(), "Stop: Root state is null.");
    call_deferred("_pre_stop");
    root->call_deferred("exit", context, root_structure);
    root->call_deferred("deinit", context, root_structure);
    set_process(false);
    set_physics_process(false);
    context->call_deferred("_undo");
    call_deferred("_on_stop");
  } else {
    WARN_PRINT("Stop: Is not running.");
  }
}

bool VDAcNode::is_running() const {
  return is_processing() || is_physics_processing();
}

void VDAcNode::_pre_start() {}
void VDAcNode::_on_start() {}
void VDAcNode::_pre_stop() {}
void VDAcNode::_on_stop() {}

Ref<VDAcContext> VDAcNode::_create_context() {
  return Ref<VDAcContext> (memnew(VDAcContext()));
}

void VDAcNode::set_automata(Ref<VDAcAutomata> new_automata) {
  ERR_FAIL_COND_MSG(is_running(), "Set automata: Node is running.");
  if(automata != new_automata) {
    Ref<VDAcAutomata> old_automata;
    automata = new_automata;
    emit_signal("automata_changed", new_automata, old_automata);
  }
}

Ref<VDAcAutomata> VDAcNode::get_automata() {
  return automata;
}

void VDAcNode::set_automata_context(Ref<VDAcContext> new_context) {
  ERR_FAIL_COND_MSG(is_running(), "Set context: Node is running.");
  if(context != new_context) {
    Ref<VDAcContext> old_context;
    context = new_context;
    emit_signal("context_changed", new_context, old_context);
  }
}

Ref<VDAcContext> VDAcNode::get_automata_context() const {
  return context;
}

void VDAcNode::set_tick_mode(VDAcNode::VDATickMode mode) {
  ERR_FAIL_COND_MSG(is_running(), "Set tick mode: Node is running.");
  tick_mode = mode;
}

VDAcNode::VDATickMode VDAcNode::get_tick_mode() const {
  return tick_mode;
}

void VDAcNode::set_is_autostarting(bool autostart) {
  bautostart = autostart;
}

bool VDAcNode::is_autostarting() const {
  return bautostart;
}
