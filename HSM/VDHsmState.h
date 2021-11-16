#ifndef VDHSMSTATE_H
#define VDHSMSTATE_H

#include "core/hash_map.h"
#include "core/reference.h"
#include "core/func_ref.h"

#include "../core/VDAutomata.h"
#include "../core/VDState.h"
#include "../core/VDCondition.h"

class VDAhsmTransition;
class VDAhsmNode;

/*
 * WIP:
 * State manager
 * : currently, State structure cant be preserved; Multiple Context-objects should not hold tree structures seperately.
 * = ???
 *
 * Condition resources
 * : encapsulated values, that provide complex compare algorithm
 * = abstract condition class, with virtual compare method
 *
 * Automatic / Eventless transitions
 * : Transitions, that auto transit if conditions are met
 * =  do conditional check from current states and transit, if applicable
 * -> check on signal: if context parameter was changed, recheck (X)
 * -> check on update: provide mechanism to recheck on update call (X)
 * -> check on tick: check on every frame if transition applicable (?)
 *
 * Pseudostates
 * : provide said states to create more flexible flow machines
 * = ???
 * -> Entry pseudostate = to allow different entry point
 * -> Exit Pseudostate = to quit the current machine
 *
 * Thread pool
 * : Usage of threads in efficient, manageable manner
 * = ???
 * -> VDAcAsyncNode, extends VDAcNode
*/
//////////
//////////
// VDAhsmOrthogonalState
//////////
class VDAhsmOrthogonalState : public VDAcParentState {
    GDCLASS(VDAhsmOrthogonalState, VDAcParentState);
protected:
    static void _bind_methods();

    //TODO: pseudostates aka. dynamic choice points
#ifdef TOOLS_ENABLED
    //define Ref<VDAhsmNode> default_subnode;
    //define HashMap<String, Ref<VDAhsmNode>> subnodes;
    //define List<String> subnode_keys;
#endif
public:
    VDAhsmOrthogonalState();

    virtual void restore_history_deep();
    virtual void restore_history_shallow();
};
//////////
//////////
// VDAhsmCompositeState
//////////
class VDAhsmCompositeState : public VDAcParentState {
    GDCLASS(VDAhsmCompositeState, VDAcParentState);
protected:
    static void _bind_methods();

    Ref<VDAcState> default_substate;
    Vector<Ref<VDAhsmTransition>> transitions;
    HashMap<StringName, Vector<Ref<VDAhsmTransition>>> state_map;
    //TODO: pseudostates aka. dynamic choice points
#ifdef TOOLS_ENABLED
    //define Ref<VDAhsmNode> default_subnode;
    //define HashMap<String, Ref<VDAhsmNode>> subnodes;
    //define List<String> subnode_keys;
#endif

    Ref<VDAhsmTransition> get_viable_transition(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
public:
    VDAhsmCompositeState();

    virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param = 0, Variant new_value = 0, Variant old_value = 0) override;
    virtual void exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;

    void add_transition(Ref<VDAhsmTransition> transition);
    void remove_transition(Ref<VDAhsmTransition> transition);
    void set_transitions(Vector<Ref<VDAhsmTransition>> new_transitions);
    Vector<Ref<VDAhsmTransition>> get_transitions() const;
    void set_transitions_open(Array new_transitions);
    Array get_transitions_open() const;

    void set_default_substate(Ref<VDAcState> substate);
    Ref<VDAcState> get_default_substate() const;
    bool is_valid_state_map_entry(Ref<VDAcState> substate);
protected:
    virtual void _on_substate_added(Ref<VDAcState> substate) override;
    virtual void _on_substate_removed(Ref<VDAcState> substate) override;

    virtual void _handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void _handle_context_params_updated(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    void _internal_transition(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
private:
    virtual void add_state_map_entry(StringName entry_name, Ref<VDAcState> substate);
    virtual void remove_state_map_entry(StringName entry_name, Ref<VDAcState> substate);
    void update_transition_from_state(Ref<VDAcState> new_state, Ref<VDAcState> old_state, Ref<VDAhsmTransition> transition);
};
//////////
// VDAhsmTransition
//////////
class VDAhsmTransition : public Resource {
    GDCLASS(VDAhsmTransition, Resource);

    friend class VDAhsmCompositeState;

    String transition_name;
    Ref<VDAcState> from;
    Ref<VDAcState> to;
    Ref<FuncRef> transition_delegate;
    Vector<Ref<VDAcCondition>> conditions;
protected:
    static void _bind_methods();

    bool transit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual bool _can_transit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) const;
public:
    VDAhsmTransition();

    void set_from_state(Ref<VDAcState> state);
    Ref<VDAcState> get_from_state() const;
    void set_to_state(Ref<VDAcState> state);
    Ref<VDAcState> get_to_state() const;
    void set_transition_delegate(Ref<FuncRef> delegate);
    Ref<FuncRef> get_transition_delegate() const;

    void set_conditions(Vector<Ref<VDAcCondition>> new_conditions);
    void set_conditions_open(Array conditions);
    Vector<Ref<VDAcCondition>> get_conditions() const;
    Array get_conditions_open();

    void set_transition_name(String name);
    String get_transition_name() const;
    virtual String to_transition_string() const;
};
//////////
// VDAhsmNode
//////////
class VDAhsmNode : public Reference {
    GDCLASS(VDAhsmNode, Reference);
protected:
    static void _bind_methods();

    Vector2 pos;

    String node_name;
    Ref<VDAcState> node_state;
    Ref<VDAcState> owning_state;
    HashMap<int, Ref<VDAcState>> transitions;
public:
    VDAhsmNode();

    void set_node_name(String name);
    String get_node_name() const;

    void set_node_state(Ref<VDAcState> state);
    Ref<VDAcState> get_node_state() const;
    void set_owning_state(Ref<VDAcState> state);
    Ref<VDAcState> get_owning_state() const;

    void add_transition(Ref<VDAcState> state, int trigger);
    HashMap<int, Ref<VDAcState>> get_transitions() const;
};

#endif
