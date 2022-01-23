#ifndef VDANODE_H
#define VDANODE_H

#include "scene/main/node.h"
#include "../core/VDState.h"
#include "../core/VDAutomata.h"

/// TODO:
/*
 *
 * - State-addition: get_required_keys + Parameter-GUI as seen in AnimationTree
 *
 */
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
    VDATickMode tick_mode = PROCESS;

    bool bautostart = false;

    virtual void _on_start();
    virtual void _pre_start();
    virtual void _pre_stop();
    virtual void _on_stop();
    virtual Ref<VDAcContext> _create_context();
public:
    VDAcNode();

    virtual void start();
    virtual void stop();
    virtual bool is_running() const;

    void set_automata(Ref<VDAcAutomata> automata);
    Ref<VDAcAutomata> get_automata();
    void set_automata_context(Ref<VDAcContext> context);
    Ref<VDAcContext> get_automata_context() const;
    void set_tick_mode(VDATickMode mode);
    VDATickMode get_tick_mode() const;
    void set_is_autostarting(bool autostart);
    bool is_autostarting() const;
};

VARIANT_ENUM_CAST(VDAcNode::VDATickMode);
#endif
