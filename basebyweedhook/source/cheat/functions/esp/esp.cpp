#include "../../../cheat/vars/vars.h"
#include <iostream>
#include <chrono>
#include "esp.h"
#include "../../overlay/design/imgui/imgui.h"
#include <Windows.h>
#include "outlines/render.h"
#include <unordered_map>
#include <algorithm>

namespace helpers {
    std::vector<Math::Vector3> GetCorners(Math::Vector3 center, Math::Vector3 halfSize) {
        return {
            { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z },
            { center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z },
            { center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z },
            { center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z },
            { center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z },
            { center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z },
            { center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z },
            { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z }
        };
    }

    Math::Vector3 MultiplyMatrixVector(const Math::Matrix3& matrix, const Math::Vector3& vector) {
        return matrix.multiplyVector(vector);
    }

    float cross2D(const ImVec2& O, const ImVec2& A, const ImVec2& B) {
        return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
    }

    std::vector<ImVec2> convexHull(std::vector<ImVec2> points) {
        int n = points.size();
        if (n <= 1) return points;

        std::sort(points.begin(), points.end(), [](const ImVec2& a, const ImVec2& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
            });

        std::vector<ImVec2> hull;

        for (int i = 0; i < n; i++) {
            while (hull.size() >= 2 && cross2D(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
                hull.pop_back();
            }
            hull.push_back(points[i]);
        }

        int t = hull.size() + 1;
        for (int i = n - 2; i >= 0; i--) {
            while (hull.size() >= t && cross2D(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
                hull.pop_back();
            }
            hull.push_back(points[i]);
        }

        hull.pop_back();
        return hull;
    }
}

namespace functions {
    class PlayerParts {
    private:
        functions::Player* player;
        std::vector<Structs::Instance::InstanceManager> all_parts;

    public:
        PlayerParts(functions::Player& p) : player(&p) {
            refresh_parts();
        }

        void refresh_parts() {
            if (player->r6) {
                all_parts = {
                    player->head, player->torso,
                    player->leftarm, player->rightarm,
                    player->leftleg, player->rightleg
                };
            }
            else {
                all_parts = {
                    player->head, player->uppertorso, player->lowertorso,
                    player->leftupperarm, player->rightupperarm,
                    player->leftlowerarm, player->rightlowerarm,
                    player->lefthand, player->righthand,
                    player->leftupperleg, player->rightupperleg,
                    player->leftlowerleg, player->rightlowerleg,
                    player->leftfoot, player->rightfoot
                };
            }
        }

        const std::vector<Structs::Instance::InstanceManager>& get_all_parts() const {
            return all_parts;
        }

        functions::Player& get_player() {
            return *player;
        }

        std::vector<Structs::Instance::InstanceManager> get_limb_parts() const {
            if (player->r6) {
                return {
                    player->leftarm, player->rightarm,
                    player->leftleg, player->rightleg
                };
            }
            else {
                return {
                    player->leftupperleg, player->rightupperleg,
                    player->rightlowerleg, player->leftlowerleg,
                    player->lefthand, player->righthand,
                    player->leftupperarm, player->rightupperarm,
                    player->leftlowerarm, player->rightlowerarm
                };
            }
        }

        std::vector<Structs::Instance::InstanceManager> get_torso_parts() const {
            if (player->r6) {
                return { player->torso };
            }
            else {
                return { player->uppertorso, player->lowertorso };
            }
        }

        std::vector<Structs::Instance::InstanceManager> get_head_parts() const {
            return { player->head };
        }
    };

    namespace esp {
        bool bounding_box(PlayerParts& parts, ImVec2& out_min, ImVec2& out_max) {
            float l = FLT_MAX, t = FLT_MAX, r = -FLT_MAX, b = -FLT_MAX;
            bool any_visible = false;

            for (const auto& part : parts.get_all_parts()) {
                if (!part.address || !part.IsValidPart()) continue;

                try {
                    Structs::part::Part part2;
                    part2.address = part.address;
                    auto sz = part2.GetSize();
                    auto pos = part2.GetPosition();

                    float hx = sz.x * 0.5f;
                    float hy = sz.y * 0.5f;
                    float hz = sz.z * 0.5f;

                    static const Math::Vector3 corners[8] = {
                        { -1, -1, -1 }, { 1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 },
                        { -1, -1, 1 },  { 1, -1, 1 },  { -1, 1, 1 },  { 1, 1, 1 }
                    };

                    for (int i = 0; i < 8; ++i) {
                        Math::Vector3 corner = { corners[i].x * hx, corners[i].y * hy, corners[i].z * hz };
                        Math::Vector3 world = corner + pos;
                        Math::Vector2 screen = vars::General::visualengine.WorldToScreen(
                            world,
                            vars::General::visualengine.GetDimensions(),
                            vars::General::visualengine.GetViewMatrix()
                        );

                        if (screen.x != -1 && screen.y != -1) {
                            any_visible = true;
                            l = std::min(l, screen.x);
                            t = std::min(t, screen.y);
                            r = std::max(r, screen.x);
                            b = std::max(b, screen.y);
                        }
                    }
                }
                catch (...) {
                    continue;
                }
            }

            if (!any_visible && parts.get_player().hrp.IsValidPart()) {
                try {
                    Structs::part::Part hrp_part;
                    hrp_part.address = parts.get_player().hrp.address;
                    auto pos = hrp_part.GetPosition();
                    Math::Vector2 screen = vars::General::visualengine.WorldToScreen(
                        pos,
                        vars::General::visualengine.GetDimensions(),
                        vars::General::visualengine.GetViewMatrix()
                    );

                    if (screen.x != -1 && screen.y != -1) {
                        float box_size = 50.0f;
                        l = screen.x - box_size;
                        r = screen.x + box_size;
                        t = screen.y - box_size;
                        b = screen.y + box_size;
                        any_visible = true;
                    }
                }
                catch (...) {}
            }

            if (!any_visible || l >= r || t >= b) return false;

            out_min = { l, t };
            out_max = { r, b };
            return true;
        }

        void chams(PlayerParts& parts) {
            if (!vars::Visuals::Chams) return;

            auto draw = ImGui::GetBackgroundDrawList();

            for (const auto& bone : parts.get_all_parts()) {
                if (!bone.address || !bone.IsValidPart()) continue;

                try {
                    Structs::part::Part part;
                    part.address = bone.address;

                    Math::Vector3 position = part.GetPosition();
                    Math::Vector3 size = part.GetSize();
                    Math::Matrix3 rotation = part.GetRotation();

                    std::vector<Math::Vector3> vertices = helpers::GetCorners({ 0, 0, 0 }, { size.x / 2, size.y / 2, size.z / 2 });

                    for (auto& vertex : vertices) {
                        vertex = helpers::MultiplyMatrixVector(rotation, vertex);
                        vertex = { vertex.x + position.x, vertex.y + position.y, vertex.z + position.z };
                    }

                    std::vector<ImVec2> screenCoords;
                    for (const auto& vertex : vertices) {
                        Math::Vector2 screenPos = vars::General::visualengine.WorldToScreen(
                            vertex,
                            vars::General::visualengine.GetDimensions(),
                            vars::General::visualengine.GetViewMatrix()
                        );
                        if (screenPos.x == -1 || screenPos.y == -1) {
                            screenCoords.clear();
                            break;
                        }
                        screenCoords.push_back(ImVec2(screenPos.x, screenPos.y));
                    }

                    if (screenCoords.size() < 3) continue;

                    auto hull = helpers::convexHull(screenCoords);

                    ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                        vars::Visuals::Colors::ChamsColor[0],
                        vars::Visuals::Colors::ChamsColor[1],
                        vars::Visuals::Colors::ChamsColor[2],
                        vars::Visuals::Colors::ChamsColor[3]
                    ));

                    draw->AddConvexPolyFilled(hull.data(), hull.size(), color);

 
                }
                catch (...) {
                    continue;
                }
            }
        }

        void box_filled(PlayerParts& parts) {}
        void box_gradient_fill(PlayerParts& parts) {}
        void box_glow(PlayerParts& parts) {}
    }

