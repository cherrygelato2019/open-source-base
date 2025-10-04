#pragma once
#include "../../functions/math/math.h"
#include "enums/humanoidstate.h"
namespace Structs {

	namespace Instance {
		class InstanceManager {
		public:
			bool isValid() const noexcept {
				return address != 0;
			}
			std::uint64_t address = 0;
			std::string GetName() const;
			std::string GetClass() const;
			float GetHealth();
			uint64_t GetPrimitive() const;
			Math::Vector3 GetPartPos() const;
			bool IsValidPart() const;
			float GetMaxHealth();
			InstanceManager GetCharacter();
			InstanceManager GetTeam();
			std::vector<InstanceManager> GetChildren(bool isHumanoid) const;
			InstanceManager FindFirstChildOfClass(const std::string& Name);
			InstanceManager FindFirstChild_2(std::string childName) const;
			InstanceManager FindFirstChild(std::string childName, bool isCharChild, std::vector<InstanceManager> loadedChildren) const;
			bool IsA(const std::string& className) const;
			bool IsDescendantOf(const InstanceManager& ancestor) const;
			std::vector<InstanceManager> GetDescendants() const;
			InstanceManager FindFirstDescendant(const std::string& name) const;
			InstanceManager GetService(std::string serviceName);
			bool SetProperty(const std::string& propertyName, std::uint64_t value);
			std::uint64_t GetProperty(const std::string& propertyName) const;
			bool Destroy();
			bool Clone(InstanceManager& newInstance) const;
			bool MoveTo(InstanceManager& parent);
			InstanceManager GetParent();
			std::vector<InstanceManager> FindAllChildrenByName(const std::string& childName, bool isCharChild) const;
			std::vector<InstanceManager> FindAllDescendantsByName(const std::string& name) const;

		};
	}

	namespace camera {
		class Camera {
		public:
			std::uint64_t address = 0;
			Camera GetCameraInstance();
			 Math::Vector3 GetCameraPosition();
			 void SetCameraPosition(const Math::Vector3& position);
			 Math::Matrix3 GetCameraRotation();
			float GetFov();
			Camera GetCameraSubject();
			int GetCameraType();
			void SetCameraRotation(Math::Matrix3 Rotation);
			void SetCameraMaxZoom(int zoom);
			void SetCameraMinZoom(int zoom);
		};
	}

	namespace part {
		class Part {
		public:
			std::uint64_t address = 0;
			std::uint64_t GetPart() const;
			int GetPartMaterial() const;
			bool SetCanCollide(bool enable) const;
			Math::Vector3 GetPosition() const;
			Math::Vector3 GetVelocity() const;
			Math::Matrix3 GetRotation() const;
			Math::Vector3 GetSize() const;
			void SetSize(Math::Vector3 size) const;
			Math::CFrame GetCframe();

		};
	}

	namespace player {
		class Player {
		public:
			std::uint64_t address = 0;
		};
	}


	namespace visualengine {
		class VisualEngine {
		public:
			std::uint64_t address = 0;
			Math::Vector2 WorldToScreen(Math::Vector3 world, Math::Vector2 dimensions, Math::Matrix4 viewmatrix);
			Math::Matrix4 GetViewMatrix();
			Math::Vector2 GetDimensions();

		};
		inline VisualEngine ve;
	}

}
