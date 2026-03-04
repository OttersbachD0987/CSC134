// CSC 134
// M3LAB1
// Daley Ottersbach
// 2/11/2026

#define __STDC_WANT_LIB_EXT1__ 1
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <random>
#include <vector>
#include <functional>
#include <array>
#include <bitset>
#include <unordered_map>
#include <typeinfo>
#include <algorithm>
#include <concepts>
#include <fstream>
#include <filesystem>
#include <optional>

#pragma region Input Utilities
template<std::integral T>
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

template<typename T>
requires(!std::integral<T>)
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
}

inline void EatInput() {
    std::cout << "Press any key to continue..." << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cout << "\x1B[F\x1B[0K\r" << std::flush;
}
#pragma endregion

#pragma region Random Distributions
std::uniform_real_distribution<float> PERCENT_DISTRIBUTION = std::uniform_real_distribution<float>(0, 1);

std::uniform_int_distribution<uint32_t> DIE_TWO_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 2);
std::uniform_int_distribution<uint32_t> DIE_THREE_DISTRIBUTION   = std::uniform_int_distribution<uint32_t>(1, 3);
std::uniform_int_distribution<uint32_t> DIE_FOUR_DISTRIBUTION    = std::uniform_int_distribution<uint32_t>(1, 4);
std::uniform_int_distribution<uint32_t> DIE_SIX_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 6);
std::uniform_int_distribution<uint32_t> DIE_EIGHT_DISTRIBUTION   = std::uniform_int_distribution<uint32_t>(1, 8);
std::uniform_int_distribution<uint32_t> DIE_TEN_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 10);
std::uniform_int_distribution<uint32_t> DIE_TWELVE_DISTRIBUTION  = std::uniform_int_distribution<uint32_t>(1, 12);
std::uniform_int_distribution<uint32_t> DIE_SIXTEEN_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 16);
std::uniform_int_distribution<uint32_t> DIE_TWENTY_DISTRIBUTION  = std::uniform_int_distribution<uint32_t>(1, 20);
std::uniform_int_distribution<uint32_t> DIE_HUNDRED_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 100);

std::unordered_map<uint32_t, std::uniform_int_distribution<uint32_t>&> DICE = {
    {  2, DIE_TWO_DISTRIBUTION    },
    {  3, DIE_THREE_DISTRIBUTION  },
    {  4, DIE_FOUR_DISTRIBUTION   },
    {  6, DIE_SIX_DISTRIBUTION    },
    {  8, DIE_EIGHT_DISTRIBUTION  },
    { 10, DIE_TEN_DISTRIBUTION    },
    { 12, DIE_TWELVE_DISTRIBUTION },
    { 16, DIE_SIXTEEN_DISTRIBUTION},
    { 20, DIE_TWENTY_DISTRIBUTION },
    {100, DIE_HUNDRED_DISTRIBUTION},
};
#pragma endregion

/*
template<typename K, typename V>
class DumbMap {
public:
    std::vector<K> keys;
    std::vector<V> values;

    DumbMap(void) {
        this->keys   = std::vector<K>();
        this->values = std::vector<V>();
    }

    bool HasKey(const K& a_key) requires Eq<K> {
        return std::find(keys.begin(), keys.end(), a_key) != keys.end();
    }

    bool HasValue(const V& a_value) requires Eq<V> {
        return std::find(values.begin(), values.end(), a_value) != values.end();
    }

    void Set(const K& a_key, const V& a_value) {
        for (size_t i = 0; i < keys.size(); ++i) {
            if (keys[i] == a_key) {
                values[i] = a_value;
                return;
            }
        }
        
        keys.push_back(a_key);
        values.push_back(a_value);
    }

    V& Get(const K& a_key, V& a_default) {
        for (size_t i = 0; i < keys.size(); ++i) {
            if (keys[i] == a_key) {
                return values[i];
            }
        }

        return a_default;
    }
};
*/

struct EntityData;
struct GameState;
struct ItemData;
struct ItemStack;
struct Class;
struct NPCData;
struct PlayerData;
struct RoomInstance;
struct ConnectionInstance;
struct ShopInstance;

/// @struct Action
/// @brief Holds the data for an action.
struct Action {
public:
    std::string name;                                                              //< The name of the action.
    std::function<bool(GameState&, EntityData*, EntityData*)> condition = nullptr; //< The condition of the action, if nullptr, then it has no condition.
    std::function<void(GameState&, EntityData*, EntityData*)> action = nullptr;    //< The action to execute, don't leave this null.

    bool operator==(const Action& other) {
        return this->name == other.name;
    }
};

/// @struct EntityTemplate
/// @brief A template for a base entity.
struct EntityTemplate {
public:
    int32_t hp;                                      //< The max HP of the entity.
    int32_t mana;                                    //< The max mana of the entity.
    int32_t manaRegen;                               //< The mana regeneration of the entity.
    int32_t armor;                                   //< The armor of the entity.
    std::unordered_map<std::string, int32_t> skills; //< The skills of the entity.
    std::vector<Action> actions;                     //< The actions of the entity.

    /// @brief An entity template with all its values set to the default.
    EntityTemplate(void) {
        this->hp = this->mana = this->manaRegen = this->armor = 0;
        this->skills = std::unordered_map<std::string, int32_t>();
        this->actions = std::vector<Action>();
    }

    /// @brief An entity template with assigned values.
    ///
    /// @param a_hp        The word max HP of the entity.
    /// @param a_mana      The max mana of the entity.
    /// @param a_manaRegen The mana regeneration of the entity.
    /// @param a_armor     The armor of the entity.
    /// @param a_skills    The skills of the entity.
    /// @param a_actions   The actions of the entity.
    EntityTemplate(int32_t a_hp, int32_t a_mana, int32_t a_manaRegen, int32_t a_armor, const std::unordered_map<std::string, int32_t>& a_skills, const std::vector<Action>& a_actions) {
        this->hp        = a_hp;
        this->mana      = a_mana;
        this->manaRegen = a_manaRegen;
        this->armor     = a_armor;
        this->skills    = a_skills;
        this->actions   = a_actions;
    }
};

struct TimedEffect {
public:
    int32_t timeLeft;
    std::function<void(GameState&, EntityData&)> onTick = nullptr;
    std::function<void(GameState&, EntityData&)> onRemove = nullptr;
};

enum struct DamageType {

};

namespace {
    extern const std::unordered_map<std::string, Class> CLASSES;
    extern const std::unordered_map<std::string, TimedEffect> STANDARD_TIMED_EFFECTS;
    extern const ItemData ITEM_DATA[];
}

struct EntityData {
public:
    int32_t  maxHP;                                  //< The max HP of the entity.
    int32_t  curHP;                                  //< The current HP of the entity.
    uint32_t maxMana;                                //< The max mana of the entity.
    uint32_t curMana;                                //< The current mana of the entity.
    int32_t  manaRegen;                              //< The mana regeneration of the entity.
    int32_t  manaSickness;                           //< How long until the entity can regain mana.
    int32_t  armor;                                  //< The armor of the entity.
    std::unordered_map<std::string, int32_t> skills; //< The skills of the entity.
    std::vector<Action> actions;                     //< The actions of the entity.
    std::unordered_map<std::string, TimedEffect> timedEffects = std::unordered_map<std::string, TimedEffect>();

    EntityData(void) {
        this->maxHP = this->curHP = this->maxMana = this->curMana = this->manaRegen = this->manaSickness = this->armor = 0;
        this->skills = std::unordered_map<std::string, int32_t>();
        this->actions = std::vector<Action>();
    }

    EntityData(const EntityTemplate* const a_entityTemplate) {
        this->maxHP = this->curHP = a_entityTemplate->hp;
        this->maxMana = this->curMana = a_entityTemplate->mana;
        this->manaRegen = a_entityTemplate->manaRegen;
        this->manaSickness = 0;
        this->armor = a_entityTemplate->armor;
        this->skills = a_entityTemplate->skills;
        this->actions = a_entityTemplate->actions;
    }

    virtual int32_t GetEffectiveArmor(void) = 0;

    virtual void Hurt(int32_t) = 0;
    virtual void Heal(int32_t) = 0;

    virtual void DrainMana(int32_t a_amount) = 0;
    virtual void RegainMana(int32_t a_amount) = 0;

    int32_t GetSkillModifier(const std::string& a_skill) {
        return skills.find(a_skill) != skills.end() ? skills.at(a_skill) : 0;
    }

    int32_t SetSkillModifier(const std::string& a_skill, int32_t a_modifier) {
        skills.insert_or_assign(a_skill, a_modifier);
        return skills.at(a_skill);
    }

    int32_t AddSkillModifier(const std::string& a_skill, int32_t a_modifier) {
        skills.insert_or_assign(a_skill, GetSkillModifier(a_skill) + a_modifier);
        return skills.at(a_skill);
    }

    void TurnEnd(GameState& a_gameState) {
        if (manaSickness <= 0) {
            if (manaRegen > 0) {
                RegainMana(manaRegen);
            } else if (manaRegen < 0) {
                DrainMana(-manaRegen);
            }
        } else {
            --manaSickness;
        }

        std::vector<std::string> toRemove = std::vector<std::string>();

        for (std::pair<std::string, TimedEffect> effectPair : timedEffects) {
            if (--effectPair.second.timeLeft <= 0) {
                toRemove.push_back(effectPair.first);
                if (effectPair.second.onRemove != nullptr) {
                    effectPair.second.onRemove(a_gameState, *this);
                }
            } else {
                if (effectPair.second.onTick != nullptr) {
                    effectPair.second.onTick(a_gameState, *this);
                }
            }
        }

        for (std::string key : toRemove) {
            timedEffects.erase(key);
        }
    }

    virtual ~EntityData() {}
};

struct ItemUsage {
public:
    std::string name = "";
    std::function<bool(GameState&, const ItemStack&, size_t)> condition = nullptr;
    std::function<void(GameState&, const ItemStack&, size_t)> usage = nullptr;
};

struct ItemData {
public:
    std::string name;
    std::string description;
    std::unordered_map<std::string, int32_t> tags;
    ItemUsage usage;
    uint32_t maxStack;

    ItemData(std::string_view a_name, std::string_view a_description, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->tags = std::unordered_map<std::string, int32_t>();
        this->usage = ItemUsage();
        this->maxStack = a_maxStack;
    }

    ItemData(std::string_view a_name, std::string_view a_description, const ItemUsage& a_usage, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->tags = std::unordered_map<std::string, int32_t>();
        this->usage = a_usage;
        this->maxStack = a_maxStack;
    }

    ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, int32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->tags = a_tags;
        this->usage = ItemUsage();
        this->maxStack = a_maxStack;
    }

    ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, const ItemUsage& a_usage, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->tags = a_tags;
        this->usage = a_usage;
        this->maxStack = a_maxStack;
    }
};

enum struct ItemStackDeserializationMode {
    COLLECTING_ID,
    COLLECTING_STACK,
    COLLECTING_METAKEY,
    COLLECTING_METAVALUE,
    FINISHED
};

struct ItemStack {
public:
    size_t itemID;
    uint32_t stackSize;
    std::unordered_map<std::string, int32_t> metadata;

    ItemStack(void) {
        this->itemID = 0;
        this->stackSize = 0;
        this->metadata = std::unordered_map<std::string, int32_t>();
    }

    ItemStack(size_t a_itemID, uint32_t a_stackSize) {
        this->itemID = a_itemID;
        this->stackSize = a_stackSize;
        this->metadata = std::unordered_map<std::string, int32_t>();
    }

    ItemStack(size_t a_itemID, uint32_t a_stackSize, const std::unordered_map<std::string, int32_t>& a_metadata) {
        this->itemID = a_itemID;
        this->stackSize = a_stackSize;
        this->metadata = a_metadata;
    }

    int32_t GetMetadata(const std::string& a_key, int32_t a_default = 0) {
        return metadata.contains(a_key) ? metadata.at(a_key) : a_default;
    }

    static ItemStack FromString(std::string_view a_serialized) {
        ItemStack itemStack;
        std::string collector = "";
        std::string stringStorage = "";
        int32_t intStorage = 0;
        ItemStackDeserializationMode mode = ItemStackDeserializationMode::COLLECTING_ID;
        for (char character : a_serialized) {
            switch (mode) {
                case ItemStackDeserializationMode::COLLECTING_ID:
                    switch (character) {
                        case 'x':
                            intStorage = std::stoi(collector);
                            collector = "";
                            mode = ItemStackDeserializationMode::COLLECTING_STACK;
                            break;
                        default:
                            collector += character;
                    }
                    break;
                case ItemStackDeserializationMode::COLLECTING_STACK:
                    switch (character) {
                        case '(':
                            itemStack = ItemStack(intStorage, std::stoi(collector));
                            collector = "";
                            mode = ItemStackDeserializationMode::COLLECTING_METAKEY;
                            break;
                        case ';':
                            itemStack = ItemStack(intStorage, std::stoi(collector));
                            collector = "";
                            mode = ItemStackDeserializationMode::FINISHED;
                            break;
                        default:
                            collector += character;
                    }
                    break;
                case ItemStackDeserializationMode::COLLECTING_METAKEY:
                    switch (character) {
                        case ':':
                            stringStorage = collector;
                            collector = "";
                            mode = ItemStackDeserializationMode::COLLECTING_METAVALUE;
                            break;
                        default:
                            collector += character;
                    }
                    break;
                case ItemStackDeserializationMode::COLLECTING_METAVALUE:
                    switch (character) {
                        case ';':
                            itemStack.metadata.emplace(stringStorage, std::stoi(collector));
                            collector = "";
                            mode = ItemStackDeserializationMode::COLLECTING_METAKEY;
                            break;
                        case ')':
                            itemStack.metadata.emplace(stringStorage, std::stoi(collector));
                            collector = "";
                            mode = ItemStackDeserializationMode::FINISHED;
                            break;
                        default:
                            collector += character;
                    }
                    break;
            }
        }

        return itemStack;
    }
};

