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


//#define SCRIPT_PARSER_DEBUG_LOGGING 1
//#define STRING_UTILS_DEBUG_LOGGING 1


//#define LEGACY_COMMA_REPLACEMENT 1

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

#pragma region String Utils
bool IsStringBool(std::string_view a_string) {
    return a_string == "true" || a_string == "false";
}

bool IsStringFloat(std::string_view a_string) {
    bool poppedDot = false;
    for (size_t i = (a_string.starts_with('-') ? 1 : 0); i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            case '.':
                if (!poppedDot) {
                    poppedDot = true;
                    break;
                }
            default:
                return false;
        }
    }
    return true;
}

bool IsStringInt(std::string_view a_string) {
    for (size_t i = (a_string.starts_with('-') ? 1 : 0); i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            default:
                return false;
                break;
        }
    }
    return true;
}

size_t SplitString(const std::string& a_string, std::string_view a_seperator, std::vector<std::string>& a_vector) {
    if (a_string.empty()) {
        return 0;
    }
    int32_t previous = 0;
    int32_t cur = 0;
    while ((cur = a_string.find(a_seperator, cur)) < a_string.size()) {
        a_vector.push_back(a_string.substr(previous, cur - previous));
        #ifdef STRING_UTILS_DEBUG_LOGGING
        std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
        #endif
        previous = ++cur;
    }
    a_vector.push_back(a_string.substr(previous, cur - previous));
    #ifdef STRING_UTILS_DEBUG_LOGGING
    std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
    #endif
    return a_vector.size();
}

size_t ParseFunctionArguments(const std::string& a_string, std::vector<std::string>& a_vector) {
    #ifdef STRING_UTILS_DEBUG_LOGGING
    std::cout << "Paring Function Arguments String:\n" << a_string << std::endl;
    #endif
    if (a_string.empty()) {
        return 0;
    }
    #ifdef STRING_UTILS_DEBUG_LOGGING
    int32_t step = 0;
    #endif
    int32_t internal = 0;
    bool inString = false;
    bool escaped = false;
    std::string stringHolder = "";
    for (char character : a_string) {
        #ifdef STRING_UTILS_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mFN_ARG\x1b[22m Step {:0>4}) {}InString\x1b[39m {}Escaped\x1b[39m Character: {} String [{:0>2}]: [{:0>3}] {}", step++, (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), character, a_vector.size(), stringHolder.size(), stringHolder) << std::endl;
        #endif
        switch (character) {
            case '"': {
                if (inString) {
                    if (escaped) {
                        stringHolder += character;
                    } else {
                        inString = false;
                    }
                    escaped = false;
                } else {
                    inString = true;
                }
                break;
            }
            case 'n': {
                if (inString && escaped) {
                    stringHolder += '\\';
                    escaped = false;
                }
                stringHolder += character;
                break;
            }
            case '\\': {
                if (inString) {
                    if (escaped) {
                        stringHolder += character;
                    }
                    escaped = !escaped;
                } else {
                    stringHolder += character;
                }
                break;
            }
            case ',': {
                if (!inString) {
                    if (internal > 0) {
                        stringHolder += character;
                    } else if (!stringHolder.empty()) {
                        a_vector.push_back(stringHolder);
                        stringHolder = "";
                    }
                } else {
                    stringHolder += character;
                    escaped = false;
                }
                break;
            }
            case '(': {
                stringHolder += character;
                if (inString) {
                    escaped = false;
                } else {
                    ++internal;
                }
                break;
            }
            case ')': {
                stringHolder += character;
                if (inString) {
                    escaped = false;
                } else {
                    --internal;
                }
                break;
            }
            default: {
                stringHolder += character;
                escaped = false;
                break;
            }
        }
    }
    if (!stringHolder.empty()) {
        a_vector.push_back(stringHolder);
    }
    return a_vector.size();
}

void SanitizeString(std::string& a_string) {
    size_t startOff = 0;
    size_t endOff = a_string.size();
    bool startDone = false;
    for (size_t i = 0; i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case ' ':
            case '\n':
            case '\t':
            case '\v':
                break;
            default:
                if (startDone) {
                    endOff = i;
                } else {
                    startOff = i;
                    startDone = true;
                }
                break;
        }
    }
    #ifdef STRING_UTILS_DEBUG_LOGGING
    //std::cout << std::format("<{}>:<{}>:<{}>", a_string.substr(0, startOff + 1), a_string.substr(startOff, endOff - startOff + 1), a_string.substr(endOff, a_string.size() - endOff + 1)) << std::endl;
    #endif
    a_string = a_string.substr(startOff, endOff - startOff + 1);
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
struct Interpreter;

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

std::string PerkDisplayName(Perks a_perk) {
    switch (a_perk) {
        case Perks::INSIGHT:
            return "INSIGHT";
        case Perks::ARCANE_EYES:
            return "ARCANE_EYES";
        case Perks::HARD_HITTER:
            return "HARD_HITTER";
        case Perks::HORDE_SLAYER:
            return "HORDE_SLAYER";
        default:
            return "INVALID";
    }
}

Perks PerkFromName(const std::string& a_perk) {
    if (a_perk == "INSIGHT") {
        return Perks::INSIGHT;
    } else if (a_perk == "ARCANE_EYES") {
        return Perks::ARCANE_EYES;
    } else if (a_perk == "HARD_HITTER") {
        return Perks::HARD_HITTER;
    } else if (a_perk == "HORDE_SLAYER") {
        return Perks::HORDE_SLAYER;
    } else {
        return Perks::SIZE;
    }
}

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

#pragma region SockScript Setup
struct VariablePtr {
public:
    void* data;
    size_t size;

