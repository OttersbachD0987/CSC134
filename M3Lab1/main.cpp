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
#include <functional>
#include <array>
#include <bitset>
#include <unordered_map>
#include <typeinfo>
#include <algorithm>
#include <concepts>
#include <fstream>
#include <filesystem>

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
    {  4, DIE_FOUR_DISTRIBUTION   },
    {  6, DIE_SIX_DISTRIBUTION    },
    {  8, DIE_EIGHT_DISTRIBUTION  },
    { 10, DIE_TEN_DISTRIBUTION    },
    { 12, DIE_TWELVE_DISTRIBUTION },
    { 20, DIE_TWENTY_DISTRIBUTION },
    {100, DIE_HUNDRED_DISTRIBUTION},
};
#pragma endregion

struct EntityData;
struct GameState;
struct ItemStack;
struct NPCData;
struct RoomInstance;
struct ConnectionInstance;

/// @struct Action
/// @brief Holds the data for an action.
struct Action {
public:
    std::string name;                                                              //< The name of the action.
    std::function<bool(GameState&, EntityData*, EntityData*)> condition = nullptr; //< The condition of the action, if nullptr, then it has no condition.
    std::function<void(GameState&, EntityData*, EntityData*)> action = nullptr;    //< The action to execute, don't leave this null.
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

    void TurnEnd(void) {
        if (manaSickness <= 0) {
            if (manaRegen > 0) {
                RegainMana(manaRegen);
            } else if (manaRegen < 0) {
                DrainMana(-manaRegen);
            }
        } else {
            --manaSickness;
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
    ItemUsage usage;
    uint32_t maxStack;

    ItemData(std::string_view a_name, std::string_view a_description, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->usage = ItemUsage();
        this->maxStack = a_maxStack;
    }

    ItemData(std::string_view a_name, std::string_view a_description, const ItemUsage& a_usage, uint32_t a_maxStack) {
        this->name = a_name;
        this->description = a_description;
        this->usage = a_usage;
        this->maxStack = a_maxStack;
    }
};

struct ItemStack {
public:
    size_t itemID;
    uint32_t stackSize;

    ItemStack(void) {
        this->itemID = 0;
        this->stackSize = 0;
    }

    ItemStack(size_t a_itemID, uint32_t a_stackSize) {
        this->itemID = a_itemID;
        this->stackSize = a_stackSize;
    }
};

enum class Perks {
    INSIGHT,
    ARCANE_EYES,
    HARD_HITTER,
    SIZE
};

struct PlayerData : public EntityData {
public:
    std::bitset<static_cast<size_t>(Perks::SIZE)> perks;
    std::vector<ItemStack> items;
    int32_t turns = 1;
    int32_t usedTurns = 0;

    PlayerData(void) : EntityData() {
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        this->items = std::vector<ItemStack>();
    }

    PlayerData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        this->items = std::vector<ItemStack>();
    }

    PlayerData(const EntityTemplate* const a_entityTemplate, const std::vector<size_t>& a_perks, size_t a_turns) : EntityData(a_entityTemplate) {
        this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
        for (const size_t& perk : a_perks) {
            this->perks.set(perk);
        }
        this->items = std::vector<ItemStack>();
        this->turns = 1;
    }

    void UseItem(size_t a_index) {
        if (--items[a_index].stackSize <= 0) {
            items.erase(items.begin() + a_index);
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
    }

    bool HasItem(size_t a_itemID, uint32_t a_itemAmount) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].itemID == a_itemID) {
                return a_itemAmount <= items[i].stackSize;
            }
        }
        return false;
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
    std::function<void(GameState&, NPCData&)> ai = nullptr;
};

struct NPCData : public EntityData {
public:
    std::string name;
    const std::function<void(GameState&, NPCData&)>* aiFunction;
    bool stunned = false;

    NPCData(void) : EntityData() {
        this->name = "Unknown";
        this->aiFunction = nullptr;
    }

    NPCData(const EntityTemplate* const a_entityTemplate, const NPCTemplate* const a_npcTemplate) : EntityData(a_entityTemplate) {
        this->name = a_npcTemplate->name;
        this->aiFunction = &a_npcTemplate->ai;
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

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<ConnectionInstance> connections;
    std::vector<NPCData> inhabitants;
    std::unordered_map<std::string, int32_t> flags;

    RoomInstance(size_t a_roomID, size_t a_roomIndex) {
        this->roomID = a_roomID;
        this->roomIndex = a_roomIndex;
        this->connections = std::vector<ConnectionInstance>();
        this->inhabitants = std::vector<NPCData>();
        this->flags = std::unordered_map<std::string, int32_t>();
    }
};

enum class Screen {
    TITLE,
    GAME_SELECT,
    GAME,
    GAME_OVER,
};

enum class Menu {
    PAUSE,
    MOVING,
    COMBAT,
    STATS,
    INVENTORY,
    NONE
};

struct StartData {
public:
    std::string name;
    std::string description;
    EntityTemplate playerEntity;
    std::vector<size_t> perks;
    std::vector<ItemStack> inventory;
    size_t turns;
    size_t startRoom;

