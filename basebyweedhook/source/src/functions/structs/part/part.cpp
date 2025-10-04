#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "../../math/math.h"
#include "../struct.h"
#include "../../../../um/um.h"
#include "../../init_cheat/offsets/offsets.h"

namespace Structs {
    namespace part {

        std::uint64_t Part::GetPart() const {
            return driver.read<std::uint64_t>(this->address + Offsets::BasePart::Primitive);
        }

        int Part::GetPartMaterial() const {
            return driver.read<int>(this->address + 0x2F0);
        }

        bool Part::SetCanCollide(bool enable) const {
            uintptr_t Primitive = driver.read<uintptr_t>(this->address + Offsets::BasePart::Primitive);
            if (!Primitive) return false;

            BYTE val = driver.read<BYTE>(Primitive + Offsets::PrimitiveFlags::CanCollide);

            if (enable)
                val |= 0x08;
            else
                val &= ~0x08;

            driver.write<BYTE>(Primitive + Offsets::PrimitiveFlags::CanCollide, val);
            return enable;
        }

        Math::Vector3 Part::GetPosition() const {
            return driver.read<Math::Vector3>(GetPart() + Offsets::BasePart::Position);
        }

        Math::Vector3 Part::GetVelocity() const {
            return driver.read<Math::Vector3>(GetPart() + Offsets::BasePart::AssemblyLinearVelocity);
        }

        Math::Matrix3 Part::GetRotation() const {
            return driver.read<Math::Matrix3>(GetPart() + Offsets::BasePart::Rotation);
        }

        Math::Vector3 Part::GetSize() const {
            return driver.read<Math::Vector3>(GetPart() + Offsets::BasePart::Size);
        }

        void Part::SetSize(Math::Vector3 size) const {
            driver.write<Math::Vector3>(GetPart() + Offsets::BasePart::Size, size);
        }

        Math::CFrame Part::GetCframe() {
            Math::CFrame res{};
            std::uint64_t primitive = GetPart();
            if (primitive) {
                res = driver.read<Math::CFrame>(primitive + Offsets::BasePart::Position);
            }
            return res;
        }

    } // namespace part
} // namespace Structs