    VariablePtr(const void* a_data, size_t a_size) {
        this->data = malloc(a_size);
        this->size = a_size;
        
        #ifdef __STDC_LIB_EXT1__
        memcpy_s(this->data, a_size, a_data, a_size);
        #else
        memcpy(this->data, a_data, a_size);
        #endif
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

    ~VariablePtr() {
        free(this->data);
    }
};

enum struct ReaderMode {
    READ,
    VAR_NAME,
    VAR_TYPE,
    VAR_VALUE,
    VAR_EXPR,
    RET_EXPR,
    OBJECT,
    EXPRESSION,
    ARGS,
    FALLTHROUGH,
    FINISHED,
};

std::string ReaderModeDisplayName(ReaderMode a_mode) {
    switch (a_mode) {
        case ReaderMode::READ:
            return "READING";
        case ReaderMode::VAR_NAME:
            return "VAR_NAM";
        case ReaderMode::VAR_TYPE:
            return "VAR_TYP";
        case ReaderMode::VAR_VALUE:
            return "VAR_VAL";
        case ReaderMode::VAR_EXPR:
            return "VAR_EXP";
        case ReaderMode::RET_EXPR:
            return "RET_EXP";
        case ReaderMode::OBJECT:
            return "OBJECTS";
        case ReaderMode::EXPRESSION:
            return "EXPRESS";
        case ReaderMode::ARGS:
            return "ARGUMEN";
        case ReaderMode::FALLTHROUGH:
            return "FALLTHR";
        case ReaderMode::FINISHED:
            return "FINISHE";
        default:
            return "INVALID";
    }
}

enum struct OperatorEvaluation {
    NONE,
    ADDI,
    SUBT,
    MULT,
    DIVI,
    MODU,
    NOT,
    AND,
    OR,
    GT,
    GTE,
    LT,
    LTE,
    EQ,
    NEQ,
};

std::string OperatorEvaluationDisplayName(OperatorEvaluation a_operator) {
    switch (a_operator) {
        case OperatorEvaluation::NONE:
            return "NON";
        case OperatorEvaluation::ADDI:
            return "ADD";
        case OperatorEvaluation::SUBT:
            return "SUB";
        case OperatorEvaluation::MULT:
            return "MUL";
        case OperatorEvaluation::DIVI:
            return "DIV";
        case OperatorEvaluation::MODU:
            return "MOD";
        case OperatorEvaluation::NOT:
            return "NOT";
        case OperatorEvaluation::AND:
            return "AND";
        case OperatorEvaluation::OR:
            return "IOR";
        case OperatorEvaluation::GT:
            return "GRT";
        case OperatorEvaluation::GTE:
            return "GTE";
        case OperatorEvaluation::LT:
            return "LST";
        case OperatorEvaluation::LTE:
            return "LTE";
        case OperatorEvaluation::EQ:
            return "EQU";
        case OperatorEvaluation::NEQ:
            return "NEQ";
        default:
            return "IVD";
    }
}

enum struct RegisterType {
    ERROR,
    STRING,
    INT,
    FLOAT,
    BOOL,
};

struct InterpreterContext {
public:
    std::vector<std::string> stringRegisters = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    std::vector<int32_t> intRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> floatRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<bool> boolRegisters = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
    std::vector<std::string> context = std::vector<std::string>();
    int32_t stringRegister = 0;
    int32_t intRegister = -1;
    int32_t floatRegister = -1;
    int32_t boolRegister = -1;
    size_t targetLevel = 0;
    size_t currentLevel = 0;
    RegisterType lastRegister = RegisterType::ERROR;
    OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
    std::vector<ReaderMode> mode = {ReaderMode::READ};

    void DumpStringRegister(void) {
        for (size_t i = 0; i < stringRegisters.size(); ++i) {
            std::cout << std::format("{}String Register {:0>2}: {}\x1b[39m", i == stringRegister ? "\x1b[32m" : "\x1b[34m", i + 1, stringRegisters[i]) << std::endl;
        }
    }

    void DumpIntRegister(void) {
        for (size_t i = 0; i < intRegisters.size(); ++i) {
            std::cout << (i == intRegister ? "\x1b[32m" : "\x1b[34m") << "Int Register " << (i + 1) << ": " << intRegisters[i] << "\x1b[39m" << std::endl;
        }
    }
    
    void DumpFloatRegister(void) {
        for (size_t i = 0; i < floatRegisters.size(); ++i) {
            std::cout << (i == floatRegister ? "\x1b[32m" : "\x1b[34m") << "Float Register " << (i + 1) << ": " << floatRegisters[i] << "\x1b[39m" << std::endl;
        }
    }
    
    void DumpBoolRegister(void) {
        for (size_t i = 0; i < boolRegisters.size(); ++i) {
            std::cout << (i == boolRegister ? "\x1b[32m" : "\x1b[34m") << "Bool Register " << (i + 1) << ": " << (boolRegisters[i] ? "true" : "false") << "\x1b[39m" << std::endl;
        }
    }

