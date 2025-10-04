#pragma once
enum class HumanoidStateType : int {
    FallingDown = 0,
    Running = 1,
    GettingUp = 2,
    Jumping = 3,
    Climbing = 4,
    Freefall = 5,
    Flying = 6,
    Landed = 7,
    Swimming = 8,
    RunningNoPhysics = 9,
    Strafe = 10,
    ClimbingNoPhysics = 11,
    Physics = 12,
    Seated = 13,
    PlatformStanding = 14,
    Dead = 15,
    Standing = 16,
    None = -1
};