enum class BladeType {
    DAGGER,
    SHORT_SWORD,
    LONG_SWORD
};

enum class Perks {
    INSIGHT,
    ARCANE_EYES,
    HARD_HITTER,
    HORDE_SLAYER,
    SIZE
};

struct ClassLevel {
public:
    std::function<void(GameState&, PlayerData&)> displayRequirements;
    std::function<bool(GameState&, PlayerData&)> applicable;
    std::vector<std::function<void(GameState&, PlayerData&, bool)>> levelUpEffects;
};

struct Class {
public:
    std::string name;
    std::string description;
    std::vector<ClassLevel> levels;

    uint32_t MaxLevel(void) {
        return levels.size();
    }
};

struct ClassInstance {
public:
    std::string classType;
    uint32_t level;

    ClassInstance(void) {
        this->classType = "";
        this->level = 0;
    }

    ClassInstance(std::string_view a_class, uint32_t a_level) {
        this->classType = a_class;
        this->level = a_level;
    }
};

struct PlayerData : public EntityData {
public:
    std::unordered_map<std::string, ClassInstance> classes;
    std::bitset<static_cast<size_t>(Perks::SIZE)> perks;
    std::vector<ItemStack> items;
    std::vector<bool> equipped;
    int32_t turns = 1;
    int32_t usedTurns = 0;
    int32_t gold = 0;
    int32_t xp = 0;

    PlayerData(void) : EntityData() {
        this->classes = std::unordered_map<std::string, ClassInstance>();
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        this->items = std::vector<ItemStack>();
        this->equipped = std::vector<bool>();
        this->gold = 0;
        this->xp = 0;
    }

    PlayerData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {
        this->classes = std::unordered_map<std::string, ClassInstance>();
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        this->items = std::vector<ItemStack>();
        this->equipped = std::vector<bool>();
        this->gold = 0;
        this->xp = 0;
    }

    PlayerData(const EntityTemplate* const a_entityTemplate, const std::vector<size_t>& a_perks, size_t a_turns) : EntityData(a_entityTemplate) {
        this->classes = std::unordered_map<std::string, ClassInstance>();
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        for (const size_t& perk : a_perks) {
            this->perks.set(perk);
        }
        this->items = std::vector<ItemStack>();
        this->equipped = std::vector<bool>();
        this->turns = a_turns;
        this->gold = 0;
        this->xp = 0;
    }

    #pragma region Inventory
    void UseItemAt(size_t a_index) {
        if (--items[a_index].stackSize <= 0) {
            items.erase(items.begin() + a_index);
            equipped.erase(equipped.begin() + a_index);
        }
    }

    void UseItemOf(size_t a_itemID) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].itemID == a_itemID) {
                UseItemAt(i);
                return;
            }
        }
    }

    void AddItem(size_t a_itemID, uint32_t a_itemAmount) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].itemID == a_itemID) {
                items[i].stackSize += a_itemAmount;
                return;
            }
        }
        items.push_back(ItemStack(a_itemID, a_itemAmount));
        equipped.push_back(false);
    }

    bool HasItem(size_t a_itemID, uint32_t a_itemAmount) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].itemID == a_itemID) {
                return a_itemAmount <= items[i].stackSize;
            }
        }
        return false;
    }

    std::optional<size_t> HasMatchingEquippedItem(GameState& a_gameState, const std::function<bool(GameState&, const ItemStack&)>& a_predicate) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (equipped[i] && a_predicate(a_gameState, items[i])) {
                return std::optional<size_t>(i);
            }
        }
        return std::optional<size_t>();
    }
    #pragma endregion

    int32_t LevelInClass(const std::string& a_class) {
        return classes.contains(a_class) ? classes.at(a_class).level + 1 : 0;
    }

    int32_t GetEffectiveArmor(void) {
        int32_t effectiveArmor = armor;
        for (size_t i = 0; i < items.size(); ++i) {
            if (equipped[i]) {
                effectiveArmor += items[i].GetMetadata("ToArmor");
                if (ITEM_DATA[items[i].itemID].tags.contains("Armor")) {
                    effectiveArmor += ITEM_DATA[items[i].itemID].tags.at("Armor");
                }
            }
        }
        return effectiveArmor;
    }

    void Hurt(int32_t a_damage) {
        curHP = std::max<int32_t>(curHP - a_damage, 0);
        if (curHP <= 0) {
            // Do stuff when the player dies. Can add things like revives or something. Probably going to add a signal to it.
        }
    }

    void Heal(int32_t a_amount) {
        curHP = std::min<int32_t>(curHP + a_amount, maxHP);
    }

    void DrainMana(int32_t a_amount) {
        curMana = std::max<int32_t>(curMana - a_amount, 0);
        if (curMana <= 0) {
            // Do stuff when the player runs out of mana. Probably going to add a signal to it.
        }
    }

    void RegainMana(int32_t a_amount) {
        curMana = std::min<int32_t>(curMana + a_amount, maxMana);
    }
};

struct NPCTemplate {
public:
    std::string name = "";
    int32_t xp = 0;
    int32_t gold = 0;
    std::function<void(GameState&, NPCData&)> ai = nullptr;
};

struct NPCData : public EntityData {
public:
    std::string name;
    int32_t xp;
    int32_t gold;
    const std::function<void(GameState&, NPCData&)>* aiFunction;
    std::vector<std::function<void(GameState&, NPCData&)>> onDeath = std::vector<std::function<void(GameState&, NPCData&)>>();
    bool stunned = false;

    NPCData(void) : EntityData() {
        this->name = "Unknown";
        this->xp = 0;
        this->gold = 0;
        this->aiFunction = nullptr;
    }

    NPCData(const EntityTemplate* const a_entityTemplate, const NPCTemplate* const a_npcTemplate) : EntityData(a_entityTemplate) {
        this->name = a_npcTemplate->name;
        this->xp = a_npcTemplate->xp;
        this->gold = a_npcTemplate->gold;
        this->aiFunction = &a_npcTemplate->ai;
    }

    int32_t GetEffectiveArmor(void) {
        return armor;
    }

    void Hurt(int32_t a_damage) {
        curHP = std::max<int32_t>(curHP - a_damage, 0);
        if (curHP <= 0) {
            // Do stuff when the NPC dies. Adding a signal for this most definitely.
        }
    }

    void Heal(int32_t a_amount) {
        curHP = std::min<int32_t>(curHP + a_amount, maxHP);
    }

    void DrainMana(int32_t a_amount) {
        curMana = std::max<int32_t>(curMana - a_amount, 0);
        if (curMana <= 0) {
            // Do stuff when the NPC runs out of mana. Probably going to add a signal to it.
        }
    }

    void RegainMana(int32_t a_amount) {
        curMana = std::min<int32_t>(curMana + a_amount, maxMana);
    }
};

struct ConnectionTest {
public:
    std::function<bool(GameState&, RoomInstance&)>       condition = nullptr;
    std::function<void(GameState&, RoomInstance&, bool)> conditionDisplay = nullptr;
    std::function<void(GameState&, RoomInstance&)>       action = nullptr;
};

struct RoomData {
public:
    std::string roomName;
    std::string roomDescription;
    std::function<uint32_t(GameState&, const RoomData&)> weight = nullptr;
    bool backlink;
    uint32_t minExisting;
    uint32_t maxExisting;
    uint32_t minNew;
    uint32_t maxNew;
    ConnectionTest toConnection;
    ConnectionTest fromConnection;

    RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew) {
        this->roomName = a_name;
        this->roomDescription = a_description;
        this->weight = a_weight;
        this->backlink = a_backlink;
        this->minExisting = a_minExisting;
        this->maxExisting = a_maxExisting;
        this->minNew = a_minNew;
        this->maxNew = a_maxNew;
        this->toConnection = ConnectionTest();
        this->fromConnection = ConnectionTest();
    }

    RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew, ConnectionTest a_to, ConnectionTest a_from) {
        this->roomName = a_name;
        this->roomDescription = a_description;
        this->weight = a_weight;
        this->backlink = a_backlink;
        this->minExisting = a_minExisting;
        this->maxExisting = a_maxExisting;
        this->minNew = a_minNew;
        this->maxNew = a_maxNew;
        this->toConnection = a_to;
        this->fromConnection = a_from;
    }
};

struct RoomAction {
public:
    std::string name        = "";
    std::string description = "";
    std::function<void(GameState&)> roomDescription = nullptr;
    std::function<bool(GameState&)> condition       = nullptr;
    std::function<void(GameState&)> usage           = nullptr;
};

struct RoomActionReference {
public:
    RoomAction roomAction;
};

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<ConnectionInstance> connections;
    std::vector<NPCData> inhabitants;
    std::vector<RoomActionReference> roomActions;
    std::unordered_map<std::string, int32_t> flags;
    std::unordered_map<std::string, void*> data;

    RoomInstance(size_t a_roomID, size_t a_roomIndex) {
        this->roomID = a_roomID;
        this->roomIndex = a_roomIndex;
        this->connections = std::vector<ConnectionInstance>();
        this->inhabitants = std::vector<NPCData>();
        this->roomActions = std::vector<RoomActionReference>();
        this->flags = std::unordered_map<std::string, int32_t>();
        this->data = std::unordered_map<std::string, void*>();
    }

    size_t LivingInhabitants(void) {
        size_t toReturn = 0;
        for (const NPCData& npc : inhabitants) {
            if (npc.curHP > 0) {
                ++toReturn;
            }
        }
        return toReturn;
    }
};

#pragma region Screen and Menu
enum class Screen {
    TITLE,
    GAME_SELECT,
    GAME,
    GAME_OVER,
};

enum class Menu {
    PAUSE,
    MOVING,
    ROOM_ACTIONS,
    COMBAT,
    STATS,
    INVENTORY,
    LEVEL_UP,
    NONE
};
#pragma endregion

struct StartData {
public:
    std::string name;
    std::string description;
    EntityTemplate playerEntity;
    std::unordered_map<std::string, ClassInstance> classes;
    std::vector<size_t> perks;
    std::vector<ItemStack> inventory;
    size_t turns;
    size_t startRoom;
    size_t xp;
    size_t gold;

    /// @brief Brief
    StartData(void) {
        this->name = "";
        this->description = "";
        this->playerEntity = EntityTemplate();
        this->classes = std::unordered_map<std::string, ClassInstance>();
        this->perks = std::vector<size_t>();
        this->inventory = std::vector<ItemStack>();
        this->turns = 1;
        this->startRoom = -1;
        this->xp = 0;
        this->gold = 0;
    }

    /// @brief Brief
    /// @param a_name a
    /// @param a_description b
    /// @param a_playerEntity c
    /// @param a_classes d
    /// @param a_perks e
    /// @param a_inventory f
    /// @param a_turns g
    /// @param a_startRoom h
    /// @param a_xp
    /// @param a_gold
    StartData(std::string_view a_name, std::string_view a_description, const EntityTemplate& a_playerEntity, const std::unordered_map<std::string, ClassInstance>& a_classes, const std::vector<size_t>& a_perks, const std::vector<ItemStack>& a_inventory, size_t a_turns, size_t a_startRoom, size_t a_xp, size_t a_gold) {
        this->name = a_name;
        this->description = a_description;
        this->playerEntity = a_playerEntity;
        this->classes = a_classes;
        this->perks = a_perks;
        this->inventory = a_inventory;
        this->turns = a_turns;
        this->startRoom = a_startRoom;
        this->xp = a_xp;
        this->gold = a_gold;
    }
};

struct GameState {
public:
    static std::vector<StartData> Starts;

    bool debugMode = false;

    bool running = true;
    Screen screen;
    Menu menu;

    size_t usedRooms;
    size_t curRoom;
    size_t start;
    std::vector<RoomInstance> rooms;
    PlayerData player;

    std::mt19937 generator;

