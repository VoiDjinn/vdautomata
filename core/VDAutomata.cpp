#include "./VDAutomata.h"
////////////////////
/// VDAcAutomata
////////////////////
VDAcAutomata::VDAcAutomata()
{}

void VDAcAutomata::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "set_automata_name", "name" ), &VDAcAutomata::set_automata_name );
    ClassDB::bind_method ( D_METHOD ( "get_automata_name" ), &VDAcAutomata::get_automata_name );
    ClassDB::bind_method ( D_METHOD ( "set_root_state", "root" ), &VDAcAutomata::set_root_state );
    ClassDB::bind_method ( D_METHOD ( "get_root_state" ), &VDAcAutomata::get_root_state );
    ClassDB::bind_method ( D_METHOD ( "add_state", "state", "path" ), &VDAcAutomata::add_state, DEFVAL("") );
    ClassDB::bind_method ( D_METHOD ( "remove_state", "path" ), &VDAcAutomata::remove_state );
    ClassDB::bind_method ( D_METHOD ( "get_state", "path" ), &VDAcAutomata::get_state );

    ADD_PROPERTY ( PropertyInfo ( Variant::STRING, "automata_name" ), "set_automata_name", "get_automata_name" );
    ADD_PROPERTY ( PropertyInfo ( Variant::OBJECT, "root_state", PROPERTY_HINT_RESOURCE_TYPE, "VDAcState" ), "set_automata_root", "get_automata_root" );
}

void VDAcAutomata::set_automata_name(String name)
{
    this->automata_name = name;
}

String VDAcAutomata::get_automata_name()
{
    return this->automata_name;
}

void VDAcAutomata::set_root_state(Ref<VDAcState> state)
{
    this->root_state = state;
}

Ref<VDAcState> VDAcAutomata::get_root_state()
{
    return this->root_state;
}

void VDAcAutomata::add_state(Ref<VDAcState> state, String path)
{
    if(state.is_valid()) {
        StringName parent_path;
        StringName current_path;
        std::string std_id = std::to_string(state->get_instance_id());
        String state_id = String(std_id.c_str());
        String scurrent_path = "";
        Ref<VDAcStateStructure> parent_structure;
        if(path.length() > 0) {
            parent_path = StringName(path);
            scurrent_path = path + "." + state_id;
            current_path = StringName(scurrent_path);
            if(this->state_hierarchy.has(parent_path)) {
                parent_structure = this->state_hierarchy.get(parent_path);
            }
        } else {
            scurrent_path = state_id;
            current_path = StringName(state_id);
        }
        if(!this->state_hierarchy.has(current_path)) {
            Ref<VDAcStateStructure> current_structure = Ref<VDAcStateStructure>(memnew(VDAcStateStructure()));
            current_structure->set_owning_state(state);
            current_structure->set_owning_automata(this);
            if(parent_structure.is_valid()) {
                parent_structure->get_children_state_structures().set(state->get_instance_id(), current_structure);
            }
            this->state_hierarchy.set(current_path, current_structure);
            if(state->has_substates()) {
                Vector<Variant> params;
                params.push_back(scurrent_path);
                state->connect("substate_added", this, "add_state", params);
                for(int i = 0; i < state->get_substates().size(); i++) {
                    Ref<VDAcState> substate = state->get_substates()[i];
                    this->add_state(substate, scurrent_path);
                }
            }
        }
    }
}

void VDAcAutomata::remove_state(String path)
{
    StringName remove_path = StringName(path);
    if(this->state_hierarchy.has(remove_path)) {
        Ref<VDAcStateStructure> remove_structure = this->state_hierarchy.get(remove_path);
        Ref<VDAcStateStructure> parent_structure = remove_structure->get_parent_state_structure();
        Ref<VDAcState> target_state = remove_structure->get_owning_state();
        if(parent_structure.is_valid()) {
            parent_structure->get_children_state_structures().erase(target_state->get_instance_id());
        }
        if(target_state->has_substates()) {
            target_state->disconnect("substate_removed", this, "remove_state");
            for(unsigned int i = 0; i < remove_structure->get_children_state_structures().size(); i++) {
                Ref<VDAcStateStructure> sub_structure = remove_structure->get_children_state_structures().get(i);
                std::string std_child_id = std::to_string(sub_structure->get_owning_state()->get_instance_id());
                String child_id = String(std_child_id.c_str());
                String child_path = path + "." + child_id;
                this->remove_state(child_path);
            }
            this->state_hierarchy.erase(remove_path);
        }
    }
}

Ref<VDAcState> VDAcAutomata::get_state(String path)
{
    StringName current_path = StringName(path);
    if(this->state_hierarchy.has(current_path)) {
        return this->state_hierarchy.get(current_path)->get_owning_state();
    }
    else if(path.length() == 0) {
        return this->get_root_state();
    }
    return nullptr;
}
////////////////////
/// VDAcStateStructure
////////////////////
VDAcStateStructure::VDAcStateStructure()
{}

void VDAcStateStructure::_bind_methods()
{}

void VDAcStateStructure::set_owning_automata(Ref<VDAcAutomata> automata)
{
    this->owning_automata = automata;
}

Ref<VDAcAutomata> VDAcStateStructure::get_owning_automata()
{
    return this->owning_automata;
}

void VDAcStateStructure::set_owning_state(Ref<VDAcState> state)
{
    this->state = state;
}

Ref<VDAcState> VDAcStateStructure::get_owning_state() const
{
    return this->state;
}

void VDAcStateStructure::set_parent_state_structure(Ref<VDAcStateStructure> structure)
{
    this->parent_state_structure = structure;
}

Ref<VDAcStateStructure> VDAcStateStructure::get_parent_state_structure()
{
    return this->parent_state_structure;
}

HashMap<int, Ref<VDAcStateStructure>> VDAcStateStructure::get_children_state_structures()
{
    return this->children_state_structures;
}