    RegisterType SetIntRegister(int32_t a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                intRegisters[++intRegister] = a_value;
                lastRegister = RegisterType::INT;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::ADDI:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        intRegisters[intRegister] = intRegisters[intRegister] + a_value;
                        lastRegister = RegisterType::INT;
                        return RegisterType::INT;
                    case RegisterType::FLOAT:
                        intRegisters[intRegister] = floatRegisters[floatRegister] + a_value;
                        lastRegister = RegisterType::FLOAT;
                        return RegisterType::FLOAT;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::SUBT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        intRegisters[intRegister] = intRegisters[intRegister] - a_value;
                        lastRegister = RegisterType::INT;
                        return RegisterType::INT;
                    case RegisterType::FLOAT:
                        intRegisters[intRegister] = floatRegisters[floatRegister] - a_value;
                        lastRegister = RegisterType::FLOAT;
                        return RegisterType::FLOAT;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::MULT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        intRegisters[intRegister] = intRegisters[intRegister] * a_value;
                        lastRegister = RegisterType::INT;
                        return RegisterType::INT;
                    case RegisterType::FLOAT:
                        intRegisters[intRegister] = floatRegisters[floatRegister] * a_value;
                        lastRegister = RegisterType::FLOAT;
                        return RegisterType::FLOAT;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::DIVI:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        intRegisters[intRegister] = intRegisters[intRegister] / a_value;
                        lastRegister = RegisterType::INT;
                        return RegisterType::INT;
                    case RegisterType::FLOAT:
                        intRegisters[intRegister] = floatRegisters[floatRegister] / a_value;
                        lastRegister = RegisterType::FLOAT;
                        return RegisterType::FLOAT;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::MODU:
                intRegisters[intRegister] = intRegisters[intRegister] % a_value;
                lastRegister = RegisterType::INT;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::GT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] > a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::GTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] >= a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::LT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] < a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::LTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] <= a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::EQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] == a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::NEQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] != a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            default:
                lastRegister = RegisterType::ERROR;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
        
    }

    RegisterType SetFloatRegister(float a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                floatRegisters[++floatRegister] = a_value;
                lastRegister = RegisterType::FLOAT;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::ADDI:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        floatRegisters[floatRegister] = intRegisters[intRegister] + a_value;
                        break;
                    case RegisterType::FLOAT:
                        floatRegisters[floatRegister] = floatRegisters[floatRegister] + a_value;
                        break;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
                lastRegister = RegisterType::FLOAT;
                return RegisterType::FLOAT;
            case OperatorEvaluation::SUBT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        floatRegisters[floatRegister] = intRegisters[intRegister] - a_value;
                        break;
                    case RegisterType::FLOAT:
                        floatRegisters[floatRegister] = floatRegisters[floatRegister] - a_value;
                        break;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
                lastRegister = RegisterType::FLOAT;
                return RegisterType::FLOAT;
            case OperatorEvaluation::MULT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        floatRegisters[floatRegister] = intRegisters[intRegister] * a_value;
                        break;
                    case RegisterType::FLOAT:
                        floatRegisters[floatRegister] = floatRegisters[floatRegister] * a_value;
                        break;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
                lastRegister = RegisterType::FLOAT;
                return RegisterType::FLOAT;
            case OperatorEvaluation::DIVI:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        floatRegisters[floatRegister] = intRegisters[intRegister] / a_value;
                        break;
                    case RegisterType::FLOAT:
                        floatRegisters[floatRegister] = floatRegisters[floatRegister] / a_value;
                        break;
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
                lastRegister = RegisterType::FLOAT;
                return RegisterType::FLOAT;
            case OperatorEvaluation::GT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] > a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::GTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] >= a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::LT:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] < a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::LTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] <= a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::EQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] == a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            case OperatorEvaluation::NEQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                switch (lastRegister) {
                    case RegisterType::INT:
                        return SetBoolRegister(intRegisters[intRegister] != a_value);
                    case RegisterType::FLOAT:
                        return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                    case RegisterType::BOOL:
                    case RegisterType::STRING:
                    case RegisterType::ERROR:
                        lastRegister = RegisterType::ERROR;
                        return RegisterType::ERROR;
                }
            default:
                lastRegister = RegisterType::ERROR;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
      
    }

    RegisterType SetBoolRegister(bool a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                boolRegisters[++boolRegister] = a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::NOT:
                boolRegisters[++boolRegister] = !a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::AND:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] && a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::OR:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] || a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::EQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] == a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::NEQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] != a_value;
                lastRegister = RegisterType::BOOL;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            default:
                lastRegister = RegisterType::ERROR;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
    }
};

struct Interpreter {
public:
    std::unordered_map<std::string, VariablePtr> variables = std::unordered_map<std::string, VariablePtr>();

    Interpreter(void) {

    }

    template <typename T>
    T ParseStatement(GameState& a_gameState, RegisterType a_returnType, std::string a_statement);

    int32_t ParseIntExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    float ParseFloatExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    int32_t ParseBoolExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    std::string FormatString(GameState& a_gameState, InterpreterContext& a_context, std::string a_string);

    void ParseRawType(GameState& a_gameState, InterpreterContext& a_context);