    GameState(std::mt19937 a_generator) {
        this->generator = a_generator;
        this->screen = Screen::TITLE;
        this->menu = Menu::NONE;
        this->usedRooms = 0;
        this->curRoom = 0;
        this->rooms = std::vector<RoomInstance>();
        EntityTemplate temp = EntityTemplate(100, 30, 5, 10, {}, {
            Action {
                "Example",
                [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { return true; },
                [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {}
            }
        });
        this->player = PlayerData(&temp);
    }

    static void DisplayStarts() {
        for (size_t i = 0; i < GameState::Starts.size(); ++i) {
            std::cout << (i + 1) << ") " << GameState::Starts[i].name << "\n- " << GameState::Starts[i].description << "\n" << std::endl;
        }
    }

    void Start(size_t a_start) {
        this->start = a_start;
        this->usedRooms = 0;
        this->curRoom = 0;
        this->rooms.clear();
        this->player = PlayerData(&GameState::Starts[a_start].playerEntity, GameState::Starts[a_start].perks, GameState::Starts[a_start].turns);
        for (const std::pair<std::string, ClassInstance>& classPair : GameState::Starts[a_start].classes) {
            for (size_t level = 0; level < classPair.second.level; ++level) {
                for (size_t levelUpEffect = 0; levelUpEffect < CLASSES.at(classPair.second.classType).levels[level].levelUpEffects.size(); ++levelUpEffect) {
                    CLASSES.at(classPair.second.classType).levels[level].levelUpEffects[levelUpEffect](*this, player, true);
                }
            }
            this->player.classes.insert(std::pair<std::string, ClassInstance>(classPair.first, classPair.second));
        }
        this->player.xp = GameState::Starts[a_start].xp;
        this->player.gold = GameState::Starts[a_start].gold;
        this->player.items = GameState::Starts[a_start].inventory;
        this->player.equipped = std::vector<bool>(player.items.size());
        this->PushBackRoom(GameState::Starts[a_start].startRoom);
    }

    size_t PushBackRoom(size_t a_index) {
        this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
        return this->usedRooms++;
    }
};

struct Encounter {
public:
    std::function<uint32_t(GameState&)> weight = nullptr;
    std::function<void(GameState&)> event = nullptr;
};

const std::unordered_map<std::string, Action> STANDARD_ACTIONS = {
    {
        "Punch",  Action {
            "Punch",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player throws a punch ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Martial Combat") +
                    a_caster->GetSkillModifier("Unarmed Combat") +
                    a_caster->GetSkillModifier("Brawling");
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                if (opponents > 1) {
                    modifier += a_caster->GetSkillModifier("Melee Combat");
                }
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = modifier * 0.5;
                    int32_t damageTier = 1;
                    PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                    if (player != nullptr) {
                        if (player->perks.test((size_t)Perks::HARD_HITTER)) {
                            damageTier += 1;
                        }
                    }

                    if (modifier >= 12) {
                        damageTier += 2;
                    } else if (modifier >= 8) {
                        damageTier += 1;
                    } else if (modifier <= -4) {
                        damageTier -= 1;
                    } else if (modifier <= -8) {
                        damageTier -= 2;
                    }

                    switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                        case 0:
                            break;
                        case 1:
                            damage = DIE_TWELVE_DISTRIBUTION(a_gameState.generator);
                            break;
                        case 2:
                            damage = DIE_SIX_DISTRIBUTION(a_gameState.generator) + DIE_SIX_DISTRIBUTION(a_gameState.generator);
                            break;
                        case 3:
                            damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                            break;
                        case 4:
                            damage = DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator);
                            break;
                    }

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and hits with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage; 
                    switch (damageTier) {
                        case 0:
                            break;
                        case 1:
                            std::cout << " (1d12)";
                            break;
                        case 2:
                            std::cout << " (2d6)";
                            break;
                        case 3:
                            std::cout << " (3d4)";
                            break;
                        case 4:
                            std::cout << " (4d3)";
                            break;
                    }
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }
            }
        }
    },
    {
        "Swift Strike",  Action {
            "Swift Strike",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_gameState.player.HasMatchingEquippedItem(a_gameState, [](GameState& a_gameState, const ItemStack& a_itemStack) { return ITEM_DATA[a_itemStack.itemID].tags.contains("Blade"); }).has_value(); 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                ItemStack& itemStack = a_gameState.player.items[a_gameState.player.HasMatchingEquippedItem(a_gameState, [](GameState& a_gameState, const ItemStack& a_itemStack) { return ITEM_DATA[a_itemStack.itemID].tags.contains("Blade"); }).value()];
                const ItemData& itemType = ITEM_DATA[itemStack.itemID];
                BladeType bladeType = (BladeType)itemType.tags.at("Blade");
                std::cout << "The player swings their " << itemType.name << " ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Martial Combat") +
                    itemStack.GetMetadata("ToHit", 0);
                switch (bladeType) {
                    case BladeType::SHORT_SWORD:
                    case BladeType::LONG_SWORD:
                        modifier += a_caster->GetSkillModifier("Swordsmanship");
                        break;
                }
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                if (opponents == 1) {
                    modifier += a_caster->GetSkillModifier("Dueling");
                } else {
                    modifier += a_caster->GetSkillModifier("Melee Combat");
                }
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                NPCData* npc = dynamic_cast<NPCData*>(a_target);
                if (npc != nullptr) {
                    if (npc->stunned) {
                        roll = std::max<int32_t>(roll, DIE_TWENTY_DISTRIBUTION(a_gameState.generator));
                    }
                }
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = modifier * 0.5;
                    int32_t damageTier = 1;
                    PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                    if (player != nullptr) {
                        if (opponents > 3 && player->perks.test((size_t)Perks::HORDE_SLAYER)) {
                            damageTier += 1;
                        }
                    }

                    if (modifier >= 12) {
                        damageTier += 3;
                    } else if (modifier >= 9) {
                        damageTier += 2;
                    } else if (modifier >= 6) {
                        damageTier += 1;
                    } else if (modifier <= -4) {
                        damageTier -= 1;
                    } else if (modifier <= -8) {
                        damageTier -= 2;
                    } else if (modifier <= -12) {
                        damageTier -= 3;
                    }

                    switch (bladeType) {
                        case BladeType::DAGGER:
                            switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                                case 0:
                                    damage = DIE_EIGHT_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 1:
                                    damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 2:
                                    damage = 2 * DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 3:
                                    damage = DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 4:
                                    damage = 3 * DIE_THREE_DISTRIBUTION(a_gameState.generator);
                                    break;
                            }
                            break;
                        case BladeType::SHORT_SWORD:
                        case BladeType::LONG_SWORD:
                            switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                                case 0:
                                    damage = DIE_TWELVE_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 1:
                                    damage = DIE_SIXTEEN_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 2:
                                    damage = DIE_EIGHT_DISTRIBUTION(a_gameState.generator) + DIE_EIGHT_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 3:
                                    damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                                    break;
                                case 4:
                                    damage = DIE_SIX_DISTRIBUTION(a_gameState.generator) + DIE_SIX_DISTRIBUTION(a_gameState.generator) + DIE_SIX_DISTRIBUTION(a_gameState.generator) + DIE_SIX_DISTRIBUTION(a_gameState.generator);
                                    break;
                            }
                            break;
                    }

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and hits with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage;
                    
                    switch (bladeType) {
                        case BladeType::DAGGER:
                            switch (damageTier) {
                                case 0:
                                    std::cout << " (1d8)";
                                    break;
                                case 1:
                                    std::cout << " (2d4)";
                                    break;
                                case 2:
                                    std::cout << " (1d4 * 2)";
                                    break;
                                case 3:
                                    std::cout << " (3d3)";
                                    break;
                                case 4:
                                    std::cout << " (1d3 * 3)";
                                    break;
                            }
                            break;
                        case BladeType::SHORT_SWORD:
                        case BladeType::LONG_SWORD:
                            switch (damageTier) {
                                case 0:
                                    std::cout << " (1d12)";
                                    break;
                                case 1:
                                    std::cout << " (1d16)";
                                    break;
                                case 2:
                                    std::cout << " (2d8)";
                                    break;
                                case 3:
                                    std::cout << " (4d4)";
                                    break;
                                case 4:
                                    std::cout << " (4d6)";
                                    break;
                            }
                            break;
                    }
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                if (a_gameState.rooms[a_gameState.curRoom].LivingInhabitants() <= 1 || a_target->curHP > 0 || DIE_THREE_DISTRIBUTION(a_gameState.generator) != 1) {
                    PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                    if (player != nullptr) {
                        ++player->usedTurns;
                    }
                }
            }
        }
    },
    {
        "Call of the Void",  Action {
            "Call of the Void",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_caster->curMana > 15; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player murmers a cursed incantation ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                    if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                        modifier += ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                    }
                }
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = modifier + opponents;

                    damage += DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_SIX_DISTRIBUTION(a_gameState.generator);

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and a torrent of whispers comes forth from the ether with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage << " (2d4 + 1d6)"; 
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                    EatInput();

                    for (size_t i = 0; i < a_gameState.rooms[a_gameState.curRoom].inhabitants.size(); ++i) {
                        if (a_gameState.rooms[a_gameState.curRoom].inhabitants[i].curHP > 0) {
                            std::cout << "The " << a_gameState.rooms[a_gameState.curRoom].inhabitants[i].name << " twitches, and then ";
                            switch (DIE_THREE_DISTRIBUTION(a_gameState.generator)) {
                                case 1:
                                    a_gameState.rooms[a_gameState.curRoom].inhabitants[i].Hurt(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].curHP * 0.5);
                                    std::cout << "experiences a violent shudder, their muscles contracting and flailing with seemingly unatural origin." << std::endl;
                                    break;
                                case 2:
                                case 3:
                                    std::cout << "locks up completely, color itself draining from the area around them." << std::endl;
                                    a_gameState.rooms[a_gameState.curRoom].inhabitants[i].stunned = true;
                                    break;
                            }
                        } else {
                            std::cout << "The corpse of the " << a_gameState.rooms[a_gameState.curRoom].inhabitants[i].name << " twitches, and the player feels a surge of ";
                            if (a_caster->curHP < a_caster->maxHP) {
                                std::cout << "life rush into them." << std::endl;
                                a_caster->Heal(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].maxHP * 0.15);
                            } else if (a_caster->curMana < a_caster->maxMana) {
                                std::cout << "mana rush into them." << std::endl;
                                a_caster->RegainMana(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].maxHP * 0.15);
                            }
                        }

                        EatInput();
                    }
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                    EatInput();
                }

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(15);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Mark for Death",  Action {
            "Mark for Death",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_caster->curMana > 20; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player gestures a wicked glyph in the air ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                    if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                        modifier += ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                    }
                }
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    NPCData* npc = dynamic_cast<NPCData*>(a_target);
                    if (npc != nullptr) {
                        std::cout << "and a dull thud rings out as the color drains away from the " << npc->name << "." << std::endl;
                        npc->onDeath.push_back(
                            [](GameState& a_gameState, NPCData& a_npc) {
                                a_gameState.player.xp += a_npc.curMana;
                                a_gameState.player.RegainMana(a_npc.xp);
                                std::cout << "The corpse of the " << a_npc.name << " violently shudders before a strange burst of mana rips away and surges into the player." << std::endl;
                            }
                        );
                    }
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(20);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Bone Shield",  Action {
            "Bone Shield",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target->curHP > 0 && a_caster->curMana > 10 && !a_target->timedEffects.contains("Bone Shield."); 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player raises their hands, and forward burst bones, creating a protective carapace around ";
                
                a_target->armor += 2;
                a_target->timedEffects.emplace("Bone Shield", STANDARD_TIMED_EFFECTS.at("Bone Shield"));

                if (a_caster == a_target) {
                    std::cout << "themselves." << std::endl;
                } else {
                    NPCData* npc = dynamic_cast<NPCData*>(a_target);
                    if (npc != nullptr) {
                        std::cout << "the " << npc->name << "." << std::endl;
                    }
                }

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(10);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Garden of Rot",  Action {
            "Garden of Rot",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_caster->curMana > 45; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player crushes a heart, and from the ground a flower bursts from the center of the room." << std::endl;
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                    if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                        modifier += ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                    }
                }
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                int32_t result = roll + modifier;

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(45);
                a_caster->manaSickness += 1;
            }
        }
    },
};

