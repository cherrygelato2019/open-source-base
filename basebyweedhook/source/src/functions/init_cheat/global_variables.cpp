#define _CRT_SECURE_NO_WARNINGS
#include "../init_cheat/init.hpp"

void init::cheat::find_stuff() {

    uintptr_t base = driver.get_module_base(L"RobloxPlayerBeta.exe");

    uintptr_t visualengineAddress = driver.read<uintptr_t>(base + Offsets::VisualEngine::Pointer);
    auto visualengine = static_cast<Structs::visualengine::VisualEngine>(visualengineAddress);

    uintptr_t datamodelPointer = driver.read<uintptr_t>(visualengine.address + Offsets::VisualEngine::ToDataModel1);
    auto datamodel = static_cast<Structs::Instance::InstanceManager>(driver.read<uintptr_t>(datamodelPointer + Offsets::VisualEngine::ToDataModel2));

    auto players = datamodel.FindFirstChild("Players", true, datamodel.GetChildren(true));
    auto workspace = datamodel.FindFirstChild("Workspace", true, datamodel.GetChildren(true));
    auto localplayer = players.FindFirstChild("LocalPlayer", true, players.GetChildren(true));
    auto replicatedstorage = datamodel.FindFirstChild("replicatedstorage", true, datamodel.GetChildren(true));
    auto camera = workspace.FindFirstChild("camera", true, players.GetChildren(true));
    auto cameraInstance = workspace.FindFirstChild("camera", true, players.GetChildren(true));


    vars::General::datamodel = datamodel;
    vars::General::workspace = workspace;
    vars::General::players = players;
    vars::General::visualengine = visualengine;
    vars::General::localplayer = localplayer;
    vars::General::replicatedstorage = replicatedstorage;


}