    void ParseVariableName(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectName(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMember(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMethod(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMethodArgs(GameState& a_gameState, InterpreterContext& a_context);
};
#pragma endregion

struct DebugData {
public:
    bool enabled = false;
    bool noFights = false;
};

struct GameState {
public:
    static std::vector<StartData> Starts;

    DebugData debug = DebugData();

    bool running = true;
    Screen screen;
    Menu menu;

    size_t usedRooms;
    size_t curRoom;
    size_t start;
    std::vector<RoomInstance> rooms;
    PlayerData player;

    std::mt19937 generator;
    Interpreter interpreter;

    GameState(std::mt19937 a_generator, const Interpreter& a_interpreter) {
        this->generator = a_generator;
        this->interpreter = a_interpreter;
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

    int32_t RollDice(size_t a_amount, size_t a_die) {
        int32_t total = 0;
        std::uniform_int_distribution<uint32_t>& dieDistribution = DICE.at(a_die);
        for (size_t i = 0; i < a_amount; ++i) {
            total += dieDistribution(generator);
        }
        return total;
    }
};

struct Encounter {
public:
    std::string weight = "";
    std::string event = "";
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
                std:: cout << "We are in." << std::endl;
                std::cout << std::format("Running with {}d{}", a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count"), a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice")) << std::endl;
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Looted", 0);
                int32_t goldAmount = a_gameState.RollDice(a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count"), a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice"));
                a_gameState.player.gold += goldAmount;
                std::cout << "You find " << goldAmount << " (" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count") << "d" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice") << ") gold." << std::endl;
                EatInput();
            }
        }
    },
    {
        "Shop", RoomAction {
            "Shop",
            "Shop at the shop.",
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
};

const std::unordered_map<std::string, EntityTemplate> ENTITY_TEMPLATES = {
    {"Goblin", EntityTemplate(32, 0, 0, 6, {}, {})}
};

const std::unordered_map<std::string, NPCTemplate> NPC_TEMPLATES = {
    {
        "Goblin Shaman", NPCTemplate {
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
        }
    },
    {
        "Goblin Grunt", NPCTemplate {
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
    },
    {
        "Goblin Squire", NPCTemplate {
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
        }
    },
    {
        "Goblin Priest", NPCTemplate {
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
        }
    },
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

std::vector<Encounter> ENCOUNTERS = {
    /*
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
        [](GameState& a_gameState) {
            a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                NPCData(&ENTITY_TEMPLATES.at("Goblin"), &NPC_TEMPLATES[1])
            );
            a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                NPCData(&ENTITY_TEMPLATES.at("Goblin"), &NPC_TEMPLATES[1])
            );
            a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(
                NPC_MODIFIERS.at("Shaman")(
                    NPCData(&ENTITY_TEMPLATES.at("Goblin"), &NPC_TEMPLATES[0])
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
        [](GameState& a_gameState) {
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
        [](GameState& a_gameState) {
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
    },*/
};

std::vector<Encounter> LoadEncountersData(std::filesystem::path a_path) {
    std::vector<Encounter> loadedEncounters = std::vector<Encounter>();
    std::ifstream reader = std::ifstream(a_path);
    char buffer[256];
    char current = 0;
    std::string stringStorage = "";
    std::string weightStorage = "";
    std::string eventStorage = "";
    int32_t depth = 0;
    bool inString = false;
    bool inCode = false;
    uint32_t step = 0;

    bool running = true;

    while (running) {
        //std::cout << std::format("Step {}: ({:0>2}) [{:0>4}] {}", step, depth, (size_t)reader.tellg(), current) << std::endl;
        switch (step) {
            case 0:
                switch (current = reader.get()) {
                    case '{': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                ++depth;
                            }
                            weightStorage += current;
                        } else {
                            depth = 0;
                            inCode = true;
                        }
                        break;
                    }
                    case '"': {
                        if(inCode) {
                            inString = !inString;
                            weightStorage += current;
                        }
                        break;
                    }
                    case '}': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                --depth;
                                if (depth < 0) {
                                    inCode = false;
                                    depth = 0;
                                    ++step;
                                    //std::cout << weightStorage << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                } else {
                                    weightStorage += current;
                                }
                            } else {
                                weightStorage += current;
                            }
                        }
                        break;
                    }
                    default: {
                        if (inCode) {
                            weightStorage += current;
                        }
                        break;
                    }
                }
                break;
            case 1:
                switch (current = reader.get()) {
                    case '{': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                ++depth;
                            }
                            eventStorage += current;
                        } else {
                            inCode = true;
                        }
                        break;
                    }
                    case '"': {
                        if(inCode) {
                            inString = !inString;
                            eventStorage += current;
                        }
                        break;
                    }
                    case '}': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                --depth;
                                if (depth < 0) {
                                    depth = 0;
                                    inCode = false;
                                    //std::cout << eventStorage << std::endl;
                                    loadedEncounters.push_back(Encounter {weightStorage, eventStorage});
                                    weightStorage = "";
                                    eventStorage = "";
                                    reader.getline(buffer, 256);
                                    //std::cout << "Skipping: " << buffer << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << "Skipping: " << buffer << std::endl;
                                    reader.getline(buffer, 256, ';');
                                    stringStorage = buffer;
                                    //std::cout << "Checking: " << stringStorage << std::endl;
                                    if (stringStorage == "FINAL") {
                                        //std::cout << "Finished" << std::endl;
                                        running = false;
                                    } else {
                                        step = 0;
                                    }
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                } else {
                                    eventStorage += current;
                                }
                            } else {
                                eventStorage += current;
                            }
                        }
                        break;
                    }
                    default: {
                        if (inCode) {
                            eventStorage += current;
                        }
                        break;
                    }
                }
                break;
        }
    }

    reader.close();
    
    return loadedEncounters;
}