namespace {
    const std::unordered_map<std::string, Class> CLASSES = {
        {
            "Fighter", Class {
                "Fighter",
                "Skill with the blade.",
                {
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 10 xp.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 10;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 10;
                                }
                                if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Swift Strike")) == a_player.actions.end()) {
                                    a_player.actions.push_back(STANDARD_ACTIONS.at("Swift Strike"));
                                }
                            }
                        }
                    },
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 7 xp.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 7;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 7;
                                }
                                a_player.turns += 1;
                            }
                        }
                    },
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 18 xp.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 18;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 18;
                                }
                                a_player.curHP += 15;
                                a_player.maxHP += 15;
                            }
                        }
                    },
                }
            }
        },
        {
            "Necromancer", Class {
                "Necromancer",
                "The dead are at your beck and call.",
                {
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 10 xp and 75 maximum mana.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 10 && a_player.maxMana >= 75;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 10;
                                }
                                if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Call of the Void")) == a_player.actions.end()) {
                                    a_player.actions.push_back(STANDARD_ACTIONS.at("Call of the Void"));
                                }
                            }
                        }
                    },
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 5 xp and 75 maximum mana.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 5 && a_player.maxMana >= 75;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 5;
                                }
                                a_player.maxMana += 15;
                            }
                        }
                    },
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 35 xp and 75 maximum mana.";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 35 && a_player.maxMana >= 75;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 35;
                                }
                                a_player.maxMana += 10;
                                if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Mark for Death")) == a_player.actions.end()) {
                                    a_player.actions.push_back(STANDARD_ACTIONS.at("Mark for Death"));
                                }
                            }
                        }
                    },
                }
            }
        },
        {
            "Death Knight", Class {
                "Death Knight",
                "Sword and bone meet flesh yet unsundered.",
                {
                    ClassLevel {
                        [](GameState& a_gameState, PlayerData& a_player) {
                            std::cout << "Have at least 30 xp and " << ((a_player.perks.test((size_t)Perks::ARCANE_EYES) || a_player.LevelInClass("Necromancer") > 0) ? "at least 1 level in Necromancer and at least 1 level in Fighter" : "[UNDECIPHERABLE]") << ".";
                        },
                        [](GameState& a_gameState, PlayerData& a_player) {
                            return a_player.xp >= 30 && a_player.LevelInClass("Necromancer") >= 1 && a_player.LevelInClass("Fighter") >= 1;
                        },
                        {
                            [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                                if (!a_free) {
                                    a_player.xp -= 30;
                                }
                                if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Bone Shield")) == a_player.actions.end()) {
                                    a_player.actions.push_back(STANDARD_ACTIONS.at("Bone Shield"));
                                }
                            }
                        }
                    },
                }
            }
        },
    };

    const std::unordered_map<std::string, TimedEffect> STANDARD_TIMED_EFFECTS = {
        {
            "Bone Shield", TimedEffect {
                1,
                nullptr,
                [](GameState& a_gameState, EntityData& a_entityData) {
                    a_entityData.armor -= 2;
                }
            }
        },
        {
            "Decay", TimedEffect {
                4,
                [](GameState& a_gameState, EntityData& a_entityData) {
                    a_entityData.Hurt(DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                },
                nullptr
            }
        },
    };

    #pragma region Items
    const ItemData ITEM_DATA[] = {
        ItemData("Bean", "A single bean, you may only have 0b11111111 beans.", 
            0b11111111
        ), // 0
        ItemData("HP Potion", "A HP Potion, restores 20 HP.", {
                {"Consumable", 0},
                {"Potion"    , 0},
                {"HPRegain"  , 0},
            }, ItemUsage {
                "Drink",
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    return a_gameState.player.curHP < a_gameState.player.maxHP;
                },
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    a_gameState.player.Heal(20);
                    a_gameState.player.UseItemAt(index);
                }
            }, 100
        ), // 1
        ItemData("Mana Potion", "A Mana Potion, restores 20 Mana.", {
                {"Consumable", 0},
                {"Potion"    , 0},
                {"ManaRegain", 0},
            }, ItemUsage {
                "Drink",
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    return a_gameState.player.curMana < a_gameState.player.maxMana;
                },
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    a_gameState.player.RegainMana(20);
                    a_gameState.player.UseItemAt(index);
                }
            }, 100
        ), // 2
        ItemData("Dungeon Key", "An old key, goes to something important.", {
                {"Key", 0},
            }, 15
        ), // 3
        ItemData("Sewer Key", "A mucky key, might be useful.", {
                {"Key", 0},
            }, 15
        ), // 4
        ItemData("Long Sword", "A long sword, good in skilled hands.", {
                {"Blade", (int32_t)BladeType::LONG_SWORD},
            }, 1
        ), // 5
        ItemData("Short Sword", "A short sword, good in skilled hands.", {
                {"Blade", (int32_t)BladeType::SHORT_SWORD},
            }, 1
        ), // 6
        ItemData("Dagger", "A small dagger, good in skilled hands.", {
                {"Blade", (int32_t)BladeType::DAGGER},
            }, 1
        ), // 7
        ItemData("Ritual Dagger", "An ornate dagger, bleeding red metals comprise it's form.", {
                {"Blade", (int32_t)BladeType::DAGGER},
            }, 1
        ), // 8
        ItemData("Ars Puppetarii Mortui", "A grimoire containing some of the most abhorred gramrye known to mortals. It is woven from skin, but of what, you do not know.", {
                {"Tome"             , 0},
                {"Necromantic Focus", 5},
            }, ItemUsage {
                "Study",
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    return a_gameState.player.xp > 8;
                },
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    std::cout << "You flip through the pages of this wretched tome..." << std::endl;
                    int32_t xpCost = DIE_EIGHT_DISTRIBUTION(a_gameState.generator);
                    switch (DIE_TWENTY_DISTRIBUTION(a_gameState.generator)) {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            a_gameState.player.RegainMana(xpCost * DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                            std::cout << "You feel your mana rejuvinate." << std::endl;
                            break;
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                            a_gameState.player.Heal(xpCost * DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                            std::cout << "You feel your flesh knit together." << std::endl;
                            break;
                        case 15:
                        case 16:
                        case 17:
                            a_gameState.player.AddSkillModifier("Knowledge of Death", xpCost);
                            std::cout << "You feel your understanding of death increase." << std::endl;
                            break;
                        case 18:
                        case 19:
                            a_gameState.player.armor += 1;
                            std::cout << "You feel your skin calcify." << std::endl;
                            break;
                        case 20:
                            a_gameState.player.Hurt(xpCost * DIE_THREE_DISTRIBUTION(a_gameState.generator));
                            std::cout << "You feel your lungs collapse, before ancient air refills them... Something has changed." << std::endl;
                            break;
                    }
                    a_gameState.player.xp -= xpCost;
                }
            }, 1
        ), // 9
        ItemData("Chainmail Armor", "Simple chain link armor for protection.", {
                {"Armor", 2},
            }, 1
        ), // 10
    };

    #pragma endregion
}

#pragma region Stats
/*
Martial Combat     - 
Melee Combat       - Multiple Opponents
Unarmed Combat     - No weapons
Brawling           - 
Swordsmanship      - Skill with Sword
Dueling            - Fighting one opponent
Persuasion         - Convincing people and things through words
Knowledge of Death - Knowledge of death and the dead
Necromancy         - Knowledge in the arcane branch of necromancy (death magic)
Geomancy           - Knowledge in the arcane branch of geomancy (earth magic)
*/
#pragma endregion

std::vector<StartData> LoadStartData(std::filesystem::path a_path) {
    std::vector<StartData> loadedStarts = std::vector<StartData>();
    std::ifstream reader = std::ifstream(a_path);
    char buffer[256];
    std::string stringStorage;
    int32_t intStorage;
    std::string name;
    std::string description;

    // ENTITY TEMPLATE
    int32_t hp;
    int32_t mana;
    int32_t manaRegen;
    int32_t armor;
    std::unordered_map<std::string, int32_t> skills = std::unordered_map<std::string, int32_t>();
    std::vector<Action> actions = std::vector<Action>();
    // ENTITY TEMPLATE

    std::unordered_map<std::string, ClassInstance> classes = std::unordered_map<std::string, ClassInstance>();
    std::vector<size_t> perks = std::vector<size_t>();
    std::vector<ItemStack> inventory = std::vector<ItemStack>();
    size_t turns;
    size_t startRoom;
    size_t xp;
    size_t gold;

    uint32_t step = 0;
    uint32_t mode = 0;

    bool running = true;

    while (running) {
        switch (mode) {
            case 0:
                reader.getline(buffer, 256);
                break;
            case 1:
                reader.getline(buffer, 256, ',');
                break;
            case 2:
                reader.getline(buffer, 256, ';');
                break;
            case 3:
                reader.getline(buffer, 256, ':');
                break;
            case 4:
                reader.getline(buffer, 256, 'x');
                break;
        }
        //std::cout << std::string(buffer) << std::endl;
        //std::cout << "S & M : " << step << ", " << mode << std::endl;
        switch (step) {
            case 0:
                name = buffer;
                mode = 0;
                ++step;
                break;
            case 1:
                description = buffer;
                mode = 2;
                ++step;
                reader.getline(buffer, 256);
                break;
            case 2:
                hp = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 3:
                mana = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 4:
                manaRegen = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 5:
                armor = std::stoi(buffer);
                mode = 3;
                ++step;
                reader.getline(buffer, 256);
                reader.getline(buffer, 256);
                break;
            case 6:
                if (mode == 3) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 1;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        mode = 1;
                    }
                } else if (mode == 1) {
                    skills.insert(std::pair(stringStorage, std::stoi(buffer)));
                    mode = 3;
                    reader.getline(buffer, 256);
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 7:
                stringStorage = buffer;
                if (stringStorage == "END") {
                    mode = 3;
                    ++step;
                    reader.getline(buffer, 256);
                    reader.getline(buffer, 256);
                } else {
                    actions.push_back(STANDARD_ACTIONS.at(stringStorage));
                    mode = 1;
                    reader.getline(buffer, 256);
                }
                break;
            case 8:
                if (mode == 3) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 1;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        mode = 1;
                    }
                } else if (mode == 1) {
                    classes.insert(std::pair(stringStorage, ClassInstance(stringStorage, std::stoi(buffer))));
                    mode = 3;
                    reader.getline(buffer, 256);
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 9:
                stringStorage = buffer;
                if (stringStorage == "END") {
                    mode = 1;
                    ++step;
                    reader.getline(buffer, 256);
                    reader.getline(buffer, 256);
                } else {
                    /*
                        INSIGHT
                        ARCANE_EYES
                        HARD_HITTER
                        HORDE_SLAYER
                    */
                    if (stringStorage == "INSIGHT") {
                        perks.push_back((size_t)Perks::INSIGHT);
                    } else if (stringStorage == "ARCANE_EYES") {
                        perks.push_back((size_t)Perks::ARCANE_EYES);
                    } else if (stringStorage == "HARD_HITTER") {
                        perks.push_back((size_t)Perks::HARD_HITTER);
                    } else if (stringStorage == "HORDE_SLAYER") {
                        perks.push_back((size_t)Perks::HORDE_SLAYER);
                    }
                    mode = 1;
                    reader.getline(buffer, 256);
                }
                break;
            case 10:
                if (mode == 1) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 2;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        inventory.push_back(ItemStack::FromString(stringStorage));
                        mode = 1;
                    reader.getline(buffer, 256);
                    }
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 11:
                turns = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 12:
                startRoom = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 13:
                xp = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 14:
                gold = std::stoi(buffer);
                mode = 0;
                ++step;
                loadedStarts.push_back(StartData(
                    name,
                    description,
                    EntityTemplate(hp, mana, manaRegen, armor, skills, actions),
                    classes,
                    perks,
                    inventory,
                    turns,
                    startRoom,
                    xp,
                    gold
                ));
                skills = std::unordered_map<std::string, int32_t>();
                actions = std::vector<Action>();
                classes = std::unordered_map<std::string, ClassInstance>();
                perks = std::vector<size_t>();
                inventory = std::vector<ItemStack>();
                reader.getline(buffer, 256);
                reader.getline(buffer, 256);
                break;
            case 15:
                stringStorage = buffer;
                if (stringStorage == "FINAL") {
                    //std::cout << "Finished" << std::endl;
                    running = false;
                } else {
                    step = 0;
                    mode = 0;
                    reader.getline(buffer, 256);
                }
                break;
        }
    }

    reader.close();
    
    return loadedStarts;
}

#pragma region Starts
std::vector<StartData> GameState::Starts = {
    /*
    StartData(
        "Default",
        "A simple start to the game, good for beginners.",
        EntityTemplate(100, 100, 10, 10, {
            {"Martial Combat", 2},
            {"Unarmed Combat", 1},
            {"Brawling"      , 1},
        }, {
            STANDARD_ACTIONS.at("Punch")
        }), {
            {"Fighter", ClassInstance("Fighter", 1)},
        }, {
            (size_t)Perks::INSIGHT
        }, {
            ItemStack(1, 4),
            ItemStack(6, 1),
        },
        1, 0
    ),
    StartData(
        "Normal",
        "Similar to Default, but with less extra help.",
        EntityTemplate(100, 100, 10, 10, {
            {"Martial Combat", 2},
        }, {
            STANDARD_ACTIONS.at("Punch")
        }), {
            {"Fighter", ClassInstance("Fighter", 1)},
        }, {

        }, {
            ItemStack(1, 2),
            ItemStack(7, 1)
        },
        1, 0
    ),
    StartData(
        "Trivial",
        "A trivial start to the game, good for people bad at the game.",
        EntityTemplate(150, 150, 10, 12, {
            {"Martial Combat", 3},
            {"Unarmed Combat", 3},
            {"Brawling"      , 3},
        }, {
            STANDARD_ACTIONS.at("Punch"),
        }), {
            {"Fighter", ClassInstance("Fighter", 1)},
        }, {
            (size_t)Perks::INSIGHT,
            (size_t)Perks::HORDE_SLAYER,
        }, {
            ItemStack(1, 16),
            ItemStack(2, 16),
            ItemStack(3, 5),
            ItemStack(4, 5),
            ItemStack(5, 1),
        },
        1, 0
    ),
    StartData(
        "Wretch",
        "This is hard mode, no mana, no perks, no items, no classes.",
        EntityTemplate(100, 0, 0, 10, 
        {}, {
            STANDARD_ACTIONS.at("Punch")
        }), 
        {}, 
        {}, 
        {},
        1, 0
    ),
    StartData(
        "Occultist",
        "They say that the Ars Puppetarii Mortui hold secrets not meant for mortals, you think they're just stupid.",
        EntityTemplate(65, 175, 15, 10, {
            {"Knowledge of Death", 2},
            {"Necromancy"        , 1}
        }, {
            STANDARD_ACTIONS.at("Punch")
        }), {
            {"Necromancer", ClassInstance("Necromancer", 1)},
        }, 
        {}, 
        {
            ItemStack(8, 1),
            ItemStack(9, 1),
        },
        1, 0
    ),
    */
};
#pragma endregion

