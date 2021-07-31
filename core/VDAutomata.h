#ifndef VDAUTOMATA_H
#define VDAUTOMATA_H

#include "core/resource.h"
#include "./VDState.h"

class VDAcState;
class VDAcStateStructure;

class VDAcAutomata : public Resource {
    GDCLASS(VDAcAutomata, Resource);
protected:
    static void _bind_methods();

    StringName automata_name;
    Ref<VDAcState> root_state;
    HashMap<StringName, Ref<VDAcStateStructure>> state_hierarchy;
public:
    VDAcAutomata();

    void set_automata_name(String name);
    String get_automata_name();
    void set_root_state(Ref<VDAcState> state);
    Ref<VDAcState> get_root_state();

    void add_state(Ref<VDAcState> state, String path = "");
    void remove_state(String path);
    Ref<VDAcState> get_state(String path);
    HashMap<StringName, Ref<VDAcStateStructure>> get_state_hierarchy();
};
//////////
// VDAcStateStructure
//////////
class VDAcStateStructure : public Reference {
    GDCLASS ( VDAcStateStructure, Reference );
protected:
    static void _bind_methods();

    Ref<VDAcAutomata> owning_automata;
    Ref<VDAcState> state;
    Ref<VDAcStateStructure> parent_state_structure;
    HashMap<int, Ref<VDAcStateStructure>> children_state_structures;
public:
    VDAcStateStructure();

    void set_owning_automata(Ref<VDAcAutomata> automata);
    Ref<VDAcAutomata> get_owning_automata();
    void set_owning_state(Ref<VDAcState> state);
    Ref<VDAcState> get_owning_state() const;
    void set_parent_state_structure(Ref<VDAcStateStructure> structure);
    Ref<VDAcStateStructure> get_parent_state_structure();
    HashMap<int, Ref<VDAcStateStructure>> get_children_state_structures();
};
#endif
