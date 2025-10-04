#ifndef VARS_H
#define VARS_H
#include <string>
#include "../../src/functions/structs/struct.h"
#include "../../src/functions/player_threading/cache/cache.hpp"
#include "../overlay/design/key/keybind.hpp"
namespace vars {

	namespace Visuals {
		inline int MaxDistance = 10000;
		inline bool masterswitch = false;
		inline bool Box = false;
		inline bool HealthBar = true;
		inline bool MovementStatus = false;
		inline bool Name = false;
		inline bool Tool = true;
		inline bool Chams = true;
		inline bool shield_bar = false;
		inline bool Distance = false;
		inline bool Skeleton = false;
		inline bool Box_outline = true;
		inline bool Box_fill = false;
		inline bool HealthBar_Outline = false;
		inline bool HealthBasedColor = false;
		inline bool Box_gradient = true;
		inline bool Box_gradient_autorotate = false;
		inline float Box_gradient_rotation_speed = 1.0f;
		namespace Colors {
			inline float alpha = 1.0f;
			inline int glow_opacity = 1;
			inline float glow_size = 30.0f;
			inline float ChamsColor[4] = { 0.788235f, 0.337255f, 0.45098f, 0.8f };
			inline float BoxOutlineColor[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float tool_color[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float BoxColor[4] = { 0.788235f, 0.337255f, 0.45098f, 1.0f };
			inline float flags[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float HealthBarColor[4] = { 0.0f, 1.0f, 0.0f };
			inline float NameColor[4] = { 0.788235f, 0.337255f, 0.45098f, 1.0f };
			inline float SkeletonColor[4] = { 0.788235f, 0.337255f, 0.45098f, 1.0f };
			inline float glow_color[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float BoxGradientTop[4] = { 0.851f, 0.298f, 0.424f, 0.5f };
			inline float BoxGradientBottom[4] = { 0.2f, 0.2f, 0.2f, 0.5f };

			inline float distance_color[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float localplayer_name_color[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float enemy_name_color[3] = { 0.788235f, 0.337255f, 0.45098f };
			inline float team_name_color[3] = { 0.788235f, 0.337255f, 0.45098f };
		}
	}

	namespace General {
		inline Structs::Instance::InstanceManager datamodel;
		inline Structs::Instance::InstanceManager workspace;
		inline Structs::Instance::InstanceManager players;
		inline Structs::camera::Camera camera;
		inline Structs::visualengine::VisualEngine visualengine;
		inline Structs::Instance::InstanceManager localplayer;
		inline Structs::Instance::InstanceManager replicatedstorage;
		inline functions::Player localplayer_CACHE;

	}

	namespace Lighting {

	}

	namespace Aimbot {
		inline bool enabled = false;
		inline bool draw_fov = false;
		inline float fov_size = 90.0f;
		inline float max_distance = 1000.0f;
		inline bool disable_out_fov = false;

		inline int aimbot_type = 0;
		inline int aimbot_mode = 0;
		inline int aimbot_part = 0;
		inline bool closest_part = false;
		inline bool sticky = false;
		inline bool resolver = false;
		inline bool spectate_target = false;
		inline bool autotype = false;

		inline bool usedeadzone = false;
		inline float deadzoneX = 3.0f;
		inline float deadzoneY = 3.0f;
		inline CKeybind aimbot_bind{ ("aimbot") };

		inline float camera_prediction_x = 1.0f;
		inline float camera_prediction_y = 1.0f;
		inline float free_aim_prediction_x = 1.0f;
		inline float free_aim_prediction_y = 1.0f;

		inline bool camera_prediction = false;

		inline float mouse_sensitivity = 1.0f;
		inline float mouse_smoothness = 1.0f;
		inline float smoothness_camera = 1.0f;
		inline float max_aimbot_distance = 200.f;
		inline bool shake = false;
		inline float shake_x = 1.0f;
		inline float shake_y = 1.0f;

		inline std::vector<bool> aimbot_checks = {
			true,
			true,
			true,
			true
		};

		// Air Part System
		inline bool air_part_enabled = false;
		inline int air_part_selection = 0;
		inline float air_detection_threshold = 5.0f;

		// Prediction System
		inline int prediction_system = 0;
		inline bool ping_interpolation_enabled = false;
		inline float ping_base = 50.0f;
		inline float ping_strength = 1.0f;

		// Resolver
		inline bool resolver_enabled = false;
		inline bool abs_v_enabled = false;
		inline float horizontal_resolver = 1.0f;
		inline float vertical_resolver = 1.0f;
		inline float decreaser_resolver = 1.0f;

		// Advanced Options
		inline float size_multiplier = 1.0f;
		inline bool closest_to_camera = false;
		inline bool exponential_effect = false;
		inline float aimbot_delay = 0.0f;
	}

	namespace listeners {
		inline bool third_person_listener = false;
	}

	namespace player {
		inline bool third_person = false;
		inline int third_person_key = 0x45;
	}

	namespace UI {
		inline bool vsync;
		inline bool streamproof;
		inline bool highcpusage = false;

	}

	namespace cache {
		inline int PlayerCacheDelay = 333;
		inline std::vector<functions::Player> PlayerCache;
		inline bool non_zero_check = true;
	}
}
#endif // VARS_H