#pragma once

class Utility : public Singleton<Utility>
{
public:
    inline static RE::TESObjectWEAP* unarmed_weapon{};

    inline static bool unequip_flag{};

    static void InitUtility() noexcept
    {
        const auto handler{ RE::TESDataHandler::GetSingleton() };
        unarmed_weapon = handler->LookupForm<RE::TESObjectWEAP>(0x1f4, "Skyrim.esm"sv);
        logger::debug("Cached unarmed weapon");
    }
};
