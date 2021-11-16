#include "./VDAutomataEditor.h"
#include "core/project_settings.h"
#include "editor/editor_scale.h"

////////////////////
// VDAcAutomataEditor
////////////////////
VDAcAutomataEditor *VDAcAutomataEditor::singleton = NULL;

VDAcAutomataEditor::VDAcAutomataEditor() {
    singleton = this;
    editor_base = memnew(MarginContainer);
    editor_base->set_v_size_flags(SIZE_EXPAND_FILL);
    add_child(editor_base);

    MenuButton *add_menu = memnew(MenuButton);
    add_menu->set_position(Point2(0, 0));
    add_menu->set_size(Point2(25, 15));
    add_child(add_menu);

    add_plugin(memnew(VDAcHSMEditorPlugin));

    PopupMenu *popup;
    popup = add_menu->get_popup();
    //popup->add_item(TTR("Hierarchical State"), AnimationTreePlayer::NODE_ANIMATION);
    //popup->add_item(TTR("Behavior Tree State"), AnimationTreePlayer::NODE_ANIMATION);
    //popup->add_item(TTR("GOAP State"), AnimationTreePlayer::NODE_ANIMATION);
    popup->add_separator();
    //popup->add_item(TTR("Import State..."), MENU_IMPORT_ANIMATIONS);
    //popup->add_item(TTR("Clear"), MENU_GRAPH_CLEAR);
}

void VDAcAutomataEditor::_bind_methods() {}

void VDAcAutomataEditor::_notification ( int p_what ) {}

void VDAcAutomataEditor::edit ( Ref<VDAcState> state ) {
    state_resource = state;
    if (state_resource.is_null()) {
        hide();
    } else {
        //order.clear();
        //p_anim_tree->get_node_list(&order);
        /*
        for(List<StringName>::Element* E=order.front();E;E=E->next()) {

            if (E->get() >= (int)last_id)
                last_id=E->get()+1;
        }*/
        //play_button->set_pressed(p_anim_tree->is_active());
        //read the orders
    }
}

void VDAcAutomataEditor::add_plugin(VDAcAutomataNodeEditorPlugin *p_editor) {
    ERR_FAIL_COND(p_editor->get_parent());
    editor_base->add_child(p_editor);
    editors.push_back(p_editor);
    p_editor->set_h_size_flags(SIZE_EXPAND_FILL);
    p_editor->set_v_size_flags(SIZE_EXPAND_FILL);
    p_editor->hide();
}

void VDAcAutomataEditor::remove_plugin(VDAcAutomataNodeEditorPlugin *p_editor) {
    ERR_FAIL_COND(p_editor->get_parent() != editor_base);
    editor_base->remove_child(p_editor);
    editors.erase(p_editor);
}

////////////////////
// VDAcAutomataEditorPlugin
////////////////////
void VDAcAutomataEditorPlugin::edit(Object *p_object) {
    automata_editor->edit(Object::cast_to<VDAcState>(p_object));
}

bool VDAcAutomataEditorPlugin::handles(Object *p_object) const {
    return p_object->is_class("VDAcState");
}

void VDAcAutomataEditorPlugin::make_visible(bool p_visible) {
    if (p_visible) {
        //editor->hide_animation_player_editors();
        //editor->animation_panel_make_visible(true);
        button->show();
        editor->make_bottom_panel_item_visible(automata_editor);
        automata_editor->set_process(true);
    } else {
        if (automata_editor->is_visible_in_tree())
            editor->hide_bottom_panel();
        button->hide();
        automata_editor->set_process(false);
    }
}

VDAcAutomataEditorPlugin::VDAcAutomataEditorPlugin(EditorNode *p_node) {
    editor = p_node;
    automata_editor = memnew(VDAcAutomataEditor);
    automata_editor->set_custom_minimum_size(Size2(0, 300) * EDSCALE);

    button = editor->add_bottom_panel_item(TTR("VDAutomata"), automata_editor);
    button->hide();
}

VDAcAutomataEditorPlugin::~VDAcAutomataEditorPlugin() {}

//////////
///// Default editors - START
//////////
/////
// HSM Editor - START
/////
VDAcHSMEditorPlugin *VDAcHSMEditorPlugin::singleton = NULL;

VDAcHSMEditorPlugin::VDAcHSMEditorPlugin()
{
    singleton = this;
    //updating = false;
    use_popup_menu_position = false;

    graph = memnew(GraphEdit);
    add_child(graph);
    graph->add_valid_right_disconnect_type(0);
    graph->add_valid_left_disconnect_type(0);
    graph->set_v_size_flags(SIZE_EXPAND_FILL);
// 	graph->connect("connection_request", this, "_connection_request", varray(), CONNECT_DEFERRED);
// 	graph->connect("disconnection_request", this, "_disconnection_request", varray(), CONNECT_DEFERRED);
// 	graph->connect("node_selected", this, "_node_selected");
// 	graph->connect("scroll_offset_changed", this, "_scroll_changed");
// 	graph->connect("delete_nodes_request", this, "_delete_nodes_request");
// 	graph->connect("popup_request", this, "_popup_request");
    float graph_minimap_opacity = EditorSettings::get_singleton()->get("editors/visual_editors/minimap_opacity");
    graph->set_minimap_opacity(graph_minimap_opacity);
}

bool VDAcHSMEditorPlugin::can_edit(const Ref<VDAcState>& p_state)
{
    Ref<VDAhsmCompositeState> hsm = p_state;
    return hsm.is_valid();
}

void VDAcHSMEditorPlugin::edit(const Ref<VDAcState>& p_state)
{

}

/////
// HSM Editor - END
/////
//////////
///// Default editors - END
//////////
