#pragma once

#include <string>
#include "../../../../cheat/overlay/design/imgui/imgui.h"
#include "../../../../cheat/overlay/design/imgui/imgui_internal.h"
#include <algorithm>
#include "../../../vars/vars.h"


#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}


struct render_t
{

#define vec_ref const ImVec2&
#define decl static auto

	inline static ImFont* verdana_12{ NULL };
	inline static ImFont* verdana_bold{ NULL };

	decl rect_filled_blur(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f, int layers = 5, float offset = 0.5f) {
		auto draw = ImGui::GetBackgroundDrawList();
		for (int i = 0; i < layers; ++i) {
			ImVec2 offset_pos{ offset * i, offset * i };
			draw->AddRectFilled(from - offset_pos, to + offset_pos, col, rounding);
		}
	}

	decl rect(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f) {
		ImGui::GetBackgroundDrawList()->AddRect(from, to, col, rounding);
	}

	decl rect_filled(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f) {
		ImGui::GetBackgroundDrawList()->AddRectFilled(from, to, col, rounding);
	}

	decl outlined_rect_blur(vec_ref pos, vec_ref size, ImU32 col, float blur_intensity = 0.5f, int layers = 30) {
		auto draw = ImGui::GetBackgroundDrawList();

		for (int i = 1; i <= layers; ++i) {
			float alpha_factor = 1.0f - (float(i) / layers);
			ImU32 faded_col = ImGui::ColorConvertFloat4ToU32(ImVec4(
				(col >> IM_COL32_R_SHIFT & 0xFF) / 255.0f,
				(col >> IM_COL32_G_SHIFT & 0xFF) / 255.0f,
				(col >> IM_COL32_B_SHIFT & 0xFF) / 255.0f,
				alpha_factor * blur_intensity
			));

			ImVec2 offset(i * 0.5f, i * 0.5f);
			draw->AddRect(pos - offset, pos + size + offset, faded_col, 0.0f);
			draw->AddRect(pos + offset, pos + size - offset, faded_col, 0.0f);
		}

		draw->AddRect(pos, pos + size, col, 0.0f);
	}

	void outlined_rect(const ImVec2& pos, const ImVec2& size, ImU32 col, float rounding = 0.f) {
		ImVec2 rounded_pos = ImVec2(std::round(pos.x), std::round(pos.y));
		ImVec2 rounded_size = ImVec2(std::round(size.x), std::round(size.y));
		auto draw = ImGui::GetBackgroundDrawList();
		ImRect rect_bb(rounded_pos, ImVec2(rounded_pos.x + rounded_size.x, rounded_pos.y + rounded_size.y));

		if (vars::Visuals::Box_fill) {
			if (vars::Visuals::Box_gradient) {
				ImU32 top_color = ImGui::ColorConvertFloat4ToU32(ImVec4(vars::Visuals::Colors::BoxGradientTop[0], vars::Visuals::Colors::BoxGradientTop[1], vars::Visuals::Colors::BoxGradientTop[2], vars::Visuals::Colors::BoxGradientTop[3]));
				ImU32 bottom_color = ImGui::ColorConvertFloat4ToU32(ImVec4(vars::Visuals::Colors::BoxGradientBottom[0], vars::Visuals::Colors::BoxGradientBottom[1], vars::Visuals::Colors::BoxGradientBottom[2], vars::Visuals::Colors::BoxGradientBottom[3]));

				if (vars::Visuals::Box_gradient_autorotate) {
					float time = ImGui::GetTime();
					float rotation_speed = vars::Visuals::Box_gradient_rotation_speed; 
					float angle = time * rotation_speed;
					ImVec2 center = ImVec2((rect_bb.Min.x + rect_bb.Max.x) * 0.5f, (rect_bb.Min.y + rect_bb.Max.y) * 0.5f);
					float half_width = (rect_bb.Max.x - rect_bb.Min.x) * 0.5f;
					float half_height = (rect_bb.Max.y - rect_bb.Min.y) * 0.5f;
					float cos_a = cosf(angle);
					float sin_a = sinf(angle);
					float corners[4];
					corners[0] = (cos_a * (-half_width) + sin_a * (-half_height)) / sqrtf(half_width * half_width + half_height * half_height);
					corners[1] = (cos_a * (half_width)+sin_a * (-half_height)) / sqrtf(half_width * half_width + half_height * half_height);
					corners[2] = (cos_a * (half_width)+sin_a * (half_height)) / sqrtf(half_width * half_width + half_height * half_height);
					corners[3] = (cos_a * (-half_width) + sin_a * (half_height)) / sqrtf(half_width * half_width + half_height * half_height);

					for (int i = 0; i < 4; i++) {
						corners[i] = (corners[i] + 1.0f) * 0.5f;
					}

			
					ImU32 corner_colors[4];
					for (int i = 0; i < 4; i++) {
						float t = corners[i];
						ImVec4 top_vec = ImGui::ColorConvertU32ToFloat4(top_color);
						ImVec4 bottom_vec = ImGui::ColorConvertU32ToFloat4(bottom_color);
						ImVec4 lerped = ImVec4(
							top_vec.x + t * (bottom_vec.x - top_vec.x),
							top_vec.y + t * (bottom_vec.y - top_vec.y),
							top_vec.z + t * (bottom_vec.z - top_vec.z),
							top_vec.w + t * (bottom_vec.w - top_vec.w)
						);
						corner_colors[i] = ImGui::ColorConvertFloat4ToU32(lerped);
					}

					draw->AddRectFilledMultiColor(rect_bb.Min, rect_bb.Max,
						corner_colors[0], corner_colors[1], corner_colors[2], corner_colors[3]);
				}
				else {
					draw->AddRectFilledMultiColor(rect_bb.Min, rect_bb.Max, top_color, top_color, bottom_color, bottom_color);
				}
			}
			else {
				ImU32 box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(vars::Visuals::Colors::BoxColor[0], vars::Visuals::Colors::BoxColor[1], vars::Visuals::Colors::BoxColor[2], 0.3f));
				draw->AddRectFilled(rect_bb.Min, rect_bb.Max, box_color);
			}
		}

