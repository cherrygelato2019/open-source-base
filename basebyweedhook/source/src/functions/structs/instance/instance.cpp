#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "../../math/math.h"
#include "../struct.h"
#include "../../../../um/um.h"
#include "../../init_cheat/offsets/offsets.h"
#include "../../../../cheat/vars/vars.h"

using namespace Structs::Instance;
using namespace Math;

namespace Structs {
    namespace Instance {

        std::string InstanceManager::GetName() const {
            return driver.ReadString(driver.read<std::uint64_t>(this->address + Offsets::Instance::Name));
        }

        std::string InstanceManager::GetClass() const {
            std::uint64_t classAddress = driver.read<std::uint64_t>(this->address + Offsets::Instance::ClassName);
            return driver.ReadString(driver.read<std::uint64_t>(classAddress + 0x8));
        }




        float InstanceManager::GetHealth() {
            return driver.read<float>(this->address + Offsets::Humanoid::Health);
        }

        uint64_t InstanceManager::GetPrimitive() const {
            return driver.read<uint64_t>(this->address + Offsets::BasePart::Primitive);
        }

        Math::Vector3 InstanceManager::GetPartPos() const
        {
            Vector3 res{};

            auto primitive = this->GetPrimitive();

            if (!primitive)
                return res;

            res = driver.read<Vector3>(primitive + Offsets::BasePart::Position);
            return res;
        }


        bool InstanceManager::IsValidPart() const
        {
            auto primitive = this->GetPrimitive();
            if (primitive) {
                auto position = this->GetPartPos();
                return !(position.x == 0 && position.y == 0 && position.z == 0);
            }
            return false;
        }

        float InstanceManager::GetMaxHealth() {
            return driver.read<float>(this->address + Offsets::Humanoid::MaxHealth);
        }


        InstanceManager InstanceManager::GetCharacter() {
            return driver.read<InstanceManager>(this->address + Offsets::Player::ModelInstance);
        }

        InstanceManager InstanceManager::GetTeam()
        {
            auto getteam = driver.read<InstanceManager>(this->address + Offsets::Player::Team);
            return getteam;
        }


        std::vector<InstanceManager> InstanceManager::GetChildren(bool isHumanoid) const {
            std::vector<InstanceManager> children;



            std::uint64_t start = driver.read<std::uint64_t>(this->address + Offsets::Instance::ChildrenStart);


            std::uint64_t end = driver.read<std::uint64_t>(start + Offsets::Instance::ChildrenEnd);


            for (auto instance = driver.read<std::uint64_t>(start); instance != end; instance += 0x10) {

                children.emplace_back(driver.read<InstanceManager>(instance));
            }
            return children;
        }


        InstanceManager InstanceManager::FindFirstChildOfClass(const std::string& Name)
        {
            if (!this->address || Name.empty())
                return InstanceManager();

            std::vector<InstanceManager> children = this->GetChildren(true);

            for (auto& object : children)
            {
                if (object.GetClass() == Name)
                {
                    return object;
                }
            }
            return InstanceManager();
        }
        InstanceManager InstanceManager::FindFirstChild_2(std::string childName) const {



            auto children = this->GetChildren(true);


            for (const auto& child : children) {
                if (child.GetName() == childName) {
                    return child;
                }
            }

            return InstanceManager();
        }

        InstanceManager InstanceManager::FindFirstChild(std::string childName, bool isCharChild, std::vector<InstanceManager> loadedChildren) const {
            auto children = this->GetChildren(isCharChild);
            for (const auto& child : children) {
                if (child.GetName() == childName) return child;
            }
            return InstanceManager();
        }

        bool InstanceManager::IsA(const std::string& className) const {
            return this->GetClass() == className;
        }

        bool InstanceManager::IsDescendantOf(const InstanceManager& ancestor) const {
            InstanceManager current = *this;
            while (current.address) {
                if (current.address == ancestor.address) return true;
                current = current.GetParent();
            }
            return false;
        }

        std::vector<InstanceManager> InstanceManager::GetDescendants() const {
            std::vector<InstanceManager> descendants;
            std::vector<InstanceManager> stack = this->GetChildren(false);

            while (!stack.empty()) {
                InstanceManager instance = stack.back();
                stack.pop_back();
                descendants.push_back(instance);
                auto children = instance.GetChildren(false);
                stack.insert(stack.end(), children.begin(), children.end());
            }

            return descendants;
        }

        InstanceManager InstanceManager::FindFirstDescendant(const std::string& name) const {
            auto descendants = this->GetDescendants();
            for (const auto& descendant : descendants) {
                if (descendant.GetName() == name) return descendant;
            }
            return InstanceManager();
        }

        InstanceManager InstanceManager::GetService(std::string serviceName) {
            for (const auto& child : this->GetChildren(false)) {
                if (child.GetClass() == serviceName) return child;
            }
            return InstanceManager();
        }

        bool InstanceManager::SetProperty(const std::string& propertyName, std::uint64_t value) {
     std::uint64_t propertyAddr = this->address + vars::General::datamodel.FindFirstChild(propertyName, false, {}).address;
        if (!propertyAddr) return false;
          driver.write<std::uint64_t>(propertyAddr, value);
          return true;
        }

        std::uint64_t InstanceManager::GetProperty(const std::string& propertyName) const {
            std::uint64_t propertyAddr = this->address + vars::General::datamodel.FindFirstChild(propertyName, false, {}).address;
            if (!propertyAddr) return 0;
           return driver.read<std::uint64_t>(propertyAddr);
        }

        bool InstanceManager::Destroy() {
            if (!this->address) return false;
            driver.write<std::uint64_t>(this->address + Offsets::Instance::Parent, 0);
            return true;
        }

        bool InstanceManager::Clone(InstanceManager& newInstance) const {
            if (!this->address) return false;
            newInstance = driver.read<InstanceManager>(this->address);
            return newInstance.address != 0;
        }

        bool InstanceManager::MoveTo(InstanceManager& parent) {
            if (!this->address || !parent.address) return false;
            driver.write<std::uint64_t>(this->address + Offsets::Instance::Parent, parent.address);
            return true;
        }

        InstanceManager InstanceManager::GetParent() {
            return driver.read<InstanceManager>(this->address + Offsets::Instance::Parent);
        }

        std::vector<InstanceManager> InstanceManager::FindAllChildrenByName(const std::string& childName, bool isCharChild) const {
            std::vector<InstanceManager> matchingInstances;
            auto children = this->GetChildren(isCharChild);
            for (const auto& child : children) {
                if (child.GetName() == childName) matchingInstances.push_back(child);
            }
            return matchingInstances;
        }

        std::vector<InstanceManager> InstanceManager::FindAllDescendantsByName(const std::string& name) const {
            std::vector<InstanceManager> matchingInstances;
            auto descendants = this->GetDescendants();
            for (const auto& descendant : descendants) {
                if (descendant.GetName() == name) matchingInstances.push_back(descendant);
            }
            return matchingInstances;
        }

      
    }
}
