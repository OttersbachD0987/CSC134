// CSC 134
// M3LAB1
// Daley Ottersbach
// 2/11/2026

#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <random>
#include <vector>

std::uniform_real_distribution<float> PERCENT_DISTRIBUTION = std::uniform_real_distribution<float>(0, 1);

struct EntityTemplate {
public:
    uint32_t hp;
    uint32_t mana;

    EntityTemplate(uint32_t a_hp, uint32_t a_mana) {
        this->hp = a_hp;
        this->mana = a_mana;
    }
};

struct EntityData {
public:
    uint32_t maxHP;
    uint32_t curHP;
    uint32_t maxMana;
    uint32_t curMana;

    EntityData(const EntityTemplate* const a_entityTemplate) {
        this->maxHP = this->curHP = a_entityTemplate->hp;
        this->maxMana = this->curMana = a_entityTemplate->mana;
    }
};

struct ItemData {
public:
    std::string name;
    std::string description;
    uint32_t maxStack;

    ItemData(std::string_view a_name, std::string_view a_description, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->maxStack = a_maxStack;
    }
};

struct ItemStack {
public:
    uint32_t itemID;
    uint32_t stackSize;
};

struct LootResult {
public:
    std::vector<ItemStack> results;

    LootResult(void) {
        this->results = std::vector<ItemStack>();
    }
};

class ILootEntry {
public:
    virtual LootResult Evaluate(std::mt19937& a_generator) = 0;
};

class ChanceLootEntry : public ILootEntry {
public:
    float chance;
    ItemStack item;

    ChanceLootEntry(float a_chance, ItemStack a_item) {
        this->chance = a_chance;
        this->item = a_item;
    }

    LootResult Evaluate(std::mt19937& a_generator) {
        LootResult result = LootResult();
        if (PERCENT_DISTRIBUTION(a_generator) <= this->chance) {
            result.results.emplace_back(this->item);
        }
        return result;
    } 
};

struct PlayerData : public EntityData {
public:
    PlayerData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {

    }
};

struct NPCData : public EntityData {
public:
    const ILootEntry* lootEntry;

    NPCData(const EntityTemplate* const a_entityTemplate, const ILootEntry* const a_lootEntry) : EntityData(a_entityTemplate) {
        this->lootEntry = a_lootEntry;
    }
};

struct RoomData {
public:
    std::string roomName;
    std::string roomDescription;
    
};

int main(int argc, char** argv) {
    #pragma region Random Setup
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    #pragma endregion

    const ILootEntry* LOOT_ENTRIES[] = {

    };

    const EntityTemplate ENTITY_TEMPLATES[] = {

    };

    return 0;
}