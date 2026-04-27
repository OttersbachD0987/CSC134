// CSC 134
// M6LAB1
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <vector>
#include <limits>
#include <concepts>
#include <print>
#include <fstream>
#include <thread>
#include <chrono>
#include <locale>

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
void GetTerminalSize(uint32_t& a_cols, uint32_t& a_rows) {
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);
    a_cols = consoleScreenBufferInfo.dwSize.X - 1;
    a_rows = consoleScreenBufferInfo.dwSize.Y - 1;
}
#elif defined(__linux__) || defined(__APPLE__)
#include <cmath>
#include <cstring>
#include <sys/ioctl.h>
void GetTerminalSize(uint32_t& a_cols, uint32_t& a_rows) {
    struct winsize window;
    ioctl(fileno(stdout), TIOCGWINSZ, &window);
    a_cols = window.ws_col;
    a_rows = window.ws_row;
}
#else
void GetTeriminalSize(uint32_t& a_cols, uint32_t& a_rows) {
    std::cerr << "I don't know how you did this, but this platform isn't supported.\n";
}
#endif

/// @brief Get a validated input.
/// @tparam T An integral type.
/// @param a_prompt The prompt to display.
/// @param a_value The value to feed into. 
/// @param a_min The minimum value.
/// @param a_max The maximum value.
template<std::integral T>
void BoundedInput(const char* a_prompt, T& a_value, T a_min, T a_max) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value) || a_value < a_min || a_value > a_max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cout << "\x1b[F\x1b[0J" << a_prompt;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

/// @brief Get a validated input.
/// @tparam T The type to get.
/// @param a_prompt The prompt to display.
/// @param a_value The value to feed into.
template<typename T>
void SafeInput(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cout << "\x1b[F\x1b[0J" << a_prompt;
    }
}

/// @brief Eat one input on the same line, works like sys("pause") on windows.
inline void EatInput(void) {
    std::cout << "Press any key to continue..." << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cout << "\x1B[F\x1B[0K\r" << std::flush;
}

float RandF(void) {
    return (float)rand() / (float)RAND_MAX;
}

enum class ShellType {
    BLANK,
    LIVE,
};

enum class ShootResult {
    BLANK_OTHER,
    BLANK_SELF,
    LIVE,
    FAULTY_VALUE,
};

enum class ItemType {
    NONE,
    BOOSTER,
    SWITCHER,
    BANDAGE,
    COUNT
};

const char* ITEM_NAMES[(size_t)ItemType::COUNT] = {
    "None",
    "Booster",
    "Switcher",
    "Bandage",
};

struct GameState;
struct Player;

struct Personality {
public:
    float caution;     // Prefer more certain options
    float paranoia;    // Prefer own safety
    float aggression;  // Prefer harming opponents
    float sadism;      // Prefer damaging
    float bloodlust;   // Prefer killing
    float retribution; // Prefer targeting damagers
    float antagonism;  // Prefer targeting others

    int32_t EvaluateSituation(GameState& a_gameState, Player& a_player);

    void DebugInfo(std::ostream& a_stream);
};

struct Player {
public:
    bool player;
    int32_t id;
    int32_t lives;
    int32_t lastHurt;
    int64_t winnings;
    ItemType items[4];
    std::string name;
    Personality personality;
};

enum class GameInitializationMode {
    BY_STRING,
    BY_INPUT,
    ALL_AI,
};

struct GameState {
public:
    std::vector<Player> players;
    std::vector<ShellType> rounds;
    int32_t currentTurn;
    int32_t startingRoundCount;
    int32_t liveCount;
    int32_t livesCount;
    int32_t currentRound;
    int32_t currentGambit;
    int64_t lastPot;
    int64_t winnings;
    bool forceTurn;
    std::ofstream logFile;

    GameState(void) {
        this->logFile = std::ofstream("Logs/BuckshotLogs.log");
        this->players = std::vector<Player>();
        this->rounds = std::vector<ShellType>();
        this->currentTurn = 0;
        this->startingRoundCount = 0;
        this->liveCount = 0;
        this->livesCount = 0;
        this->currentRound = 1;
        this->currentGambit = 1;
        this->lastPot = 50000;
        this->forceTurn = false;
        logFile << ">>> NEW BUCKSHOT INSTANCE <<<\n";
        
    }

