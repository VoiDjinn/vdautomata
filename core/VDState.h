#ifndef VDSTATE_H
#define VDSTATE_H

#include "core/resource.h"
#include "scene/main/scene_tree.h"
#include "./VDAutomata.h"
#include "../../vdcore/VDCore.h"

class VDAcContext;
class VDAcStateData;
class VDAcBlackboard;
class VDAcAutomata;
class VDAcStateStructure;
//////////
// VDAcState
//////////
class VDAcState : public Resource {
    GDCLASS(VDAcState, Resource);
protected:
    static void _bind_methods();

    StringName state_ident;
    String state_name;
    bool bhas_tick = true;
    bool bis_listening_updates = true;
    bool bis_debug_mode = false;

    virtual bool _on_tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _pre_init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _on_init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _pre_enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _on_enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _on_update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param = 0, Variant new_value = 0, Variant old_value = 0);
    virtual void _pre_exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _on_exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _pre_deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _on_deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);

    virtual void _handle_context_param_updated(Variant param, Variant new_value, Variant old_value, Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void _handle_context_params_updated(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);

    SceneTree* get_tree() const;
public:
    VDAcState();

    virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param = 0, Variant new_value = 0, Variant old_value = 0);
    virtual void exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);
    virtual void deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure);

    void set_has_tick(bool has_tick);
    virtual bool has_tick() const;
    void set_listening_updates(bool listening);
    virtual bool is_listening_updates() const;

    void set_state_ident(StringName ident);
    StringName get_state_ident() const;
    void set_state_name(String name);
    String get_state_name() const;
    virtual String to_state_string() const;
    void set_debug_mode(bool mode);
    bool is_debug_mode() const;
};
//////////
// VDAcParentState
//////////
class VDAcParentState : public VDAcState {
    GDCLASS(VDAcParentState, VDAcState);
protected:
    static void _bind_methods();

    Vector<Ref<VDAcState>> substates;

    virtual void _on_substate_added(Ref<VDAcState> substate);
    virtual void _on_substate_removed(Ref<VDAcState> substate);
public:
    VDAcParentState();

    virtual bool tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void init(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void enter(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void update(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, Variant param = 0, Variant new_value = 0, Variant old_value = 0) override;
    virtual void exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;
    virtual void deinit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;

    virtual void add_substate(Ref<VDAcState> substate);
    virtual void remove_substate(Ref<VDAcState> substate);
    virtual void set_substates(Vector<Ref<VDAcState>> new_substates);
    virtual Vector<Ref<VDAcState>> get_substates() const;
    virtual void set_substates_open(Array new_substates);
    virtual Array get_substates_open() const;
    bool is_substate_unique(Ref<VDAcState> substate);
    bool has_substates() const;
};
//////////
// VDAcStateData
//////////
class VDAcStateData: public Reference {
    GDCLASS(VDAcStateData, Reference);
protected:
    static void _bind_methods();

    Ref<VDAcContext> context;
    Ref<VDAcStateStructure> owning_structure;
    Ref<VDAcStateStructure> current_structure;
public:
    VDAcStateData();

    void set_data_context ( Ref<VDAcContext> new_context );
    Ref<VDAcContext> get_data_context() const;
    void set_owning_structure ( Ref<VDAcStateStructure> structure );
    Ref<VDAcStateStructure> get_owning_structure() const;
    void set_current_structure ( Ref<VDAcStateStructure> structure );
    Ref<VDAcStateStructure> get_current_structure() const;
};
//////////
// VDAcContext
//////////
class VDAcContext : public Resource
{
    GDCLASS ( VDAcContext, Resource );

    friend class VDState;
protected:
    static void _bind_methods();

    HashMap<Variant, Variant, VariantHasher> params;
    List<Variant> param_keys;
    Ref<VDAcAutomata> owning_automata;
    Ref<VDAcBlackboard> blackboard;
    float delta = 1.0;

    Vector<Ref<VDAcStateStructure>> active_structures;
    Vector<Ref<VDAcStateStructure>> queued_structures;
    //TODO: All state machine formalisms, including UML state machines, universally assume that a state machine completes processing of each event before it can start processing the next event.
    HashMap<StringName, Ref<VDAcStateData>> state_datas;

    virtual void _setup();
    virtual void _undo();
    SceneTree* get_tree() const;
public:
    VDAcContext();
    ~VDAcContext();

    void set_context_params(HashMap<Variant, Variant, VariantHasher> new_params);
    HashMap<Variant, Variant, VariantHasher> get_context_params() const;
    void set_context_params_open(Dictionary new_params);
    Dictionary get_context_params_open() const;
    void set_context_value(const Variant &key, const Variant &value, bool notify = true);
    Variant get_context_value(const Variant &key) const;
    bool has_context_param(const Variant &key);

    void set_owning_automata(Ref<VDAcAutomata> automata);
    Ref<VDAcAutomata> get_owning_automata() const;
    void set_blackboard(Ref<VDAcBlackboard> blackboard);
    Ref<VDAcBlackboard> get_blackboard() const;
    void set_delta(float delta);
    float get_delta() const;

    void add_active_structure(Ref<VDAcStateStructure> structure);
    void remove_active_structure(Ref<VDAcStateStructure> structure);
    void set_active_structures(Vector<Ref<VDAcStateStructure>> structures);
    Vector<Ref<VDAcStateStructure>> get_active_structures() const;
    Array get_active_structures_open() const;

    void set_state_data(StringName state_path, Ref<VDAcStateData> data);
    Ref<VDAcStateData> get_state_data(StringName state_path) const;
    void remove_state_data(StringName state_path);
    HashMap<StringName, Ref<VDAcStateData>> get_state_datas() const;
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

