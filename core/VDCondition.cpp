#include "VDCondition.h"
//////////
/// VDAcCondition
//////////
VDAcCondition::VDAcCondition() {}

void VDAcCondition::_bind_methods() {
  ClassDB::bind_method(D_METHOD("can_pass", "context"), &VDAcCondition::can_pass);
  ClassDB::bind_method(D_METHOD("_on_check", "context"), &VDAcCondition::_on_check);

  ClassDB::bind_method(D_METHOD("set_condition_name", "name"), &VDAcCondition::set_condition_name);
  ClassDB::bind_method(D_METHOD("get_condition_name"), &VDAcCondition::get_condition_name);
  ClassDB::bind_method(D_METHOD("set_is_not", "is_not"), &VDAcCondition::set_is_not);
  ClassDB::bind_method(D_METHOD("is_not"), &VDAcCondition::is_not);

  BIND_VMETHOD(MethodInfo("_on_check", PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext")));

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "condition_name"), "set_condition_name", "get_condition_name");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "NOT"), "set_is_not", "is_not");
}

bool VDAcCondition::can_pass(Ref<VDAcContext> context) {
  bool pass = call("_on_check", context);
  return bis_not ? !pass : pass;
}

bool VDAcCondition::_on_check(Ref<VDAcContext> context) {
  return true;
}

void VDAcCondition::set_condition_name(String name) {
  if(condition_name != name) {
    condition_name = name;
  }
}

String VDAcCondition::get_condition_name() const {
  return condition_name;
}

void VDAcCondition::set_is_not(bool is_not) {
  if(bis_not != is_not) {
    bis_not = is_not;
  }
}

bool VDAcCondition::is_not() const {
  return bis_not;
}
//////////
/// VDAcFunctionCondition
//////////
VDAcFunctionCondition::VDAcFunctionCondition() {}

void VDAcFunctionCondition::_bind_methods() {}

bool VDAcFunctionCondition::_on_check(Ref<VDAcContext> context) {
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

bool VDAcGroupedCondition::_on_check(Ref<VDAcContext> context) {
  for(int i = 0; i < subconditions.size(); i++) {
    Ref<VDAcCondition> condition = subconditions[i];
    bool result = condition->can_pass(context);
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

