#include "./VDBtSelector.h"

VDAbtNode::BTState VDAbtSelector::_on_tick ( Ref<VDAcContext> context ) {
    for(int i = 0; i < children.size(); i++){
        Ref<VDAbtNode> child = children[i];
        Array args;
        args.push_back(context);
        int state_index = (int)child->call("tick", args);
        VDAbtNode::BTState status = static_cast<VDAbtNode::BTState>(state_index);
        if(status != FAIL){
            return status;
        }
    }
    return FAIL;
//     for child in children:
//         status = child.run()
//         if status == RUNNING or status == SUCCESS:
//             return status
//     return FAILURE
}