    void PlayersByInput(int32_t a_playerCount) {
        int32_t type;
        int32_t aiNumber = 1;
        std::string name;

        for (int32_t i = 0; i < a_playerCount; ++i) {
            std::println("Player {}:\n1) Player\n2) AI", i + 1);
            BoundedInput<int32_t>("Choice: ", type, 1, 2);
            switch (type) {
                case 1: {
                    SafeInput<std::string>("Name: ", name);
                    this->players.push_back(Player{true, i, 4, -1, 0,
                        {ItemType::NONE, ItemType::NONE, ItemType::NONE, ItemType::NONE},
                        name,
                        Personality{
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f
                        }
                    });
                    break;
                }
                case 2: {
                    this->players.push_back(Player{false, i, 4, -1, 0,
                        {ItemType::NONE, ItemType::NONE, ItemType::NONE, ItemType::NONE},
                        std::format("AI {}", aiNumber++),
                        Personality{
                            0.5f + RandF(),
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                        }
                    });
                    break;
                }
            }
            logFile << "> Player " << (i) << "\n";
            if (type == 2) {
                players.back().personality.DebugInfo(logFile);
            }
        }
    }

    void PlayersByString(std::string a_players) {
        int32_t aiNumber = 1;
        std::string name;

        for (int32_t i = 0; i < a_players.size(); ++i) {
            logFile << "> Player " << (i) << "\n";
            switch (toupper(a_players[i])) {
                case 'P': {
                    SafeInput<std::string>("Name: ", name);
                    this->players.push_back(Player{true, i, 4, -1, 0,
                        {ItemType::NONE, ItemType::NONE, ItemType::NONE, ItemType::NONE},
                        name,
                        Personality{
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f, 
                            0.0f
                        }
                    });
                    break;
                }
                case 'A': {
                    this->players.push_back(Player{false, i, 4, -1, 0,
                        {ItemType::NONE, ItemType::NONE, ItemType::NONE, ItemType::NONE},
                        std::format("AI {}", aiNumber++),
                        Personality{
                            0.5f + RandF(),
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                            0.5f + RandF() * 1.5f,
                        }
                    });
                    players.back().personality.DebugInfo(logFile);
                    break;
                }
            }
        }
    }

    void LogRounds(void) {
        logFile << "> Rounds [";
        if (rounds.size() > 0) {
            for (size_t i = 0; i < rounds.size() - 1; ++i) {
                logFile << (rounds[i] == ShellType::BLANK ? "o" : "•");
            }
            logFile << std::format("\u032D\u030C{}]\n", rounds.back() == ShellType::BLANK ? "o" : "•");
        } else {
            logFile << "]\n";
        }
        logFile << std::format("Total: {}\nLive: {}\nBlank: {}\n", rounds.size(), liveCount, rounds.size() - liveCount);
    }

    void LoadRounds(int32_t a_base, int32_t a_rand) {
        rounds.clear();
        startingRoundCount = a_base + (rand() % a_rand);
        liveCount = 0;
        
        while (rounds.size() < startingRoundCount) {
            ShellType round = (ShellType)(rand() % 2 == 0);
            rounds.push_back(round);
            if (round == ShellType::LIVE) {
                ++liveCount;
            }
        }

        logFile << std::format("> Loading Rounds\nLIVE:  {:2}\nBLANK: {:2}\nTOTAL: {:2}\n", liveCount, startingRoundCount - liveCount, startingRoundCount);
        LogRounds();
    }

    ShootResult Shoot(int32_t a_shooter, int32_t a_target) {
        switch (rounds.back()) { 
            case ShellType::BLANK:
                rounds.pop_back(); 
                logFile << std::format("> Player {} shot Player {} (BLANK)\n", a_shooter, a_target);
                LogRounds();
                return (a_shooter != a_target) ? ShootResult::BLANK_OTHER : ShootResult::BLANK_SELF;
            case ShellType::LIVE:
                rounds.pop_back();
                --liveCount;
                players[a_target].lastHurt = a_shooter;
                players[a_target].lives -= 1;
                logFile << std::format("> Player {} shot Player {} (LIVE)\n", a_shooter, a_target);
                LogRounds();
                return ShootResult::LIVE;
        }
        return ShootResult::FAULTY_VALUE;
    }

    void NextTurn(void) {
        if (players[(currentTurn = (currentTurn + 1) % players.size())].lives <= 0) {
            NextTurn();
        }
    }

    int64_t CurrentPot(void) {
        winnings = lastPot;
        lastPot += 50000 * (1.0f + RandF() + logf(currentGambit));
        return winnings;
    }
};


