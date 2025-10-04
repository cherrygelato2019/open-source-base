#define _CRT_SECURE_NO_WARNINGS
#include "init.hpp"
#include "../../../cheat/functions/units/thread_entites.hpp"
#include "../../../cheat/functions/aimbot/aimbot.h"
using namespace std;

void init::cheat::load() {
    if (!driver.attach(L"RobloxPlayerBeta.exe")) {
        Logger::Log(Type::FAIL, "[Entry] Driver Failed.\n");
        return;
    }
    Logger::Log(Type::LOAD, "[Entry] Welcome [skid].\n");
    Logger::Log(Type::FAIL, "[Entry] Welcome [skid].\n");
    Logger::Log(Type::DEBUG, "[Entry] Welcome [skid].\n");
    Logger::Log(Type::OK, "[Entry] Welcome [skid].\n");

    uintptr_t base = driver.get_module_base(L"RobloxPlayerBeta.exe");
    init::cheat::find_stuff();
    std::thread(functions::units::thread).detach();
    std::thread(function::load_aimbot).detach();

    std::thread(overlay::render).detach();
    PlayerCache::PlayerThreadStart();
    std::cin.get();

    while (true) {}
}                                                                                                      