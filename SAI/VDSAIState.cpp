#include "VDSAIState.h"
//////////
// VDAsaiBehaviorState
//////////
VDAsaiBehaviorState::VDAsaiBehaviorState() {}

void VDAsaiBehaviorState::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "set_key_acceleration", "key" ), &VDAsaiBehaviorState::set_key_acceleration );
    ClassDB::bind_method ( D_METHOD ( "get_key_acceleration" ), &VDAsaiBehaviorState::get_key_acceleration );

    BIND_VMETHOD ( MethodInfo ( "_calculate_steering", PropertyInfo ( Variant::OBJECT, "acceleration", PROPERTY_HINT_NONE, "VDAsaiTargetAcceleration" ), PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );

    ADD_PROPERTY ( PropertyInfo ( Variant::STRING, "key_acceleration" ), "set_key_acceleration", "get_key_acceleration" );
}

void VDAsaiBehaviorState::_calculate_steering ( Ref<VDAsaiTargetAcceleration> acceleration, Ref<VDAcContext> context )
{
    acceleration->set_zero();
}

void VDAsaiBehaviorState::exit ( Ref<VDAcContext> context, Ref<VDAcStateStructure> structure )
{
    if ( context->get_context_params().has ( this->key_acceleration ) ) {
        Ref<VDAsaiTargetAcceleration> acceleration = context->get_context_value ( this->key_acceleration );
        acceleration->set_zero();
    }
}

void VDAsaiBehaviorState::set_key_acceleration ( String key )
{
    this->key_acceleration = key;
}

String VDAsaiBehaviorState::get_key_acceleration() const
{
    return this->key_acceleration;
}
//////////
// VDAsaiGroupBehaviorState
//////////
VDAsaiGroupBehaviorState::VDAsaiGroupBehaviorState() {}

void VDAsaiGroupBehaviorState::_bind_methods()
{
    BIND_VMETHOD ( MethodInfo ( "_report_neighbor", PropertyInfo ( Variant::OBJECT, "neighbor", PROPERTY_HINT_NONE, "VDAsaiSteeringAgent" ), PropertyInfo ( Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "VDAcContext" ) ) );
}
//////////
// VDAsaiTargetAcceleration
//////////
VDAsaiTargetAcceleration::VDAsaiTargetAcceleration() {}

void VDAsaiTargetAcceleration::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "add_scaled_accel", "acceleration", "scalar" ), &VDAsaiTargetAcceleration::add_scaled_accel );
    ClassDB::bind_method ( D_METHOD ( "set_zero" ), &VDAsaiTargetAcceleration::set_zero );
    ClassDB::bind_method ( D_METHOD ( "get_magnitude" ), &VDAsaiTargetAcceleration::get_magnitude );
    ClassDB::bind_method ( D_METHOD ( "get_magnitude_squared" ), &VDAsaiTargetAcceleration::get_magnitude_squared );
}

void VDAsaiTargetAcceleration::set_linear ( Vector3 linear )
{
    this->linear = linear;
}

Vector3 VDAsaiTargetAcceleration::get_linear() const
{
    return this->linear;
}

void VDAsaiTargetAcceleration::add_scaled_accel ( Ref<VDAsaiTargetAcceleration> acceleration, float scalar )
{
    linear += acceleration->linear * scalar;
    angular += acceleration->angular * scalar;
}

void VDAsaiTargetAcceleration::set_zero()
{
    linear.x = 0.0;
    linear.y = 0.0;
    linear.z = 0.0;
    angular = 0.0;
}

float VDAsaiTargetAcceleration::get_magnitude()
{
    return Math::sqrt ( get_magnitude_squared() );
}

float VDAsaiTargetAcceleration::get_magnitude_squared()
{
    return linear.length_squared() + angular * angular;
}
//////////
// VDAsaiAgentLocation
//////////
VDAsaiAgentLocation::VDAsaiAgentLocation() {}

void VDAsaiAgentLocation::_bind_methods()
{
    ClassDB::bind_method ( D_METHOD ( "get_position" ), &VDAsaiAgentLocation::get_position );
}

