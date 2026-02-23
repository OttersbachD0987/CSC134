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

template <typename T>
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n'); 
    }
}

std::uniform_real_distribution<float> PERCENT_DISTRIBUTION = std::uniform_real_distribution<float>(0, 1);

std::uniform_int_distribution<uint32_t> DIE_TWO_DISTRIBUTION    = std::uniform_int_distribution<uint32_t>(1, 2);
std::uniform_int_distribution<uint32_t> DIE_FOUR_DISTRIBUTION   = std::uniform_int_distribution<uint32_t>(1, 4);
std::uniform_int_distribution<uint32_t> DIE_SIX_DISTRIBUTION    = std::uniform_int_distribution<uint32_t>(1, 6);
std::uniform_int_distribution<uint32_t> DIE_EIGHT_DISTRIBUTION  = std::uniform_int_distribution<uint32_t>(1, 8);
std::uniform_int_distribution<uint32_t> DIE_TEN_DISTRIBUTION    = std::uniform_int_distribution<uint32_t>(1, 10);
std::uniform_int_distribution<uint32_t> DIE_TWELVE_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 12);
std::uniform_int_distribution<uint32_t> DIE_TWENTY_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 20);

struct EntityData;

struct Action {
public:
    std::string name;
    std::function<bool(EntityData*, EntityData*)> condition = [](EntityData* a_source, EntityData* a_target) { return true; };
    std::function<void(EntityData*, EntityData*)> action = [](EntityData* a_source, EntityData* a_target) {};
};

struct EntityTemplate {
public:
    uint32_t hp;
    uint32_t mana;
    std::vector<Action> actions;

    EntityTemplate(uint32_t a_hp, uint32_t a_mana, std::vector<Action> a_actions) {
        this->hp = a_hp;
        this->mana = a_mana;
        this->actions = a_actions;
    }
};

struct EntityData {
public:
    uint32_t maxHP;
    uint32_t curHP;
    uint32_t maxMana;
    uint32_t curMana;
    std::vector<Action> actions;

    EntityData(void) {
        this->maxHP = this->curHP = this->maxMana = this->curMana = 0;
    }

    EntityData(const EntityTemplate* const a_entityTemplate) {
        this->maxHP = this->curHP = a_entityTemplate->hp;
        this->maxMana = this->curMana = a_entityTemplate->mana;
        this->actions = a_entityTemplate->actions;
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

struct PlayerData : public EntityData {
public:
    std::vector<ItemStack> items;

    PlayerData(void) : EntityData() {
        this->items = std::vector<ItemStack>();
    }

    PlayerData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {
        this->items = std::vector<ItemStack>();
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
};

struct NPCTemplate {
public:
    std::string name;

    NPCTemplate(std::string_view a_name) {
        this->name = a_name;
    }
};

struct NPCData : public EntityData {
public:
    std::string name;

    NPCData(void) : EntityData() {
        this->name = "Unknown";
    }

    NPCData(const EntityTemplate* const a_entityTemplate, const NPCTemplate* const a_npcTemplate) : EntityData(a_entityTemplate) {
        this->name = a_npcTemplate->name;
    }
};

struct ConnectionTest {
public:
    std::function<bool(PlayerData*)> condition = [](PlayerData* a_player) { return true; };
    std::function<void(PlayerData*, bool)> conditionDisplay = [](PlayerData* a_player, bool a_success) { };
    std::function<void(PlayerData*)> action = [](PlayerData* a_player) { };
};

struct RoomData {
public:
    std::string roomName;
    std::string roomDescription;
    bool backlink;
    uint32_t minExisting;
    uint32_t maxExisting;
    uint32_t minNew;
    uint32_t maxNew;
    ConnectionTest toConnection;
    ConnectionTest fromConnection;

    RoomData(std::string_view a_name, std::string_view a_description, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew, ConnectionTest a_to = ConnectionTest(), ConnectionTest a_from = ConnectionTest()) {
        this->roomName = a_name;
        this->roomDescription = a_description;
        this->backlink = a_backlink;
        this->minExisting = a_minExisting;
        this->maxExisting = a_maxExisting;
        this->minNew = a_minNew;
        this->maxNew = a_maxNew;
        this->toConnection = a_to;
        this->fromConnection = a_from;
    }
};

struct ConnectionTemplate {
public:
    ConnectionTest currentTest;
    ConnectionTest arrivalTest;
};

struct ConnectionInstance;

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<ConnectionInstance> connections;
    std::vector<NPCData> inhabitants;

    RoomInstance(size_t a_roomID, size_t a_roomIndex) {
        this->roomID = a_roomID;
        this->roomIndex = a_roomIndex;
        this->connections = std::vector<ConnectionInstance>();
        this->inhabitants = std::vector<NPCData>();
    }
};

enum class Screen {
    TITLE,
    GAME
};

enum class Menu {
    PAUSE,
    MOVING,
    STATS,
    INVENTORY,
    NONE
};

struct GameState {
public:
    bool running = true;
    Screen screen;
    Menu menu;

    size_t usedRooms;
    size_t curRoom;
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
        EntityTemplate temp = EntityTemplate(100, 30, {
            Action {
                "Example",
                [](EntityData* a_caster, EntityData* a_target) {
                    return true;
                },
                [](EntityData* a_caster, EntityData* a_target) {
                    
                }
            }
        });
        this->player = PlayerData(&temp);
    }

    size_t PushBackRoom(size_t a_index) {
        this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
        return this->usedRooms++;
    }
};

struct ConnectionInstance {
public:
    size_t destination;
    ConnectionTest currentTest;
    ConnectionTest arrivalTest;

