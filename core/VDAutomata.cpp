#include "./VDAutomata.h"
////////////////////
////////////////////
/// VDAcAutomata ///
////////////////////
////////////////////
VDAcAutomata::VDAcAutomata()
{
    Ref<VDAcStateStructure> new_root = Ref<VDAcStateStructure>(memnew(VDAcStateStructure()));
    new_root->set_owning_automata(this);
    new_root->set_display_name("ROOT");
    new_root->automata_path = PATH_ROOT;
    root_structure = new_root;
    add_structure_to_hierarchy(new_root);
}

void VDAcAutomata::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_automata_name", "name"), &VDAcAutomata::set_automata_name);
    ClassDB::bind_method ( D_METHOD ( "get_automata_name" ), &VDAcAutomata::get_automata_name );
    ClassDB::bind_method ( D_METHOD ( "set_root_state", "root" ), &VDAcAutomata::set_root_state );
    ClassDB::bind_method ( D_METHOD ( "get_root_state" ), &VDAcAutomata::get_root_state );
    ClassDB::bind_method ( D_METHOD ( "get_root_structure" ), &VDAcAutomata::get_root_structure );

    ClassDB::bind_method ( D_METHOD ( "set_state", "state", "path" ), &VDAcAutomata::set_state, DEFVAL("/") );
    ClassDB::bind_method ( D_METHOD ( "get_state", "path" ), &VDAcAutomata::get_state );
    ClassDB::bind_method ( D_METHOD ( "remove_state", "path" ), &VDAcAutomata::remove_state );
    ClassDB::bind_method ( D_METHOD ( "set_state_hierarchy", "hierarchy" ), &VDAcAutomata::set_state_hierarchy_open );
    ClassDB::bind_method ( D_METHOD ( "get_state_hierarchy" ), &VDAcAutomata::get_state_hierarchy_open );

    ClassDB::bind_method ( D_METHOD ( "_on_substate_added", "substate", "path", "structure" ), &VDAcAutomata::_on_substate_added );
    ClassDB::bind_method ( D_METHOD ( "_on_substate_removed", "substate", "path", "structure" ), &VDAcAutomata::_on_substate_removed );

    BIND_VMETHOD (MethodInfo ("_on_substate_added", PropertyInfo (Variant::OBJECT, "added_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo (Variant::STRING, "path"), PropertyInfo (Variant::OBJECT, "owning_structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
    BIND_VMETHOD (MethodInfo ("_on_substate_removed", PropertyInfo (Variant::OBJECT, "added_substate", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo (Variant::STRING, "path"), PropertyInfo (Variant::OBJECT, "owning_structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "automata_name"), "set_automata_name", "get_automata_name");
    ADD_PROPERTY(PropertyInfo (Variant::OBJECT, "root_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), "set_root_state", "get_root_state");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "state_hierarchy", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure"/*, PROPERTY_USAGE_NOEDITOR */), "set_state_hierarchy", "get_state_hierarchy");

    ADD_SIGNAL(MethodInfo("automata_name_changed", PropertyInfo(Variant::STRING, "new_name"), PropertyInfo(Variant::STRING, "old_name")));
    ADD_SIGNAL(MethodInfo("root_state_changed", PropertyInfo(Variant::OBJECT, "new_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState"), PropertyInfo(Variant::OBJECT, "old_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
    ADD_SIGNAL(MethodInfo("state_hierarchy_changed"));
    ADD_SIGNAL(MethodInfo("state_hierarchy_updated"));
    ADD_SIGNAL(MethodInfo("structure_added", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
    ADD_SIGNAL(MethodInfo("structure_removed", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure")));
}

void VDAcAutomata::set_automata_name(String name)
{
    if(automata_name != name) {
        String old_name = automata_name;
        automata_name = name;
        emit_signal("automata_name_changed", name, old_name);
        property_list_changed_notify();
    }
}

String VDAcAutomata::get_automata_name()
{
    return automata_name;
}

void VDAcAutomata::set_root_state(Ref<VDAcState> state)
{
    ERR_FAIL_COND_MSG(!root_structure.is_valid(), "Set root: Non-valid root structure.");
    Ref<VDAcState> old = root_structure->get_owning_state();
    if(old != state) {
        if(old.is_valid()) {
            remove_substates_from_structure(old, root_structure);
        }
        root_structure->set_owning_state(state);
        if(state.is_valid()) {
            add_substates_to_structure(state, root_structure);
        }
        emit_signal("root_state_changed", state, old);
        property_list_changed_notify();
    }
}

Ref<VDAcState> VDAcAutomata::get_root_state() const
{
    Ref<VDAcState> state;
    if(root_structure.is_valid()) {
        state = root_structure->get_owning_state();
    }
    return state;
}

Ref<VDAcStateStructure> VDAcAutomata::get_root_structure() const
{
    return root_structure;
}

void VDAcAutomata::set_state(Ref<VDAcState> state, StringName path)
{
    ERR_FAIL_COND_MSG(!state.is_valid(), "Set: State not valid.");
    Ref<VDAcStateStructure> parent_structure = get_structure(path);
    ERR_FAIL_COND_MSG(!parent_structure.is_valid(), "Set: Path not valid.");
    bool is_root = path == "/" || String(path).empty();
    StringName current_state_path = String(!is_root ? path : "") + "/" + state->get_state_ident();
    Ref<VDAcStateStructure> current_structure;
    if(parent_structure->has_child_structure_path(current_state_path)) {
        current_structure = parent_structure->get_child_structure(current_state_path);
        Ref<VDAcState> old_state = current_structure->get_owning_state();
        if(old_state != state) {
            if(old_state.is_valid()) {
                remove_substates_from_structure(old_state, current_structure);
            }
            current_structure->set_owning_state(state);
            current_structure->set_display_name(state->get_state_name());
            current_structure->set_parent_structure(parent_structure);
            current_structure->automata_path = current_state_path;
            if(state.is_valid()) {
                add_substates_to_structure(state, current_structure);
            } else {
                remove_structure_from_parent_structure(current_structure, parent_structure);
                if(!is_root) {
                    remove_structure_from_hierarchy(current_structure);
                }
            }
            property_list_changed_notify();
        } else {
            // Same state, so do nothing
            return;
        }
    } else {
        current_structure = Ref<VDAcStateStructure>(memnew(VDAcStateStructure()));
        current_structure->set_owning_automata(this);
        current_structure->set_owning_state(state);
        current_structure->set_display_name(state->get_state_name());
        current_structure->set_parent_structure(parent_structure);
        current_structure->automata_path = current_state_path;
        add_structure_to_hierarchy(current_structure);
        add_structure_to_parent_structure(current_structure, parent_structure);
        if(state.is_valid()) {
            add_substates_to_structure(state, current_structure);
        }
        property_list_changed_notify();
    }
}

Ref<VDAcState> VDAcAutomata::get_state(StringName path)
{
    if(state_hierarchy.has(path)) {
        return state_hierarchy.get(path)->get_owning_state();
    }
    else if(String(path).length() == 0) {
        return get_root_state();
    }
    return nullptr;
}

Ref<VDAcStateStructure> VDAcAutomata::get_structure(StringName path)
{
    String comp_path = String(path);
    if(!comp_path.empty() && comp_path != "/") {
        StringName search_path = StringName(path);
        if(state_hierarchy.has(path)) {
            return state_hierarchy[path];
        } else {
            ERR_FAIL_COND_V_MSG(true, nullptr,"Get structure: No structure with given path.");
        }
    } else {
        return get_root_structure();
    }
}

// TODO: recheck cleaning for substates (didnt worked on last test)
void VDAcAutomata::remove_state(StringName path)
{
    ERR_FAIL_COND_MSG(path == "/", "Remove: Cannot remove root structure.");
    ERR_FAIL_COND_MSG(String(path).length() == 0, "Remove: Empty path.");
    ERR_FAIL_COND_MSG(!state_hierarchy.has(path), "Remove: Non-valid path.");
    Ref<VDAcStateStructure> remove_structure = state_hierarchy.get(path);
    Ref<VDAcState> state = remove_structure->get_owning_state();
    if(state.is_valid()) {
        remove_substates_from_structure(state, remove_structure);
    }
    if(remove_structure->get_parent_structure().is_valid()) {
        remove_structure_from_parent_structure(remove_structure, remove_structure->get_parent_structure());
    }
    remove_structure_from_hierarchy(remove_structure);
    property_list_changed_notify();
}

void VDAcAutomata::set_state_hierarchy(HashMap<StringName, Ref<VDAcStateStructure>> hierarchy)
{
    int removed = 0;
    List<StringName> new_entries;
    List<StringName> contained_entries;
    List<StringName> new_keys;
    hierarchy.get_key_list(&new_keys);
    for(int i = 0; i < new_keys.size(); i++) {
        StringName new_key = new_keys[i];
        if(state_hierarchy.has(new_key)) {
            contained_entries.push_back(new_key);
        } else {
            new_entries.push_back(new_key);
        }
    }
    int path_index;
    for(path_index = 0; path_index < state_hierarchy_paths.size(); path_index++) {
        StringName path = state_hierarchy_paths[path_index];
        if(path != PATH_ROOT && !contained_entries.find(path)) {
            remove_state(path);
            path_index--;
            removed++;
        }
    }
    for(int i = 0; i < new_entries.size(); i++) {
        StringName key = new_entries[i];
        Ref<VDAcStateStructure> structure = hierarchy[key];
        add_structure_to_hierarchy(structure);
        Ref<VDAcState> state = structure->get_owning_state();
        Ref<VDAcParentState> parent_state = Object::cast_to<VDAcParentState>(state.ptr());
        if(parent_state.is_valid()) {
            Vector<Variant> params;
            params.push_back(structure->automata_path);
            params.push_back(structure);
            state->connect("substate_added", this, "_on_substate_added", params);
            state->connect("substate_removed", this, "_on_substate_removed", params);
        }
    }
    if(removed > 0 || new_entries.size() > 0) {
        emit_signal("state_hierarchy_changed");
        property_list_changed_notify();
    }
}

HashMap<StringName, Ref<VDAcStateStructure>> VDAcAutomata::get_state_hierarchy() const
{
    return state_hierarchy;
}

void VDAcAutomata::set_state_hierarchy_open(Dictionary hierarchy)
{
    Array keys = hierarchy.keys();
    HashMap<StringName, Ref<VDAcStateStructure>> new_hierarchy;
    for(int i = 0; i < keys.size(); i++) {
        String key = keys[i];
        Ref<VDAcStateStructure> structure = hierarchy[key];
        structure->set_owning_automata(this);
        new_hierarchy.set(key, structure);
    }
    set_state_hierarchy(new_hierarchy);
}

Dictionary VDAcAutomata::get_state_hierarchy_open() const
{
    Dictionary open_hierarchy;
    for(int i = 0; i < state_hierarchy_paths.size(); i++) {
        StringName key = state_hierarchy_paths[i];
        Ref<VDAcStateStructure> structure = state_hierarchy[key];
        open_hierarchy[key] = structure;
    }
    return open_hierarchy;
}

void VDAcAutomata::add_structure_to_parent_structure(Ref<VDAcStateStructure> structure, Ref<VDAcStateStructure> parent_structure)
{
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Add struct to parent struct: Non-valid structure.");
    ERR_FAIL_COND_MSG(!parent_structure.is_valid(), "Add struct to parent struct: Non-valid parent structure.");
    Ref<VDAcState> state = structure->get_owning_state();
    parent_structure->set_child_structure(structure, state->get_state_ident());
}

void VDAcAutomata::remove_structure_from_parent_structure(Ref<VDAcStateStructure> structure, Ref<VDAcStateStructure> parent_structure)
{
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Remove struct from parent struct: Non-valid structure.");
    ERR_FAIL_COND_MSG(!parent_structure.is_valid(), "Remove struct from parent struct: Non-valid parent structure.");
    Ref<VDAcState> state = structure->get_owning_state();
    parent_structure->remove_child_structure(state->get_state_ident());
}

void VDAcAutomata::add_substates_to_structure(Ref<VDAcState> state, Ref<VDAcStateStructure> structure)
{
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Add substates to struct: Non-valid structure.");
    ERR_FAIL_COND_MSG(!state.is_valid(), "Add substates to struct: Non-valid state.");
    Ref<VDAcParentState> parent_state = Object::cast_to<VDAcParentState>(state.ptr());
    if(parent_state.is_valid()) {
        Vector<Variant> params;
        String state_path = String(structure->automata_path);
        params.push_back(state_path);
        params.push_back(structure);
        state->connect("substate_added", this, "_on_substate_added", params);
        state->connect("substate_removed", this, "_on_substate_removed", params);
        for(int i = 0; i < parent_state->get_substates().size(); i++) {
            set_state(parent_state->get_substates()[i], String(structure->automata_path));
        }
    }
}

void VDAcAutomata::remove_substates_from_structure(Ref<VDAcState> state, Ref<VDAcStateStructure> structure) {
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Remove substates from struct: Non-valid structure.");
    ERR_FAIL_COND_MSG(!state.is_valid(), "Remove substates from struct: Non-valid state.");
    Ref<VDAcParentState> parent_state = Object::cast_to<VDAcParentState>(state.ptr());
    if(parent_state.is_valid()) {
        while(structure->get_children_structure_paths().size()) {
            StringName child_path = structure->get_children_structure_paths()[0];
            Ref<VDAcStateStructure> child_structure = structure->get_child_structure(child_path);
            remove_state(child_structure->get_automata_path());
        }
        state->disconnect("substate_added", this, "_on_substate_added");
        state->disconnect("substate_removed", this, "_on_substate_removed");
    }
}

void VDAcAutomata::add_structure_to_hierarchy(Ref<VDAcStateStructure> structure)
{
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Add structure to hierarchy: Non-valid structure.");
    state_hierarchy.set(structure->automata_path, structure);
    if(!state_hierarchy_paths.find(structure->automata_path)) {
        state_hierarchy_paths.push_back(structure->automata_path);
        emit_signal("structure_added", structure);
    }
    emit_signal("state_hierarchy_updated");
    property_list_changed_notify();
}

void VDAcAutomata::remove_structure_from_hierarchy(Ref<VDAcStateStructure> structure)
{
    ERR_FAIL_COND_MSG(!structure.is_valid(), "Remove structure from hierarchy: Non-valid structure.");
    StringName path = structure->automata_path;
    state_hierarchy.erase(path);
    if(state_hierarchy_paths.find(path)) {
        state_hierarchy_paths.erase(path);
    }
    emit_signal("structure_removed", structure);
    emit_signal("state_hierarchy_updated");
    property_list_changed_notify();
}

void VDAcAutomata::_on_substate_added(Ref<VDAcState> substate, StringName path, Ref<VDAcStateStructure> structure)
{
    set_state(substate, path);
}

void VDAcAutomata::_on_substate_removed(Ref<VDAcState> substate, StringName path, Ref<VDAcStateStructure> structure)
{
    String path_ext = String(path) != "/" ? "/" : "";
    StringName remove_path = String(path) + path_ext + substate->get_state_ident();
    remove_state(remove_path);
}
//////////////////////////
//////////////////////////
/// VDAcStateStructure ///
//////////////////////////
//////////////////////////
VDAcStateStructure::VDAcStateStructure() {}

void VDAcStateStructure::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "set_owning_state", "state" ), &VDAcStateStructure::set_owning_state );
    ClassDB::bind_method ( D_METHOD ( "get_owning_state" ), &VDAcStateStructure::get_owning_state );
    ClassDB::bind_method ( D_METHOD ( "get_automata_path" ), &VDAcStateStructure::get_automata_path );
    ClassDB::bind_method ( D_METHOD ( "get_owning_automata" ), &VDAcStateStructure::get_owning_automata );

    ClassDB::bind_method ( D_METHOD ( "set_children_structures", "structures" ), &VDAcStateStructure::set_children_structures_open );
    ClassDB::bind_method ( D_METHOD ( "get_children_structures" ), &VDAcStateStructure::get_children_structures_open );
    ClassDB::bind_method ( D_METHOD ( "get_child_structure", "path" ), &VDAcStateStructure::get_child_structure );
    ClassDB::bind_method ( D_METHOD ( "has_child_structure", "structure" ), &VDAcStateStructure::has_child_structure );
    ClassDB::bind_method ( D_METHOD ( "has_child_structure_path", "path" ), &VDAcStateStructure::has_child_structure_path );
    ClassDB::bind_method ( D_METHOD ( "get_parent_structure" ), &VDAcStateStructure::get_parent_structure );

    ClassDB::bind_method ( D_METHOD ( "set_display_name", "name" ), &VDAcStateStructure::set_display_name );
    ClassDB::bind_method ( D_METHOD ( "get_display_name" ), &VDAcStateStructure::get_display_name );

    ADD_PROPERTY ( PropertyInfo ( Variant::OBJECT, "owning_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState" ), "set_owning_state", "get_owning_state" );
    ADD_PROPERTY ( PropertyInfo ( Variant::STRING, "display_name"), "set_display_name", "get_display_name" );
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "children_structures", PROPERTY_HINT_RESOURCE_TYPE, "VDAcStateStructure"/*, PROPERTY_USAGE_NOEDITOR */), "set_children_structures", "get_children_structures");

    ADD_SIGNAL(MethodInfo("owning_automata_changed", PropertyInfo(Variant::OBJECT, "automata", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
    ADD_SIGNAL(MethodInfo("parent_structure_changed", PropertyInfo(Variant::OBJECT, "parent_structure", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
    ADD_SIGNAL(MethodInfo("owning_state_changed", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState")));
    ADD_SIGNAL(MethodInfo("display_name_changed", PropertyInfo(Variant::STRING, "new_name"), PropertyInfo(Variant::STRING, "old_name")));
}

// protected
void VDAcStateStructure::set_owning_automata(Ref<VDAcAutomata> automata)
{
    if(owning_automata != automata) {
        owning_automata = automata;
        emit_signal("owning_automata_changed", automata);
        property_list_changed_notify();
    }
}

void VDAcStateStructure::set_parent_structure(Ref<VDAcStateStructure> structure)
{
    if(parent_structure != structure) {
        parent_structure = structure;
        emit_signal("parent_structure_changed", structure);
        property_list_changed_notify();
    }
}

void VDAcStateStructure::set_child_structure(Ref<VDAcStateStructure> structure, StringName path)
{
    children_structures.set(path, structure);
    if(!children_structure_paths.find(path)) {
        children_structure_paths.push_back(path);
    }
    property_list_changed_notify();
}

void VDAcStateStructure::remove_child_structure(StringName path)
{
    if(children_structures.has(path)) {
        children_structures.erase(path);
        if(children_structure_paths.find(path)) {
            children_structure_paths.erase(path);
        }
        property_list_changed_notify();
    }
}

void VDAcStateStructure::set_children_structures(HashMap<StringName, Ref<VDAcStateStructure>> structures)
{
    int removed = 0;
    List<StringName> new_entries;
    List<StringName> contained_entries;
    List<StringName> new_keys;
    structures.get_key_list(&new_keys);
    for(int i = 0; i < new_keys.size(); i++) {
        StringName new_key = new_keys[i];
        if(children_structures.has(new_key)) {
            contained_entries.push_back(new_key);
        } else {
            new_entries.push_back(new_key);
        }
    }
    int path_index;
    for(path_index = 0; path_index < children_structure_paths.size(); path_index++) {
        StringName path = children_structure_paths[path_index];
        if(!contained_entries.find(path)) {
            remove_child_structure(path);
            path_index--;
            removed++;
        }
    }
    for(int i = 0; i < new_entries.size(); i++) {
        StringName path = new_entries[i];
        Ref<VDAcStateStructure> structure = structures[path];
        set_child_structure(structure, path);
    }
    if(removed > 0 || new_entries.size() > 0) {
        emit_signal("children_structures_changed");
        property_list_changed_notify();
    }
}

void VDAcStateStructure::set_children_structures_open(Dictionary structures)
{
    Array keys = structures.keys();
    HashMap<StringName, Ref<VDAcStateStructure>> new_children;
    for(int i = 0; i < keys.size(); i++) {
        String key = keys[i];
        Ref<VDAcStateStructure> structure = structures[key];
        new_children.set(StringName(key), structure);
    }
    set_children_structures(new_children);
}

// public
StringName VDAcStateStructure::get_automata_path() const
{
    return automata_path;
}

void VDAcStateStructure::set_owning_state(Ref<VDAcState> state)
{
    if(this->state != state) {
        this->state = state;
        emit_signal("owning_state_changed", state);
        property_list_changed_notify();
    }
}

Ref<VDAcState> VDAcStateStructure::get_owning_state() const
{
    return state;
}

Ref<VDAcAutomata> VDAcStateStructure::get_owning_automata()
{
    return owning_automata;
}

Ref<VDAcStateStructure> VDAcStateStructure::get_child_structure(StringName path) const
{
    Ref<VDAcStateStructure> structure;
    if(has_child_structure_path(path)) {
        structure = children_structures.get(path);
    }
    return structure;
}

HashMap<StringName, Ref<VDAcStateStructure>> VDAcStateStructure::get_children_structures() const
{
    return children_structures;
}

Dictionary VDAcStateStructure::get_children_structures_open() const {
    Dictionary ostructures;
    for(int i = 0; i < children_structure_paths.size(); i++) {
        StringName path = children_structure_paths[i];
        Ref<VDAcStateStructure> structure = children_structures[path];
        ostructures[path] = structure;
    }
    return ostructures;
}

List<StringName> VDAcStateStructure::get_children_structure_paths() const
{
    return children_structure_paths;
}

bool VDAcStateStructure::has_child_structure(Ref<VDAcStateStructure> structure) const
{
    return structure.is_valid() && structure->get_owning_state().is_valid() && has_child_structure_path(structure->get_owning_state()->get_state_ident());
}

bool VDAcStateStructure::has_child_structure_path(StringName path) const
{
    return children_structures.has(path);
}

Ref<VDAcStateStructure> VDAcStateStructure::get_parent_structure() const {
    return parent_structure;
}

void VDAcStateStructure::set_display_name(String name)
{
    if(display_name != name) {
        String old_name = display_name;
        display_name = name;
        emit_signal("display_name_changed", name, old_name);
        property_list_changed_notify();
    }
}

String VDAcStateStructure::get_display_name() const
{
    return display_name;
}