struct ConnectionInstance {
public:
    size_t destination;
    ConnectionTest currentTest;
    ConnectionTest arrivalTest;

    bool CanPass(GameState& a_gameState) {
        bool currentResult = true;
        bool arrivalResult = true;

        if (currentTest.condition != nullptr) {
            currentResult = currentTest.condition(a_gameState, a_gameState.rooms[destination]);
        }

        if (arrivalTest.condition != nullptr) {
            arrivalResult = arrivalTest.condition(a_gameState, a_gameState.rooms[destination]);
        }

        if (currentTest.conditionDisplay != nullptr) {
            currentTest.conditionDisplay(a_gameState, a_gameState.rooms[destination], currentResult);
        }
        
        if (arrivalTest.conditionDisplay != nullptr) {
            arrivalTest.conditionDisplay(a_gameState, a_gameState.rooms[destination], arrivalResult);
        }

        return currentResult && arrivalResult;
    }

    void Passes(GameState& a_gameState) {
        if (currentTest.action != nullptr) {
            currentTest.action(a_gameState, a_gameState.rooms[destination]);
        }

        if (arrivalTest.action != nullptr) {
            arrivalTest.action(a_gameState, a_gameState.rooms[destination]);
        }
    }
};

namespace DataComponents {
    /*
    # EBNF Form
    
    <value_char> ::= ([a-z] | [A-Z] | [0-9] | "_" | "-" | "\\n" | "\\\"" | "\\t" | "\\v" | "\\;" | "|" | "." | "," | "'" | ":" | "[" | "]" | "{" | "}" | "/" | "?" | "<" | ">" | "=" | "+" | "!" | "`" | "~" | "@" | "#" | "$" | "%" | "^" | "&" | "*" | "(" | ")")
    <value_string> ::= ("\"" (<value_char> | ";" | <deadspace>)+ "\"" | (<value_char> | <deadspace>)+)
    <value_uint> ::= [0-9]+
    <value_int> ::= "-"? <value_uint>

    <type_int> ::= "INT" ("8" | "16" | "32" | "64")
    <type_uint> ::= "U" <type_int>

    <delim> ::= ":"

    <identifier> ::= ([a-z] | [A-Z] | [0-9] | "_")+
    <deadspace> ::= (" " | "\n" | "\t" | "\v")
    <statement_name> ::= <deadspace>* <identifier> <deadspace>*
    <statement_data> ::= <deadspace>* <delim> <deadspace>*
    <statement> ::= (<statement_uint> | <statement_int> | <statement_bool> | <statement_string>)

    <statement_uint> ::= <statement_name> "<" <deadspace>* <type_uint> <deadspace>* ">" <statement_data> <value_uint> <deadspace>* ";"
    <statement_int> ::= <statement_name> "<" <deadspace>* <type_int> <deadspace>* ">" <statement_data> <value_int> <deadspace>* ";"
    <statement_bool> ::= <statement_name> "<" <deadspace>* "BOOL" <deadspace>* ">" <statement_data> ((("t" | "T") ("r" | "R") ("u" | "U") ("e" | "E")) | (("f" | "F") ("a" | "A") ("l" | "L") ("s" | "S") ("e" | "E"))) <deadspace>* ";"
    <statement_string> ::= <statement_name> "<" <deadspace>* "STRING" <deadspace>* ">" <statement_data> (<value_string> <deadspace>*)+ ";"
    */

    enum class DataType {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8,
        INT16,
        INT32,
        INT64,
        BOOL,
        STRING,
        INVALID,
    };

    struct DataHolder {
    public:
        std::string key;
        DataType type;
        void* data;
        size_t size;

        DataHolder(std::string_view a_key, const DataType& a_dataType, const void* a_data, size_t a_size) {
            this->key = a_key;
            this->type = a_dataType;
            this->data = malloc(a_size);
            this->size = a_size;
            
            
            #ifdef __STDC_LIB_EXT1__
            memcpy_s(this->data, a_size, a_data, a_size);
            #else
            memcpy(this->data, a_data, a_size);
            #endif

            //if (a_dataType == DataType::STRING) {
            //    std::cout << "Value: " << (char*)data << std::endl;
            //}
        }

        void SetData(const void* a_data, size_t a_size) {
            this->data = realloc(this->data, a_size);
            this->size = a_size;

            #ifdef __STDC_LIB_EXT1__
            memcpy_s(this->data, a_size, a_data, a_size);
            #else
            memcpy(this->data, a_data, a_size);
            #endif
        }

        ~DataHolder() {
            //this->key.clear();
            //this->type = DataType::INVALID;
            free(this->data);
            //this->size = 0;
        }
    };

    struct DataContainer {
    public:
        std::unordered_map<std::string, DataHolder> data;

        DataContainer(void) {
            this->data = std::unordered_map<std::string, DataHolder>();
        }

        template<typename T>
        T* Get(const std::string& a_key) {
            return (T*)data.at(a_key).data;
        }
    };

    enum class ReaderMode {
        COLLECTING_NAME,
        COLLECTING_TYPE,
        COLLECTING_DELM,
        COLLECTING_DATA,
    };

    DataContainer ParseDataFile(std::filesystem::path a_path) {
        std::ifstream reader = std::ifstream(a_path.string());
        DataContainer container = DataContainer();
        ReaderMode mode = ReaderMode::COLLECTING_NAME;
        bool inString = false;
        bool escaped = false;
        char character = 0;
        std::string collectorName = "";
        std::string collectorType = "";
        std::string collectorData = "";
        DataType dataType = DataType::INVALID;
        while ((character = reader.get()) != -1) {
            //std::cout << character;
            switch (mode) {
                case ReaderMode::COLLECTING_NAME:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case '<':
                            mode = ReaderMode::COLLECTING_TYPE;
                            break;
                        default:
                            collectorName += character;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_TYPE:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case '>':
                            if (collectorType == "UINT8") {
                                dataType = DataType::UINT8;
                            } else if (collectorType == "UINT16") {
                                dataType = DataType::UINT16;
                            } else if (collectorType == "UINT32") {
                                dataType = DataType::UINT32;
                            } else if (collectorType == "UINT64") {
                                dataType = DataType::UINT64;
                            } else if (collectorType == "INT8") {
                                dataType = DataType::INT8;
                            } else if (collectorType == "INT16") {
                                dataType = DataType::INT16;
                            } else if (collectorType == "INT32") {
                                dataType = DataType::INT32;
                            } else if (collectorType == "INT64") {
                                dataType = DataType::INT64;
                            } else if (collectorType == "BOOL") {
                                dataType = DataType::BOOL;
                            } else if (collectorType == "STRING") {
                                dataType = DataType::STRING;
                            } else {
                                dataType = DataType::INVALID;
                                std::cout << "Error, invalid type \"" << collectorType << "\"." << std::endl;
                            }
                            mode = ReaderMode::COLLECTING_DELM;
                            break;
                        default:
                            collectorType += character;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_DELM:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case ':':
                            mode = ReaderMode::COLLECTING_DATA;
                            break;
                        default:
                            std::cout << "Invalid char between delimeter \"" << character << "\"" << std::endl;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_DATA:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            if (inString) {
                                collectorData += character;
                            }
                            escaped = false;
                            continue;
                        case '\\':
                            if (escaped) {
                                collectorData += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                            break;
                        case ';':
                            if (escaped || inString) {
                                collectorData += character;
                            } else {
                                mode = ReaderMode::COLLECTING_NAME;
                                switch (dataType) {
                                    case DataType::UINT8: {
                                        uint8_t data = std::stoul(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(uint8_t)));
                                        break;
                                    }
                                    case DataType::UINT16: {
                                        uint16_t data = std::stoul(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(uint16_t)));
                                        break;
                                    }
                                    case DataType::UINT32: {
                                        uint32_t data = std::stoul(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(uint32_t)));
                                        break;
                                    }
                                    case DataType::UINT64: {
                                        uint64_t data = std::stoull(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(uint64_t)));
                                        break;
                                    }
                                    case DataType::INT8: {
                                        int8_t data = std::stoi(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(int8_t)));
                                        break;
                                    }
                                    case DataType::INT16: {
                                        int16_t data = std::stoi(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(int16_t)));
                                        break;
                                    }
                                    case DataType::INT32: {
                                        int32_t data = std::stoi(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(int32_t)));
                                        break;
                                    }
                                    case DataType::INT64: {
                                        int64_t data = std::stoll(collectorData);
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(&data), sizeof(int64_t)));
                                        break;
                                    }
                                    case DataType::BOOL: {
                                        std::transform(collectorData.begin(), collectorData.end(), collectorData.begin(), [](unsigned char a_character) { return std::tolower(a_character);});
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(collectorData == "true"), sizeof(bool)));
                                        break;
                                    }
                                    case DataType::STRING: {
                                        container.data.emplace(collectorName, DataHolder(collectorName, dataType, (void*)(collectorData + '\0').c_str(), collectorData.size() + 1));
                                        break;
                                    }
                                }
                                collectorName = "";
                                collectorType = "";
                                collectorData = "";
                            }
                            escaped = false;
                            break;
                        case '\"':
                            if (escaped) {
                                collectorData += '\"';
                            } else {
                                inString = !inString;
                            }
                            escaped = false;
                            break;
                        default:
                            if (escaped) {
                                switch (character) {
                                    case 't':
                                        collectorData += '\t';
                                    case 'v':
                                        collectorData += '\v';
                                    case 'n':
                                        collectorData += '\n';
                                    default:
                                        collectorData += character;
                                }
                            } else {
                                collectorData += character;
                            }
                            escaped = false;
                            break;
                    }
                    break;
            }
        }

        reader.close();

        return container;
    }
}

#pragma region Shops
struct ShopGenerationRule {
public:
    std::function<bool(GameState&)> valid = nullptr;
    std::function<void(GameState&, ShopInstance&)> generate = nullptr;
};

struct ShopCatalog {
public:
    size_t cost;
    ItemStack stack;
};

struct ShopData {
public:
    std::string shopName;
    std::vector<ShopGenerationRule> generationRules;
};

struct ShopInstance {
public:
    std::string shopName;
    std::vector<ShopCatalog> catalog;
};


const std::unordered_map<std::string, ShopGenerationRule> SHARED_GENERATION_RULES = {
    {
        "Standard Potions", ShopGenerationRule {
            [](GameState& a_gameState) {
                return true;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                a_shop.catalog.push_back(ShopCatalog { 5, ItemStack(1, 4 + DIE_SIXTEEN_DISTRIBUTION(a_gameState.generator)) });
                a_shop.catalog.push_back(ShopCatalog { 5, ItemStack(2, 4 + DIE_SIXTEEN_DISTRIBUTION(a_gameState.generator)) });
            }
        }
    },
    {
        "Standard Weaponry", ShopGenerationRule {
            [](GameState& a_gameState) {
                return DIE_SIX_DISTRIBUTION(a_gameState.generator) > 1;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                switch (DIE_THREE_DISTRIBUTION(a_gameState.generator)) {
                    case 1:
                        a_shop.catalog.push_back(ShopCatalog { 10, ItemStack(5, 1) });
                        break;
                    case 2:
                        a_shop.catalog.push_back(ShopCatalog { 10, ItemStack(6, 1) });
                        break;
                    case 3:
                        a_shop.catalog.push_back(ShopCatalog {  5, ItemStack(7, 1) });
                        break;
                }
                if (DIE_FOUR_DISTRIBUTION(a_gameState.generator) == 1) {
                    a_shop.catalog.back().stack.metadata.emplace("ToHit", DIE_SIX_DISTRIBUTION(a_gameState.generator));
                }
            }
        }
    },
    {
        "Standard Keys", ShopGenerationRule {
            [](GameState& a_gameState) {
                return DIE_SIX_DISTRIBUTION(a_gameState.generator) > 4;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                a_shop.catalog.push_back(ShopCatalog { 3, ItemStack(4, DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator)) });
            }
        }
    },
};

const std::unordered_map<std::string, ShopData> SHOP_DATA = {
    {
        "Barterer", ShopData {
            "Barterer",
            {
                SHARED_GENERATION_RULES.at("Standard Potions"),
                SHARED_GENERATION_RULES.at("Standard Weaponry"),
                SHARED_GENERATION_RULES.at("Standard Keys"),
            }
        }
    },
};

void GenerateShopInstance(GameState& a_gameState, const ShopData& a_shopData, ShopInstance& a_shopInstance) {
    a_shopInstance.shopName = a_shopData.shopName;
    for (const ShopGenerationRule& shopGenerationRule  : a_shopData.generationRules) {
        if (shopGenerationRule.valid(a_gameState)) {
            shopGenerationRule.generate(a_gameState, a_shopInstance);
        }
    }
}
#pragma endregion

