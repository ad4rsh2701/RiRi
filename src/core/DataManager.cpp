#include "src/include/DataManager.h"
#include "src/include/MemoryMaps.h"

using namespace RiRi;

bool Internal::setValue(std::string_view key, const RapidDataType& value) noexcept {
    return Internal::MemoryMap.insert({key, value}).second;
}