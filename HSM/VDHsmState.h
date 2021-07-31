#ifndef VDHSMSTATE_H
#define VDHSMSTATE_H

#include "core/hash_map.h"
#include "core/reference.h"
#include "core/func_ref.h"

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
 * : encapsulated calues, that provide complex compare algorithm
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
 * : Usage of threads in efficient, manageable spread manner
 * = ???
 * -> VDAcAsyncNode, extends VDAcNode
*/

//////////
// VDAhsmState
//////////
class VDAhsmState : public VDAcState {
    GDCLASS(VDAhsmState, VDAcState);
protected:
    static void _bind_methods();

    bool bhas_regions = false;

    Ref<VDAcState> default_substate;
    Vector<Ref<VDAhsmTransition>> transitions;
    HashMap<Ref<VDAcState>, Vector<Ref<VDAhsmTransition>>, VDAcReferenceInstanceHasher<VDAcState>> state_map;
    //TODO: pseudostates aka. dynamic choice points

#ifdef TOOLS_ENABLED
    //define Ref<VDAhsmNode> default_subnode;
    //define HashMap<String, Ref<VDAhsmNode>> subnodes;
    //define List<String> subnode_keys;
#endif

    bool transit(Ref<VDAcState> target, Ref<VDAcContext> context);
    Ref<VDAhsmTransition> get_viable_transition(Ref<VDAcContext> context, Ref<VDAcEvent> event = nullptr);
public:
    VDAhsmState();

    virtual bool tick(Ref<VDAcContext> context) override;
    virtual bool has_tick() override;
    virtual bool dispatch(Ref<VDAcEvent> event, Ref<VDAcContext> context) override;

    virtual void init(Ref<VDAcContext> context) override;
    virtual void enter(Ref<VDAcContext> context) override;
    virtual void update(Ref<VDAcContext> context) override;
    virtual void exit(Ref<VDAcContext> context) override;
    virtual void deinit(Ref<VDAcContext> context) override;
	virtual bool has_substates() const override;

    virtual void restore_history_deep();
    virtual void restore_history_shallow();

    Ref<VDAhsmTransition> create_transition(Ref<VDAcState> from, Ref<VDAcState> to, String name = "", Ref<VDAcEvent> trigger = nullptr, HashMap<Variant, Variant, VariantHasher> conditions = {});
    Ref<VDAhsmTransition> create_transition_open(Ref<VDAcState> from, Ref<VDAcState> to, String name = "", Ref<VDAcEvent> trigger = nullptr, Dictionary conditions = {});
    void add_transition(Ref<VDAhsmTransition> transition);
    void remove_transition(Ref<VDAhsmTransition> transition);
    void set_transitions(Vector<Ref<VDAhsmTransition>> transitions);
    Vector<Ref<VDAhsmTransition>> get_transitions() const;
    void set_transitions_open(Array transitions);
    Array get_transitions_open() const;

    void set_default_substate(Ref<VDAcState> substate);
    Ref<VDAcState> get_default_substate() const;
    void set_has_regions(bool has);
    bool has_regions();
protected:
    virtual void _on_substate_added(Ref<VDAcState> state) override;
    virtual void _on_substate_removed(Ref<VDAcState> state) override;

    void _internal_transition(Ref<VDAcContext> context);
    void _handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context);
    void _handle_context_params_updated(Ref<VDAcContext> context);
};
//////////
// VDAhsmTransition
//////////
class VDAhsmTransition : public Resource {
    GDCLASS(VDAhsmTransition, Resource);

    String transition_name;
    Ref<VDAcState> from;
    Ref<VDAcState> to;
    Ref<FuncRef> transition_delegate;
    Ref<VDAcEvent> triggering_event;

    Vector<Variant> condition_keys;
    HashMap<Variant, Variant, VariantHasher> conditions;

    //TODO: bis_auto_transit WIP
    bool bis_automatic_transition = false;
    bool bis_inner_transition = false;
    bool bis_dynamic_transition = false;
    bool bhas_conditions = false;
    bool bwill_always_occur = false;
protected:
    static void _bind_methods();
public:
    VDAhsmTransition();

    bool transit(Ref<VDAcContext> context);
    bool can_transit(Ref<VDAcContext> context) const;

    void set_from_state(Ref<VDAcState> state);
    Ref<VDAcState> get_from_state() const;
    void set_to_state(Ref<VDAcState> state);
    Ref<VDAcState> get_to_state() const;
    void set_transition_delegate(Ref<FuncRef> delegate);
    Ref<FuncRef> get_transition_delegate() const;
    void set_triggering_event(Ref<VDAcEvent> event);
    Ref<VDAcEvent> get_triggering_event() const;

    Vector<Variant> get_condition_keys() const;
    void set_conditions(HashMap<Variant, Variant, VariantHasher> conditions);
    HashMap<Variant, Variant, VariantHasher> get_conditions() const;
    void set_conditions_open(Dictionary conditions);
    Dictionary get_conditions_open();

    void set_is_automatic_transition(bool automatic);
    bool is_automatic_transition() const;

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

//Sources

//NR.1: https://medium.com/dotcrossdot/hierarchical-finite-state-machine-c9e3f4ce0d9e