    StartData(void) {
        this->name = "";
        this->description = "";
        this->playerEntity = EntityTemplate();
        this->perks = std::vector<size_t>();
        this->inventory = std::vector<ItemStack>();
        this->turns = 1;
        this->startRoom = -1;
    }

    StartData(std::string_view a_name, std::string_view a_description, const EntityTemplate& a_playerEntity, const std::vector<size_t>& a_perks, const std::vector<ItemStack>& a_inventory, size_t a_turns, size_t a_startRoom) {
        this->name = a_name;
        this->description = a_description;
        this->playerEntity = a_playerEntity;
        this->perks = a_perks;
        this->inventory = a_inventory;
        this->turns = a_turns;
        this->startRoom = a_startRoom;
    }
};

struct GameState {
public:
    static std::vector<StartData> Starts;

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
        this->player.items = GameState::Starts[a_start].inventory;
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
        "Punch", 
        Action {
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
                int32_t roll = DIE_TWENTY_DISTRIBUTION(a_gameState.generator);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->armor) {
                    int32_t damage = 0;
                    int32_t damageModifier = modifier * 0.5;
                    uint32_t damageTier = 1;
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

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }
            }
        }
    }
};

#pragma region Stats
/*
Martial Combat
Unarmed Combat
Brawling
*/
#pragma endregion

#pragma region Starts
std::vector<StartData> GameState::Starts = {
    StartData(
        "Default",
        "A simple start to the game, good for beginners.",
        EntityTemplate(100, 100, 10, 10, {
            {"Martial Combat", 2},
            {"Unarmed Combat", 1},
            {"Brawling", 1},
        }, {
            STANDARD_ACTIONS.at("Punch")
        }), {
            (size_t)Perks::INSIGHT
        }, {
            ItemStack(1, 4)
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

        }, {
            ItemStack(1, 2),
        },
        1, 0
    ),
    StartData(
        "Trivial",
        "A trivial start to the game, good for people bad at the game.",
        EntityTemplate(150, 150, 10, 12, {
            {"Martial Combat", 3},
            {"Unarmed Combat", 3},
            {"Brawling", 3},
        }, {
            STANDARD_ACTIONS.at("Punch"),
        }), {
            (size_t)Perks::INSIGHT
        }, {
            ItemStack(1, 16),
            ItemStack(2, 16),
            ItemStack(3, 5),
            ItemStack(4, 5),
        },
        1, 0
    ),
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
            
            memcpy_s(this->data, a_size, a_data, a_size);

            //if (a_dataType == DataType::STRING) {
            //    std::cout << "Value: " << (char*)data << std::endl;
            //}
        }

        void SetData(const void* a_data, size_t a_size) {
            this->data = realloc(this->data, a_size);
            this->size = a_size;

            memcpy_s(this->data, a_size, a_data, a_size);
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

        return container;
    }
}

