#ifndef VDSTATE_H
#define VDSTATE_H

#include "core/resource.h"
#include "./VDAutomata.h"
#include "../../vdcore/VDCore.h"

class VDAcContext;
class VDAcStateData;
class VDAcBlackboard;
class VDAcEvent;
class VDAcAutomata;
class VDAcStateStructure;
//////////
// VDAcState
//////////
class VDAcState : public Resource {
    GDCLASS(VDAcState, Resource);

public:
    virtual bool tick(Ref<VDAcContext> context);
    virtual bool has_tick() {
        return true;
    };
    bool debug_mode = false;

protected:
    static void _bind_methods();

    String state_name;
    Vector<Ref<VDAcState>> substates;

    virtual bool _on_tick(Ref<VDAcContext> context);
    virtual void _on_init(Ref<VDAcContext> context);
    virtual void _after_init(Ref<VDAcContext> context);
    virtual void _on_enter(Ref<VDAcContext> context);
    virtual void _after_enter(Ref<VDAcContext> context);
    virtual void _on_update(Ref<VDAcContext> context);
    virtual void _after_update(Ref<VDAcContext> context);
    virtual void _before_exit(Ref<VDAcContext> context);
    virtual void _on_exit(Ref<VDAcContext> context);
    virtual void _before_deinit(Ref<VDAcContext> context);
    virtual void _on_deinit(Ref<VDAcContext> context);

    virtual void _on_substate_added(Ref<VDAcState> state);
    virtual void _on_substate_removed(Ref<VDAcState> state);
public:
    VDAcState();

    virtual bool dispatch(Ref<VDAcEvent> event, Ref<VDAcContext> context);
    virtual bool process(Ref<VDAcEvent> event, Ref<VDAcContext> context);

    virtual void init(Ref<VDAcContext> context);
    virtual void enter(Ref<VDAcContext> context);
    virtual void update(Ref<VDAcContext> context);
    virtual void exit(Ref<VDAcContext> context);
    virtual void deinit(Ref<VDAcContext> context);

    void add_substate(Ref<VDAcState> substate);
    void remove_substate(Ref<VDAcState> substate);
    virtual bool has_substates() const;
    virtual void set_substates(Vector<Ref<VDAcState>> substates);
    virtual Vector<Ref<VDAcState>> get_substates() const;
    virtual void set_substates_open(Array substates);
    virtual Array get_substates_open() const;

    void set_state_name(String name);
    String get_state_name() const;
    virtual String to_state_string() const;
    void set_debug_mode(bool mode);
    bool is_debug_mode() const;
};
//////////
// VDAcStateData
//////////
class VDAcStateData: public Reference {
    GDCLASS(VDAcStateData, Reference);
protected:
    static void _bind_methods();

    Ref<VDAcContext> context;
    Ref<VDAcState> owning_state;
    Ref<VDAcState> current_state;
public:
    VDAcStateData();

    void set_data_context ( Ref<VDAcContext> context );
    Ref<VDAcContext> get_data_context() const;
    void set_owning_state ( Ref<VDAcState> state );
    Ref<VDAcState> get_owning_state() const;
    void set_current_state ( Ref<VDAcState> state );
    Ref<VDAcState> get_current_state() const;
};
//////////
// VDAcEvent
//////////
class VDAcEvent : public Resource {
    GDCLASS(VDAcEvent, Resource);
protected:
    static void _bind_methods();

    String event_name;
public:
    VDAcEvent();

    virtual bool process_from(Ref<VDAcContext> context, Ref<VDAcState> state = nullptr);

    void set_event_name(String name);
    String get_event_name() const;
};
//////////
// VDAcContext
//////////
class VDAcContext : public Reference
{
    GDCLASS ( VDAcContext, Reference );

    friend class VDState;
protected:
    static void _bind_methods();

    Variant owner;
    HashMap<Variant, Variant, VariantHasher> params;
    List<Variant> param_keys;
    Ref<VDAcAutomata> owning_automata;
    Ref<VDAcBlackboard> blackboard;
    float delta = 1.0;

    Vector<Ref<VDAcState>> active_states;
    Vector<Ref<VDAcState>> queued_states;
    //TODO: All state machine formalisms, including UML state machines, universally assume that a state machine completes processing of each event before it can start processing the next event.
    Vector<Ref<VDAcEvent>> queued_events;
    HashMap<int, Ref<VDAcStateData>> state_datas;
public:
    VDAcContext();
    ~VDAcContext();

    void set_owner(Variant owner);
    Variant get_owner() const;

    void set_context_params(HashMap<Variant, Variant, VariantHasher> params);
    HashMap<Variant, Variant, VariantHasher> get_context_params() const;
    void set_context_params_open(Dictionary params);
    Dictionary get_context_params_open() const;
    void set_context_value(Variant key, Variant value);
    Variant get_context_value(Variant key) const;
    bool has_context_param(Variant key);

    void set_owning_automata(Ref<VDAcAutomata> automata);
    Ref<VDAcAutomata> get_owning_automata() const;
    void set_blackboard(Ref<VDAcBlackboard> blackboard);
    Ref<VDAcBlackboard> get_blackboard() const;
    void set_delta(float delta);
    float get_delta() const;

    void add_active_state(Ref<VDAcState> state);
    void remove_active_state(Ref<VDAcState> state);
    void set_active_states(Vector<Ref<VDAcState>> states);
    Vector<Ref<VDAcState>> get_active_states() const;
    Array get_active_states_open() const;

    void add_queued_event(Ref<VDAcEvent> event);
    void remove_queued_event(Ref<VDAcEvent> event);
    Vector<Ref<VDAcEvent>> get_queued_events() const;

    void set_state_data(Ref<VDAcState> state, Ref<VDAcStateData> data);
    Ref<VDAcStateData> get_state_data(Ref<VDAcState> state) const;
    HashMap<int, Ref<VDAcStateData>> get_state_datas() const;
    Dictionary get_state_datas_open() const;
};
//////////
// VDAcBlackboard
//////////
class VDAcBlackboard : public Resource {
    GDCLASS(VDAcBlackboard, Resource);

    HashMap<Variant, Variant> params;
    Node* world;

protected:
    static void _bind_methods();

public:
    VDAcBlackboard();
};
#endif