const std::unordered_map<std::string, RoomAction> ROOM_ACTIONS = {
    {
        "Treasure Trove", RoomAction {
            "Treasure Trove",
            "A pile of riches.",
            [](GameState& a_gameState) {
                std::cout << (a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Looted") ? "- There isn't even a single coin that remains.\n" : "- There is a glittering pile of riches.\n");
            },
            [](GameState& a_gameState) {
                return 
                    a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Count") && 
                    a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Dice") &&
                    !a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Looted");
            },
            [](GameState& a_gameState) {
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Looted", 0);
                int32_t goldAmount = 0;
                std::uniform_int_distribution<uint32_t>& dieDistribution = DICE.at(a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice"));
                for (size_t i = 0; i < a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count"); ++i) {
                    goldAmount += dieDistribution(a_gameState.generator);
                }
                a_gameState.player.gold += goldAmount;
                std::cout << "You find " << goldAmount << " (" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count") << "d" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice") << ") gold." << std::endl;
                EatInput();
            }
        }
    }
};

int main(int argc, char** argv) {
    //DataComponents::DataContainer container = DataComponents::ParseDataFile(std::filesystem::current_path().append("test.data"));
    for (const StartData& start : LoadStartData(std::filesystem::current_path().append("starts.data"))) {
        GameState::Starts.push_back(start);
    }
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()));
    #pragma endregion

    //std::cout << "The answer is: " << (std::pow(29, 452)) << "." << std::endl;

    //for (const std::pair<size_t, std::uniform_int_distribution<uint32_t>&>& dice : DICE) {
    //    std::cout << "DIE " << dice.first << "\n====================" << std::endl;
    //    for (int32_t i = 0; i < 50; ++i) {
    //        std::cout << (i + 1) << ") " << dice.second(gameState.generator) << "/" << dice.first << std::endl;
    //    }
    //    std::cout << "====================" << std::endl;
    //}

    const EntityTemplate ENTITY_TEMPLATES[] = {
        EntityTemplate(32, 0, 0, 6, {}, {}), // Base Goblin
    };

    const NPCTemplate NPC_TEMPLATES[] = {
        NPCTemplate {
            "Goblin Shaman",
            12,
            8,
            [](GameState& a_gameState, NPCData& a_npc) {
                size_t healed = 0;
                if (a_npc.curMana > 10 && DIE_TWO_DISTRIBUTION(a_gameState.generator) == 1) {
                    for (NPCData& npc : a_gameState.rooms[a_gameState.curRoom].inhabitants) {
                        if (npc.curHP < npc.maxHP) {
                            npc.Heal(DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                            ++healed;
                        }
                    }
                }

                if (healed == 0) {
                    std::cout << "The Goblin Shaman swings their staff at the player and ";
                    if (DIE_TWENTY_DISTRIBUTION(a_gameState.generator) >= a_gameState.player.GetEffectiveArmor()) {
                        int32_t damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                        a_gameState.player.Hurt(damage);
                        std::cout << "hits, dealing " << damage << " damage." << std::endl;
                    } else {
                        std::cout << "misses." << std::endl;
                    }
                } else {
                    a_npc.manaSickness += 1;
                    a_npc.DrainMana(10);
                    std::cout << "The Goblin Shaman murmurs under their breath; as they do this the opponents are engufled in a pale green mist, which quickly dissipates, and " << healed << " of the opponents seem rejuvinated." << std::endl;
                }
            }
        }, // 0
        NPCTemplate {
            "Goblin Grunt",
            4,
            6,
            [](GameState& a_gameState, NPCData& a_npc) {
                std::cout << "The Goblin Grunt swings their club at the player and ";
                if (DIE_TWENTY_DISTRIBUTION(a_gameState.generator) >= a_gameState.player.GetEffectiveArmor()) {
                    int32_t damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        }, // 1
        NPCTemplate {
            "Goblin Squire",
            6,
            6,
            [](GameState& a_gameState, NPCData& a_npc) {
                std::cout << "The Goblin Squire swings their shortsword at the player and ";
                int32_t modifier = 
                    a_npc.GetSkillModifier("Martial Combat") +
                    a_npc.GetSkillModifier("Swordsmanship");
                if (DIE_TWENTY_DISTRIBUTION(a_gameState.generator) + modifier >= a_gameState.player.GetEffectiveArmor()) {
                    int32_t damage = DIE_SIX_DISTRIBUTION(a_gameState.generator) + modifier * 0.5;
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        }, // 2
        NPCTemplate {
            "Goblin Priest",
            15,
            10,
            [](GameState& a_gameState, NPCData& a_npc) {
                size_t healed = 0;
                if (a_npc.curMana > 10 && DIE_TWO_DISTRIBUTION(a_gameState.generator) == 1) {
                    for (NPCData& npc : a_gameState.rooms[a_gameState.curRoom].inhabitants) {
                        if (npc.curHP < npc.maxHP) {
                            npc.Heal(DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator) + DIE_THREE_DISTRIBUTION(a_gameState.generator));
                            ++healed;
                        }
                    }
                }

                if (healed == 0) {
                    int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                    int32_t modifier = 0;
                    if (a_npc.curMana > 5 && DIE_THREE_DISTRIBUTION(a_gameState.generator) == 1) {
                        std::cout << "The Goblin Priest raises their arms in prayer and a swarm of pebbles sling towards the player and they ";
                        modifier += a_npc.GetSkillModifier("Geomancy");
                        a_npc.DrainMana(5);
                        if (roll + modifier >= a_gameState.player.GetEffectiveArmor()) {
                            int32_t damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator) + DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                            a_gameState.player.Hurt(damage);
                            std::cout << "hit, dealing " << damage << " damage." << std::endl;
                        } else {
                            std::cout << "miss." << std::endl;
                        }
                    } else {
                        std::cout << "The Goblin Priest swings their staff at the player and ";
                        modifier += a_npc.GetSkillModifier("Martial Combat");
                        if (roll + modifier >= a_gameState.player.GetEffectiveArmor()) {
                            int32_t damage = 2 + DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                            a_gameState.player.Hurt(damage);
                            std::cout << "hits, dealing " << damage << " damage." << std::endl;
                        } else {
                            std::cout << "misses." << std::endl;
                        }
                    }
                } else {
                    a_npc.manaSickness += 1;
                    a_npc.DrainMana(10);
                    std::cout << "The Goblin Priest murmurs under their breath; as they do this the opponents are engufled in a pale green mist, which quickly dissipates, and " << healed << " of the opponents seem rejuvinated." << std::endl;
                }
            }
        }, // 3
    };

    const std::array<RoomData, 6> ROOM_DATA = {
        RoomData("Sanctuary", "A strange room with a strange promise.",
            [](GameState& a_gameState, const RoomData& a_roomData) { return 0; },
            false, 0, 0, 2, 4
        ),
        RoomData("Simple", "",
            [](GameState& a_gameState, const RoomData& a_roomData) { return 35; }, 
            true, 0, 1, 1, 3
        ),
        RoomData("Loopback", "",
            [](GameState& a_gameState, const RoomData& a_roomData) { return a_gameState.usedRooms > 8 ? 15 : 0; }, 
            true, 1, 3, 0, 2
        ),
        RoomData("Downfall", "",
            [](GameState& a_gameState, const RoomData& a_roomData) { return (a_gameState.curRoom % 6) > 3 ? 10 : 0; }, 
            false, 0, 1, 2, 4
        ),
        RoomData("Lotus", "The room branches into many different corridors, seemingly into new domains.",
            [](GameState& a_gameState, const RoomData& a_roomData) { return 20; }, 
            true, 0, 0, 2, 6
        ),
        RoomData("Sewer", "",
            [](GameState& a_gameState, const RoomData& a_roomData) { return (a_roomData.minNew >= 2 || a_roomData.backlink) ? 8 : 0; }, 
            true, 0, 0, 2, 4, 
            ConnectionTest {
                [](GameState& a_gameState, RoomInstance& a_destination) {
                    return a_destination.flags.contains("OpenDoor") || a_gameState.player.HasItem(4, 1);
                },
                [](GameState& a_gameState, RoomInstance& a_destination, bool a_success) {
                    if (!a_success) {
                        std::cout << "You require a key to get through the grate." << std::endl;
                    }
                },
                [](GameState& a_gameState, RoomInstance& a_destination) {
                    if (!a_destination.flags.contains("OpenDoor")) {
                        a_gameState.player.UseItemOf(4);
                        a_destination.flags.emplace("OpenDoor", 1);
                        std::cout << "You use one sewer key to open the grate." << std::endl;
                    }
                }
            }, 
            ConnectionTest()
        ),
    };

    const std::unordered_map<std::string, std::function<NPCData(NPCData)>> NPC_MODIFIERS = {
        {
            "Shaman", [](NPCData a_npc){
                a_npc.maxMana += 20;
                a_npc.curMana += 20;
                a_npc.manaRegen += 7;
                return a_npc;
            }
        },
        {
            "Squire", [](NPCData a_npc){
                a_npc.maxHP += 8;
                a_npc.curHP += 8;
                a_npc.armor += 2;
                a_npc.AddSkillModifier("Martial Combat", 1);
                a_npc.AddSkillModifier("Swordsmanship" , 1);
                return a_npc;
            }
        },
        {
            "Priest", [](NPCData a_npc){
                a_npc.maxHP += 6;
                a_npc.curHP += 6;
                a_npc.maxMana += 35;
                a_npc.curMana += 35;
                a_npc.manaRegen += 10;
                a_npc.AddSkillModifier("Martial Combat", 1);
                a_npc.AddSkillModifier("Geomancy", 1);
                return a_npc;
            }
        },
        {
            "Elder", [](NPCData a_npc){
                a_npc.name = "Elder " + a_npc.name;
                a_npc.maxHP += a_npc.maxHP * 0.5;
                a_npc.curHP += a_npc.curHP * 0.5;
                a_npc.maxMana += a_npc.maxMana * 0.5;
                a_npc.curMana += a_npc.curMana * 0.5;
                a_npc.manaRegen += 5;
                for (const std::pair<std::string, int32_t>& skillPair : a_npc.skills) {
                    a_npc.AddSkillModifier(skillPair.first, 1);
                }
                return a_npc;
            }
        },
    };

    const std::array<Encounter, 10> ENCOUNTERS = {
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom <= 0 ? 1 : 0; },
            nullptr
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 0 ? 5 : 0; },
            [](GameState& a_gameState) {
                std::cout << "Nothing happens as you enter the room." << std::endl;
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 0 ? 5 : 0; },
            [](GameState& a_gameState) {
                if (a_gameState.player.maxMana <= 0) {
                    if (a_gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))) {
                        std::cout << "There is a mana fountain in the area, but you can't benefit from it." << std::endl;
                    } else {
                        std::cout << "Nothing of interest can be found in the room." << std::endl;
                    }
                    return;
                }

                std::cout << "A mana fountain is in the area, ";

                if (a_gameState.player.curMana == a_gameState.player.maxMana) {
                    std::cout << "however, you are already at your maximum." << std::endl;
                } else if (a_gameState.player.curMana > a_gameState.player.maxMana) {
                    std::cout << "however, due to you being overloaded with mana, you take 5 damage." << std::endl;
                    a_gameState.player.Hurt(5);
                } else {
                    uint32_t manaGain = std::min<uint32_t>(a_gameState.player.maxMana - a_gameState.player.curMana, 25);
                    std::cout << "you have regained " << manaGain << " mana." << std::endl;
                    a_gameState.player.RegainMana(manaGain);
                    if (a_gameState.player.curMana == a_gameState.player.maxMana) {
                        std::cout << "Your mana is fully replinished." << std::endl;
                    }
                }
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 0 ? 5 : 0; },
            [](GameState& a_gameState) {
                std::cout << "A restorative spring is in the area, ";

                if (a_gameState.player.curHP == a_gameState.player.maxHP) {
                    std::cout << "however, you are already at your maximum health." << std::endl;
                } else if (a_gameState.player.curHP > a_gameState.player.maxHP) {
                    std::cout << "however, you are healthier than is expected." << std::endl;
                } else {
                    uint32_t hpGain = std::min<uint32_t>(a_gameState.player.maxHP - a_gameState.player.curHP, 25);
                    std::cout << "you have regained " << hpGain << " health." << std::endl;
                    a_gameState.player.Heal(hpGain);
                    if (a_gameState.player.curHP == a_gameState.player.maxHP) {
                        std::cout << "Your health is fully replinished." << std::endl;
                    }
                }
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 0 ? 5 : 0; },
            [&ENTITY_TEMPLATES, &NPC_TEMPLATES, &NPC_MODIFIERS](GameState& a_gameState) {
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[1])
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[1])
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Shaman")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[0])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(
                    RoomActionReference {
                        ROOM_ACTIONS.at("Treasure Trove")
                    }
                );
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Count", 3);
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Dice", 12);
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 15 ? 4 : 0; },
            [&ENTITY_TEMPLATES, &NPC_TEMPLATES, &NPC_MODIFIERS](GameState& a_gameState) {
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Squire")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[2])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Shaman")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[0])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(
                    RoomActionReference {
                        ROOM_ACTIONS.at("Treasure Trove")
                    }
                );
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Count", 2);
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Dice", 20);
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 0 ? 2 : 0; },
            [](GameState& a_gameState) {
                switch (DIE_THREE_DISTRIBUTION(a_gameState.generator)) {
                    case 1:
                        a_gameState.player.AddSkillModifier("Martial Combat", DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                        break;
                    case 2:
                        a_gameState.player.AddSkillModifier("Unarmed Combat", DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                        break;
                    case 3:
                        a_gameState.player.AddSkillModifier("Brawling", DIE_FOUR_DISTRIBUTION(a_gameState.generator));
                        break;
                }
                
                std::cout << "There is a training dummy in the middle of the room... You feel better at something." << std::endl;
            }
        },
        Encounter {
            [](GameState& a_gameState) { return (a_gameState.curRoom > 5 && a_gameState.player.gold >= 15) ? 3 : 0; },
            [](GameState& a_gameState) {
                std::cout << "There is a barterer in this room." << std::endl;
                a_gameState.rooms[a_gameState.curRoom].data.emplace("Shop", new ShopInstance());
                GenerateShopInstance(a_gameState, SHOP_DATA.at("Barterer"), *(ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(
                    RoomActionReference {
                        RoomAction {
                            "Shop",
                            "Shop at the barterer.",
                            [](GameState& a_gameState) {
                                std::cout << "- A barterer is humming away in their humble shop.\n";
                            },
                            [](GameState& a_gameState) {
                                return true;
                            },
                            [](GameState& a_gameState) {
                                size_t option = 0;
                                while (option != 3) {
                                    ShopInstance* shop = ((ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                                    std::cout << "-------------------------------\n\x1b[1m" << shop->shopName << "\x1b[22m\n\nOptions:\n1) Buy\n2) Sell\n3) Exit\n\nOption: ";
                                    SafeInput<size_t>(option);
                                    switch (option) {
                                        case 1:
                                            std::cout << "-------------------------------\nGold: " << a_gameState.player.gold << "\n\nCatalog:\n";
                                            for (size_t i = 0; i < shop->catalog.size(); ++i) {
                                                const ItemData& itemType = ITEM_DATA[shop->catalog[i].stack.itemID];
                                                std::cout << (i + 1) << ") " << itemType.name << " (" << shop->catalog[i].cost << ") x" << shop->catalog[i].stack.stackSize << "\n";
                                            }
                                            std::cout << (shop->catalog.size() + 1) << ") Back\n\nOption: ";
                                            SafeInput<size_t>(option);

                                            if (--option < shop->catalog.size()) {
                                                if (shop->catalog[option].cost <= a_gameState.player.gold) {
                                                    a_gameState.player.gold -= shop->catalog[option].cost;
                                                    a_gameState.player.AddItem(shop->catalog[option].stack.itemID, 1);
                                                    std::cout << "You bought 1 " << ITEM_DATA[shop->catalog[option].stack.itemID].name << " for " << shop->catalog[option].cost << " gold." << std::endl;
                                                    if (--shop->catalog[option].stack.stackSize <= 0) {
                                                        shop->catalog.erase(shop->catalog.begin() + option);
                                                    }
                                                } else {
                                                    std::cout << "You do not have enough gold." << std::endl;
                                                }
                                            }
                                            option = 0;
                                            break;
                                        case 2:
                                            std::cout << "They do not wish to buy anything from you..." << std::endl;
                                            break;
                                    }
                                }
                            }
                        }
                    }
                );
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 20 ? 3 : 0; },
            [&ENTITY_TEMPLATES, &NPC_TEMPLATES, &NPC_MODIFIERS](GameState& a_gameState) {
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Squire")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[2])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Priest")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[3])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(
                    RoomActionReference {
                        ROOM_ACTIONS.at("Treasure Trove")
                    }
                );
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Count", 4);
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Dice", 12);
            }
        },
        Encounter {
            [](GameState& a_gameState) { return a_gameState.curRoom > 30 ? std::min<uint32_t>(7, a_gameState.curRoom * 0.1) : 0; },
            [&ENTITY_TEMPLATES, &NPC_TEMPLATES, &NPC_MODIFIERS](GameState& a_gameState) {
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Squire")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[2])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Squire")(
                        NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[2])
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                    NPC_MODIFIERS.at("Elder")(
                        NPC_MODIFIERS.at("Priest")(
                            NPCData(&ENTITY_TEMPLATES[0], &NPC_TEMPLATES[3])
                        )
                    )
                );
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(
                    RoomActionReference {
                        ROOM_ACTIONS.at("Treasure Trove")
                    }
                );
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Count", 4);
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Dice", 16);
            }
        },
    };

    std::function<size_t(const RoomData&)> GetRandomRoom = [&gameState, &ROOM_DATA](const RoomData& a_roomData) {
        size_t maximum = 0;
        size_t weight = 0;
        std::vector<int64_t> weights = std::vector<int64_t>();

        for (const RoomData& roomData : ROOM_DATA) {
            if (roomData.weight != nullptr && (weight = roomData.weight(gameState, a_roomData)) > 0) {
                maximum += weight;
                weights.push_back(weight);
            } else {
                weights.push_back(-1);
            }
        }

        int64_t choice = static_cast<int64_t>(PERCENT_DISTRIBUTION(gameState.generator) * maximum);

        for (size_t i = 0; i < ROOM_DATA.size(); ++i) {
            if (weights[i] != -1 && (choice -= weights[i]) <= 0) {
                return i;
            }
        }

        return (size_t)0;
    };

    std::function<size_t(void)> GetRandomEncounter = [&gameState, &ENCOUNTERS]() {
        size_t maximum = 0;
        size_t weight = 0;
        std::vector<int64_t> weights = std::vector<int64_t>();

        for (Encounter encounter : ENCOUNTERS) {
            if (encounter.weight != nullptr && (weight = encounter.weight(gameState)) > 0) {
                maximum += weight;
                weights.push_back(weight);
            } else {
                weights.push_back(-1);
            }
        }

        int64_t choice = static_cast<int64_t>(PERCENT_DISTRIBUTION(gameState.generator) * maximum);

        for (size_t i = 0; i < ENCOUNTERS.size(); ++i) {
            if (weights[i] != -1 && (choice -= weights[i]) <= 0) {
                return i;
            }
        }

        return (size_t)0;
    };

    std::function<void(RoomInstance* const)> InitializeRoom = [&GetRandomRoom, &gameState, &ROOM_DATA](RoomInstance* const a_roomInstance) {
        a_roomInstance->initialized = true;
        std::vector<ConnectionInstance> newConnections = a_roomInstance->connections;
        size_t newRoomThing = 0;
        const RoomData& roomType = ROOM_DATA[a_roomInstance->roomID];
        size_t roomIndex = a_roomInstance->roomIndex;
        size_t newRoomCount = roomType.minNew + PERCENT_DISTRIBUTION(gameState.generator) * (roomType.maxNew - roomType.minNew);
        for (int i = 0; i < std::min<int32_t>(roomType.minExisting + PERCENT_DISTRIBUTION(gameState.generator) * (roomType.maxExisting - roomType.minExisting), gameState.usedRooms); ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = size_t(PERCENT_DISTRIBUTION(gameState.generator) * gameState.usedRooms)),
                    ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    roomType.fromConnection
                }
            );
        }
        for (int i = 0; i < newRoomCount; ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = gameState.PushBackRoom(GetRandomRoom(roomType))),
                    ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    roomType.fromConnection
                }
            );

            if (ROOM_DATA[gameState.rooms[newRoomThing].roomID].backlink) {
                gameState.rooms[newRoomThing].connections.push_back(
                    ConnectionInstance {
                        roomIndex,
                        ConnectionTest(),
                        ConnectionTest()
                    }
                );
            }
        }
        gameState.rooms[roomIndex].connections = newConnections;
    };

    RoomInstance* room = nullptr;
    uint32_t choice;
    size_t encounter;
    bool runEncounter = false;
    while (gameState.running) {
        switch (gameState.screen) {
            case Screen::TITLE:
                std::cout << "-------------------------------\n      >>> Ars Timoris <<<\n-------------------------------\n1) New Game\n2) Quit\n------------------------------\n\nOption: ";
                SafeInput<uint32_t>(choice);
                switch (choice) {
                    case 1:
                        gameState.screen = Screen::GAME_SELECT;
                        break;
                    case 2:
                        gameState.running = false;
                        break;
                }
                break;
            case Screen::GAME_SELECT:
                GameState::DisplayStarts();
                
                std::cout << "\nOption: ";

                SafeInput<uint32_t>(choice);

                if (--choice < GameState::Starts.size()) {
                    gameState.screen = Screen::GAME;
                    gameState.Start(choice);
                }
                break;
            case Screen::GAME:
                switch (gameState.menu) {
                    case Menu::NONE:
                        if (!gameState.rooms[gameState.curRoom].initialized) {
                            InitializeRoom(&gameState.rooms[gameState.curRoom]);
                            encounter = GetRandomEncounter();
                            if (ENCOUNTERS[encounter].event != nullptr) {
                                runEncounter = true;
                            }
                        }
                    
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout 
                            << "-------------------------------\n>>> " 
                            << ROOM_DATA[room->roomID].roomName 
                            << " <<<\n\n>" 
                            << ROOM_DATA[room->roomID].roomDescription 
                            << "<\n" << std::endl;
                        
                        if (runEncounter) {
                            ENCOUNTERS[encounter].event(gameState);
                            runEncounter = false;
                        }

                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0) {
                            gameState.menu = Menu::COMBAT;
                            break;
                        }

                        if (room->roomActions.size() > 0) {
                            for (const RoomActionReference& roomAction : room->roomActions) {
                                if (roomAction.roomAction.roomDescription != nullptr) {
                                    roomAction.roomAction.roomDescription(gameState);
                                }
                            }
                        }
                        
                        std::cout << "\nActions:\n1) Move\n2) Actions\n3) Stats\n4) Inventory\n5) Quit\n\nOption: ";
                        SafeInput<uint32_t>(choice);
                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::MOVING;
                                break;
                            case 2:
                                gameState.menu = Menu::ROOM_ACTIONS;
                                break;
                            case 3:
                                gameState.menu = Menu::STATS;
                                break;
                            case 4:
                                gameState.menu = Menu::INVENTORY;
                                break;
                            case 5:
                                gameState.screen = Screen::TITLE;
                                gameState.menu = Menu::NONE;
                                break;
                            case 333:
                                if (gameState.debugMode) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (EDIT)\x1b[22m\n1) Change Gold\n2) Change XP\n3) Back\n\nOption: ";
                                    SafeInput<uint32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            std::cout << "Gold to add: ";
                                            SafeInput<int32_t>(number);
                                            gameState.player.gold += number;
                                            break;
                                        case 2:
                                            std::cout << "XP to add: ";
                                            SafeInput<int32_t>(number);
                                            gameState.player.xp += number;
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            case 444:
                                if (gameState.debugMode) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (INFO)\x1b[22m\n1) View Classes\n2) View Items\n3) Back\n\nOption: ";
                                    SafeInput<uint32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            for (std::pair<std::string, Class> classPair : CLASSES) {
                                                const Class& classData = classPair.second;
                                                std::cout << classData.name << "\n- " << classData.description << "\nLEVELS\n";
                                                size_t levelIndex = 1;
                                                for (const ClassLevel& classLevel : classData.levels) {
                                                    std::cout << levelIndex++ << ". ";
                                                    classLevel.displayRequirements(gameState, gameState.player);
                                                    std::cout << "\n";
                                                }
                                                std::cout << std::endl;
                                            }
                                            break;
                                        case 2:
                                            std::cout << "This is exceedingly difficult to implement.\n" << std::endl;
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            case 666:
                                gameState.debugMode = !gameState.debugMode;
                                break;
                        }
                        break;
                    case Menu::MOVING:
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nRooms:\n";
                        for (size_t connectionIndex = 0; connectionIndex < room->connections.size();) {
                            std::cout << ++connectionIndex << ") [" << room->connections[connectionIndex - 1].destination  << "] \x1b[38;5;" << (gameState.rooms[room->connections[connectionIndex - 1].destination].initialized ? "8m" : "15m") << ROOM_DATA[gameState.rooms[room->connections[connectionIndex - 1].destination].roomID].roomName << "\x1b[39m\n";
                        }
                        std::cout << "\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < room->connections.size()) {
                            if (room->connections[choice].CanPass(gameState)) {
                                room->connections[choice].Passes(gameState);
                                gameState.curRoom = room->connections[choice].destination;
                            }
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    case Menu::ROOM_ACTIONS:
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nActions:\n";
                        for (size_t i = 0; i < room->roomActions.size(); ++i) {
                            std::cout << (i + 1) << ") " << room->roomActions[i].roomAction.name << "\n- " << room->roomActions[i].roomAction.description << "\n";
                        }
                        std::cout << (room->roomActions.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < room->roomActions.size()) {
                            if (room->roomActions[choice].roomAction.condition(gameState)) {
                                room->roomActions[choice].roomAction.usage(gameState);
                            }
                            gameState.menu = Menu::NONE;
                        } else if (choice == room->roomActions.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    case Menu::STATS:
                        std::cout 
                            << "-------------------------------\n\x1b[1mStats:\x1b[22m\nHP: " 
                            << gameState.player.curHP << "/" << gameState.player.maxHP 
                            << "\nMana: " 
                            << gameState.player.curMana << "/" << gameState.player.maxMana 
                            << "\nArmor: "
                            << gameState.player.GetEffectiveArmor()
                            << " ("
                            << gameState.player.armor
                            << " + "
                            << (gameState.player.GetEffectiveArmor() - gameState.player.armor)
                            << ")\nXP: "
                            << gameState.player.xp
                            << "\nGold: "
                            << gameState.player.gold
                            << "\n\n\x1b[1mClasses:\x1b[22m\n";

                        for (const std::pair<std::string, ClassInstance>& classPair : gameState.player.classes) {
                            std::cout << classPair.first << "(" << classPair.second.level << "/" << CLASSES.at(classPair.first).levels.size() << "): " << CLASSES.at(classPair.first).description << "\n";
                        }
                            
                        std::cout << "\n\x1b[1mSkills:\x1b[22m\n";
                        
                        for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                            std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                        }
                            
                        std::cout << "\nOptions:\n1) Level Up\n2) Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::LEVEL_UP;
                                break;
                            case 2:
                                gameState.menu = Menu::NONE;
                                break;
                        }
                        break;
                    case Menu::LEVEL_UP: {
                        std::cout 
                            << "-------------------------------\nXP: " 
                            << gameState.player.xp
                            << "\nClasses:\n";
                        
                        std::vector<std::string> classics = std::vector<std::string>();
                        size_t accum = 1;
                        for (const std::pair<std::string, Class>& classPair : CLASSES) {
                            classics.push_back(classPair.first);
                            std::cout << (accum++) << ") " << classPair.first << "(" << (gameState.player.classes.contains(classPair.first) ? (gameState.player.classes.at(classPair.first).level) : 0) << "/" << classPair.second.levels.size() << "): " << classPair.second.description << "\n";
                            size_t levelToUse = (gameState.player.classes.contains(classPair.second.name) ? (gameState.player.classes.at(classPair.second.name).level + 1) : 1);
                            //std::cout << "Level: " << levelToUse << " : " << classPair.second.levels.size() << std::endl;
                            if (classPair.second.levels.size() >= levelToUse && classPair.second.levels[--levelToUse].displayRequirements != nullptr) {
                                std::cout << "- \x1b[" << ((classPair.second.levels[levelToUse].applicable == nullptr || classPair.second.levels[levelToUse].applicable(gameState, gameState.player)) ? "32m" : "31m");
                                classPair.second.levels[levelToUse].displayRequirements(gameState, gameState.player);
                                std::cout << "\x1b[39m\n";
                            }
                        }
                            
                        std::cout << accum << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < CLASSES.size()) {
                            const Class& classRef = CLASSES.at(classics[choice]);
                            size_t levelToUseRedone = (gameState.player.classes.contains(classRef.name) ? (gameState.player.classes.at(classRef.name).level + 1) : 1);
                            if (classRef.levels.size() < levelToUseRedone) {
                                std::cout << "Already max level." << std::endl;
                            } else  {
                                if (classRef.levels[levelToUseRedone - 1].applicable == nullptr || classRef.levels[levelToUseRedone - 1].applicable(gameState, gameState.player)) {
                                    for (size_t levelUpEffect = 0; levelUpEffect < classRef.levels[levelToUseRedone - 1].levelUpEffects.size(); ++levelUpEffect) {
                                        classRef.levels[levelToUseRedone - 1].levelUpEffects[levelUpEffect](gameState, gameState.player, false);
                                    }

                                    if (gameState.player.classes.contains(classRef.name)) {
                                        ++gameState.player.classes.at(classRef.name).level;
                                    } else {
                                        gameState.player.classes.insert(std::pair<std::string, ClassInstance>(classRef.name, ClassInstance(classRef.name, 1)));
                                    }
                                }
                            }
                        } else if (choice == CLASSES.size()) {
                            gameState.menu = Menu::STATS;
                        }
                        break;
                    }
                    case Menu::INVENTORY:
                        std::cout << "-------------------------------\nInventory:\n";
                        for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                            std::cout << ++inventoryItem << ") [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                        }
                        std::cout << (gameState.player.items.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < gameState.player.items.size()) {
                            size_t itemIndex = choice;
                            ItemStack& itemStack = gameState.player.items[itemIndex];
                            const ItemData& itemType = ITEM_DATA[itemStack.itemID];
                            std::cout << "-------------------------------\n" << itemType.name << " x" << itemStack.stackSize << "/" << itemType.maxStack << "\n- " << itemType.description << "\nEquipped [" << (gameState.player.equipped[itemIndex] ? 'X' : ' ') << "]\n";
                            for (const std::pair<std::string, int32_t>& modPair : itemStack.metadata) {
                                std::cout << "- " << modPair.first << ": " << modPair.second << "\n";
                            }
                            std::cout << "\n1) Use\n2) " << (gameState.player.equipped[itemIndex] ? "Unequip" : "Equip") << "\n3) Back\n\nOption: ";
                            SafeInput<uint32_t>(choice);
                            switch (choice) {
                                case 1:
                                    if (itemType.usage.usage != nullptr) {
                                        if (itemType.usage.condition == nullptr || itemType.usage.condition(gameState, itemStack, itemIndex)) {
                                            itemType.usage.usage(gameState, itemStack, itemIndex);
                                        } else {
                                            std::cout << "You don't know why you would use this right now." << std::endl;
                                        }
                                    } else {
                                        std::cout << "This item is not usable." << std::endl;
                                    }
                                    break;
                                case 2:
                                    gameState.player.equipped[itemIndex] = !gameState.player.equipped[itemIndex];
                                    break;
                            }
                        } else if (choice == gameState.player.items.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    case Menu::COMBAT:
                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0) {
                            bool enemiesGo = true;
                            std::cout << "-------------------------------\nOpponents:\n";
                            for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                std::cout << (npcIndex + 1)  << ") " << room->inhabitants[npcIndex].name<< "\n";

                                if (
                                    gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT))
                                ) {
                                    std::cout << "- HP [" << room->inhabitants[npcIndex].curHP << "/" << room->inhabitants[npcIndex].maxHP << "]\n";
                                }

                                if (
                                    gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
                                    gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))
                                ) {
                                    std::cout << "- Mana [" << room->inhabitants[npcIndex].curMana << "/" << room->inhabitants[npcIndex].maxMana << "]\n";
                                }
                            }
                            std::cout << "\n\nTurns: " << gameState.player.usedTurns << "/" << gameState.player.turns << "\n-------------------------------\nActions:\n";
                            for (size_t i = 0; i < gameState.player.actions.size(); ++i) {
                                std::cout << (i + 1) << ") " << gameState.player.actions[i].name << "\n";
                            }
                            std::cout << "\nOption: ";

                            SafeInput<uint32_t>(choice);

                            if (--choice < gameState.player.actions.size()) {
                                size_t action = choice;
                                std::cout << "-------------------------------\nTargets:\n";
                                for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                    std::cout << (npcIndex + 1)  << ") " << room->inhabitants[npcIndex].name<< "\n";

                                    if (
                                        gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
                                        gameState.player.GetSkillModifier("Brawler") >= 6
                                    ) {
                                        std::cout << "- HP [" << room->inhabitants[npcIndex].curHP << "/" << room->inhabitants[npcIndex].maxHP << "]\n";
                                    }

                                    if (
                                        gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
                                        gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))
                                    ) {
                                        std::cout << "- Mana [" << room->inhabitants[npcIndex].curMana << "/" << room->inhabitants[npcIndex].maxMana << "]\n";
                                    }
                                }
                                std::cout << (room->inhabitants.size() + 1)  << ") Yourself\n- HP [" << gameState.player.curHP << "/" << gameState.player.maxHP << "]\n- Mana [" << gameState.player.curMana << "/" << gameState.player.maxMana << "]\n"<< (room->inhabitants.size() + 2)  << ") Back\n\nOption: ";
                                SafeInput<uint32_t>(choice);
                                if (--choice < room->inhabitants.size()) {
                                    std::cout << "-------------------------------\n";
                                    if (gameState.player.actions[action].condition == nullptr || gameState.player.actions[action].condition(gameState, &gameState.player, &room->inhabitants[choice])) {
                                        gameState.player.actions[action].action(gameState, &gameState.player, &room->inhabitants[choice]);
                                    }
                                } else if (choice == room->inhabitants.size()) {
                                    std::cout << "-------------------------------\n";
                                    if (gameState.player.actions[action].condition == nullptr || gameState.player.actions[action].condition(gameState, &gameState.player, &gameState.player)) {
                                        gameState.player.actions[action].action(gameState, &gameState.player, &gameState.player);
                                    }
                                } else {
                                    enemiesGo = false;
                                    if (choice != room->inhabitants.size() + 1) {
                                        std::cout << "Invalid choice." << std::endl;
                                    }
                                }

                                if (gameState.player.usedTurns < gameState.player.turns) {
                                    enemiesGo = false;
                                }

                                if (room->LivingInhabitants() <= 0) {
                                    enemiesGo = true;
                                }

                                if (enemiesGo) {
                                    gameState.player.usedTurns = 0;

                                    for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                        if (room->inhabitants[npcIndex].stunned) {
                                            room->inhabitants[npcIndex].stunned = false;
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is stunned." << std::endl;
                                        } else if (room->inhabitants[npcIndex].curHP <= 0) {
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is dead." << std::endl;
                                        } else if (room->inhabitants[npcIndex].aiFunction != nullptr) {
                                            (*room->inhabitants[npcIndex].aiFunction)(gameState, room->inhabitants[npcIndex]);
                                        }
                                        EatInput();
                                    }

                                    room = &gameState.rooms[gameState.curRoom];

                                    std::vector<size_t> remove = std::vector<size_t>();

                                    for (int32_t j = room->inhabitants.size() - 1; j > -1; --j) {
                                        if (room->inhabitants[j].curHP <= 0) {
                                            gameState.player.xp += room->inhabitants[j].xp;
                                            gameState.player.gold += room->inhabitants[j].gold;
                                            for (size_t k = 0; k < room->inhabitants[j].onDeath.size(); ++k) {
                                                gameState.rooms[gameState.curRoom].inhabitants[j].onDeath[k](gameState, gameState.rooms[gameState.curRoom].inhabitants[j]);
                                            }
                                            remove.push_back(j);
                                        } else {
                                            room->inhabitants[j].TurnEnd(gameState);
                                        }
                                    }

                                    for (const size_t& toRemove : remove) {
                                        room->inhabitants.erase(room->inhabitants.begin() + toRemove);
                                        room = &gameState.rooms[gameState.curRoom];
                                    }

                                    if (gameState.player.curHP <= 0) {
                                        std::cout << "\nYou collapse to the floor, dead." << std::endl;
                                        gameState.screen = Screen::GAME_OVER;
                                        gameState.menu = Menu::NONE;
                                    } else {
                                        gameState.player.TurnEnd(gameState);
                                    }
                                }
                            }
                        } else {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                }
                break;
            case Screen::GAME_OVER:
                room = &gameState.rooms[gameState.curRoom];
                const RoomData& roomType = ROOM_DATA[room->roomID];
                std::cout 
                    << "-------------------------------\n> >> >>> MEMENTO  MORI <<< << <\n"
                    << ">>> STATISTICS\n>> Exploration\nRooms: "
                    << gameState.usedRooms
                    << "\n\n>> Death Room\nName: "
                    << roomType.roomName
                    << "\nDescription: "
                    << roomType.roomDescription
                    << "\nIndex: "
                    << gameState.curRoom
                    << "\n\n>>> PLAYER\n>> Stats\nHP: " 
                    << gameState.player.maxHP 
                    << "\nMana: " 
                    << gameState.player.maxMana 
                    << "\nXP: " 
                    << gameState.player.xp 
                    << "\nGold: " 
                    << gameState.player.gold 
                    << "\n\n>> Classes\n";

                for (const std::pair<std::string, ClassInstance>& classPair : gameState.player.classes) {
                    std::cout << classPair.first << "(" << classPair.second.level << "/" << CLASSES.at(classPair.first).levels.size() << "): " << CLASSES.at(classPair.first).description << "\n";
                }
                    
                std::cout << "\n>> Perks\n";

                for (size_t perkIndex = 0; perkIndex < (size_t)Perks::SIZE; ++perkIndex) {
                    if (gameState.player.perks.test(perkIndex)) {
                        switch ((Perks)perkIndex) {
                            case Perks::INSIGHT:
                                std::cout << " - Insight\n";
                                break;
                            case Perks::ARCANE_EYES:
                                std::cout << " - Arcane Eyes\n";
                                break;
                            case Perks::HARD_HITTER:
                                std::cout << " - Hard Hitter\n";
                                break;
                            case Perks::HORDE_SLAYER:
                                std::cout << " - Horde Slayer\n";
                                break;
                        }
                    }
                }
                
                std::cout << "\n>> Skills\n";

                for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                    std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                }

                std::cout << "\n>> Inventory\n";
                for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                    std::cout << ++inventoryItem << ". [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                }

                std::cout << "-------------------------------\n1) Title Screen\n2) Quit\n------------------------------\n\nOption: ";
                SafeInput<uint32_t>(choice);
                switch (choice) {
                    case 1:
                        gameState.screen = Screen::TITLE;
                        break;
                    case 2:
                        gameState.running = false;
                        break;
                }
                break;
        }
    }

    return 0;
}