#pragma region Sockscript
template <typename T>
T Interpreter::ParseStatement(GameState& a_gameState, RegisterType a_returnType, std::string a_statement) {
    InterpreterContext context = InterpreterContext();
    std::vector<std::pair<int32_t, bool>> ifTrue = {};
    int32_t ifLevel = 0;
    bool inString = false;
    bool escaped = false;
    int32_t embedLevel = 0;
    int32_t innerLevel = 0;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_statement) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mPARSER\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v': {
                        if (context.stringRegisters[context.stringRegister] == "return") {
                            context.stringRegisters[context.stringRegister] = "";
                            context.mode.push_back(ReaderMode::RET_EXPR);
                        }
                        break;
                    }
                    case '<': {
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_TYPE);
                        break;
                    }
                    case ';': {
                        if (context.stringRegisters[context.stringRegister] == "end") {
                            if (ifTrue.size() > 0 && ifTrue.back().first == context.currentLevel) {
                                ifTrue.pop_back();
                            } else {
                                context.mode.push_back(ReaderMode::FINISHED);
                            }
                        }
                        context.stringRegisters[context.stringRegister] = "";
                        break;
                    }
                    case '{': {
                        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                        std::cout << "Bracket hit with: " << context.stringRegisters[context.stringRegister] << std::endl;
                        #endif
                        if (context.stringRegisters[context.stringRegister] == "else") {
                            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                            std::cout << "Truism: <" << ifTrue.back().first << ", " << (ifTrue.back().second ? "true" : "false") << ">" << std::endl;
                            #endif
                            if (ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            } else {
                                ++context.currentLevel;
                            }
                        } else {
                            ++context.currentLevel;
                        }
                        context.stringRegisters[context.stringRegister] = "";
                        break;
                    }
                    case '}': {
                        --context.currentLevel;
                        while (ifTrue.back().first > context.currentLevel) {
                            ifTrue.pop_back();
                        }
                        break;
                    }
                    case '.': {
                        ParseObjectName(a_gameState, context);
                        break;
                    }
                    case '(': {
                        if (context.stringRegisters[context.stringRegister] == "if") {
                            context.stringRegisters[context.stringRegister] = "";
                            context.targetLevel = context.currentLevel;
                            context.mode.push_back(ReaderMode::EXPRESSION);
                        } else if (context.stringRegisters[context.stringRegister] == "elif") {
                            context.stringRegisters[context.stringRegister] = "";
                            if (ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            } else {
                                ifTrue.pop_back();
                                context.mode.push_back(ReaderMode::EXPRESSION);
                            }
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::VAR_TYPE: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '>': {
                        context.mode.pop_back();
                        context.mode.push_back(ReaderMode::VAR_VALUE);
                        context.stringRegister += 1;
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_VALUE: {
                switch (character) {
                    case ';':
                        std::cout << "[ERROR] Semicolon before variable expression provided." << std::endl;
                        break;
                    case '=': {
                        context.mode.pop_back();
                        context.mode.push_back(ReaderMode::VAR_EXPR);
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case ReaderMode::VAR_EXPR: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                            } else {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case ';': {
                        if (inString) { 
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        } else {
                            context.mode.pop_back();
                            std::string varName = context.stringRegisters[context.stringRegister - 2];
                            std::string varType = context.stringRegisters[context.stringRegister - 1];
                            if (varType == "float") {
                                int32_t value = ParseFloatExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                variables.insert_or_assign(varName, VariablePtr((void*)&value, sizeof(float)));
                            } else if (varType == "uint32") {
                                int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                variables.insert_or_assign(varName, VariablePtr((void*)&value, sizeof(int32_t)));
                            } else if (varType == "bool") {
                                bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                variables.insert_or_assign(varName, VariablePtr((void*)&value, sizeof(bool)));
                            } else if (varType == "string") {
                                std::cout << "String is not a supported data type." << std::endl;
                            }

                            //context.DumpStringRegister();
                            for (size_t i = 0; i < 3; ++i) {
                                context.stringRegisters[context.stringRegister--] = "";
                            }
                            context.stringRegister += 1;
                            //context.DumpStringRegister();
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                }
                break;
            }
            case ReaderMode::RET_EXPR: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                            } else {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case ';': {
                        if (inString) { 
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        } else {
                            context.mode.pop_back();
                            switch (a_returnType) {
                                case RegisterType::INT: {
                                    int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::FLOAT: {
                                    float value = ParseFloatExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::BOOL: {
                                    bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::STRING: {
                                    std::string value = FormatString(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::ERROR: {
                                    return (T)(0);
                                }
                            }
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                }
                break;
            }
            case ReaderMode::EXPRESSION: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(':
                        embedLevel += 1;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case ')': {
                        if (embedLevel-- <= 0) {
                            ifTrue.push_back(std::pair<int32_t, bool>(context.currentLevel, ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister])));
                            context.mode.pop_back();
                            if (!ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            }
                            context.stringRegisters[context.stringRegister] = "";
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::FALLTHROUGH: {
                switch (character) {
                    case '\\':
                        escaped = !escaped;
                        break;
                    case '"':
                        if (!escaped) {
                            inString = !inString;
                        }
                        escaped = false;
                        break;
                    case '{':
                        if (!inString && !escaped) {
                            ++context.currentLevel;
                        }
                        escaped = false;
                        break;
                    case '}':
                        if (!inString && !escaped) {
                            --context.currentLevel;
                            if (context.currentLevel == context.targetLevel) {
                                context.mode.pop_back();
                            }
                        }
                        escaped = false;
                        break;
                    default:
                        escaped = false;
                        break;
                }
                break;
            }
        }
    }

    switch (a_returnType) {
        case RegisterType::INT:
        case RegisterType::FLOAT:
        case RegisterType::ERROR:
        case RegisterType::BOOL:
            return (T)0;
        case RegisterType::STRING:
            return (T)"";
    }
    
    return (T)(0);
}

int32_t Interpreter::ParseIntExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Parsing Int Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    int32_t innerLevel = 0;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        std::cout << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '<':
                        context.operatorEvaluation = OperatorEvaluation::LT;
                        justOperated = true;
                        break;
                    case '>':
                        context.operatorEvaluation = OperatorEvaluation::GT;
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>4}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
    #endif

    if (context.stringRegisters[context.stringRegister] != "") {
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    int32_t toReturn;
    switch (context.lastRegister) {
        case RegisterType::INT:
            toReturn = context.intRegisters[context.intRegister];
            break;
        case RegisterType::FLOAT:
            toReturn = context.floatRegisters[context.floatRegister];
            break;
        case RegisterType::BOOL:
        case RegisterType::STRING:
        case RegisterType::ERROR:
            toReturn = 0;
            break;
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Returning: " << toReturn << std::endl;
    #endif

    return toReturn;
}

float Interpreter::ParseFloatExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Parsing Float Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    int32_t innerLevel = 0;

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mFLT_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        std::cout << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '<':
                        context.operatorEvaluation = OperatorEvaluation::LT;
                        justOperated = true;
                        break;
                    case '>':
                        context.operatorEvaluation = OperatorEvaluation::GT;
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << std::format("\x1b[1mFLT_EX\x1b[22m Step {:0>4}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
    #endif

    if (context.stringRegisters[context.stringRegister] != "") {
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    float toReturn;
    switch (context.lastRegister) {
        case RegisterType::INT:
            toReturn = context.intRegisters[context.intRegister];
            break;
        case RegisterType::FLOAT:
            toReturn = context.floatRegisters[context.floatRegister];
            break;
        case RegisterType::BOOL:
        case RegisterType::STRING:
        case RegisterType::ERROR:
            toReturn = 0;
            break;
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Returning: " << toReturn << std::endl;
    #endif

    return toReturn;
}

int32_t Interpreter::ParseBoolExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Parsing Bool Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    size_t innerLevel = 0;

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mBOO_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        std::cout << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '<':
                        context.operatorEvaluation = OperatorEvaluation::LT;
                        justOperated = true;
                        break;
                    case '>':
                        context.operatorEvaluation = OperatorEvaluation::GT;
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    if (context.stringRegisters[context.stringRegister] != "") {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mBOO_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Returning: " << (context.boolRegisters[context.boolRegister] ? "true" : "false") << std::endl;
    #endif

    return context.boolRegisters[context.boolRegister];
}

std::string Interpreter::FormatString(GameState& a_gameState, InterpreterContext& a_context, std::string a_string) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Formatting String: \n" << a_string << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();
    std::string result = "";
    int32_t innerLevel = 0;
    RegisterType registerType = RegisterType::INT;
    bool inString = true;
    bool escaped = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_string) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        if (inString) {
            std::cout << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), result.size(), result) << std::endl;
        } else {
            std::cout << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        }
        #endif

        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (inString) {
                            result += character;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString) {
                            if (escaped) {
                                result += '\n';
                            } else {
                                result += character;
                            }
                            escaped = false;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                    case '@':
                        if (inString) {
                            result += character;
                        } else {
                            context.stringRegister += 1;
                            context.mode.push_back(ReaderMode::VAR_NAME);
                        }
                        break;
                    case '{':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                inString = false;
                            }
                        } else {
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                        }
                        break;
                    case '}':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                std::cout << "[ERROR] Invalid characters." << std::endl;
                            }
                        } else {
                            inString = true;
                            // Match Register Type?
                            switch (registerType) {
                                case RegisterType::STRING:
                                    result += context.stringRegisters[context.stringRegister];
                                    break;
                                case RegisterType::INT:
                                    result += std::to_string(context.intRegisters[context.intRegister]);
                                    break;
                                case RegisterType::FLOAT:
                                    result += std::to_string(context.floatRegisters[context.floatRegister]);
                                    break;
                                case RegisterType::BOOL:
                                    result += context.boolRegisters[context.boolRegister] ? "true" : "false";
                                    break;
                            }
                        }
                        break;
                    case '.':
                        if (inString) {
                            result += character;
                        } else {
                            ParseObjectName(a_gameState, context);
                        }
                        break;
                    case '+':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::ADDI;
                        }
                        break;
                    case '-':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::SUBT;
                        }
                        break;
                    case '*':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::MULT;
                        }
                        break;
                    case '/':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::DIVI;
                        }
                        break;
                    default:
                        if (inString) {
                            result += character;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    return result;
}

void Interpreter::ParseRawType(GameState& a_gameState, InterpreterContext& a_context) {
    if (IsStringInt(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetIntRegister(std::stoi(a_context.stringRegisters[a_context.stringRegister]));
        a_context.stringRegisters[a_context.stringRegister] = "";
    } else if (IsStringFloat(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetFloatRegister(std::stof(a_context.stringRegisters[a_context.stringRegister]));
        a_context.stringRegisters[a_context.stringRegister] = "";
    } else if (IsStringBool(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetBoolRegister(a_context.stringRegisters[a_context.stringRegister] == "true");
        a_context.stringRegisters[a_context.stringRegister] = "";
    }
}

void Interpreter::ParseVariableName(GameState& a_gameState, InterpreterContext& a_context) {
    std::string varType = a_context.stringRegisters[a_context.stringRegister - 1];
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << std::format("Finished with the variable: {} {}", varType, a_context.stringRegisters[a_context.stringRegister]) << std::endl;
    std::cout << std::format("\x1b[1mVariables\x1b[22m ({})", variables.size()) << std::endl;
    //for (const std::pair<std::string, VariablePtr>& varPair : variables) {
    //    std::cout << std::format("Variable: [{}] {:0>3}", varPair.first, varPair.second.size) << std::endl;
    //}
    std::cout << "Prae Var Val" << std::endl;
    #endif
    if (varType == "int32") {
        std::cout << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], *((int32_t*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data))) << std::endl;
        a_context.SetIntRegister(*((int32_t*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data)));
    } else if (varType == "uint32") {
        std::cout << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], *((int32_t*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data))) << std::endl;
        a_context.SetIntRegister(*((int32_t*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data)));
    } else if (varType == "float") {
        std::cout << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], *((float*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data))) << std::endl;
        a_context.SetFloatRegister(*((float*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data)));
    } else if (varType == "bool") {
        std::cout << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], *((bool*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data))) << std::endl;
        a_context.SetBoolRegister(*((bool*)(variables.at(a_context.stringRegisters[a_context.stringRegister]).data)));
    }
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Post Var Val" << std::endl;
    #endif

    for (size_t i = 0; i < 2; ++i) {
        a_context.stringRegisters[a_context.stringRegister--] = "";
    }
    a_context.stringRegister += 1;
    a_context.mode.pop_back();
}

