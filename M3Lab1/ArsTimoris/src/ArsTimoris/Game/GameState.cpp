#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/Util/Random.hpp>

GameState::GameState(std::mt19937 a_generator, const Interpreter& a_interpreter) {
    this->generator = a_generator;
    this->interpreter = a_interpreter;
    this->screen = Screen::TITLE;
    this->menu = Menu::NONE;
    this->usedRooms = 0;
    this->curRoom = 0;
    this->rooms = std::vector<RoomInstance>();
    this->player = PlayerData(EntityTemplate("Player", 100, 30, 5, 10, {}, {Action {"Example", [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { return true; }, [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {}}}));
}

void GameState::DisplayStarts() {
    for (size_t i = 0; i < GameState::Starts.size(); ++i) {
        std::cout << (i + 1) << ") " << GameState::Starts[i].name << "\n- " << GameState::Starts[i].description << "\n" << std::endl;
    }
}


void GameState::Start(size_t a_start) {
    this->start = a_start;
    this->usedRooms = 0;
    this->curRoom = 0;
    this->rooms.clear();
    this->player = PlayerData(GameState::Starts[a_start].playerEntity, GameState::Starts[a_start].perks, GameState::Starts[a_start].turns);
    for (const std::pair<std::string, ClassInstance>& classPair : GameState::Starts[a_start].classes) {
        for (size_t level = 0; level < classPair.second.level; ++level) {
            for (size_t levelUpEffect = 0; levelUpEffect < GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects.size(); ++levelUpEffect) {
                GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects[levelUpEffect](*this, player, true);
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

size_t GameState::PushBackRoom(size_t a_index) {
    this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
    return this->usedRooms++;
}

int32_t GameState::RollDice(size_t a_amount, size_t a_die) {
    int32_t total = 0;
    std::uniform_int_distribution<uint32_t>& dieDistribution = DICE.at(a_die);
    for (size_t i = 0; i < a_amount; ++i) {
        total += dieDistribution(generator);
    }
    return total;
}

std::vector<StartData> GameState::Starts = {};