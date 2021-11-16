#ifndef VDSAISTATE_H
#define VDSAISTATE_H

#include "core/reference.h"
#include "scene/main/scene_tree.h"
#include "../core/VDState.h"

class VDAsaiTargetAcceleration;
class VDAsaiSteeringAgent;
class VDAsaiProximity;
class VDAsaiSegment;
//////////
// VDAsaiBehaviorState
//////////
class VDAsaiBehaviorState : public VDAcState {
    GDCLASS(VDAsaiBehaviorState, VDAcState);
protected:
    static void _bind_methods();

    String key_acceleration = "SAIAcceleration";
    Ref<VDAsaiSteeringAgent> agent;

    virtual void _calculate_steering(Ref<VDAsaiTargetAcceleration> acceleration, Ref<VDAcContext> context);
public:
    VDAsaiBehaviorState();

    virtual void exit(Ref<VDAcContext> context, Ref<VDAcStateStructure> structure) override;

    void set_key_acceleration(String key);
    String get_key_acceleration() const;
};
//////////
// VDAsaiGroupBehaviorState
//////////
class VDAsaiGroupBehaviorState : public VDAsaiBehaviorState {
    GDCLASS(VDAsaiGroupBehaviorState, VDAsaiBehaviorState);
protected:
    static void _bind_methods();

    Ref<VDAsaiProximity> proximity;

    virtual bool _report_neighbor(Ref<VDAsaiSteeringAgent> neighbor, Ref<VDAcContext> context) {
        return false;
    }
public:
    VDAsaiGroupBehaviorState();
};
//////////
// VDAsaiTargetAcceleration
//////////
class VDAsaiTargetAcceleration : public Reference {
    GDCLASS(VDAsaiTargetAcceleration, Reference);

    Vector3 linear = Vector3(0.0, 0.0, 0.0);
    float angular = 0.0;
protected:
    static void _bind_methods();
public:
    VDAsaiTargetAcceleration();

    void set_linear(Vector3 linear);
    Vector3 get_linear() const;

    void add_scaled_accel(Ref<VDAsaiTargetAcceleration> acceleration, float scalar);
    void set_zero();
    float get_magnitude();
    float get_magnitude_squared();
};
//////////
// VDAsaiAgentLocation
//////////
class VDAsaiAgentLocation : public Reference {
    GDCLASS(VDAsaiAgentLocation, Reference);
protected:
    static void _bind_methods();

    Vector3 position = Vector3(0.0, 0.0, 0.0);
    float orientation = 0.0;
public:
    VDAsaiAgentLocation();

    Vector3 get_position();
};
//////////
// VDAsaiSteeringAgent
//////////
class VDAsaiSteeringAgent : public VDAsaiAgentLocation {
    GDCLASS(VDAsaiSteeringAgent, VDAsaiAgentLocation);
protected:
    static void _bind_methods();

    float zero_linear_speed_threshold = 0.01;

    float linear_speed_max = 0.0;
    float linear_acceleration_max = 0.0;
    Vector3 linear_velocity = Vector3(0.0,0.0,0.0);

    float angular_speed_max = 0.0;
    float angular_acceleration_max = 0.0;
    float angular_velocity = 0.0;

    float bounding_radius = 0.0;
    bool bis_tagged = false;
public:
    VDAsaiSteeringAgent();

    float get_linear_acceleration_max() const;

    float get_bounding_radius() const;
    void set_tagged(bool tagged);
    bool is_tagged() const;
};
//////////
// VDAsaiProximity
//////////
class VDAsaiProximity : public Reference {
    GDCLASS(VDAsaiProximity, Reference);
protected:
    static void _bind_methods();

    Ref<VDAsaiSteeringAgent> agent;
    List<Ref<VDAsaiSteeringAgent>> agents;

    //virtual int _find_neighbors(FuncRef _callback){
    virtual int _find_neighbors() {
        return 0;
    }
public:
    VDAsaiProximity();
};
//////////
// VDAsaiInfiniteProximity
//////////
class VDAsaiInfiniteProximity : public VDAsaiProximity {
    GDCLASS(VDAsaiInfiniteProximity, VDAsaiProximity);
protected:
    static void _bind_methods();

    virtual int _find_neighbors() override;
public:
    VDAsaiInfiniteProximity();
};
//////////
// VDAsaiRadiusProximity
//////////
class VDAsaiRadiusProximity : public VDAsaiProximity {
    GDCLASS(VDAsaiRadiusProximity, VDAsaiProximity);
protected:
    static void _bind_methods();

    float radius = 0.0;
    int last_frame = 0;
    SceneTree* scene_tree;

    virtual int _find_neighbors() override;
public:
    VDAsaiRadiusProximity();
};
//////////
// VDAsaiPath
//////////
class VDAsaiPath : public Reference {
    GDCLASS(VDAsaiPath, Reference);
protected:
    static void _bind_methods();

    bool is_open = false;
    float length = 0.0;

    List<Ref<VDAsaiSegment>> segments;
    Vector3 nearest_point_on_segment;
    Vector3 nearest_point_on_path;
public:
    VDAsaiPath();

    void init(Array waypoints, bool is_open = false);
    void create_path(Array waypoints);
    float calculate_distance(Vector3 agent_current_position);
    Vector3 calculate_target_position(float target_distance);
    float calculate_point_segment_distance_squared(Vector3 start, Vector3 end, Vector3 position);
    Vector3 get_start_point();
    Vector3 get_end_point();
};
//////////
// VDAsaiSegment
//////////
class VDAsaiSegment: public Reference {
    GDCLASS(VDAsaiSegment, Reference);

    Vector3 begin;
    Vector3 end;
    float length;
    float cumulative_length;
protected:
    static void _bind_methods();
public:
    VDAsaiSegment();

    void init(Vector3 begin, Vector3 end);

    float get_length() const;
    void set_cumulative_length(float length);
    float get_cumulative_length() const;

    Vector3 get_begin() const;
    Vector3 get_end() const;
};
#endif