int main(int argc, char** argv) {
    //DataComponents::DataContainer container = DataComponents::ParseDataFile(std::filesystem::current_path().append("test.data"));
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()));
    #pragma endregion

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
                    if (DIE_TWENTY_DISTRIBUTION(a_gameState.generator) >= a_gameState.player.armor) {
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
        },
        NPCTemplate {
            "Goblin Grunt",
            [](GameState& a_gameState, NPCData& a_npc) {
                std::cout << "The Goblin Grunt swings their club at the player and ";
                if (DIE_TWENTY_DISTRIBUTION(a_gameState.generator) >= a_gameState.player.armor) {
                    int32_t damage = DIE_FOUR_DISTRIBUTION(a_gameState.generator);
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        },
    };

    const ItemData ITEM_DATA[] = {
        ItemData("Bean", "A single bean, you may only have 0b11111111 beans.", 0b11111111),
        ItemData("HP Potion", "A HP Potion, restores 20 HP.", 
            ItemUsage {
                "Drink",
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    return a_gameState.player.curHP < a_gameState.player.maxHP;
                },
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    a_gameState.player.Heal(20);
                    a_gameState.player.UseItem(index);
                }
            }, 100
        ),
        ItemData("Mana Potion", "A Mana Potion, restores 20 Mana.", 
            ItemUsage {
                "Drink",
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    return a_gameState.player.curHP < a_gameState.player.maxHP;
                },
                [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                    a_gameState.player.RegainMana(20);
                    a_gameState.player.UseItem(index);
                }
            }, 100
        ),
        ItemData("Dungeon Key", "An old key, goes to something important.", 15),
        ItemData("Sewer Key", "A mucky key, might be useful.", 15),
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
        RoomData("Lotus", "",
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
                        a_gameState.player.UseItem(4);
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
        }
    };

    const std::array<Encounter, 5> ENCOUNTERS = {
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
                        
                        std::cout << "\nActions:\n1) Move\n2) Stats\n3) Inventory\n4) Quit\n\nOption: ";
                        SafeInput<uint32_t>(choice);
                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::MOVING;
                                break;
                            case 2:
                                gameState.menu = Menu::STATS;
                                break;
                            case 3:
                                gameState.menu = Menu::INVENTORY;
                                break;
                            case 4:
                                gameState.screen = Screen::TITLE;
                                gameState.menu = Menu::NONE;
                                break;
                        }
                        break;
                    case Menu::MOVING:
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nRooms:\n";
                        for (size_t connectionIndex = 0; connectionIndex < room->connections.size();) {
                            std::cout << ++connectionIndex << ") " << ROOM_DATA[gameState.rooms[room->connections[connectionIndex - 1].destination].roomID].roomName << "[" << room->connections[connectionIndex - 1].destination << "]\n";
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
                    case Menu::STATS:
                        std::cout 
                            << "-------------------------------\nStats:\n\nHP: " 
                            << gameState.player.curHP << "/" << gameState.player.maxHP 
                            << "\nMana: " 
                            << gameState.player.curMana << "/" << gameState.player.maxMana 
                            << "\n\nSkills:\n\n";
                        
                        for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                            std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                        }
                            
                        std::cout << "\nOptions:\n1) Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::NONE;
                                break;
                        }
                        break;
                    case Menu::INVENTORY:
                        std::cout << "-------------------------------\nInventory:\n";
                        for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                            std::cout << ++inventoryItem << ") "  << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                        }
                        std::cout << (gameState.player.items.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < gameState.player.items.size()) {
                            const ItemData& item = ITEM_DATA[gameState.player.items[choice].itemID];
                            if (item.usage.usage != nullptr) {
                                if (item.usage.condition == nullptr || item.usage.condition(gameState, gameState.player.items[choice], choice)) {
                                    item.usage.usage(gameState, gameState.player.items[choice], choice);
                                } else {
                                    std::cout << "You don't know why you would use this right now." << std::endl;
                                }
                            } else {
                                std::cout << "This item is not usable." << std::endl;
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
                            std::cout << "-------------------------------\nActions:\n";
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

                                if (enemiesGo) {
                                    gameState.player.usedTurns = 0;
                                    //std::cout << "Tonk: " << room->inhabitants.size() << std::endl;

                                    for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                        if (room->inhabitants[npcIndex].stunned) {
                                            room->inhabitants[npcIndex].stunned = false;
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is stunned." << std::endl;
                                            continue;
                                        } else if (room->inhabitants[npcIndex].curHP <= 0) {
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is dead." << std::endl;
                                            continue;
                                        } else if (room->inhabitants[npcIndex].aiFunction != nullptr) {
                                            (*room->inhabitants[npcIndex].aiFunction)(gameState, room->inhabitants[npcIndex]);
                                        }
                                        EatInput();
                                    }

                                    room = &gameState.rooms[gameState.curRoom];

                                    std::vector<size_t> remove = std::vector<size_t>();

                                    //std::cout << "Bonk: " << room->inhabitants.size() << std::endl;

                                    for (int32_t j = room->inhabitants.size() - 1; j > -1; --j) {
                                        //std::cout << (j + 1) << ") " << room->inhabitants[j].curHP << std::endl;
                                        if (room->inhabitants[j].curHP <= 0) {
                                            //std::cout << "Killing " << j << std::endl;
                                            remove.push_back(j);
                                        } else {
                                            room->inhabitants[j].TurnEnd();
                                        }
                                    }

                                    for (const size_t& toRemove : remove) {
                                        room->inhabitants.erase(room->inhabitants.begin() + toRemove);
                                        room = &gameState.rooms[gameState.curRoom];
                                    }

                                    if (gameState.player.curHP <= 0) {
                                        std::cout << "\nYou collapse to the floor, dead." << std::endl;
                                        // TODO: Switch to a Stats Screen
                                        gameState.screen = Screen::GAME_OVER;
                                        gameState.menu = Menu::NONE;
                                    } else {
                                        gameState.player.TurnEnd();
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
                    << "\n\n>> Perks\n";

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
                        }
                    }
                }
                
                std::cout << "\n>> Skills\n";

                for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                    std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                }

                std::cout << "\n>> Inventory\n";
                for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                    std::cout << ++inventoryItem << ". "  << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
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