Vector3 VDAsaiAgentLocation::get_position()
{
    return this->position;
}
//////////
// VDAsaiSteeringAgent
//////////
VDAsaiSteeringAgent::VDAsaiSteeringAgent() {}

void VDAsaiSteeringAgent::_bind_methods() {}

float VDAsaiSteeringAgent::get_linear_acceleration_max() const
{
    return this->linear_acceleration_max;
}

float VDAsaiSteeringAgent::get_bounding_radius() const
{
    return this->bounding_radius;
}

void VDAsaiSteeringAgent::set_tagged ( bool tagged )
{
    this->bis_tagged = tagged;
}

bool VDAsaiSteeringAgent::is_tagged() const
{
    return this->bis_tagged;
}
/////
// VDAsaiProximity
/////
VDAsaiProximity::VDAsaiProximity() {}

void VDAsaiProximity::_bind_methods() {
    BIND_VMETHOD ( MethodInfo ( "_find_neighbors" ) );
}
/////
// VDAsaiInfiniteProximity
/////
VDAsaiInfiniteProximity::VDAsaiInfiniteProximity() {}

void VDAsaiInfiniteProximity::_bind_methods() {}

int VDAsaiInfiniteProximity::_find_neighbors() {
    int neighbor_count = 0;
    int agent_count = agents.size();
    for ( int i = 0; i < agent_count; i++ ) {
        Ref<VDAsaiSteeringAgent> current_agent = agents[i];
        if ( current_agent != agent ) {
            //TODO: some check for agent
            if ( true/*callback.call_func(current_agent)*/ ) {
                neighbor_count += 1;
            }
        }
    }
    return neighbor_count;
}
/////
// VDAsaiRadiusProximity
/////
VDAsaiRadiusProximity::VDAsaiRadiusProximity() {
    this->scene_tree = SceneTree::get_singleton();
}

void VDAsaiRadiusProximity::_bind_methods() {}

int VDAsaiRadiusProximity::_find_neighbors() {
    int agent_count = agents.size();
    int neighbor_count = 0;
    int current_frame = scene_tree != nullptr ? scene_tree->get_frame() : last_frame;
    if ( current_frame != last_frame ) {
        last_frame = current_frame;
        Vector3 owner_position = agent->get_position();
        for ( int i = 0; i < agent_count; i++ ) {
            Ref<VDAsaiSteeringAgent> current_agent = agents[i];
            if ( current_agent != agent ) {
                float distance_squared = owner_position.distance_squared_to ( current_agent->get_position() );
                float range_to = radius + current_agent->get_bounding_radius();
                if ( distance_squared < range_to * range_to ) {
                    //TODO: some check for agent
                    if ( true /*callback.call_func(current_agent)*/ ) {
                        current_agent->set_tagged ( true );
                        neighbor_count += 1;
                        continue;
                    }
                }
            }
            current_agent->set_tagged ( false );
        }
    } else {
        for ( int i = 0; i < agent_count; i++ ) {
            Ref<VDAsaiSteeringAgent> current_agent = agents[i];
            if ( current_agent != agent && current_agent->is_tagged() ) {
                //TODO: some check for agent
                if ( true /*callback.call_func(current_agent*/ ) {
                    neighbor_count += 1;
                }
            }
        }
    }
    return neighbor_count;
}
/////
// VDAsaiPath
/////
VDAsaiPath::VDAsaiPath() {}

void VDAsaiPath::_bind_methods() {}

void VDAsaiPath::init ( Array waypoints, bool is_open ) {
    this->is_open = is_open;
    create_path ( waypoints );
    nearest_point_on_segment = waypoints[0];
    nearest_point_on_path = waypoints[0];
}

