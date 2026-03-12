// CSC 134
// M3LAB1
// Daley Ottersbach
// 3/6/2026

//#define __STDC_WANT_LIB_EXT1__ 1
//#include <cstring>
#include <ArsTimoris/Game/Map/RoomData.h>
#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/DataComponents/DataContainer.h>
#include <ArsTimoris/Util/Input.hpp>
#include <ArsTimoris/Util/Random.hpp>

#pragma region Debug Flags
//#define SCRIPT_PARSER_DEBUG_LOGGING 1
//#define STRING_UTILS_DEBUG_LOGGING 1
#pragma endregion

#pragma region Compilation Flags
//#define LEGACY_COMMA_REPLACEMENT 1
#pragma endregion

void PrintCombatNPCData(GameState& a_gameState, const NPCData& a_npc) {
    std::cout << a_npc.name<< "\n";

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT))
    ) {
        std::cout << "- Armor [" << a_npc.armor << "]\n";
    }

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
        a_gameState.player.GetSkillModifier("Brawler") >= 6
    ) {
        std::cout << "- HP [" << a_npc.curHP << "/" << a_npc.maxHP << "]\n";
    }

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
        a_gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))
    ) {
        std::cout << "- Mana [" << a_npc.curMana << "/" << a_npc.maxMana << "]\n";
    }
}

