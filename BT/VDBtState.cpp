#include "./VDBtState.h"
#include "./Nodes/VDBtNode.h"

VDAbtState::VDAbtState(){}

void VDAbtState::_bind_methods() {}

bool VDAbtState::tick(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure, float delta){
    if(!this->is_active){
        return _fail(context);
    }
    if(this->_running(context)){
        return false;
    }
    this->_run(context);
    Array tick_args;
    tick_args.push_back(context);
    tick_args.push_back(structure);
    bool result = this->_succeed(context);
// 	if result is GDScriptFunctionState:
// 		if not running():
// 			push_error(name + " exited execution, but it's not running().")
// 			assert(false)
// 		result = yield(result, "completed")
// 	if running():
// 		push_error(name + " completed but it is still running(). Must either succeed() or fail().")
// 		assert(false)
	return result;
}

bool VDAbtState::_succeed(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, VDAbtNode::BTState::SUCCESS);
// 	emit_signal("tick", true)
	return true;
}

bool VDAbtState::_fail(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, VDAbtNode::BTState::FAIL);
// 	emit_signal("tick", true)
	return false;
}

void VDAbtState::_run(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, VDAbtNode::BTState::RUNNING);
}

bool VDAbtState::_succeeded(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == VDAbtNode::BTState::SUCCESS;
}

bool VDAbtState::_failed(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == VDAbtNode::BTState::FAIL;
}

bool VDAbtState::_running(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == VDAbtNode::BTState::RUNNING;
}
