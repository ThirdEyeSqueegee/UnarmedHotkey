#include "Hooks.h"

#include "Utility.h"

namespace Hooks
{
    void Install() noexcept
    {
        stl::write_thunk_call<MainUpdate>();
        logger::info("Installed main update hook");
        logger::info("");
    }

    std::int32_t MainUpdate::Thunk() noexcept
    {
        const auto now{ std::chrono::steady_clock::now() };

        if (last_run != TTimePoint::min() && now - last_run <= 250ms) {
            return func();
        }

        if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
            if (player->GetGraphVariableBool("IsUnequipping", is_unequipping)) {
                if (is_unequipping) {
                    if (!started_unequip_anim) {
                        started_unequip_anim = true;
                    }
                }
                else {
                    if (started_unequip_anim) {
                        Utility::sem.release();
                        started_unequip_anim = false;
                    }
                }
            }
        }

        last_run = std::chrono::steady_clock::now();

        return func();
    }
} // namespace Hooks
