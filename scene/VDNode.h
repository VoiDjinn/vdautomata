#ifndef VDANODE_H
#define VDANODE_H

#include "scene/main/node.h"
#include "../core/VDState.h"
#include "../core/VDAutomata.h"

class VDAcNode : public Node {
    GDCLASS(VDAcNode, Node);

public:
    enum VDATickMode {
        PROCESS,
        PHYSICS_PROCESS,
        ALL,
        NONE
    };

protected:
    static void _bind_methods();

    void _notification(int p_what);

	Ref<VDAcAutomata> automata;
    Ref<VDAcContext> context;
    HashMap<Variant, Variant, VariantHasher> initial_params;
    VDATickMode tick_mode = PROCESS;
    NodePath np_agent;

    bool boverwrites_context_params = false;

    virtual void _on_starting();
    virtual void _after_starting();
    virtual void _on_stopping();
    virtual void _after_stopping();

    void init_context_owning_node();
public:
    VDAcNode();

    virtual void start();
    virtual void stop();
    virtual bool is_running() const;

    void set_automata(Ref<VDAcAutomata> automata);
    Ref<VDAcAutomata> get_automata();
    void set_automata_context(Ref<VDAcContext> context);
    Ref<VDAcContext> get_automata_context() const;

    void set_initial_params(HashMap<Variant, Variant, VariantHasher> params);
    HashMap<Variant, Variant, VariantHasher> get_initial_params() const;
    void set_initial_params_open(Dictionary params);
    Dictionary get_initial_params_open() const;

    void set_tick_mode(VDATickMode mode);
    VDATickMode get_tick_mode() const;
    void set_agent_path(const NodePath &path);
    NodePath get_agent_path() const;
    void set_overwrites_context_params(bool overwrite);
    bool is_overwrites_context_params() const;
};

VARIANT_ENUM_CAST(VDAcNode::VDATickMode);
#endif
