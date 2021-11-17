#ifndef VDCONTEXTCONDITION_H
#define VDCONTEXTCONDITION_H

#include "VDCondition.h"

class VDAcContext;

// TODO: not working yet
	// - setter is not called
	// - key and value inspector fields should be like in dictionary
//////////
/// VDAcContextValueCondition
//////////
class VDAcContextValueCondition : public VDAcCondition {
    GDCLASS(VDAcContextValueCondition, VDAcCondition);
public:
    VDAcContextValueCondition();

    enum VDValueComparator {
        EQUALS = 0,
		LESS = 2,
		LESS_THAN = 3,
        GREATER = 4,
        GREATER_THAN = 5,
		IN = 24
    };
protected:
    static void _bind_methods();

	Variant context_key;
	Variant context_value;
	VDValueComparator value_comparator = EQUALS;
	Variant::Operator internal_comparator;

    virtual bool _on_check(Ref<VDAcContext> context) override;
public:
	void set_context_key(Variant key);
	Variant get_context_key() const;
	void set_context_value(Variant key);
	Variant get_context_value() const;
	void set_value_comparator(VDValueComparator comparator);
	VDValueComparator get_value_comparator() const;
};

VARIANT_ENUM_CAST(VDAcContextValueCondition::VDValueComparator);
//////////
/// VDAcContextParamsCondition
//////////
class VDAcContextParamsCondition : public VDAcCondition {
    GDCLASS(VDAcContextParamsCondition, VDAcCondition);

    Vector<Variant> parameter_keys;
    HashMap<Variant, Variant, VariantHasher> parameters;
protected:
    static void _bind_methods();

    virtual bool _on_check(Ref<VDAcContext> context) override;
public:
    VDAcContextParamsCondition();

    Vector<Variant> get_parameter_keys() const;
    void set_parameters(HashMap<Variant, Variant, VariantHasher> new_parameters);
    HashMap<Variant, Variant, VariantHasher> get_parameters() const;
    void set_parameters_open(Dictionary new_parameters);
    Dictionary get_parameters_open();
};
#endif