    namespace player_indicators {
        void* china_hat() { return nullptr; }

        void distance(ImVec2 box_min, ImVec2 box_max, functions::Player& player, float& current_y_offset) {
            if (!vars::Visuals::Distance) return;
            if (player.distance <= 0.0f || player.localplayer) return;

            std::string distance_text = std::to_string((int)player.distance) + "m";
            ImVec2 text_size = ImGui::CalcTextSize(distance_text.c_str());

            ImVec2 distance_pos = ImVec2(
                box_min.x + (box_max.x - box_min.x) * 0.5f - text_size.x * 0.5f,
                box_max.y + current_y_offset
            );

            ImU32 distance_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                vars::Visuals::Colors::distance_color[0],
                vars::Visuals::Colors::distance_color[1],
                vars::Visuals::Colors::distance_color[2],
                1.0f
            ));

            Visualize.text_Voided(distance_pos, distance_text, distance_color);
            current_y_offset += text_size.y + 2.0f;
        }

        void name(ImVec2 box_min, ImVec2 box_max, functions::Player& player) {
            if (!vars::Visuals::Name) return;

            std::string playerName = player.Name;
            if (playerName.empty()) return;

            ImVec2 text_size = ImGui::CalcTextSize(playerName.c_str());
            ImVec2 name_pos = ImVec2(
                box_min.x + (box_max.x - box_min.x) * 0.5f - text_size.x * 0.5f,
                box_min.y - text_size.y - 5.0f
            );

            ImU32 name_color;

            if (player.localplayer) {
                name_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                    vars::Visuals::Colors::localplayer_name_color[0],
                    vars::Visuals::Colors::localplayer_name_color[1],
                    vars::Visuals::Colors::localplayer_name_color[2],
                    1.0f
                ));
            }
            else {
                try {
                    if (player.team.address != vars::General::localplayer_CACHE.team.address) {
                        name_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                            vars::Visuals::Colors::enemy_name_color[0],
                            vars::Visuals::Colors::enemy_name_color[1],
                            vars::Visuals::Colors::enemy_name_color[2],
                            1.0f
                        ));
                    }
                    else {
                        name_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                            vars::Visuals::Colors::team_name_color[0],
                            vars::Visuals::Colors::team_name_color[1],
                            vars::Visuals::Colors::team_name_color[2],
                            1.0f
                        ));
                    }
                }
                catch (...) {
                    name_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                        vars::Visuals::Colors::NameColor[0],
                        vars::Visuals::Colors::NameColor[1],
                        vars::Visuals::Colors::NameColor[2],
                        vars::Visuals::Colors::NameColor[3]
                    ));
                }
            }

            Visualize.text_Voided(name_pos, playerName, name_color);
        }

        namespace flags {
            static std::unordered_map<uintptr_t, std::string> movement_states;
            static std::unordered_map<uintptr_t, Math::Vector3> last_positions;
            static std::unordered_map<uintptr_t, std::chrono::steady_clock::time_point> last_update_times;

            void update_movement_state(functions::Player& player) {
                uintptr_t player_id = reinterpret_cast<uintptr_t>(&player);
                PlayerParts parts(player);

                try {
                    auto torso_parts = parts.get_torso_parts();
                    if (torso_parts.empty()) {
                        movement_states[player_id] = "INVALID";
                        return;
                    }

                    Structs::Instance::InstanceManager valid_part;
                    bool found_valid = false;

                    for (const auto& part : torso_parts) {
                        if (part.address && part.IsValidPart()) {
                            valid_part = part;
                            found_valid = true;
                            break;
                        }
                    }

                    if (!found_valid) {
                        auto limb_parts = parts.get_limb_parts();
                        for (const auto& part : limb_parts) {
                            if (part.address && part.IsValidPart()) {
                                valid_part = part;
                                found_valid = true;
                                break;
                            }
                        }
                    }

                    if (!found_valid) {
                        movement_states[player_id] = "INVALID";
                        return;
                    }

                    Structs::part::Part movement_part;
                    movement_part.address = valid_part.address;

                    auto current_pos = movement_part.GetPosition();
                    auto current_time = std::chrono::steady_clock::now();

                    auto pos_it = last_positions.find(player_id);
                    auto time_it = last_update_times.find(player_id);

                    if (pos_it != last_positions.end() && time_it != last_update_times.end()) {
                        auto time_delta = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - time_it->second).count() / 1000.0f;

                        if (time_delta > 0.016f) {
                            Math::Vector3 velocity = {
                                (current_pos.x - pos_it->second.x) / time_delta,
                                (current_pos.y - pos_it->second.y) / time_delta,
                                (current_pos.z - pos_it->second.z) / time_delta
                            };

                            float velocity_magnitude = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
                            float vertical_velocity = velocity.y;

                            if (vertical_velocity > 8.0f) {
                                movement_states[player_id] = "JUMPING";
                            }
                            else if (vertical_velocity < -8.0f) {
                                movement_states[player_id] = "FALLING";
                            }
                            else if (velocity_magnitude >= 20.0f && abs(vertical_velocity) < 5.0f) {
                                movement_states[player_id] = "RUNNING";
                            }
                            else if (velocity_magnitude > 2.0f && velocity_magnitude < 20.0f && abs(vertical_velocity) < 5.0f) {
                                movement_states[player_id] = "WALKING";
                            }
                            else if (velocity_magnitude <= 2.0f) {
                                auto state_it = movement_states.find(player_id);
                                if (state_it != movement_states.end() &&
                                    (state_it->second == "WALKING" || state_it->second == "RUNNING")) {
                                    movement_states[player_id] = "STOPPING";
                                }
                                else {
                                    movement_states[player_id] = "IDLE";
                                }
                            }

                            last_positions[player_id] = current_pos;
                            last_update_times[player_id] = current_time;
                        }
                    }
                    else {
                        last_positions[player_id] = current_pos;
                        last_update_times[player_id] = current_time;
                        movement_states[player_id] = "INITIALIZING";
                    }
                }
                catch (...) {
                    movement_states[player_id] = "ERROR";
                }
            }

            void render(ImVec2 box_min, ImVec2 box_max, functions::Player& player) {
                if (!vars::Visuals::MovementStatus) return;

                update_movement_state(player);
                uintptr_t player_id = reinterpret_cast<uintptr_t>(&player);

                auto it = movement_states.find(player_id);
                if (it == movement_states.end()) return;

                std::vector<std::string> flags;
                flags.push_back(it->second);

                float flag_y_offset = 0.0f;
                for (const std::string& flag : flags) {
                    ImVec2 text_size = ImGui::CalcTextSize(flag.c_str());
                    ImVec2 flag_pos = ImVec2(box_max.x + 5.0f, box_min.y + flag_y_offset);

                    ImU32 flag_color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                        vars::Visuals::Colors::flags[0],
                        vars::Visuals::Colors::flags[1],
                        vars::Visuals::Colors::flags[2],
                        1.0f
                    ));

                    Visualize.text_Voided(flag_pos, flag, flag_color);
                    flag_y_offset += text_size.y + 1.0f;
                }
            }

            void cleanup_old_players() {
                static auto last_cleanup = std::chrono::steady_clock::now();
                auto now = std::chrono::steady_clock::now();

                if (std::chrono::duration_cast<std::chrono::seconds>(now - last_cleanup).count() > 30) {
                    auto it = last_update_times.begin();
                    while (it != last_update_times.end()) {
                        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > 30) {
                            uintptr_t player_id = it->first;
                            movement_states.erase(player_id);
                            last_positions.erase(player_id);
                            it = last_update_times.erase(it);
                        }
                        else {
                            ++it;
                        }
                    }
                    last_cleanup = now;
                }
            }
        }
    }

    namespace detection_features {
        void* sonar() { return nullptr; }
        void* head_dot() { return nullptr; }
        void* ooof_arrows() { return nullptr; }
        void* tracers() { return nullptr; }
    }
}

void visuals::init::Load_1() {
    if (vars::cache::PlayerCache.empty()) return;

    for (auto& player : vars::cache::PlayerCache) {
        if (player.localplayer) continue;

        functions::PlayerParts parts(player);
        ImVec2 box_min, box_max;

        if (!functions::esp::bounding_box(parts, box_min, box_max)) continue;

        functions::esp::chams(parts);

        if (vars::Visuals::Box) {
            ImVec2 size = { box_max.x - box_min.x, box_max.y - box_min.y };
            ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(
                vars::Visuals::Colors::BoxColor[0],
                vars::Visuals::Colors::BoxColor[1],
                vars::Visuals::Colors::BoxColor[2],
                1.0f
            ));
            Visualize.outlined_rect(box_min, size, color, 0.0f);
        }

        functions::player_indicators::name(box_min, box_max, player);

        float y_offset = 2.0f;
        functions::player_indicators::distance(box_min, box_max, player, y_offset);

        functions::player_indicators::flags::render(box_min, box_max, player);
    }

    functions::player_indicators::flags::cleanup_old_players();
}