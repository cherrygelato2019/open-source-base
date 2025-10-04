#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "aimbot.h"
#include "../../../src/functions/structs/struct.h"
#include "../../../src/functions/player_threading/cache/cache.hpp"
#include "../../vars/vars.h"
using namespace Structs;
using namespace Structs::visualengine;
float calulcateDist(Math::Vector2 a, Math::Vector2 b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Instance::InstanceManager findPartByName(Instance::InstanceManager& character, const std::string& part_name) {
}

std::vector<Instance::InstanceManager> getPlayerBones(functions::Player& entity) {
    std::vector<Instance::InstanceManager> parts = {
        entity.head,
        entity.hrp,
        entity.humanoid,
        entity.leftarm,
        entity.rightarm,
        entity.lefthand,
        entity.righthand,
        entity.leftlowerarm,
        entity.rightlowerarm,
        entity.leftupperarm,
        entity.rightupperarm,
        entity.leftfoot,
        entity.rightfoot,
        entity.leftupperleg,
        entity.rightupperleg,
        entity.leftlowerleg,
        entity.rightlowerleg,
        entity.uppertorso,
        entity.lowertorso,
        entity.torso,
        entity.leftleg,
        entity.rightleg
    };

    return parts;
}

Instance::InstanceManager getClosestPart(functions::Player& entity, const POINT& cursor_point) {
    std::vector<Instance::InstanceManager> parts = {
        entity.head,
        entity.hrp,
        entity.uppertorso,
        entity.lowertorso,
        entity.leftupperleg,
        entity.leftfoot,
        entity.rightfoot,
        entity.leftupperarm,
        entity.lefthand,
        entity.rightupperarm,
        entity.righthand
    };

    Math::Vector2 dimensions = vars::General::visualengine.GetDimensions();
    Math::Matrix4 view_matrix = vars::General::visualengine.GetViewMatrix();
    Math::Vector2 cursor = { static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };

    float min_distance = FLT_MAX;
    Instance::InstanceManager closest_part{};

    for (auto& part : parts) {
        if (part.address == 0)
            continue;

        Math::Vector3 part_position = part.GetPartPos();

        Math::Vector2 screen_position = ve.WorldToScreen(part_position, dimensions, view_matrix);

        if (screen_position.x == 0 && screen_position.y == 0)
            continue;

        float distance = (screen_position - cursor).getMagnitude();
        if (distance < min_distance) {
            min_distance = distance;
            closest_part = part;
        }
    }

    return closest_part;
}

std::tuple<Instance::InstanceManager, Instance::InstanceManager> getClosestPlrToCursor() {
    POINT cursor_point;
    GetCursorPos(&cursor_point);
    ScreenToClient(FindWindowA(nullptr, "Roblox"), &cursor_point);

    Math::Vector2 cursor = { (float)cursor_point.x, (float)cursor_point.y };
    Math::Vector2 dimensions = vars::General::visualengine.GetDimensions();
    Math::Matrix4 view_matrix = vars::General::visualengine.GetViewMatrix();

    Instance::InstanceManager closest_player{};
    Instance::InstanceManager closest_part{};
    float min_dist = FLT_MAX;
    auto& varPlayer = vars::General::players;
    for (auto& cached : varPlayer) {
        auto player = cached.player;

        if (!player.isValid() || player.address == vars::General::localplayer) {
            continue;
        }

        if (cached.health <= 0)
            continue;

        Instance::InstanceManager body_part;
        
        switch (vars::Aimbot::aimbot_part) {
        case 0: body_part = player.head; break;
        case 1: body_part = player.hrp; break;
        case 2: body_part = getClosestPart(player, cursor_point); break;
        default: body_part = player.hrp; break;
        }

        if (!body_part.isValid())
            continue;

        Math::Vector3 part_pos = body_part.GetPartPos();
        Math::Vector2 screen_pos = ve.WorldToScreen(part_pos, dimensions, view_matrix);

        if (screen_pos.x == 0 && screen_pos.y == 0)
            continue;

        float dist = calulcateDist(screen_pos, cursor);
        if ((!vars::Aimbot::disable_out_fov || dist < vars::Aimbot::fov_size) && dist < min_dist) {
            min_dist = dist;
            closest_player = player.self;
            closest_part = body_part;
        }
    }

    return { closest_part, closest_player };
}

struct RMatrix3 {
    float data[9];
    RMatrix3() { for (int i = 0; i < 9; ++i) data[i] = 0.0f; }
};

static Math::Vector3 crossProductV(const Math::Vector3& a, const Math::Vector3& b) {
    return Math::Vector3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
static Math::Vector3 normalizeV(const Math::Vector3& v) {
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f) return v;
    return Math::Vector3{ v.x / len, v.y / len, v.z / len };
}

static RMatrix3 LookAtToRMatrix(const Math::Vector3& cameraPosition, const Math::Vector3& targetPosition) {
    Math::Vector3 forward = normalizeV(Math::Vector3{ targetPosition.x - cameraPosition.x,
                                                     targetPosition.y - cameraPosition.y,
                                                     targetPosition.z - cameraPosition.z });
    Math::Vector3 right = normalizeV(crossProductV(Math::Vector3{ 0.0f, 1.0f, 0.0f }, forward));
    Math::Vector3 up = crossProductV(forward, right);

    RMatrix3 m;
    // formula [ r.x, u.x, -f.x, r.y, u.y, -f.y, r.z, u.z, -f.z ]
    m.data[0] = (right.x * -1.0f); m.data[1] = up.x;    m.data[2] = -forward.x;
    m.data[3] = right.y;           m.data[4] = up.y;    m.data[5] = -forward.y;
    m.data[6] = (right.z * -1.0f); m.data[7] = up.z;    m.data[8] = -forward.z;
    return m;
}

static RMatrix3 lerp_rmatrix3(const RMatrix3& a, const RMatrix3& b, float t) {
    RMatrix3 out;
    for (int i = 0; i < 9; ++i) out.data[i] = a.data[i] + (b.data[i] - a.data[i]) * t;
    return out;
}

struct savedTarget {
    Instance::InstanceManager player{};
    Instance::InstanceManager bodypart{};
    // abyss was here
};

static savedTarget saved_target{};
void function::load_aimbot() {
    static std::atomic_bool running{ false };
    if (running.load()) return;
    running.store(true);

    std::thread([]() {
        // delay wit tick
        const int tick_ms = 4;

        while (running.load()) {
            if (!vars::Aimbot::enabled || !vars::Aimbot::enabled || !vars::General::datamodel.FindFirstChildOfClass("Players").isValid()) {
                saved_target.player = Instance::InstanceManager{};
                vars::Aimbot::aimbot_target = Math::Vector3{ -1.f, -1.f, -1.f };
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            bool should_aim = false;

            vars::Aimbot::aimbot_bind.update(); // or .enabled idk

            if (!should_aim) {
                saved_target.player = Instance::InstanceManager{};
                vars::Aimbot::aimbot_target = Math::Vector3{ -1.f, -1.f, -1.f };
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            // get target (use cached if present)
            Instance::InstanceManager target_player;
            Instance::InstanceManager target_part;
            if (!saved_target.player.isValid()) {
                std::tie(target_part, target_player) = getClosestPlrToCursor();
                saved_target = { target_player, target_part };
            }
            else {
                target_player = saved_target.player;
                target_part = saved_target.bodypart;
            }

            if (!target_player.isValid() || !target_part.isValid()) {
                saved_target.player = Instance::InstanceManager{};
                std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms));
                continue;
            }

            auto camera_inst = vars::General::workspace.FindFirstChildOfClass("Camera");
         
            Math::Vector3 part_pos_3d = target_part.GetPartPos();
            if (vars::Aimbot::camera_prediction) {
           //     Math::Vector3 vel = target_part.get();
               
                float px = (vars::Aimbot::camera_prediction_x == 0.0f) ? 1.0f : vars::Aimbot::camera_prediction_x;
                float py = (vars::Aimbot::camera_prediction_y == 0.0f) ? 1.0f : vars::Aimbot::camera_prediction_y;
                float pz = 2.f;
              //  part_pos_3d.x += vel.x / px;
              //  part_pos_3d.y += vel.y / py; make ur getpartvelocity
          //      part_pos_3d.z += vel.y / pz;
            }

            vars::General::aimbot_target = part_pos_3d;

            // compute screen position
            Math::Vector2 screen_pos = ve.WorldToScreen(part_pos_3d, vars::General::visualengine.GetDimensions(), vars::General::visualengine.GetViewMatrix());
            if (screen_pos.x == 0.0f && screen_pos.y == 0.0f) {
                std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms));
                continue;
            }

            // get cursor in client coords
            POINT cursor_point;
            GetCursorPos(&cursor_point);
            ScreenToClient(FindWindowA(nullptr, "Roblox"), &cursor_point);
            Math::Vector2 cursor{ static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };

            float dist = calulcateDist(screen_pos, cursor);
            if ((!vars::Aimbot::disable_out_fov || dist < vars::Aimbot::fov_size)) {
                float smoothness = 1.0f;
                if (vars::Aimbot::mouse_smoothness >= 1.0f && vars::Aimbot::mouse_smoothness <= 100.0f) {
                    smoothness = ((101.0f - vars::Aimbot::mouse_smoothness) / 100.0f);
                }

                if (vars::Aimbot::aimbot_type == 0) { // mouse
                    Math::Vector2 relative = {
                        (screen_pos.x - cursor_point.x) * 0.1f / vars::Aimbot::mouse_smoothness,
                        (screen_pos.y - cursor_point.y) * 0.1f / vars::Aimbot::mouse_smoothness
                    };
                    INPUT input{}; input.type = INPUT_MOUSE;
                    input.mi.dx = static_cast<LONG>(relative.x);
                    input.mi.dy = static_cast<LONG>(relative.y);
                    input.mi.dwFlags = MOUSEEVENTF_MOVE;
                    SendInput(1, &input, sizeof(input));
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                else if (vars::Aimbot::aimbot_type == 1) {
                    // camera
                }
                else if (vars::Aimbot::aimbot_type == 2) {
                   // silent aim if u want to put it in aimbot.
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms));
        }

        }).detach();
}