int32_t Personality::EvaluateSituation(GameState& a_gameState, Player& a_player) {
    float chanceLive = (float)a_gameState.liveCount / (float)a_gameState.rounds.size();
    if (chanceLive <= 0.0f) {
        return a_player.id;
    } 
    bool isLive = chanceLive >= 1.0f;
    chanceLive *= caution;

    float switchMod = 0.5f;
    bool boosted = false;

    for (int32_t i = 0; i < 4; ++i) {
        switch (a_player.items[i]) {
            case ItemType::BOOSTER: {
                if (!boosted && RandF() < chanceLive) {
                    std::println("{} injects themselves with a \x1b[95mBOOSTER\x1b[39m", a_player.name);
                    a_player.items[i] = ItemType::NONE;
                    a_gameState.forceTurn = true;
                    boosted = true;
                }
                break;
            }
            case ItemType::SWITCHER: {
                if (RandF() < switchMod) {
                    std::println("{} flicks a \x1b[91mS\x1b[96mw\x1b[91mI\x1b[96mt\x1b[91mC\x1b[96mh\x1b[91mE\x1b[96mr\x1b[39m", a_player.name);
                    a_player.items[i] = ItemType::NONE;
                    if (a_gameState.rounds.back() == ShellType::BLANK) {
                        ++a_gameState.liveCount;
                        a_gameState.rounds[a_gameState.rounds.size() - 1] = ShellType::LIVE;
                    } else {
                        --a_gameState.liveCount;
                        a_gameState.rounds[a_gameState.rounds.size() - 1] = ShellType::BLANK;
                    }
                    chanceLive = (float)a_gameState.liveCount / (float)a_gameState.rounds.size();
                    if (chanceLive <= 0.0f) {
                        return a_player.id;
                    } 
                    isLive = chanceLive >= 1.0f;
                    chanceLive *= caution;
                    switchMod *= 0.5f;
                }
                break;
            }
            case ItemType::BANDAGE: {
                if (a_player.lives < a_gameState.livesCount && RandF() * a_player.personality.paranoia - RandF() * a_player.personality.bloodlust > 0.0f) {
                    std::println("{} wraps a \x1b[95mBANDAGE\x1b[39m around their arm", a_player.name);
                    a_player.items[i] = ItemType::NONE;
                    ++a_player.lives;
                }
                break;
            }
        }
    } 

    std::vector<std::pair<int32_t, float>> targetEvaluations = std::vector<std::pair<int32_t, float>>();

    std::pair<int32_t, float> curTarget = std::pair<int32_t, float>(-1, -1.0f);
    float maximum = 0.0f;

    for (const Player& target : a_gameState.players) {
        if (target.lives <= 0) {
            continue;
        }
        curTarget.first = target.id;
        if (target.id == a_player.id) {
            if (isLive) {
                continue;
            }
            curTarget.second = paranoia / chanceLive;
        } else {
            curTarget.second = chanceLive * aggression * (target.lives == 1 ? bloodlust : sadism) * (target.id == a_player.lastHurt ? retribution : antagonism);
        }
        maximum += curTarget.second;
        a_gameState.logFile << std::format("> Player {} ranks shooting Player {} as {}\n", a_player.id, target.id, curTarget.second);
        targetEvaluations.push_back(curTarget);
    }

    float choice = RandF() * maximum;
    for (const std::pair<int32_t, float>& targetPair : targetEvaluations) {
        if ((choice -= targetPair.second) <= 0.0f) {
            return targetPair.first;
        }
    }
    return -1;
}

void Personality::DebugInfo(std::ostream& a_stream) {
    /*
        caution
        paranoia
        aggression
        sadism
        bloodlust
        retribution
        antagonism        
    */
    a_stream << std::format("Caution:     {:<5.3f}\nParanoia:    {:5.3f}\nAggression:  {:5.3f}\nSadism:      {:5.3f}\nBloodlust:   {:5.3f}\nRetribution: {:5.3f}\nAntagonism:  {:5.3f}\n", caution, paranoia, aggression, sadism, bloodlust, retribution, antagonism);
}

std::string FormatNumberFancy(int32_t a_number) {
    std::string returner = "\x1b[38;5;8m";
    for (int i = 0; i < (a_number < 10 ? 2 : (a_number < 100 ? 1 : 0)); ++i) {
        returner += "0";
    }
    returner += std::format("\x1b[39m{}", a_number);
    return returner;
}