    bool CanPass(GameState* a_gameState) {
        bool currentResult = true;
        bool arrivalResult = true;

        if (currentTest.condition != nullptr) {
            currentResult = currentTest.condition(&a_gameState->player);
        }

        if (arrivalTest.condition != nullptr) {
            arrivalResult = arrivalTest.condition(&a_gameState->player);
        }

        if (currentTest.conditionDisplay != nullptr) {
            currentTest.conditionDisplay(&a_gameState->player, currentResult);
        }
        
        if (arrivalTest.conditionDisplay != nullptr) {
            arrivalTest.conditionDisplay(&a_gameState->player, arrivalResult);
        }

        return currentResult && arrivalResult;
    }

    void Passes(GameState* a_gameState) {
        if (currentTest.action != nullptr) {
            currentTest.action(&a_gameState->player);
        }

        if (arrivalTest.action != nullptr) {
            arrivalTest.action(&a_gameState->player);
        }
    }
};

int main(int argc, char** argv) {
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()));
    #pragma endregion
    
    const EntityTemplate ENTITY_TEMPLATES[] = {
        EntityTemplate(32, 0, {})
    };

    const ItemData ITEM_DATA[] = {
        ItemData("Bean", "A single bean, you may only have 0b11111111 beans.", 0b11111111),
        ItemData("HP Potion", "A HP Potion, restores 20 HP.", 100),
        ItemData("Mana Potion", "A Mana Potion, restores 20 Mana.", 100),
        ItemData("Dungeon Key", "An old key, goes to something important.", 15),
        ItemData("Sewer Key", "A mucky key, might be useful.", 15)
    };

    const std::array<RoomData, 6> ROOM_DATA = {
        RoomData("Sanctuary", "A strange room with a strange promise.", false, 0, 0, 2, 4),
        RoomData("Simple", "", true, 0, 1, 1, 3),
        RoomData("Loopback", "", true, 1, 3, 0, 2),
        RoomData("Downfall", "", false, 0, 1, 2, 4),
        RoomData("Lotus", "", true, 0, 0, 2, 6),
        RoomData("Sewer", "", true, 0, 0, 2, 4, 
            ConnectionTest {
                [](PlayerData* a_player) {
                    return a_player->HasItem(4, 1);
                },
                [](PlayerData* a_player, bool a_success) {
                    if (!a_success) {
                        std::cout << "You require a key to get through the grate." << std::endl;
                    }
                },
                [](PlayerData* a_player) {
                    a_player->UseItem(4);
                    std::cout << "You use one sewer key to open the grate." << std::endl;
                }
            }, 
            ConnectionTest()
        )
    };

    std::function<size_t(void)> GetRandomRoom = [&gameState, &ROOM_DATA]() {
        return PERCENT_DISTRIBUTION(gameState.generator) * ROOM_DATA.size();
    };

    std::function<void(RoomInstance* const)> InitializeRoom = [&GetRandomRoom, &gameState, &ROOM_DATA](RoomInstance* const a_roomInstance) {
        std::vector<ConnectionInstance> newConnections = std::vector<ConnectionInstance>();
        size_t newRoomThing;
        size_t roomIndex = a_roomInstance->roomIndex;
        size_t newRoomCount = ROOM_DATA[a_roomInstance->roomID].minNew + PERCENT_DISTRIBUTION(gameState.generator) * (ROOM_DATA[a_roomInstance->roomID].maxNew - ROOM_DATA[a_roomInstance->roomID].minNew);
        for (int i = 0; i < std::min<int32_t>(ROOM_DATA[a_roomInstance->roomID].minExisting + PERCENT_DISTRIBUTION(gameState.generator) * (ROOM_DATA[a_roomInstance->roomID].maxExisting - ROOM_DATA[a_roomInstance->roomID].minExisting), gameState.usedRooms); ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = size_t(PERCENT_DISTRIBUTION(gameState.generator) * gameState.usedRooms)),
                    ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    ROOM_DATA[a_roomInstance->roomID].fromConnection
                }
            );
        }
        for (int i = 0; i < newRoomCount; ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = gameState.PushBackRoom(GetRandomRoom())),
                    ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    ROOM_DATA[a_roomInstance->roomID].fromConnection
                }
            );
        }
        gameState.rooms[roomIndex].connections = newConnections;
    };

    RoomInstance* room = nullptr;
    uint32_t choice;
    while (gameState.running) {
        switch (gameState.screen) {
            case Screen::TITLE:
                std::cout << "-------------------------------\n      >>> Ars Timoris <<<\n-------------------------------\n1) New Game\n2) Quit\n------------------------------\n\nOption: ";
                SafeInput<uint32_t>(choice);
                switch (choice) {
                    case 1:
                        gameState.screen = Screen::GAME;
                        gameState.PushBackRoom(0);
                        gameState.player.AddItem(1, 10);
                        gameState.player.AddItem(2, 10);
                        break;
                    case 2:
                        gameState.running = false;
                        break;
                }
                break;
            case Screen::GAME:
                switch (gameState.menu) {
                    case Menu::NONE:
                        if (!gameState.rooms[gameState.curRoom].initialized) {
                            InitializeRoom(&gameState.rooms[gameState.curRoom]);
                        }
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout 
                            << "-------------------------------\n>>> " 
                            << ROOM_DATA[room->roomID].roomName 
                            << " <<<\n\n>" 
                            << ROOM_DATA[room->roomID].roomDescription 
                            << "<\n\nActions:\n1) Move\n2) Stats\n3) Inventory\n4) Quit\n\nOption: ";
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
                            if (room->connections[choice].CanPass(&gameState)) {
                                room->connections[choice].Passes(&gameState);
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
                            << std::endl;
                        std::cin.get();
                        std::cin.get();
                        gameState.menu = Menu::NONE;
                        break;
                    case Menu::INVENTORY:
                        std::cout << "-------------------------------\nInventory:\n";
                        for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                            std::cout << ++inventoryItem << ") "  << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                        }
                        std::cout << "\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < gameState.player.items.size()) {
                            switch (gameState.player.items[choice].itemID) {
                                case 1:
                                    gameState.player.curHP = std::min(gameState.player.curHP + 20, gameState.player.maxHP);
                                    gameState.player.UseItem(choice); 
                                    break;
                                case 2:
                                    gameState.player.curMana = std::min(gameState.player.curMana + 20, gameState.player.maxMana);
                                    gameState.player.UseItem(choice);
                                    break;
                            }
                            gameState.menu = Menu::NONE;
                        }
                        break;
                }
                break;
        }
    }

    return 0;
}