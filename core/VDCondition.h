#ifndef VDCONDITION_H
#define VDCONDITION_H

#include "core/resource.h"

class VDAcCondition : public Resource {
    GDCLASS(VDAcCondition, Resource);

    String condition_name;
protected:
    static void _bind_methods();
public:
    VDAcCondition();
};

#endif
