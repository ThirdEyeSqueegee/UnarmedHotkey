#pragma once

#undef GetObject

class Utility : public Singleton<Utility>
{
    inline static RE::BGSEquipSlot* right_hand_slot{};

    inline static RE::BGSEquipSlot* left_hand_slot{};

    inline static RE::TESObjectWEAP* unarmed_weapon{};

    inline static std::atomic<bool> equip_unarmed_flag{};

public:
    using EquippedPairType = std::pair<RE::TESBoundObject*, RE::TESBoundObject*>;

    inline static EquippedPairType equip_cache{};

    inline static std::binary_semaphore sem{ 0 };

    static auto EquipUnarmedWorker(RE::PlayerCharacter* player, RE::ActorEquipManager* manager) noexcept
    {
        while (true) {
            sem.acquire();

            if (equip_unarmed_flag) {
                SKSE::GetTaskInterface()->AddTask([=] {
                    manager->EquipObject(player, unarmed_weapon, nullptr, 1, right_hand_slot);
                    player->DrawWeaponMagicHands(true);
                });
            }
        }
    }

    static auto Init() noexcept
    {
        if (const auto handler{ RE::TESDataHandler::GetSingleton() }) {
            unarmed_weapon = handler->LookupForm<RE::TESObjectWEAP>(0x1f4, "Skyrim.esm");
            logger::debug("Cached unarmed weapon");
        }

        if (const auto dom{ RE::BGSDefaultObjectManager::GetSingleton() }) {
            right_hand_slot = dom->GetObject<RE::BGSEquipSlot>(RE::DEFAULT_OBJECT::kRightHandEquip);
            left_hand_slot  = dom->GetObject<RE::BGSEquipSlot>(RE::DEFAULT_OBJECT::kLeftHandEquip);
            logger::debug("Cached hand slots");
            logger::debug("");
        }

        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            if (const auto manager{ RE::ActorEquipManager::GetSingleton() }) {
                std::jthread([=] { EquipUnarmedWorker(player, manager); }).detach();
            }
        }
    }

    static auto IsPlayerUnarmed() noexcept
    {
        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            if (const auto equipped_right{ player->GetEquippedObject(false) }) {
                if (equipped_right->GetFormID() == unarmed_weapon->GetFormID()) {
                    return true;
                }
            }
        }

        return false;
    }

    static auto Equip() noexcept
    {
        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            if (const auto manager{ RE::ActorEquipManager::GetSingleton() }) {
                manager->UnequipObject(player, unarmed_weapon);

                if (equip_cache.first) {
                    if (const auto right_spell_item{ equip_cache.first->As<RE::SpellItem>() }) {
                        manager->EquipSpell(player, right_spell_item, right_hand_slot);
                    }
                    else {
                        manager->EquipObject(player, equip_cache.first, nullptr, 1, right_hand_slot);
                    }
                    logger::debug("Equipped right: {} (0x{:x})", equip_cache.first->GetName(), equip_cache.first->GetFormID());
                    equip_cache.first = nullptr;
                }
                if (equip_cache.second) {
                    if (const auto left_spell_item{ equip_cache.second->As<RE::SpellItem>() }) {
                        manager->EquipSpell(player, left_spell_item, left_hand_slot);
                    }
                    else {
                        manager->EquipObject(player, equip_cache.second, nullptr, 1, left_hand_slot);
                    }
                    logger::debug("Equipped left: {} (0x{:x})", equip_cache.second->GetName(), equip_cache.second->GetFormID());
                    equip_cache.second = nullptr;
                }

                player->DrawWeaponMagicHands(true);
                equip_unarmed_flag = false;
            }
        }
    }

    static auto Unequip() noexcept
    {
        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            bool is_2h{};
            if (const auto right{ player->GetEquippedObject(false) }) {
                if (const auto right_bound_obj{ right->As<RE::TESBoundObject>() }) {
                    logger::debug("Unequipping right: {} (0x{:x})", right_bound_obj->GetName(), right_bound_obj->GetFormID());
                    equip_cache.first = right_bound_obj;

                    if (const auto right_weapon{ right_bound_obj->As<RE::TESObjectWEAP>() }) {
                        if (right_weapon->IsTwoHandedAxe() || right_weapon->IsTwoHandedSword()) {
                            is_2h = true;
                        }
                    }
                    if (!is_2h) {
                        if (const auto left{ player->GetEquippedObject(true) }) {
                            const auto left_bound_obj{ left->As<RE::TESBoundObject>() };
                            logger::debug("Unequipping left: {} (0x{:x})", left_bound_obj->GetName(), left_bound_obj->GetFormID());
                            equip_cache.second = left_bound_obj;
                        }
                    }

                    player->DrawWeaponMagicHands(false);
                    equip_unarmed_flag = true;
                }
            }
        }
    }
};
