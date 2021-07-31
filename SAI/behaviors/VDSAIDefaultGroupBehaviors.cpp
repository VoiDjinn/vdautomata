#include "VDSAIDefaultGroupBehaviors.h"

VDAsaiDGBSeparation::VDAsaiDGBSeparation() {}

void VDAsaiDGBSeparation::_bind_methods() {}

void VDAsaiDGBSeparation::_calculate_steering ( Ref<VDAsaiTargetAcceleration> acceleration, Ref<VDAcContext> context ) {
    acceleration->set_zero();
    this->acceleration = acceleration;
    //proximity._find_neighbors(_callback)
}

bool VDAsaiDGBSeparation::_report_neighbor ( Ref<VDAsaiSteeringAgent> neighbor, Ref<VDAcContext> context ) {
    Vector3 to_agent = agent->get_position() - neighbor->get_position();
    float distance_squared = to_agent.length_squared();
    float acceleration_max = agent->get_linear_acceleration_max();

    float strength = decay_coefficient / distance_squared;
    if ( strength > acceleration_max ) {
        strength = acceleration_max;
    }
    acceleration->set_linear ( acceleration->get_linear() + to_agent * ( strength / sqrt ( distance_squared ) ) );
    return true;
}
