#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\UnarmedHotkey.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    hotkey = ini.GetLongValue("General", "uHotkey");

    logger::info("Loaded settings");
    logger::info("\tuHotkey = {}", hotkey);
    logger::info("");
}
