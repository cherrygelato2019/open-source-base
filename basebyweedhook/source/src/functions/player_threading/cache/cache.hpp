#pragma once
#include "../../structs/struct.h"

namespace functions {
	class Player {
	public:

		std::uint64_t address = 0;

		bool localplayer = false;
		Structs::Instance::InstanceManager team;
		bool r6 = true;

		float Health;
		float MaxHealth;

		float distance;

		std::string Name;
		std::vector<Structs::Instance::InstanceManager> children;
		Structs::Instance::InstanceManager tool;
		Structs::Instance::InstanceManager player;
		Structs::Instance::InstanceManager character;
		std::string currentToolName;
		Structs::Instance::InstanceManager head;
		Structs::Instance::InstanceManager hrp;
		Structs::Instance::InstanceManager humanoid;
		Structs::Instance::InstanceManager currentTool;
		Structs::Instance::InstanceManager leftarm;
		Structs::Instance::InstanceManager rightarm;

		Structs::Instance::InstanceManager lefthand;
		Structs::Instance::InstanceManager righthand;
		Structs::Instance::InstanceManager leftlowerarm;
		Structs::Instance::InstanceManager rightlowerarm;
		Structs::Instance::InstanceManager leftupperarm;
		Structs::Instance::InstanceManager rightupperarm;

		Structs::Instance::InstanceManager leftfoot;
		Structs::Instance::InstanceManager rightfoot;
		Structs::Instance::InstanceManager rightupperleg;
		Structs::Instance::InstanceManager leftupperleg;
		Structs::Instance::InstanceManager leftlowerleg;
		Structs::Instance::InstanceManager rightlowerleg;

		Structs::Instance::InstanceManager uppertorso;
		Structs::Instance::InstanceManager lowertorso;
		Structs::Instance::InstanceManager torso;

		Structs::Instance::InstanceManager leftleg;
		Structs::Instance::InstanceManager rightleg;

		bool operator==(functions::Player& other) {
			return character.address == other.character.address;
		}



	};
}

namespace PlayerCache {
	void PlayerList();
	void PlayerThreadStart();
}
