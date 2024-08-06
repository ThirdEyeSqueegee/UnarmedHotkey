#pragma once

namespace Hooks
{
    void Install() noexcept;

    class MainUpdate : public Singleton<MainUpdate>
    {
        inline static bool is_unequipping{};

        inline static bool started_unequip_anim{};

        using TTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

        inline static TTimePoint last_run{ TTimePoint::min() };

    public:
        static std::int32_t Thunk() noexcept;

        inline static REL::Relocation<decltype(&Thunk)> func;

        inline static const REL::Relocation target{ RELOCATION_ID(35565, 36564), REL::Relocate(0x748, 0xc26, 0x7ee) };

        inline static const auto address{ target.address() };
    };
} // namespace Hooks
