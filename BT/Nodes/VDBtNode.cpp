#include "./VDBtNode.h"

VDAbtNode::VDAbtNode() {}

void VDAbtNode::_bind_methods() {
    BIND_ENUM_CONSTANT(RUNNING);
    BIND_ENUM_CONSTANT(SUCCESS);
    BIND_ENUM_CONSTANT(FAIL);
}

VDAbtNode::BTState VDAbtNode::tick(Ref<VDAcContext> context){
    if(!this->is_active){
        return _fail(context);
    }
    if(this->_running(context)){
        return RUNNING;
        //return false;
    }
    this->_run(context);
    Array tick_args;
    tick_args.push_back(context);
    int state_index = (int)call("_on_tick", tick_args);
    VDAbtNode::BTState result = static_cast<VDAbtNode::BTState>(state_index);
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

VDAbtNode::BTState VDAbtNode::_on_tick(Ref<VDAcContext> context){
    return _succeed(context);
}

VDAbtNode::BTState VDAbtNode::_succeed(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, SUCCESS);
// 	emit_signal("tick", true)
	return SUCCESS;
}

VDAbtNode::BTState VDAbtNode::_fail(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, FAIL);
// 	emit_signal("tick", true)
	return FAIL;
}

void VDAbtNode::_run(Ref<VDAcContext> context){
    context->set_context_value(param_key_btstate, RUNNING);
}

bool VDAbtNode::_succeeded(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == SUCCESS;
}

bool VDAbtNode::_failed(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == FAIL;
}

bool VDAbtNode::_running(Ref<VDAcContext> context){
    return (int)context->get_context_value(param_key_btstate) == RUNNING;
}
