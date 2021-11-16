#include "VDCondition.h"
//////////
/// VDAcCondition
//////////
VDAcCondition::VDAcCondition() {}

void VDAcCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_pass", "context"), &VDAcCondition::_pass);

  BIND_VMETHOD(MethodInfo("_pass", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));
}

bool VDAcCondition::_pass(Ref<VDAcContext> context) {
  return false;
}
//////////
/// VDAcContextCondition
//////////
VDAcContextCondition::VDAcContextCondition() {}

void VDAcContextCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_parameters", "new_parameters"), &VDAcContextCondition::set_parameters_open);
  ClassDB::bind_method(D_METHOD("get_parameters"), &VDAcContextCondition::get_parameters_open);

  ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters"), "set_parameters", "get_parameters");

  ADD_SIGNAL(MethodInfo("parameter_added", PropertyInfo(Variant::NIL, "parameter")));
  ADD_SIGNAL(MethodInfo("parameter_removed", PropertyInfo(Variant::NIL, "parameter")));
  ADD_SIGNAL(MethodInfo("parameters_changed"));
}

bool VDAcContextCondition::_pass(Ref<VDAcContext> context) {
  HashMap<Variant, Variant, VariantHasher> context_params = context->get_context_params();
  for(int i = 0; i < parameter_keys.size(); i++) {
    const Variant &key = parameter_keys[i];
    const Variant &parameter = parameters[key];
    if(!context_params.has(key) || context_params[key] != parameter) {
      return false;
    }
  }
  return true;
}

Vector<Variant> VDAcContextCondition::get_parameter_keys() const {
  return parameter_keys;
}

void VDAcContextCondition::set_parameters(HashMap<Variant, Variant, VariantHasher> new_parameters) {
  int removed = 0;
  List<Variant> new_entries;
  List<Variant> contained_entries;
  List<Variant> new_keys;
  new_parameters.get_key_list(&new_keys);
  for(int i = 0; i < new_keys.size(); i++) {
    const Variant &key = new_keys[i];
    if(parameters.has(key)) {
      contained_entries.push_back(key);
    } else {
      new_entries.push_back(key);
    }
  }
  for(int i = 0; i < parameter_keys.size(); i++) {
    const Variant &key = parameter_keys[i];
    if(!contained_entries.find(key)) {
      parameters.erase(key);
      parameter_keys.erase(key);
      removed++;
      emit_signal("parameter_removed", key);
    }
  }
  for(int i = 0; i < new_entries.size(); i++) {
    const Variant &key = new_entries[i];
    parameters.set(key, new_parameters[key]);
    parameter_keys.push_back(key);
    emit_signal("parameter_added", key);
  }
  if(removed > 0 || new_entries.size() > 0) {
    emit_signal("parameters_changed");
    property_list_changed_notify();
  }
}

HashMap<Variant, Variant, VariantHasher> VDAcContextCondition::get_parameters() const {
  return parameters;
}

void VDAcContextCondition::set_parameters_open(Dictionary new_parameters) {
  HashMap<Variant, Variant, VariantHasher> hash_parameters;
  List<Variant> keys;
  new_parameters.get_key_list(&keys);
  for(int i = 0; i < keys.size(); i++) {
    const Variant &key = keys[i];
    const Variant &value = new_parameters[key];
    hash_parameters[key] = value;
  }
  set_parameters(hash_parameters);
}

Dictionary VDAcContextCondition::get_parameters_open() {
  Dictionary dict;
  for(int i = 0; i < parameter_keys.size(); i++) {
    const Variant &key = parameter_keys[i];
    const Variant &value = parameters[key];
    dict[key] = value;
  }
  return dict;
}
//////////
/// VDAcFunctionCondition
//////////
VDAcFunctionCondition::VDAcFunctionCondition() {}

void VDAcFunctionCondition::_bind_methods() {}

bool VDAcFunctionCondition::_pass(Ref<VDAcContext> context) {
  return false;
}
//////////
/// VDAcGroupedCondition
//////////
VDAcGroupedCondition::VDAcGroupedCondition() {}

void VDAcGroupedCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_subconditions", "new_subconditions"), &VDAcGroupedCondition::set_subconditions_open);
  ClassDB::bind_method(D_METHOD("get_subconditions"), &VDAcGroupedCondition::get_subconditions_open);
  ClassDB::bind_method(D_METHOD("set_comparator", "new_comparator"), &VDAcGroupedCondition::set_comparator);
  ClassDB::bind_method(D_METHOD("get_comparator"), &VDAcGroupedCondition::get_comparator);

  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "subconditions", PROPERTY_HINT_TYPE_STRING, "17/17:VDAcCondition"), "set_subconditions", "get_subconditions");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "comparator", PROPERTY_HINT_ENUM, "ALL, ANY, NONE"), "set_comparator", "get_comparator");

  ADD_SIGNAL(MethodInfo("subcondition_added", PropertyInfo(Variant::OBJECT, "added_subcondition", PROPERTY_HINT_RESOURCE_TYPE, "VDAcCondition")));
  ADD_SIGNAL(MethodInfo("subcondition_removed", PropertyInfo(Variant::OBJECT, "removed_subcondition", PROPERTY_HINT_RESOURCE_TYPE, "VDAcCondition")));
  ADD_SIGNAL(MethodInfo("subconditions_changed"));

  ADD_SIGNAL(MethodInfo("comparator_changed", PropertyInfo(Variant::INT, "new_comparator"), PropertyInfo(Variant::INT, "old_comparator")));
}

bool VDAcGroupedCondition::_pass(Ref<VDAcContext> context) {
  for(int i = 0; i < subconditions.size(); i++) {
    Ref<VDAcCondition> condition = subconditions[i];
    bool result = condition->call("_pass", context);
    switch(comparator) {
      case ALL:
        if(!result) return false;
      case ANY:
        if(result) return true;
      case NONE:
        if(result) return false;
    }
  }
  return comparator == ANY ? false : true;
}

void VDAcGroupedCondition::set_subconditions(Vector<Ref<VDAcCondition>> new_subconditions) {
  int added = 0;
  int removed = 0;
  for(int i = 0; i < new_subconditions.size(); i++) {
    Ref<VDAcCondition> subcondition = new_subconditions[i];
    if(subconditions.find(subcondition) < 0) {
      subconditions.insert(i, subcondition);
      emit_signal("subcondition_added", subcondition);
      added++;
    }
  }
  int condition_index;
  for(condition_index = 0; condition_index < subconditions.size(); condition_index++) {
    Ref<VDAcCondition> subcondition = subconditions[condition_index];
    if(new_subconditions.find(subcondition) < 0) {
      subconditions.erase(subcondition);
      emit_signal("subcondition_removed", subcondition);
      condition_index--;
      removed++;
    }
  }
  if(removed > 0 || added > 0) {
    emit_signal("subconditions_changed");
    property_list_changed_notify();
  }
}

void VDAcGroupedCondition::set_subconditions_open(Array new_subconditions) {
  Vector<Ref<VDAcCondition>> vconditions;
  for(int i = 0; i < new_subconditions.size(); i++) {
    Ref<VDAcCondition> condition = new_subconditions[i];
    vconditions.push_back(condition);
  }
  set_subconditions(vconditions);
}

Vector<Ref<VDAcCondition>> VDAcGroupedCondition::get_subconditions() const {
  return subconditions;
}

Array VDAcGroupedCondition::get_subconditions_open() const {
  Array oconditions;
  for(int i = 0; i < subconditions.size(); i++) {
    Ref<VDAcCondition> condition = subconditions[i];
    oconditions.append(condition);
  }
  return oconditions;
}

void VDAcGroupedCondition::set_comparator(VDAcGroupedCondition::VDSubconditionComparator new_comparator) {
  if(comparator != new_comparator) {
    VDSubconditionComparator old_comparator = comparator;
    comparator = new_comparator;
    emit_signal("comparator_changed", new_comparator, old_comparator);
  }
}

VDAcGroupedCondition::VDSubconditionComparator VDAcGroupedCondition::get_comparator() const {
  return comparator;
}

