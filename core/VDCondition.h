#ifndef VDCONDITION_H
#define VDCONDITION_H

#include "core/resource.h"
#include "core/func_ref.h"
#include "VDState.h"

//////////
/// VDAcCondition
//////////
class VDAcCondition : public Resource {
    GDCLASS(VDAcCondition, Resource);

    String condition_name;
	bool bis_not = false;
protected:
    static void _bind_methods();

    virtual bool _on_check(Ref<VDAcContext> context);
public:
    VDAcCondition();

	bool can_pass(Ref<VDAcContext> context);

	void set_condition_name(String name);
	String get_condition_name() const;
	void set_is_not(bool is_not);
	bool is_not() const;
};
//////////
/// VDAcFunctionCondition
//////////
class VDAcFunctionCondition : public VDAcCondition {
    GDCLASS(VDAcFunctionCondition, VDAcCondition);

    Ref<FuncRef> function;
protected:
    static void _bind_methods();

    virtual bool _on_check(Ref<VDAcContext> context) override;
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

    virtual bool _on_check(Ref<VDAcContext> context) override;
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
