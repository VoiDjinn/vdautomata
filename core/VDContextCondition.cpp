#include "VDContextCondition.h"

//////////
/// VDAcContextValueCondition
//////////
VDAcContextValueCondition::VDAcContextValueCondition() {
  internal_comparator = static_cast<Variant::Operator>(value_comparator);
}

void VDAcContextValueCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_context_key", "context_key"), &VDAcContextValueCondition::set_context_key);
  ClassDB::bind_method(D_METHOD("get_context_key"), &VDAcContextValueCondition::get_context_key);
  ClassDB::bind_method(D_METHOD("set_context_value", "context_value"), &VDAcContextValueCondition::set_context_value);
  ClassDB::bind_method(D_METHOD("get_context_value"), &VDAcContextValueCondition::get_context_value);
  ClassDB::bind_method(D_METHOD("set_value_comparator", "value_comparator"), &VDAcContextValueCondition::set_value_comparator);
  ClassDB::bind_method(D_METHOD("get_value_comparator"), &VDAcContextValueCondition::get_value_comparator);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "value_comparator", PROPERTY_HINT_ENUM, "EQUALS,LESS,LESS_THAN,GREATER,GREATER_THAN,IN"), "set_value_comparator", "get_value_comparator");
  ADD_PROPERTY(PropertyInfo(Variant::NIL, "context_key", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT), "set_context_key", "get_context_key");
  ADD_PROPERTY(PropertyInfo(Variant::NIL, "context_value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT), "set_context_value", "get_context_value");
}

bool VDAcContextValueCondition::_on_check(Ref<VDAcContext> context) {
  if(context->has_context_param(context_key)) {
    const Variant comp_value = context->get_context_value(context_key);
    return Variant::evaluate(internal_comparator, comp_value, context_value);
  }
  return false;
}

void VDAcContextValueCondition::set_context_key(Variant key) {
  if(context_key != key) {
    Variant old_key = key;
    context_key = key;
    emit_signal("context_key_changed", key, old_key);
  }
}

Variant VDAcContextValueCondition::get_context_key() const {
  return context_key;
}

void VDAcContextValueCondition::set_context_value(Variant value) {
  if(context_value != value) {
    Variant old_value = value;
    context_value = value;
    emit_signal("context_value_changed", value, old_value);
  }
}

Variant VDAcContextValueCondition::get_context_value() const {
  return context_value;
}

void VDAcContextValueCondition::set_value_comparator(VDAcContextValueCondition::VDValueComparator comparator) {
  if(value_comparator != comparator) {
    VDValueComparator old_comparator = value_comparator;
    value_comparator = comparator;
    internal_comparator = static_cast<Variant::Operator>(comparator);
    emit_signal("value_comparator_changed", comparator, old_comparator);
  }
}

VDAcContextValueCondition::VDValueComparator VDAcContextValueCondition::get_value_comparator() const {
  return value_comparator;
}
//////////
/// VDAcContextParamsCondition
//////////
VDAcContextParamsCondition::VDAcContextParamsCondition() {}

void VDAcContextParamsCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_parameters", "new_parameters"), &VDAcContextParamsCondition::set_parameters_open);
  ClassDB::bind_method(D_METHOD("get_parameters"), &VDAcContextParamsCondition::get_parameters_open);

  ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters"), "set_parameters", "get_parameters");

  ADD_SIGNAL(MethodInfo("parameter_added", PropertyInfo(Variant::NIL, "parameter")));
  ADD_SIGNAL(MethodInfo("parameter_removed", PropertyInfo(Variant::NIL, "parameter")));
  ADD_SIGNAL(MethodInfo("parameters_changed"));
}

bool VDAcContextParamsCondition::_on_check(Ref<VDAcContext> context) {
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

Vector<Variant> VDAcContextParamsCondition::get_parameter_keys() const {
  return parameter_keys;
}

void VDAcContextParamsCondition::set_parameters(HashMap<Variant, Variant, VariantHasher> new_parameters) {
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

HashMap<Variant, Variant, VariantHasher> VDAcContextParamsCondition::get_parameters() const {
  return parameters;
}

void VDAcContextParamsCondition::set_parameters_open(Dictionary new_parameters) {
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

Dictionary VDAcContextParamsCondition::get_parameters_open() {
  Dictionary dict;
  for(int i = 0; i < parameter_keys.size(); i++) {
    const Variant &key = parameter_keys[i];
    const Variant &value = parameters[key];
    dict[key] = value;
  }
  return dict;
}
