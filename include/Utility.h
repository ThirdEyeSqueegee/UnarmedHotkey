#pragma once

#undef GetObject

class Utility : public Singleton<Utility>
{
public:
    using EquippedPairType = std::pair<RE::TESBoundObject*, RE::TESBoundObject*>;

    inline static RE::TESObjectWEAP* unarmed_weapon{};

    inline static bool unequip_flag{};

    inline static EquippedPairType equip_cache{};

    inline static RE::BGSEquipSlot* right_hand_slot{};

    inline static RE::BGSEquipSlot* left_hand_slot{};

    inline static bool is_right_weapon{};

    inline static bool is_left_weapon{};

    static void InitUtility() noexcept
    {
        const auto handler{ RE::TESDataHandler::GetSingleton() };
        unarmed_weapon = handler->LookupForm<RE::TESObjectWEAP>(0x1f4, "Skyrim.esm"sv);
        logger::debug("Cached unarmed weapon");

        const auto dom{ RE::BGSDefaultObjectManager::GetSingleton() };

        right_hand_slot = dom->GetObject(RE::DEFAULT_OBJECT::kRightHandEquip)->As<RE::BGSEquipSlot>();
        left_hand_slot  = dom->GetObject(RE::DEFAULT_OBJECT::kLeftHandEquip)->As<RE::BGSEquipSlot>();
        logger::debug("Cached hand slots");
    }

    static void Equip()
    {
        const auto player{ RE::PlayerCharacter::GetSingleton() };
        const auto manager{ RE::ActorEquipManager::GetSingleton() };

        manager->UnequipObject(player, unarmed_weapon);

        if (equip_cache.first) {
            if (is_right_weapon) {
                manager->EquipObject(player, equip_cache.first, nullptr, 1, right_hand_slot);
                is_right_weapon = false;
            }
            else {
                if (const auto right_spell_item{ equip_cache.first->As<RE::SpellItem>() }) {
                    manager->EquipSpell(player, right_spell_item, right_hand_slot);
                }
            }
            logger::debug("Equipped right: {}", equip_cache.first->GetName());
            equip_cache.first = nullptr;
        }
        if (equip_cache.second) {
            if (is_left_weapon) {
                manager->EquipObject(player, equip_cache.second, nullptr, 1, left_hand_slot);
                is_left_weapon = false;
            }
            else {
                if (const auto left_spell_item{ equip_cache.second->As<RE::SpellItem>() }) {
                    manager->EquipSpell(player, left_spell_item, left_hand_slot);
                }
            }
            logger::debug("Equipped left: {}", equip_cache.second->GetName());
            equip_cache.second = nullptr;
        }
        unequip_flag = false;
    }

    static void Unequip()
    {
        const auto player{ RE::PlayerCharacter::GetSingleton() };
        const auto manager{ RE::ActorEquipManager::GetSingleton() };

        bool is_2h{};

        if (player->GetEquippedObject(false)) {
            const auto right_bound_obj{ player->GetEquippedObject(false)->As<RE::TESBoundObject>() };
            logger::debug("Found right hand bound object: {}", right_bound_obj->GetName());
            equip_cache.first = right_bound_obj;
            if (const auto right_weapon{ right_bound_obj->As<RE::TESObjectWEAP>() }) {
                is_right_weapon = true;
                if (right_weapon->IsTwoHandedAxe() || right_weapon->IsTwoHandedSword()) {
                    is_2h = true;
                }
            }
            if (player->GetEquippedObject(true)) {
                const auto left_bound_obj{ player->GetEquippedObject(true)->As<RE::TESBoundObject>() };
                logger::debug("Found left hand bound object: {}", left_bound_obj->GetName());
                if (!is_2h) {
                    equip_cache.second = left_bound_obj;
                    if (const auto left_weapon{ left_bound_obj->As<RE::TESObjectWEAP>() }) {
                        is_left_weapon = true;
                    }
                }
            }
        }

        manager->EquipObject(player, unarmed_weapon);
        unequip_flag = true;
    }
};