void Interpreter::ParseObjectName(GameState& a_gameState, InterpreterContext& a_context) {
    if (a_context.stringRegisters[a_context.stringRegister] == "player") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "mathf") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "mathi") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "random") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "console") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "room") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "gameState") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    }
}

void Interpreter::ParseObjectMember(GameState& a_gameState, InterpreterContext& a_context) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Operating on member " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
    #endif
    std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
    if (object == "player") {
        if (a_context.stringRegisters[a_context.stringRegister] == "maxMana") {
            a_context.SetIntRegister(a_gameState.player.maxMana);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "curMana") {
            a_context.SetIntRegister(a_gameState.player.curMana);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "maxHP") {
            a_context.SetIntRegister(a_gameState.player.maxHP);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "curHP") {
            a_context.SetIntRegister(a_gameState.player.curHP);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "armor") {
            a_context.SetIntRegister(a_gameState.player.armor);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "gold") {
            a_context.SetIntRegister(a_gameState.player.gold);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "xp") {
            a_context.SetIntRegister(a_gameState.player.xp);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "manaSickness") {
            a_context.SetIntRegister(a_gameState.player.manaSickness);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "manaRegen") {
            a_context.SetIntRegister(a_gameState.player.manaRegen);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "turns") {
            a_context.SetIntRegister(a_gameState.player.turns);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "usedTurns") {
            a_context.SetIntRegister(a_gameState.player.usedTurns);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "effectiveArmor") {
            a_context.SetIntRegister(a_gameState.player.GetEffectiveArmor());
        }
    } else if (object == "room") {
        if (a_context.stringRegisters[a_context.stringRegister] == "livingInhabitants") {
            a_context.SetIntRegister(a_gameState.rooms[a_gameState.curRoom].LivingInhabitants());
        }
    } else if (object == "gameState") {
        if (a_context.stringRegisters[a_context.stringRegister] == "curRoom") {
            a_context.SetIntRegister(a_gameState.curRoom);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "usedRooms") {
            a_context.SetIntRegister(a_gameState.usedRooms);
        }
    }
    a_context.stringRegisters[a_context.stringRegister] = "";
    a_context.mode.pop_back();
}