int main(int argc, char** argv) {
    std::filesystem::path dataPath = std::filesystem::current_path().append("Assets").append("Data");
    //DataComponents::DataContainer container = DataComponents::ParseDataFile(std::filesystem::current_path().append("test.data"));
    //std::cout << std::format("Pre loading Starts ({})", dataPath.string()) << std::endl;
    for (const StartData& start : LoadStartData(dataPath / "starts.data")) {
        GameState::Starts.push_back(start);
    }
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()), Interpreter());
    #pragma endregion

    //std::cout << std::format("Pre loading Encounters ({})", dataPath.string()) << std::endl;
    for (const Encounter& encounter : LoadEncountersData(dataPath / "encounters.data")) {
        GameData::ENCOUNTERS.push_back(encounter);
    }

    //std::cout << std::format("Pre loading Entity Templates ({})", dataPath.string()) << std::endl;
    for (const std::pair<std::string, EntityTemplate>& entityTemplate : LoadEntityData(dataPath / "entities.data")) {
        GameData::ENTITY_TEMPLATES.emplace(entityTemplate);
    }
    
    //std::cout << "The answer is: " << (std::pow(29, 452)) << "." << std::endl;

    //for (const std::pair<size_t, std::uniform_int_distribution<uint32_t>&>& dice : DICE) {
    //    std::cout << "DIE " << dice.first << "\n====================" << std::endl;
    //    for (int32_t i = 0; i < 50; ++i) {
    //        std::cout << (i + 1) << ") " << dice.second(gameState.generator) << "/" << dice.first << std::endl;
    //    }
    //    std::cout << "====================" << std::endl;
    //}

    std::function<size_t(const RoomData&)> GetRandomRoom = [&gameState](const RoomData& a_roomData) {
        size_t maximum = 0;
        size_t weight = 0;
        std::vector<int64_t> weights = std::vector<int64_t>();

        for (const RoomData& roomData : GameData::ROOM_DATA) {
            if (roomData.weight != nullptr && (weight = roomData.weight(gameState, a_roomData)) > 0) {
                maximum += weight;
                weights.push_back(weight);
            } else {
                weights.push_back(-1);
            }
        }

        int64_t choice = static_cast<int64_t>(PERCENT_DISTRIBUTION(gameState.generator) * maximum);

        for (size_t i = 0; i < GameData::ROOM_DATA.size(); ++i) {
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

        for (Encounter encounter : GameData::ENCOUNTERS) {
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

        for (size_t i = 0; i < GameData::ENCOUNTERS.size(); ++i) {
            if (weights[i] != -1 && (choice -= weights[i]) <= 0) {
                return i;
            }
        }

        return (size_t)0;
    };

    std::function<void(RoomInstance* const)> InitializeRoom = [&GetRandomRoom, &gameState](RoomInstance* const a_roomInstance) {
        a_roomInstance->initialized = true;
        std::vector<ConnectionInstance> newConnections = a_roomInstance->connections;
        size_t newRoomThing = 0;
        const RoomData& roomType = GameData::ROOM_DATA[a_roomInstance->roomID];
        size_t roomIndex = a_roomInstance->roomIndex;
        size_t newRoomCount = roomType.minNew + PERCENT_DISTRIBUTION(gameState.generator) * (roomType.maxNew - roomType.minNew);
        for (int i = 0; i < std::min<int32_t>(roomType.minExisting + PERCENT_DISTRIBUTION(gameState.generator) * (roomType.maxExisting - roomType.minExisting), gameState.usedRooms); ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = size_t(PERCENT_DISTRIBUTION(gameState.generator) * gameState.usedRooms)),
                    GameData::ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    roomType.fromConnection
                }
            );
        }
        for (int i = 0; i < newRoomCount; ++i) {
            newConnections.push_back(
                ConnectionInstance {
                    (newRoomThing = gameState.PushBackRoom(GetRandomRoom(roomType))),
                    GameData::ROOM_DATA[gameState.rooms[newRoomThing].roomID].toConnection,
                    roomType.fromConnection
                }
            );

            if (GameData::ROOM_DATA[gameState.rooms[newRoomThing].roomID].backlink) {
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
                    case Menu::NONE: {
                        if (!gameState.rooms[gameState.curRoom].initialized) {
                            InitializeRoom(&gameState.rooms[gameState.curRoom]);
                            encounter = GetRandomEncounter();
                            if (GameData::ENCOUNTERS[encounter].event != "{}") {
                                runEncounter = true;
                            }
                        }
                    
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout 
                            << "-------------------------------\n>>> " 
                            << GameData::ROOM_DATA[room->roomID].roomName 
                            << " <<<\n\n>" 
                            << GameData::ROOM_DATA[room->roomID].roomDescription 
                            << "<\n" << std::endl;
                        
                        if (runEncounter) {
                            gameState.interpreter.ParseStatement<int32_t>(gameState, RegisterType::ERROR, GameData::ENCOUNTERS[encounter].event);
                            runEncounter = false;
                        }

                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0 && !gameState.debug.noFights) {
                            gameState.menu = Menu::COMBAT;
                            break;
                        }

                        if (room->roomActions.size() > 0) {
                            for (const RoomAction& roomAction : room->roomActions) {
                                if (roomAction.roomDescription != nullptr) {
                                    roomAction.roomDescription(gameState);
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
                            case 333: {
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
                            }
                            case 444: {
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (INFO)\x1b[22m\n1) View Classes\n2) View Encounters\n3) View Entity Templates\n4) Back\n\nOption: ";
                                    SafeInput<uint32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            for (std::pair<std::string, ClassData> classPair : GameData::CLASSES) {
                                                const ClassData& classData = classPair.second;
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
                                            for (const Encounter& encounter : GameData::ENCOUNTERS) {
                                                std::cout << std::format("Weight:\n{}\n\nEvent:\n{}", encounter.weight, encounter.event) << std::endl;
                                            }
                                            break;
                                        case 3:
                                            for (const std::pair<std::string, EntityTemplate>& entityTemplate : GameData::ENTITY_TEMPLATES) {
                                                std::cout << std::format("Name: {}\nHP: {}\nMana: {}\nMana Regeneration: {}\nArmor: {}\n\nSkills:\n", entityTemplate.first, entityTemplate.second.hp, entityTemplate.second.mana, entityTemplate.second.manaRegen, entityTemplate.second.armor);
                                                for (const std::pair<std::string, int32_t>& skill : entityTemplate.second.skills) {
                                                    std::cout << std::format("{}: {}\n", skill.first, skill.second);
                                                }
                                                std::cout << "\nActions:\n";
                                                for (const Action& action : entityTemplate.second.actions) {
                                                    std::cout << std::format("{}\n", action.name);
                                                }
                                                std::cout << std::endl;
                                            }
                                            break;
                                        case 4:
                                            break;
                                    }
                                }
                                break;
                            }
                            case 555: {
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
                            }
                            case 666: {
                                gameState.debug.enabled = !gameState.debug.enabled;
                                break;
                            }
                        }
                        break;
                    }
                    case Menu::MOVING: {
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nRooms:\n";
                        for (size_t connectionIndex = 0; connectionIndex < room->connections.size();) {
                            std::cout << ++connectionIndex << ") [" << room->connections[connectionIndex - 1].destination  << "] \x1b[38;5;" << (gameState.rooms[room->connections[connectionIndex - 1].destination].initialized ? "8m" : "15m") << GameData::ROOM_DATA[gameState.rooms[room->connections[connectionIndex - 1].destination].roomID].roomName << "\x1b[39m\n";
                        }
                        std::cout << (room->connections.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < room->connections.size()) {
                            if (room->connections[choice].CanPass(gameState)) {
                                room->connections[choice].Passes(gameState);
                                gameState.curRoom = room->connections[choice].destination;
                            }
                            gameState.menu = Menu::NONE;
                        } else if (choice == room->connections.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    }
                    case Menu::ROOM_ACTIONS: {
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nActions:\n";
                        for (size_t i = 0; i < room->roomActions.size(); ++i) {
                            std::cout << (i + 1) << ") " << room->roomActions[i].name << "\n- " << room->roomActions[i].description << "\n";
                        }
                        std::cout << (room->roomActions.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < room->roomActions.size()) {
                            if (room->roomActions[choice].condition(gameState)) {
                                room->roomActions[choice].usage(gameState);
                            }
                            gameState.menu = Menu::NONE;
                        } else if (choice == room->roomActions.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    }
                    case Menu::STATS: {
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
                            std::cout << classPair.first << "(" << classPair.second.level << "/" << GameData::CLASSES.at(classPair.first).levels.size() << "): " << GameData::CLASSES.at(classPair.first).description << "\n";
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
                    }
                    case Menu::LEVEL_UP: {
                        std::cout 
                            << "-------------------------------\nXP: " 
                            << gameState.player.xp
                            << "\nClasses:\n";
                        
                        std::vector<std::string> classics = std::vector<std::string>();
                        size_t accum = 1;
                        for (const std::pair<std::string, ClassData>& classPair : GameData::CLASSES) {
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

                        if (--choice < GameData::CLASSES.size()) {
                            const ClassData& classRef = GameData::CLASSES.at(classics[choice]);
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
                        } else if (choice == GameData::CLASSES.size()) {
                            gameState.menu = Menu::STATS;
                        }
                        break;
                    }
                    case Menu::INVENTORY:
                        std::cout << "-------------------------------\nInventory:\n";
                        for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                            std::cout << ++inventoryItem << ") [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                        }
                        std::cout << (gameState.player.items.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<uint32_t>(choice);

                        if (--choice < gameState.player.items.size()) {
                            size_t itemIndex = choice;
                            ItemStack& itemStack = gameState.player.items[itemIndex];
                            const ItemData& itemType = GameData::ITEM_DATA[itemStack.itemID];
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
                                std::cout << (npcIndex + 1)  << ") ";
                                PrintCombatNPCData(gameState, room->inhabitants[npcIndex]);
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
                                    std::cout << (npcIndex + 1)  << ") ";
                                    PrintCombatNPCData(gameState, room->inhabitants[npcIndex]);
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
                const RoomData& roomType = GameData::ROOM_DATA[room->roomID];
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
                    std::cout << classPair.first << "(" << classPair.second.level << "/" << GameData::CLASSES.at(classPair.first).levels.size() << "): " << GameData::CLASSES.at(classPair.first).description << "\n";
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
                    std::cout << ++inventoryItem << ". [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
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