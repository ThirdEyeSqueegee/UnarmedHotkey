#include "Serialization.h"

#include "Utility.h"

void Serialization::Init() noexcept
{
    if (const auto intfc{ SKSE::GetSerializationInterface() }) {
        intfc->SetUniqueID(serialization_id);
        intfc->SetSaveCallback(SaveCallback);
        intfc->SetLoadCallback(LoadCallback);
        logger::info("Initialized SKSE co-save serialization");
        logger::info("");
    }
}

void Serialization::SaveCallback(SKSE::SerializationInterface* intfc) noexcept
{
    if (Utility::equip_cache.first) {
        if (intfc->OpenRecord(serialization_type, serialization_version)) {
            logger::debug("Opened co-save record");
            const auto right_form_id{ Utility::equip_cache.first->GetFormID() };
            if (intfc->WriteRecordData(right_form_id)) {
                logger::debug("\tWrote right hand equip cache form ID 0x{:x} to co-save", right_form_id);
            }
            else {
                logger::error("ERROR: Failed to write right hand equip cache form ID to co-save");
            }
            if (Utility::equip_cache.second) {
                const auto left_form_id{ Utility::equip_cache.second->GetFormID() };
                if (intfc->WriteRecordData(left_form_id)) {
                    logger::debug("\tWrote left hand equip cache form ID 0x{:x} to co-save", left_form_id);
                }
                else {
                    logger::error("ERROR: Failed to write left hand equip cache form ID to co-save");
                }
            }
        }
        else {
            logger::error("ERROR: Failed to open record for co-save serialization");
        }
    }
}

void Serialization::LoadCallback(SKSE::SerializationInterface* intfc) noexcept
{
    static std::uint32_t a_type{};
    static std::uint32_t a_version{};
    static std::uint32_t a_length{};

    intfc->GetNextRecordInfo(a_type, a_version, a_length);

    if (a_type != serialization_type || a_version != serialization_version) {
        return;
    }

    RE::FormID right_hand_form_id{};
    RE::FormID left_hand_form_id{};

    logger::debug("Reading co-save record");

    intfc->ReadRecordData(right_hand_form_id);
    if (!intfc->ResolveFormID(right_hand_form_id, right_hand_form_id)) {
        logger::error("ERROR: Failed to resolve right hand form ID");
    }
    else {
        logger::debug("\tResolved right hand form ID: 0x{:x}", right_hand_form_id);
        if (const auto bound_obj{ RE::TESForm::LookupByID<RE::TESBoundObject>(right_hand_form_id) }) {
            logger::debug("\tFound bound object: {} (0x{:x})", bound_obj->GetName(), bound_obj->GetFormID());
            Utility::equip_cache.first = bound_obj;
            logger::debug("\tSet right hand equip cache to {} (0x{:x})", Utility::equip_cache.first->GetName(), Utility::equip_cache.first->GetFormID());
        }
    }

    intfc->ReadRecordData(left_hand_form_id);

    if (left_hand_form_id == 0) {
        return;
    }

    if (!intfc->ResolveFormID(left_hand_form_id, left_hand_form_id)) {
        logger::error("ERROR: Failed to resolve left hand form ID");
    }
    else {
        logger::debug("\tResolved left hand form ID: 0x{:x}", left_hand_form_id);
        if (const auto bound_obj{ RE::TESForm::LookupByID<RE::TESBoundObject>(left_hand_form_id) }) {
            logger::debug("\tFound bound object: {} (0x{:x})", bound_obj->GetName(), bound_obj->GetFormID());
            Utility::equip_cache.second = bound_obj;
            logger::debug("\tSet left hand equip cache to {} (0x{:x})", Utility::equip_cache.second->GetName(), Utility::equip_cache.second->GetFormID());
        }
    }
}

void Serialization::RevertCallback(SKSE::SerializationInterface* intfc) noexcept
{
    Utility::equip_cache.first  = nullptr;
    Utility::equip_cache.second = nullptr;
}