int main(int argc, char** argv) {
    std::locale::global(std::locale("en_US.UTF-8"));
    bool doDelay = true;
    bool doConfirm = true;
    std::string players = "";
    for (int32_t i = 1; i < argc; ++i) {
        std::println("Arg ({}/{}) = '{}'", i, argc - 1, argv[i]);
        if (strcmp(argv[i], "--nd") == 0) {
            doDelay = false;
        } else if (strcmp(argv[i], "--nc") == 0) {
            doConfirm = false;
        } else if (strcmp(argv[i], "--players") == 0) {
            players = argv[++i];
        }
    }
    srand(time(0));
    GameState game = GameState();
    if (players.empty()) {
        int32_t participants = 2;
        BoundedInput<int32_t>("Number of players: ", participants, 2, std::numeric_limits<int32_t>::max());
        game.PlayersByInput(participants);
    } else {
        game.PlayersByString(players);
    }
    game.livesCount = rand() % (int32_t)ceilf(logf(game.currentGambit + 1)) + 3;
    for (Player& player : game.players) {
        player.lives = game.livesCount;
    }
    game.LoadRounds(ceilf((24 + game.currentGambit) * 0.12f), 1);
    std::println("\x1b[H\x1b[2JGAMBIT {} | ROUND {}\n{} participants - {} rounds: {} live, {} blank", FormatNumberFancy(game.currentGambit), FormatNumberFancy(game.currentRound), game.players.size(), game.startingRoundCount, game.liveCount, game.startingRoundCount - game.liveCount);
    if (doConfirm) EatInput();
    std::cout << "\x1b[2;1H\x1b[0J";
    bool running = true;
    int32_t target = 0;
    while (running) {
        game.logFile.flush();
        Player& currentPlayer = game.players[game.currentTurn];
        std::println("{}'s Turn", currentPlayer.name);
        for (const Player& player : game.players) {
            std::print("{:>2}) [", (player.id + 1));
            for (int32_t i = 0; i < player.lives; ++i) {
                std::cout << "█";
            }
            for (int32_t i = 0; i < game.livesCount - player.lives; ++i) {
                std::cout << "░";
            }
            std::cout << "] " << player.name << std::endl;
        }
        for (int32_t i = 0; i < 4; ++i) {
            std::print("\x1b[{};24H\x1b[0K|{}) {}", i + 1, i + 1, ITEM_NAMES[(size_t)currentPlayer.items[i]]);
        }
        std::print("\x1b[{};1H\x1b[0J", 3 + game.players.size());
        if (currentPlayer.player) {
            int32_t choice;
            h:
            BoundedInput<int32_t>(std::format("\x1b[{};1H\x1b[0J1) Items\n2) Shoot\nChoice: ", 3 + game.players.size()).c_str(), choice, 1, 2);
            switch (choice) {
                case 1: {
                    for (int32_t i = 0; i < 4; ++i) {
                        std::print("\x1b[{};24H\x1b[0K|{}) {}", i + 1, i + 1, ITEM_NAMES[(size_t)currentPlayer.items[i]]);
                    }
                    BoundedInput<int32_t>(std::format("\x1b[{};1H\x1b[0JItem: ", 3 + game.players.size()).c_str(), choice, 1, 4);
                    switch (currentPlayer.items[--choice]) {
                        case ItemType::BOOSTER: {
                            std::println("{} injects themselves with a \x1b[95mBOOSTER\x1b[39m", currentPlayer.name);
                            currentPlayer.items[choice] = ItemType::NONE;
                            game.forceTurn = true;
                            break;
                        }
                        case ItemType::SWITCHER: {
                            std::println("{} flicks a \x1b[91mS\x1b[96mw\x1b[91mI\x1b[96mt\x1b[91mC\x1b[96mh\x1b[91mE\x1b[96mr\x1b[39m", currentPlayer.name);
                            currentPlayer.items[choice] = ItemType::NONE;
                            if (game.rounds.back() == ShellType::BLANK) {
                                ++game.liveCount;
                                game.rounds[game.rounds.size() - 1] = ShellType::LIVE;
                            } else {
                                --game.liveCount;
                                game.rounds[game.rounds.size() - 1] = ShellType::BLANK;
                            }
                            break;
                        }
                        case ItemType::BANDAGE: {
                            if (currentPlayer.lives < game.livesCount) {
                                std::println("{} wraps a \x1b[95mBANDAGE\x1b[39m around their arm", currentPlayer.name);
                                currentPlayer.items[choice] = ItemType::NONE;
                                ++currentPlayer.lives;
                            }
                        }
                    }
                    for (int32_t i = 0; i < 4; ++i) {
                        std::print("\x1b[{};24H\x1b[0K|{}) {}", i + 1, i + 1, ITEM_NAMES[(size_t)currentPlayer.items[i]]);
                    }
                    goto h;
                }
                case 2: {
                    do {
                        BoundedInput<int32_t>(std::format("\x1b[{};1H\x1b[0JTarget: ", 3 + game.players.size()).c_str(), target, 1, (int32_t)game.players.size() + 1);
                        if (--target == game.players.size()) {
                            goto h;
                        }
                    } while (game.players[target].lives <= 0);
                    break;
                }
            }
        } else {
            target = currentPlayer.personality.EvaluateSituation(game, currentPlayer);
            currentPlayer = game.players[game.currentTurn];
        }
        
        if (target == currentPlayer.id) {
            std::print("{} puts the barrel to their chin", currentPlayer.name);
        } else {
            std::print("{} points the barrel at {}", currentPlayer.name, game.players[target].name);
        }

        if (doDelay) {
            for (uint32_t i = 0; i < 3; ++i) {
                std::print(".");
                std::this_thread::sleep_for(std::chrono::milliseconds(333));
            }
        }
        std::print("\n");

        switch (game.Shoot(currentPlayer.id, target)) {
            case ShootResult::BLANK_OTHER:
                if (!game.forceTurn) {
                    game.NextTurn();
                }
                game.forceTurn = false;
            case ShootResult::BLANK_SELF:
                std::println("*click*");
                break;
            case ShootResult::LIVE:
                if (!game.forceTurn) {
                    game.NextTurn();
                }
                game.forceTurn = false;
                std::println("*BANG*");
                break;
        }

        if (doConfirm) EatInput();

        int32_t alive = -1;
        for (const Player& player : game.players) {
            if (player.lives > 0) {
                if (alive == -1) {
                    alive = player.id;
                } else {
                    alive = -1;
                    break;
                }
            } else if (player.id == currentPlayer.id) {
                game.NextTurn();
            }
        }

        if (alive != -1) {
            game.players[alive].winnings += game.CurrentPot();
            int32_t choice;
            std::println("\x1b[H\x1b[2J\x1b[1m{}\x1b[22m is the winner.\n${}\n\n1) Another Gambit\n2) Fold", game.players[alive].name, std::format(std::locale(), "{:L}", game.players[alive].winnings));
            BoundedInput<int32_t>("Choice: ", choice, 1, 2);
            if (choice == 1) {
                ++game.currentGambit;
                game.currentRound = 1;
                game.livesCount = rand() % (int32_t)ceilf(logf(game.currentGambit + 1)) + 3;
                for (Player& player : game.players) {
                    player.lives = game.livesCount;
                }
                game.LoadRounds(ceilf((24 + game.currentGambit) * 0.12f), 1);
                std::println("\x1b[H\x1b[2JGAMBIT {} | ROUND {}\n{} participants - {} rounds: {} live, {} blank", FormatNumberFancy(game.currentGambit), FormatNumberFancy(game.currentRound), game.players.size(), game.startingRoundCount, game.liveCount, game.startingRoundCount - game.liveCount);
                if (doConfirm) EatInput();
                std::cout << "\x1b[2;1H\x1b[0J";
            } else if (choice == 2) {
                running = false;
            }
            continue;
        }

        if (game.rounds.size() <= 0) {
            ++game.currentRound;
            game.LoadRounds(
                (int32_t)ceilf(((5.0f * (1.0f - 1.0f / powf(game.currentRound, game.currentGambit))) + logf((float)game.currentRound)) * (24 + game.currentGambit) * 0.04f), 
                (int32_t)ceilf(log10f(powf((float)(game.currentRound + 1), 3.0f)))
            );
            for (int32_t i = 0; i < 4; ++i) {
                for (int32_t j = 0; j < game.players.size(); ++j) {
                    game.players[j].items[i] = (ItemType)(rand() % (int32_t)ItemType::COUNT);
                }
            }
            std::println("\x1b[H\x1b[2JGAMBIT {} | ROUND {}\n{} participants - {} rounds: {} live, {} blank", FormatNumberFancy(game.currentGambit), FormatNumberFancy(game.currentRound), game.players.size(), game.startingRoundCount, game.liveCount, game.startingRoundCount - game.liveCount);
            if (doConfirm) EatInput();
            std::cout << "\x1b[2;1H\x1b[0J";
        } else {
            std::cout << "\x1b[2;1H\x1b[0J";
        }
    }
    game.logFile.close();
    return 0;
}
