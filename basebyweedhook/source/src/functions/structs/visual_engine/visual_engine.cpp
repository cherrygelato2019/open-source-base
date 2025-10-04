#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "../../math/math.h"
#include "../struct.h"
#include "../../../../um/um.h"
#include "../../init_cheat/offsets/offsets.h"
#include "../../../../cheat/vars/vars.h"

using namespace Structs::visualengine;
using namespace Math;

namespace Structs {
    namespace visualengine {

         Math::Vector2 VisualEngine::WorldToScreen(Math::Vector3 world, Math::Vector2 dimensions, Math::Matrix4 viewmatrix) {
         
             Math::Vector4 clipCoords = {
              world.x * viewmatrix.data[0] + world.y * viewmatrix.data[1] + world.z * viewmatrix.data[2] + viewmatrix.data[3],
              world.x * viewmatrix.data[4] + world.y * viewmatrix.data[5] + world.z * viewmatrix.data[6] + viewmatrix.data[7],
              world.x * viewmatrix.data[8] + world.y * viewmatrix.data[9] + world.z * viewmatrix.data[10] + viewmatrix.data[11],
              world.x * viewmatrix.data[12] + world.y * viewmatrix.data[13] + world.z * viewmatrix.data[14] + viewmatrix.data[15]
             };

             if (vars::cache::non_zero_check) {
                 if (clipCoords.w <= 1e-6f) {
                     return { -1.0f, -1.0f };
                 }
             }

             float inv_w = 1.0f / clipCoords.w;
             Math::Vector3 ndc = { clipCoords.x * inv_w, clipCoords.y * inv_w, clipCoords.z * inv_w };

             return {
                 (dimensions.x / 2.0f) * (ndc.x + 1.0f),
                 (dimensions.y / 2.0f) * (1.0f - ndc.y)
             };
        }

        Math::Matrix4 VisualEngine::GetViewMatrix() {
            return driver.read<Math::Matrix4>(this->address + Offsets::VisualEngine::ViewMatrix);
        }

        Math::Vector2 VisualEngine::GetDimensions()
        {
            return driver.read<Math::Vector2>(this->address + Offsets::VisualEngine::Dimensions);
        }

    }
}