void VDAsaiPath::create_path ( Array waypoints ) {
    if ( waypoints.size() < 2 ) {
        ERR_FAIL_MSG ( "Waypoints cannot be null and must contain at least two (2) waypoints." );
        return;
    }
    segments.clear();
    length = 0.0;
    Vector3 current = waypoints.front();
    Vector3 previous;
    for ( int i = 0; i < waypoints.size(); i++ ) {
        previous = current;
        if ( i < waypoints.size() ) {
            current = waypoints[i];
        } else if ( is_open ) {
            break;
        } else {
            current = waypoints[0];
        }
        Ref<VDAsaiSegment> segment = Ref<VDAsaiSegment> ( memnew ( VDAsaiSegment() ) );
        segment->init ( previous, current );
        length += segment->get_length();
        segment->set_cumulative_length ( length );
        segments.push_back ( segment );
    }
}

float VDAsaiPath::calculate_distance ( Vector3 agent_current_position ) {
    if ( segments.size() == 0 ) {
        return 0.0;
    }
    float smallest_distance_squared = Math_INF;
    Ref<VDAsaiSegment> nearest_segment;
    for ( int i = 0; i < segments.size(); i++ ) {
        Ref<VDAsaiSegment> segment = segments[i];
        float distance_squared = calculate_point_segment_distance_squared (
                                     segment->get_begin(), segment->get_end(), agent_current_position
                                 );
        if ( distance_squared < smallest_distance_squared ) {
            nearest_point_on_path = nearest_point_on_segment;
            smallest_distance_squared = distance_squared;
            nearest_segment = segment;
        }
    }
    float length_on_path = ( nearest_segment->get_cumulative_length()
                             - nearest_point_on_path.distance_to ( nearest_segment->get_end() ) );
    return length_on_path;
}

Vector3 VDAsaiPath::calculate_target_position ( float target_distance ) {
    if ( is_open ) {
        target_distance = CLAMP ( target_distance, 0, length );
    } else {
        if ( target_distance < 0 ) {
            target_distance = length + fmod ( target_distance, length );
        } else if ( target_distance > length ) {
            target_distance = fmod ( target_distance, length );
        }
    }
    Ref<VDAsaiSegment> desired_segment;
    for ( int i = 0; i < segments.size(); i++ ) {
        Ref<VDAsaiSegment> segment = segments[i];
        if ( segment->get_cumulative_length() >= target_distance ) {
            desired_segment = segment;
            break;
        }
    }
    if ( desired_segment.is_null() ) {
        desired_segment = segments.back();
    }
    float distance = desired_segment->get_cumulative_length() - target_distance;
    return ( ( ( desired_segment->get_begin() - desired_segment->get_end()
               ) * ( distance / desired_segment->get_length() ) )
             + desired_segment->get_end() );
}

Vector3 VDAsaiPath::get_start_point() {
    Ref<VDAsaiSegment> start_segment = segments.front()->get();
    return start_segment->get_begin();
}

Vector3 VDAsaiPath::get_end_point() {
    Ref<VDAsaiSegment> end_segment = segments.back()->get();
    return end_segment->get_end();
}

float VDAsaiPath::calculate_point_segment_distance_squared ( Vector3 start, Vector3 end, Vector3 position ) {
    nearest_point_on_segment = start;
    Vector3 start_end = end - start;
    float start_end_length_squared = start_end.length_squared();
    if ( start_end_length_squared != 0 ) {
        float t = ( position - start ).dot ( start_end ) / start_end_length_squared;
        nearest_point_on_segment += start_end * CLAMP ( t, 0, 1 );
    }
    return nearest_point_on_segment.distance_squared_to ( position );
}
/////
// VDAsaiSegment
/////
VDAsaiSegment::VDAsaiSegment() {}

void VDAsaiSegment::_bind_methods() {}

void VDAsaiSegment::init ( Vector3 begin, Vector3 end ) {
    this->begin = begin;
    this->end = end;
    this->length = begin.distance_to ( end );
}

float VDAsaiSegment::get_length() const {
    return this->length;
}

void VDAsaiSegment::set_cumulative_length ( float length ) {
    this->cumulative_length = length;
}

float VDAsaiSegment::get_cumulative_length() const {
    return this->cumulative_length;
}

Vector3 VDAsaiSegment::get_begin() const {
    return this->begin;
}

Vector3 VDAsaiSegment::get_end() const {
    return this->end;
}
