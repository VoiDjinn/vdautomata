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

    const StringName PATH_ROOT = StringName("/");

    String automata_name;
    Ref<VDAcStateStructure> root_structure;
    List<StringName> state_hierarchy_paths;
    HashMap<StringName, Ref<VDAcStateStructure>> state_hierarchy;

    void _on_substate_added(Ref<VDAcState> substate, StringName path, Ref<VDAcStateStructure> structure);
    void _on_substate_removed(Ref<VDAcState> substate, StringName path, Ref<VDAcStateStructure> structure);
public:
    VDAcAutomata();

    void set_automata_name(String name);
    String get_automata_name();
    void set_root_state(Ref<VDAcState> state);
    Ref<VDAcState> get_root_state() const;
    Ref<VDAcStateStructure> get_root_structure() const;

    void set_state(Ref<VDAcState> state, StringName path = "/");
    Ref<VDAcState> get_state(StringName path);
    Ref<VDAcStateStructure> get_structure(StringName path);
    void remove_state(StringName path);
    void set_state_hierarchy(HashMap<StringName, Ref<VDAcStateStructure>> hierarchy);
    HashMap<StringName, Ref<VDAcStateStructure>> get_state_hierarchy() const;
    void set_state_hierarchy_open(Dictionary hierarchy);
    Dictionary get_state_hierarchy_open() const;
private:
    void add_structure_to_parent_structure(Ref<VDAcStateStructure> structure, Ref<VDAcStateStructure> parent_structure);
    void remove_structure_from_parent_structure(Ref<VDAcStateStructure> structure, Ref<VDAcStateStructure> parent_structure);
    void add_substates_to_structure(Ref<VDAcState> state, Ref<VDAcStateStructure> structure);
    void remove_substates_from_structure(Ref<VDAcState> state, Ref<VDAcStateStructure> structure);
    void add_structure_to_hierarchy(Ref<VDAcStateStructure> structure);
    void remove_structure_from_hierarchy(Ref<VDAcStateStructure> structure);
};
//////////
// VDAcStateStructure
//////////
class VDAcStateStructure : public Resource {
    GDCLASS ( VDAcStateStructure, Resource );
protected:
    static void _bind_methods();

    friend class VDAcAutomata;

    String display_name;
    StringName automata_path;
    Ref<VDAcAutomata> owning_automata;
    Ref<VDAcState> state;
    HashMap<StringName, Ref<VDAcStateStructure>> children_structures;
    List<StringName> children_structure_paths;
    Ref<VDAcStateStructure> parent_structure;

    void set_owning_automata(Ref<VDAcAutomata> automata);
    void set_parent_structure(Ref<VDAcStateStructure> structure);
    void set_child_structure(Ref<VDAcStateStructure> structure, StringName path);
    void remove_child_structure(StringName path);
    void set_children_structures(HashMap<StringName, Ref<VDAcStateStructure>> structures);
    void set_children_structures_open(Dictionary structures);
public:
    VDAcStateStructure();

    StringName get_automata_path() const;
    Ref<VDAcAutomata> get_owning_automata();
    void set_owning_state(Ref<VDAcState> state);
    Ref<VDAcState> get_owning_state() const;

    Ref<VDAcStateStructure> get_child_structure(StringName path) const;
    HashMap<StringName, Ref<VDAcStateStructure>> get_children_structures() const;
    Dictionary get_children_structures_open() const;
    List<StringName> get_children_structure_paths() const;
    bool has_child_structure(Ref<VDAcStateStructure> structure) const;
    bool has_child_structure_path(StringName path) const;
    Ref<VDAcStateStructure> get_parent_structure() const;

    void set_display_name(String name);
    String get_display_name() const;
};
#endif
