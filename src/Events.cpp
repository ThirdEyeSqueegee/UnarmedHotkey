#include "Events.h"

#include "Settings.h"
#include "Utility.h"

namespace Events
{
    using enum RE::INPUT_DEVICE;

    RE::BSEventNotifyControl InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) noexcept
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }

        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (ui->GameIsPaused() || ui->IsApplicationMenuOpen() || ui->IsItemMenuOpen() || ui->IsModalMenuOpen()) {
                return RE::BSEventNotifyControl::kContinue;
            }
            if (const auto control_map{ RE::ControlMap::GetSingleton() }; control_map->IsFightingControlsEnabled()) {
                if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
                    for (auto e{ *a_event }; e != nullptr; e = e->next) {
                        if (const auto btn_event{ e->AsButtonEvent() }) {
                            if (!btn_event->IsDown()) {
                                return RE::BSEventNotifyControl::kContinue;
                            }
                            const auto device{ btn_event->GetDevice() };
                            auto keycode{ btn_event->GetIDCode() };
                            if (device != kKeyboard && device != kGamepad) {
                                return RE::BSEventNotifyControl::kContinue;
                            }
                            if (device == kGamepad) {
                                keycode = SKSE::InputMap::GamepadMaskToKeycode(keycode);
                            }
                            const auto manager{ RE::ActorEquipManager::GetSingleton() };
                            if (keycode == Settings::hotkey) {
                                if (Utility::unequip_flag) {
                                    logger::debug("Toggling unequip off");
                                    manager->UnequipObject(player, Utility::unarmed_weapon);
                                    Utility::unequip_flag = false;
                                }
                                else {
                                    logger::debug("Toggling unequip on");
                                    manager->EquipObject(player, Utility::unarmed_weapon);
                                    Utility::unequip_flag = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
} // namespace Events
