#include "./VDBtSequence.h"

VDAbtNode::BTState VDAbtSequence::_on_tick ( Ref<VDAcContext> context ) {
    for(int i = 0; i < children.size(); i++){
        Ref<VDAbtNode> child = children[i];
        Array args;
        args.push_back(context);
        int state_index = (int)child->call("tick", args);
        VDAbtNode::BTState status = static_cast<VDAbtNode::BTState>(state_index);
        if(status != SUCCESS){
            return status;
        }
    }
    return SUCCESS;
//     for child in children:
//     status = child.run()
//     if status == RUNNING or status == FAILURE:
//         return status
//     return SUCCESS
}