		if (vars::Visuals::Box_outline) {
			draw->AddRect(rect_bb.Min, rect_bb.Max, IM_COL32(0, 0, 0, col >> 24), rounding);
			draw->AddRect(ImVec2(rect_bb.Min.x - 2.f, rect_bb.Min.y - 2.f), ImVec2(rect_bb.Max.x + 2.f, rect_bb.Max.y + 2.f), IM_COL32(0, 0, 0, col >> 24), rounding);
		}

		draw->AddRect(ImVec2(rect_bb.Min.x - 1.f, rect_bb.Min.y - 1.f), ImVec2(rect_bb.Max.x + 1.f, rect_bb.Max.y + 1.f), ImGui::ColorConvertFloat4ToU32(ImVec4(vars::Visuals::Colors::BoxOutlineColor[0], vars::Visuals::Colors::BoxOutlineColor[1], vars::Visuals::Colors::BoxOutlineColor[2], 1)), rounding);
	}




	
	void health_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor, float x_pad = 4.f, float bar_width = 4.f, bool blue = false)
	{
		int clamped_health = static_cast<int>(std::min(max, current));
		float bar_height = ((size.y - 1.f) * clamped_health) / max;

		auto adjusted_left = ImVec2{ pos.x + size.x, pos.y + size.y - 1.f };
		auto adjusted_right = ImVec2{ pos.x + size.x, pos.y + 1.f };

		rect_filled(
			{ (adjusted_left.x - bar_width) - x_pad, adjusted_left.y },
			{ adjusted_left.x - x_pad, adjusted_right.y },
			IM_COL32(60, 60, 60, static_cast<int>(200 * alpha_factor))
		);

		float health_percentage = current / max;
		health_percentage = std::clamp(health_percentage, 0.0f, 1.0f);

		ImU32 healthBarColor;

		if (vars::Visuals::HealthBasedColor) {
			if (health_percentage > 0.5f) {
				float green_to_red_ratio = 2 * (1.0f - health_percentage);
				healthBarColor = IM_COL32(
					static_cast<int>(255 * green_to_red_ratio),
					255,
					0,
					static_cast<int>(255 * alpha_factor)
				);
			}
			else {
				float red_to_blue_ratio = 2 * health_percentage;
				healthBarColor = IM_COL32(
					255,
					static_cast<int>(255 * red_to_blue_ratio),
					0,
					static_cast<int>(255 * alpha_factor)
				);
			}
		}
		else {
			ImVec4 clampedColor(
				std::clamp(vars::Visuals::Colors::HealthBarColor[0], 0.0f, 1.0f),  // Red
				std::clamp(vars::Visuals::Colors::HealthBarColor[1], 0.0f, 1.0f),  // Green
				std::clamp(vars::Visuals::Colors::HealthBarColor[2], 0.0f, 1.0f),  // Blue
				std::clamp(vars::Visuals::Colors::HealthBarColor[3] * alpha_factor, 0.0f, 1.0f)  // Alpha
			);

			healthBarColor = ImGui::ColorConvertFloat4ToU32(clampedColor);

		}

		rect_filled(
			{ (adjusted_left.x - bar_width - x_pad + 1.f), adjusted_left.y - bar_height },
			{ (adjusted_left.x - x_pad - 1.f), adjusted_left.y },
			healthBarColor
		);

		if (vars::Visuals::HealthBar_Outline) {
			rect(
				{ (adjusted_left.x - bar_width) - x_pad, adjusted_left.y },
				{ adjusted_left.x - x_pad, adjusted_right.y },
				IM_COL32(0, 0, 0, static_cast<int>(255 * alpha_factor))
			);
		}
	}

	decl shield_bar_blur(float max, float current, vec_ref pos, vec_ref size, float alpha_factor, int layers = 16, float blur_intensity = 0.3f) {
		auto draw = ImGui::GetBackgroundDrawList();

		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;
		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		float bar_width = current * adj_size_x / max;

		for (int i = 1; i <= layers; ++i) {
			float alpha_layer = (1.0f - (float(i) / layers)) * blur_intensity;
			ImU32 blur_color = IM_COL32(52, 103, 235, static_cast<int>(150 * alpha_layer * alpha_factor));

			ImVec2 offset(i * 0.3f, i * 0.3f);
			draw->AddRectFilled(adj_from - offset, adj_to + offset, blur_color, 0.0f);
		}

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		ImVec2 shield_bar_from = ImVec2((adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f);
		ImVec2 shield_bar_to = ImVec2((adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f);
		rect_filled(shield_bar_from, shield_bar_to, IM_COL32(52, 103, 235, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));
	}
	decl flame_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor)
	{
		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;

		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		auto bar_width = current * adj_size_x / max;

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		rect_filled({ (adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f }, { (adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f }, IM_COL32(255, 0, 0, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));
	}

	decl shield_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor)
	{

		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;

		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		auto bar_width = current * adj_size_x / max;

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		rect_filled({ (adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f }, { (adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f }, IM_COL32(52, 103, 235, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));

	}

	decl text(vec_ref pos, const std::string& text, ImU32 col, ImFont* font) {
		ImGui::GetBackgroundDrawList()->AddText(font, 11.0f, pos, col, text.c_str());
	}

	decl text_Voided(vec_ref pos, const std::string& text, ImU32 col)
	{
		auto alpha = col >> 24;
		if (alpha > 0)
		{
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(-1.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(1.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(1.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(-1.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(0.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(0.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(1.f, 0.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(pos + ImVec2(-1.f, 0.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
		}
		ImGui::GetBackgroundDrawList()->AddText(pos, col, text.c_str());
	}

	decl line(vec_ref from, vec_ref to, ImU32 col, float thickness) {
		ImGui::GetBackgroundDrawList()->AddLine(from, to, col, thickness);
	}

	decl line_segment(vec_ref from, vec_ref to, ImU32 col, float thickness, float segments = 1.f) {

		if (segments > 1)
		{

			auto draw_list = ImGui::GetBackgroundDrawList();

			float segment_length = 1.0f / segments;
			ImVec2 delta = to - from;

			for (int i = 0; i < segments; ++i)
			{

				float alpha = segment_length * i;

				ImVec2 segment_pos = ImVec2(from.x + delta.x * alpha, from.y + delta.y * alpha);
				draw_list->AddCircleFilled(segment_pos, thickness, col);

			}

		}

	}


	decl circle(vec_ref pos, ImU32 col, float radius, int segments = 6) {
		ImGui::GetBackgroundDrawList()->AddCircle(pos, radius, col, segments);
	}

	decl circle_filled(vec_ref pos, ImU32 col, float radius) {
		ImGui::GetBackgroundDrawList()->AddCircleFilled(pos, radius, col, 128);
	}

	decl initialize()
	{

	}

}; inline render_t Visualize{};