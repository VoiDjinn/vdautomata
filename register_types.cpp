#include "register_types.h"
#include "core/class_db.h"
#include "core/engine.h"

#include "./core/VDAutomata.h"
#include "./core/VDState.h"
#include "./scene/VDNode.h"
#include "./editor/VDAutomataEditor.h"

#include "./HSM/VDHsmState.h"

#include "./BT/VDBtState.h"
#include "./BT/Nodes/Composite/VDBtComposite.h"
#include "./BT/Nodes/Composite/VDBtDecorator.h"

#include "./GOAP/VDPlannerState.h"

#include "./SAI/VDSAIState.h"
#include "./SAI/behaviors/VDSAIDefaultBehaviors.h"
#include "./SAI/behaviors/VDSAIDefaultGroupBehaviors.h"

void register_vdautomata_types() {
	ClassDB::register_class<VDAcAutomata>();
    ClassDB::register_class<VDAcState>();
    ClassDB::register_class<VDAcContext>();
    ClassDB::register_class<VDAcBlackboard>();
    ClassDB::register_class<VDAcNode>();
    ClassDB::register_class<VDAcStateData>();
    ClassDB::register_class<VDAcEvent>();

    ClassDB::register_class<VDAhsmState>();
	ClassDB::register_class<VDAhsmTransition>();
    ClassDB::register_class<VDAhsmNode>();

    //ClassDB::register_class<VDAbtState>();
    //ClassDB::register_class<VDAbtComposite>();
    //ClassDB::register_class<VDAbtDecorator>();

    //ClassDB::register_class<VDAgpState>();

    ClassDB::register_class<VDAsaiBehaviorState>();
    ClassDB::register_class<VDAsaiGroupBehaviorState>();
    ClassDB::register_class<VDAsaiAgentLocation>();
    ClassDB::register_class<VDAsaiSteeringAgent>();
    ClassDB::register_class<VDAsaiTargetAcceleration>();
    ClassDB::register_class<VDAsaiPath>();
    ClassDB::register_class<VDAsaiSegment>();
    ClassDB::register_class<VDAsaiProximity>();
    ClassDB::register_class<VDAsaiInfiniteProximity>();
    ClassDB::register_class<VDAsaiRadiusProximity>();

    ClassDB::register_class<VDAsaiDBArrive>();
    ClassDB::register_class<VDAsaiDGBSeparation>();

#ifdef TOOLS_ENABLED
    EditorPlugins::add_by_type<VDAcAutomataEditorPlugin>();
#endif
}

void unregister_vdautomata_types() {}

