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

    EntityData(void) {
        this->maxHP = this->curHP = this->maxMana = this->curMana = 0;
    }

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
    size_t itemID;
    uint32_t stackSize;
};

struct PlayerData : public EntityData {
public:
    PlayerData(void) : EntityData() {

    }

    PlayerData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {

    }
};

struct NPCData : public EntityData {
public:
    NPCData(void) : EntityData() {
        
    }

    NPCData(const EntityTemplate* const a_entityTemplate) : EntityData(a_entityTemplate) {

    }
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

    RoomData(std::string_view a_name, std::string_view a_description, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew) {
        this->roomName = a_name;
        this->roomDescription = a_description;
        this->backlink = a_backlink;
        this->minExisting = a_minExisting;
        this->maxExisting = a_maxExisting;
        this->minNew = a_minNew;
        this->maxNew = a_maxNew;
    }
};

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<size_t> connections;

    RoomInstance(size_t a_roomID, size_t a_roomIndex) {
        this->roomID = a_roomID;
        this->roomIndex = a_roomIndex;
        this->connections = std::vector<size_t>();
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

    GameState(void) {
        this->screen = Screen::TITLE;
        this->menu = Menu::NONE;
        this->usedRooms = 0;
        this->curRoom = 0;
        this->rooms = std::vector<RoomInstance>();
        EntityTemplate temp = EntityTemplate(100, 30);
        this->player = PlayerData(&temp);
    }

    size_t PushBackRoom(size_t a_index) {
        this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
        return this->usedRooms++;
    }
};

int main(int argc, char** argv) {
    #pragma region Random Setup
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    #pragma endregion

    GameState gameState = GameState();
    
    const EntityTemplate ENTITY_TEMPLATES[] = {
        EntityTemplate(32, 0)
    };

    const ItemData ITEM_DATA[] = {
        ItemData("Bean", "A single bean, you may only have 0b11111111 beans.", 0b11111111)
    };

    const std::array<RoomData, 5> ROOM_DATA = {
        RoomData("Sanctuary", "", false, 0, 0, 2, 4),
        RoomData("Simple", "", true, 0, 1, 1, 3),
        RoomData("Loopback", "", true, 1, 3, 0, 2),
        RoomData("Downfall", "", false, 0, 1, 2, 4),
        RoomData("Lotus", "", true, 0, 0, 2, 6)
    };

    std::function<size_t(void)> GetRandomRoom = [&generator, &ROOM_DATA]() {
        return PERCENT_DISTRIBUTION(generator) * ROOM_DATA.size();
    };

    std::function<void(RoomInstance* const)> InitializeRoom = [&generator, &GetRandomRoom, &gameState, &ROOM_DATA](RoomInstance* const a_roomInstance) {
        std::vector<size_t> newConnections = std::vector<size_t>();
        size_t roomIndex = a_roomInstance->roomIndex;
        size_t newRoomCount = ROOM_DATA[a_roomInstance->roomID].minNew + PERCENT_DISTRIBUTION(generator) * (ROOM_DATA[a_roomInstance->roomID].maxNew - ROOM_DATA[a_roomInstance->roomID].minNew);
        for (int i = 0; i < std::min<int32_t>(ROOM_DATA[a_roomInstance->roomID].minExisting + PERCENT_DISTRIBUTION(generator) * (ROOM_DATA[a_roomInstance->roomID].maxExisting - ROOM_DATA[a_roomInstance->roomID].minExisting), gameState.usedRooms); ++i) {
            newConnections.push_back(PERCENT_DISTRIBUTION(generator) * gameState.usedRooms);
        }
        for (int i = 0; i < newRoomCount; ++i) {
            newConnections.push_back(gameState.PushBackRoom(GetRandomRoom()));
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
                            << "<\n\nActions:\n1) Move\n2) Stats\n\nOption: ";
                        SafeInput<uint32_t>(choice);
                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::MOVING;
                                break;
                            case 2:
                                gameState.menu = Menu::STATS;
                                break;
                        }
                        break;
                    case Menu::MOVING:
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nRooms:\n";
                        for (size_t connectionIndex = 0; connectionIndex < room->connections.size();) {
                            std::cout << ++connectionIndex << ") " << ROOM_DATA[gameState.rooms[room->connections[connectionIndex - 1]].roomID].roomName << "[" << room->connections[connectionIndex - 1] << "]\n";
                        }
                        std::cout << "\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < room->connections.size()) {
                            gameState.curRoom = room->connections[choice];
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
                        gameState.menu = Menu::NONE;
                        break;
                }
                break;
        }
    }

    return 0;
}