void Interpreter::ParseObjectMethod(GameState& a_gameState, InterpreterContext& a_context) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    std::cout << "Operating on function " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
    #endif
    std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
    a_context.mode.pop_back();
    if (object == "player") {
        if (a_context.stringRegisters[a_context.stringRegister] == "HasPerk") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Hurt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Heal") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "DrainMana") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RegainMana") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "GetSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "SetSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "mathf") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Min") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Max") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Clamp") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "mathi") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Min") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Max") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Clamp") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "random") {
        if (a_context.stringRegisters[a_context.stringRegister] == "RangeI") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RangeF") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RollDice") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Random") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "console") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Print") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "EatInput") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "room") {
        if (a_context.stringRegisters[a_context.stringRegister] == "AddInhabitant") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddRoomAction") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddShop") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "HasFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "GetFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "SetFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    }
}

void Interpreter::ParseObjectMethodArgs(GameState& a_gameState, InterpreterContext& a_context) {
    std::vector<std::string> args = std::vector<std::string>();
    size_t argc = ParseFunctionArguments(a_context.stringRegisters[a_context.stringRegister], args);
    for (size_t i = 0; i < argc; ++i) {
        SanitizeString(args[i]);
        #ifdef LEGACY_COMMA_REPLACEMENT
        int32_t index = 0;
        while ((index = args[i].find("<comma>", 0)) < args[i].size()) {
            args[i] = args[i].substr(0, index) + "," + args[i].substr(index + 7, args[i].size() - index - 7);
        }
        #endif
    }
    std::string object = a_context.stringRegisters[a_context.stringRegister - 2];
    std::string method = a_context.stringRegisters[a_context.stringRegister - 1];
    if (object == "player") {
        if (method == "HasPerk") {
            if (argc == 1) {
                Perks perk = PerkFromName(args[0]);
                if (perk != Perks::SIZE) {
                    a_context.SetBoolRegister(a_gameState.player.perks.test((size_t)perk));
                } else {
                    std::cout << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                }
            } else {
                std::cout << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Hurt") {
            if (argc == 1) {
                a_gameState.player.Hurt(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                std::cout << "[ERROR]: player.Hurt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Heal") {
            if (argc == 1) {
                a_gameState.player.Heal(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                std::cout << "[ERROR]: player.Heal must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "DrainMana") {
            if (argc == 1) {
                a_gameState.player.DrainMana(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                std::cout << "[ERROR]: player.DrainMana must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RegainMana") {
            if (argc == 1) {
                a_gameState.player.RegainMana(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                std::cout << "[ERROR]: player.RegainMana must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "GetSkillModifier") {
            if (argc == 1) {
                a_context.SetIntRegister(a_gameState.player.GetSkillModifier(FormatString(a_gameState, a_context, args[0])));
            } else {
                std::cout << "[ERROR]: player.GetSkillModifier must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "SetSkillModifier") {
            if (argc == 2) {
                a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: player.SetSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddSkillModifier") {
            if (argc == 2) {
                a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: player.AddSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "mathf") {
        if (method == "Min") {
            if (argc == 2) {
                a_context.SetFloatRegister(std::min<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: mathf.Min must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Max") {
            if (argc == 2) {
                a_context.SetFloatRegister(std::max<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: mathf.Max must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Clamp") {
            if (argc == 3) {
                a_context.SetFloatRegister(std::clamp<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1]), ParseFloatExpression(a_gameState, a_context, args[2])));
            } else {
                std::cout << "[ERROR]: mathf.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "mathi") {
        if (method == "Min") {
            if (argc == 2) {
                a_context.SetIntRegister(std::min<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: mathi.Min must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Max") {
            if (argc == 2) {
                a_context.SetIntRegister(std::max<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                std::cout << "[ERROR]: mathi.Max must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Clamp") {
            if (argc == 3) {
                a_context.SetIntRegister(std::clamp<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1]), ParseIntExpression(a_gameState, a_context, args[2])));
            } else {
                std::cout << "[ERROR]: mathi.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "random") {
        if (method == "RangeI") {
            if (argc == 2) {
                int32_t minimum = ParseIntExpression(a_gameState, a_context, args[0]);
                a_context.SetIntRegister(minimum + PERCENT_DISTRIBUTION(a_gameState.generator) * (ParseIntExpression(a_gameState, a_context, args[1]) - minimum));
            } else {
                std::cout << "[ERROR]: random.RangeI must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RangeF") {
            if (argc == 2) {
                float minimum = ParseFloatExpression(a_gameState, a_context, args[0]);
                a_context.SetFloatRegister(minimum + PERCENT_DISTRIBUTION(a_gameState.generator) * (ParseFloatExpression(a_gameState, a_context, args[1]) - minimum));
            } else {
                std::cout << "[ERROR]: random.RangeF must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RollDice") {
            if (argc == 2) {
                int32_t count = ParseIntExpression(a_gameState, a_context, args[0]);
                int32_t dice = ParseIntExpression(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << std::format("{} & {}", args[0], args[1]) << std::endl;
                std::cout << std::format("{}d{}", count, dice) << std::endl;
                #endif
                a_context.SetIntRegister(a_gameState.RollDice(count, dice));
            } else {
                std::cout << "[ERROR]: random.RollDice must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Random") {
            if (argc == 0) {
                a_context.SetFloatRegister(PERCENT_DISTRIBUTION(a_gameState.generator));
            } else {
                std::cout << "[ERROR]: random.Random must have 0 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "console") {
        if (method == "Print") {
            if (argc == 1) {
                std::cout << FormatString(a_gameState, a_context, args[0]);
            } else {
                std::cout << "[ERROR]: console.Print must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "EatInput") {
            if (argc == 0) {
                EatInput();
            } else {
                std::cout << "[ERROR]: console.Print must have 0 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "room") {
        if (method == "AddInhabitant") {
            if (argc == 3) {
                std::string entityTemplate = FormatString(a_gameState, a_context, args[0]);
                std::string npcTemplate = FormatString(a_gameState, a_context, args[1]);
                std::string modifierTemplates = FormatString(a_gameState, a_context, args[2]);
                std::vector<std::string> modifiers = std::vector<std::string>();
                size_t modifierCount = SplitString(modifierTemplates, ":", modifiers);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << std::format("{}({}) = {}", entityTemplate, npcTemplate, modifierTemplates) << std::endl;
                #endif
                
                NPCData npc = NPCData(&ENTITY_TEMPLATES.at(entityTemplate), &NPC_TEMPLATES.at(npcTemplate));
                for (const std::string& modifier : modifiers) {
                    npc = NPC_MODIFIERS.at(modifier)(npc);
                }

                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(npc);
            } else if (argc == 2) {
                std::string entityTemplate = FormatString(a_gameState, a_context, args[0]);
                std::string npcTemplate = FormatString(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << std::format("{}({})", entityTemplate, npcTemplate) << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(NPCData(&ENTITY_TEMPLATES.at(entityTemplate), &NPC_TEMPLATES.at(npcTemplate)));
            } else {
                std::cout << "[ERROR]: room.AddInhabitant must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddRoomAction") {
            if (argc == 1) {
                std::string roomAction = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << roomAction << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(RoomActionReference { ROOM_ACTIONS.at(roomAction) });
            } else {
                std::cout << "[ERROR]: room.AddRoomAction must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddShop") {
            if (argc == 1) {
                std::string name = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << name << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].data.emplace("Shop", new ShopInstance());
                GenerateShopInstance(a_gameState, SHOP_DATA.at(name), *(ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(RoomActionReference { "Shop" });
            } else {
                std::cout << "[ERROR]: room.AddShop must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "HasFlag") {
            if (argc == 1) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << flag << std::endl;
                #endif
                a_context.SetBoolRegister(a_gameState.rooms[a_gameState.curRoom].flags.contains(flag));
            } else {
                std::cout << "[ERROR]: room.HasFlag must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "GetFlag") {
            if (argc == 1) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << flag << std::endl;
                #endif
                a_context.SetIntRegister(a_gameState.rooms[a_gameState.curRoom].flags.at(flag));
            } else {
                std::cout << "[ERROR]: room.GetFlag must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "SetFlag") {
            if (argc == 2) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                int32_t value = ParseIntExpression(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                std::cout << std::format("Setting flag {} to {}.", flag, value) << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].flags.insert_or_assign(flag, value);
            } else {
                std::cout << "[ERROR]: room.SetFlag must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    }

    a_context.mode.pop_back();
}
#pragma endregion

int main(int argc, char** argv) {
    //DataComponents::DataContainer container = DataComponents::ParseDataFile(std::filesystem::current_path().append("test.data"));
    for (const StartData& start : LoadStartData(std::filesystem::current_path().append("starts.data"))) {
        GameState::Starts.push_back(start);
    }
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()), Interpreter());
    #pragma endregion

    for (const Encounter& encounter : LoadEncountersData(std::filesystem::current_path().append("encounters.data"))) {
        ENCOUNTERS.push_back(encounter);
    }
    
    //std::cout << "The answer is: " << (std::pow(29, 452)) << "." << std::endl;

    //for (const std::pair<size_t, std::uniform_int_distribution<uint32_t>&>& dice : DICE) {
    //    std::cout << "DIE " << dice.first << "\n====================" << std::endl;
    //    for (int32_t i = 0; i < 50; ++i) {
    //        std::cout << (i + 1) << ") " << dice.second(gameState.generator) << "/" << dice.first << std::endl;
    //    }
    //    std::cout << "====================" << std::endl;
    //}

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

    std::function<size_t(void)> GetRandomEncounter = [&gameState]() {
        size_t maximum = 0;
        size_t weight = 0;
        std::vector<int64_t> weights = std::vector<int64_t>();

        for (Encounter encounter : ENCOUNTERS) {
            if (encounter.weight != "{}" && (weight = gameState.interpreter.ParseStatement<int32_t>(gameState, RegisterType::INT, encounter.weight)) > 0) {
                maximum += weight;
                weights.push_back(weight);
                //std::cout << std::format("Encounter {:0>2} {:0>4}", weights.size(), weight) << std::endl;
            } else {
                weights.push_back(-1);
                //std::cout << std::format("Encounter {:0>2} {:0>4}", weights.size(), 0) << std::endl;
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
                            if (ENCOUNTERS[encounter].event != "{}") {
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
                            gameState.interpreter.ParseStatement<int32_t>(gameState, RegisterType::ERROR, ENCOUNTERS[encounter].event);
                            runEncounter = false;
                        }

                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0 && !gameState.debug.noFights) {
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
                                if (gameState.debug.enabled) {
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
                            case 444: {
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (INFO)\x1b[22m\n1) View Classes\n2) View Encounters\n3) Back\n\nOption: ";
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
                                            for (const Encounter& encounter : ENCOUNTERS) {
                                                std::cout << std::format("Weight:\n{}\n\nEvent:\n{}", encounter.weight, encounter.event) << std::endl;
                                            }
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            }
                            case 555:
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << std::format("\x1b[1mDebug Menu (FLAG)\x1b[22m\n1) {}No Fights\x1b[39m\n2) ERR\n3) Back\n\nOption: ", (gameState.debug.noFights ? "\x1b[32m" : "\x1b[31m"));
                                    SafeInput<uint32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            gameState.debug.noFights = !gameState.debug.noFights;
                                            break;
                                        case 2:
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            case 666:
                                gameState.debug.enabled = !gameState.debug.enabled;
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