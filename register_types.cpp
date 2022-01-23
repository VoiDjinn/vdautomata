#include "register_types.h"
#include "core/class_db.h"
#include "core/engine.h"

#include "./core/VDAutomata.h"
#include "./core/VDState.h"
#include "./core/VDCondition.h"
#include "./core/VDContextCondition.h"
#include "./scene/VDNode.h"
#include "./editor/VDAutomataEditor.h"

#include "./HSM/VDHsmState.h"

#include "./BT/VDBtState.h"
#include "./BT/Nodes/Composite/VDBtComposite.h"
#include "./BT/Nodes/Composite/VDBtDecorator.h"

#include "./GOAP/VDPlannerState.h"

#include "./SAI/VDSAIState.h"
#include "./SAI/behaviors/SeekFlee.h"
#include "./SAI/behaviors/PursueEvade.h"
#include "./SAI/behaviors/ArriveLeave.h"
#include "./SAI/behaviors/AlignFace.h"
/*
#include "./SAI/behaviors/VDSAITargetBehaviors.h"
#include "./SAI/behaviors/VDSAIDefaultBehaviors.h"
#include "./SAI/behaviors/VDSAIDefaultGroupBehaviors.h"
*/

void register_vdautomata_types() {
  ClassDB::register_class<VDAcAutomata>();
  ClassDB::register_class<VDAcState>();
  ClassDB::register_class<VDAcParentState>();
  ClassDB::register_class<VDAcContext>();
  ClassDB::register_class<VDAcBlackboard>();
  ClassDB::register_class<VDAcNode>();
  ClassDB::register_class<VDAcStateData>();
  ClassDB::register_class<VDAcStateStructure>();

  ClassDB::register_class<VDAcCondition>();
  ClassDB::register_class<VDAcFunctionCondition>();
  ClassDB::register_class<VDAcGroupedCondition>();
  //TODO: make it work
  //ClassDB::register_class<VDAcContextValueCondition>();
  ClassDB::register_class<VDAcContextParamsCondition>();

  ClassDB::register_class<VDAhsmCompositeState>();
  ClassDB::register_class<VDAhsmOrthogonalState>();
  ClassDB::register_class<VDAhsmTransition>();
  //ClassDB::register_class<VDAhsmNode>();

  //ClassDB::register_class<VDAbtState>();
  //ClassDB::register_class<VDAbtComposite>();
  //ClassDB::register_class<VDAbtDecorator>();

  //ClassDB::register_class<VDAgpState>();

  ClassDB::register_class<VDAsaiKinematic>();
  ClassDB::register_class<VDAsaiSteeringData>();
  ClassDB::register_class<VDAsaiState>();

  ClassDB::register_class<VDAsaiBehaviorSeek>();
  ClassDB::register_class<VDAsaiBehaviorFlee>();
  ClassDB::register_class<VDAsaiBehaviorPursue>();
  ClassDB::register_class<VDAsaiBehaviorEvade>();
  ClassDB::register_class<VDAsaiBehaviorArrive>();
  ClassDB::register_class<VDAsaiBehaviorLeave>();
  ClassDB::register_class<VDAsaiBehaviorAlign>();
  ClassDB::register_class<VDAsaiBehaviorFace>();
  /*
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
  */

#ifdef TOOLS_ENABLED
  //EditorPlugins::add_by_type<VDAcAutomataEditorPlugin>();
#endif
}

void unregister_vdautomata_types() {}

