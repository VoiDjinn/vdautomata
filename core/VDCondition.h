#ifndef VDCONDITION_H
#define VDCONDITION_H

#include "core/resource.h"
#include "core/func_ref.h"
#include "VDState.h"

class VDAcContext;

//////////
/// VDAcCondition
//////////
class VDAcCondition : public Resource {
    GDCLASS(VDAcCondition, Resource);

    String condition_name;
protected:
    static void _bind_methods();

    virtual bool _pass(Ref<VDAcContext> context);
public:
    VDAcCondition();
};
//////////
/// VDAcContextCondition
//////////
class VDAcContextCondition : public VDAcCondition {
    GDCLASS(VDAcContextCondition, VDAcCondition);

    Vector<Variant> parameter_keys;
    HashMap<Variant, Variant, VariantHasher> parameters;
protected:
    static void _bind_methods();

    virtual bool _pass(Ref<VDAcContext> context) override;
public:
    VDAcContextCondition();

    Vector<Variant> get_parameter_keys() const;
    void set_parameters(HashMap<Variant, Variant, VariantHasher> new_parameters);
    HashMap<Variant, Variant, VariantHasher> get_parameters() const;
    void set_parameters_open(Dictionary new_parameters);
    Dictionary get_parameters_open();
};
//////////
/// VDAcFunctionCondition
//////////
class VDAcFunctionCondition : public VDAcCondition {
    GDCLASS(VDAcFunctionCondition, VDAcCondition);

    Ref<FuncRef> function;
protected:
    static void _bind_methods();

    virtual bool _pass(Ref<VDAcContext> context) override;
public:
    VDAcFunctionCondition();
};
//////////
/// VDAcGroupedCondition
//////////
class VDAcGroupedCondition : public VDAcCondition {
    GDCLASS(VDAcGroupedCondition, VDAcCondition);
public:
    VDAcGroupedCondition();
    enum VDSubconditionComparator {
        ALL = 0, // all subconditions have to be true
        ANY = 1, // just one subcondition needs to be true
        NONE = 2, // no subconditions have to be true
    };
protected:
    static void _bind_methods();

    VDSubconditionComparator comparator = VDSubconditionComparator::ALL;
    Vector<Ref<VDAcCondition>> subconditions;

    virtual bool _pass(Ref<VDAcContext> context) override;
public:
    void set_subconditions(Vector<Ref<VDAcCondition>> new_subconditions);
    void set_subconditions_open(Array new_subconditions);
    Vector<Ref<VDAcCondition>> get_subconditions() const;
    Array get_subconditions_open() const;

    void set_comparator(VDSubconditionComparator new_comparator);
    VDSubconditionComparator get_comparator() const;
};

VARIANT_ENUM_CAST(VDAcGroupedCondition::VDSubconditionComparator);

#endif
