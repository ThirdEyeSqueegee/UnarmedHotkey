#pragma once

class Serialization : public Singleton<Serialization>
{
    static constexpr std::uint32_t serialization_id{ '7ESI' };

    static constexpr std::uint32_t serialization_type{ '7EST' };

    static constexpr std::uint32_t serialization_version{ 1 };

public:
    static void Init() noexcept;

    static void SaveCallback(SKSE::SerializationInterface* intfc) noexcept;

    static void LoadCallback(SKSE::SerializationInterface* intfc) noexcept;

    static void RevertCallback(SKSE::SerializationInterface* intfc) noexcept;
};
