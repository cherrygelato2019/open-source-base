#include <iostream>
#include <thread>
#include "cache.hpp"
#include "../../../../cheat/vars/vars.h"
#include "../../../../um/um.h"
#include "../../init_cheat/offsets/offsets.h"

auto calculateDistance = [](const Math::Vector3& pos1, const Math::Vector3& pos2) -> float {
    return std::sqrt(
        (pos1.x - pos2.x) * (pos1.x - pos2.x) +
        (pos1.y - pos2.y) * (pos1.y - pos2.y) +
        (pos1.z - pos2.z) * (pos1.z - pos2.z)
    );
    };

void PlayerCache::PlayerList() {
    auto game = vars::General::datamodel;
    auto players = vars::General::players;
    auto playerlistc = players.GetChildren(true);
    std::vector<functions::Player> playerlist;

    Math::Vector3 localPlayerPosition;

    for (auto player : playerlistc) {
        functions::Player plyaer;
        plyaer.player = player;
        plyaer.character = plyaer.player.GetCharacter();
        plyaer.children = plyaer.character.GetChildren(true);

        plyaer.Name = plyaer.player.GetName();
        plyaer.team = plyaer.player.GetTeam();

        for (auto characterchild : plyaer.children) {
            std::string characterchildname = characterchild.GetName();
            if (characterchildname == "UpperTorso") {
                plyaer.uppertorso = characterchild;
                plyaer.r6 = false;
            }
            else if (characterchildname == "LowerTorso") {
                plyaer.lowertorso = characterchild;
            }
            else if (characterchildname == "Torso") {
                plyaer.torso = characterchild;
            }
            else if (characterchildname == "Left Leg") {
                plyaer.leftleg = characterchild;
            }
            else if (characterchildname == "Right Leg") {
                plyaer.rightleg = characterchild;
            }
            else if (characterchildname == "Head") {
                plyaer.head = characterchild;
            }
            else if (characterchildname == "Humanoid") {
                plyaer.humanoid = characterchild;
            }
            else if (characterchildname == "HumanoidRootPart") {
                plyaer.hrp = characterchild;
            }
            else if (characterchildname == "LeftUpperLeg") {
                plyaer.leftupperleg = characterchild;
            }
            else if (characterchildname == "RightUpperLeg") {
                plyaer.rightupperleg = characterchild;
            }
            else if (characterchildname == "RightFoot") {
                plyaer.rightfoot = characterchild;
            }
            else if (characterchildname == "LeftFoot") {
                plyaer.leftfoot = characterchild;
            }
            else if (characterchildname == "RightHand") {
                plyaer.righthand = characterchild;
            }
            else if (characterchildname == "LeftHand") {
                plyaer.lefthand = characterchild;
            }
            else if (characterchildname == "RightLowerArm") {
                plyaer.rightlowerarm = characterchild;
            }
            else if (characterchildname == "LeftLowerArm") {
                plyaer.leftlowerarm = characterchild;
            }
            else if (characterchildname == "RightUpperArm") {
                plyaer.rightupperarm = characterchild;
            }
            else if (characterchildname == "LeftUpperArm") {
                plyaer.leftupperarm = characterchild;
            }
            else if (characterchildname == "LeftLowerLeg") {
                plyaer.leftlowerleg = characterchild;
            }
            else if (characterchildname == "RightLowerLeg") {
                plyaer.rightlowerleg = characterchild;
            }
            else if (characterchildname == "Left Arm") {
                plyaer.leftarm = characterchild;
            }
            else if (characterchildname == "Right Arm") {
                plyaer.rightarm = characterchild;
            }
        }

        plyaer.tool = player.GetCharacter().FindFirstChildOfClass("Tool");
        if (vars::Visuals::Tool) {
            plyaer.currentTool = plyaer.character.FindFirstChildOfClass("Tool");
            plyaer.currentToolName = plyaer.character.FindFirstChildOfClass("Tool").GetName();
        }

        plyaer.Health = plyaer.humanoid.GetHealth();
        plyaer.MaxHealth = plyaer.humanoid.GetMaxHealth();

        if (player.address == playerlistc.front().address) {
            plyaer.localplayer = true;  
            vars::General::localplayer_CACHE = plyaer;

            if (plyaer.hrp.IsValidPart()) {
                localPlayerPosition = plyaer.hrp.GetPartPos();
            }
        }
        else {
            plyaer.localplayer = false;  
        }

        if (!plyaer.localplayer) {
            playerlist.push_back(plyaer);
        }
    }

    for (auto& plyaer : playerlist) {
        if (plyaer.hrp.IsValidPart()) {
            plyaer.distance = calculateDistance(plyaer.hrp.GetPartPos(), localPlayerPosition);
        }
        else {
            plyaer.distance = 69420.0f;
        }
    }

    vars::cache::PlayerCache = playerlist;
}

void playerlistloop() {
    while (true) {
        PlayerCache::PlayerList();
        std::this_thread::sleep_for(std::chrono::milliseconds(vars::cache::PlayerCacheDelay));
    }
}

void PlayerCache::PlayerThreadStart() {
    std::thread playerthread(playerlistloop);
    playerthread.detach();
}