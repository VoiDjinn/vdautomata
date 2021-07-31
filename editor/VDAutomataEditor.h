#ifndef VDAUTOMATAEDITOR_H
#define VDAUTOMATAEDITOR_H

#include "editor/editor_plugin.h"
#include "editor/editor_node.h"

#include "../core/VDState.h"
#include "../HSM/VDHsmState.h"

#include "scene/gui/graph_edit.h"

class VDAcAutomataNodeEditorPlugin : public VBoxContainer {
    GDCLASS(VDAcAutomataNodeEditorPlugin, VBoxContainer);

public:
    virtual bool can_edit(const Ref<VDAcState> &p_state) = 0;
    virtual void edit(const Ref<VDAcState> &p_state) = 0;
};

class VDAcAutomataEditor : public VBoxContainer {
    GDCLASS(VDAcAutomataEditor, VBoxContainer);

    MarginContainer *editor_base;
    Vector<VDAcAutomataNodeEditorPlugin *> editors;

protected:
    void _notification(int p_what);
    static void _bind_methods();

    static VDAcAutomataEditor *singleton;

    Ref<VDAcState> state_resource;
public:

    void edit(Ref<VDAcState> state);
    void add_plugin(VDAcAutomataNodeEditorPlugin *p_editor);
    void remove_plugin(VDAcAutomataNodeEditorPlugin *p_editor);
    static VDAcAutomataEditor *get_singleton() {
        return singleton;
    }

    VDAcAutomataEditor();
};

class VDAcAutomataEditorPlugin : public EditorPlugin {
    GDCLASS(VDAcAutomataEditorPlugin, EditorPlugin);

    VDAcAutomataEditor *automata_editor;
    EditorNode *editor;
    Button *button;

public:
    virtual String get_name() const {
        return "VDAutomata";
    }
    bool has_main_screen() const {
        return false;
    }
    virtual void edit(Object *p_object);
    virtual bool handles(Object *p_object) const;
    virtual void make_visible(bool p_visible);

    VDAcAutomataEditorPlugin(EditorNode *p_node);
    ~VDAcAutomataEditorPlugin();
};

//////////
///// Default editors - START
//////////
/////
// HSM Editor - START
/////
class VDAcHSMEditorPlugin : public VDAcAutomataNodeEditorPlugin {
    GDCLASS(VDAcHSMEditorPlugin, VDAcAutomataNodeEditorPlugin);

    static VDAcHSMEditorPlugin *singleton;

    Ref<VDAhsmState> hsm_state;
    GraphEdit *graph;
    Vector2 popup_menu_position;
    bool use_popup_menu_position;
public:
    VDAcHSMEditorPlugin();

    static VDAcHSMEditorPlugin *get_singleton() {
        return singleton;
    }

    virtual bool can_edit(const Ref<VDAcState> &p_state) override;
    virtual void edit(const Ref<VDAcState> &p_state) override;
};
/////
// HSM Editor - END
/////
//////////
///// Default editors - END
//////////

#endif
