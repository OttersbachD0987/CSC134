// CSC 134
// M6LAB1
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <limits>
#include <concepts>
#include <print>
#include <fstream>

template<std::integral T>
void BoundedInput(const char* a_prompt, T& a_value, T a_min, T a_max) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value) || a_value < a_min || a_value > a_max) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

float RandF() {
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
    ERROR,
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

    int32_t EvaluateSituation(const GameState& a_gameState, const Player& a_player);

    void DebugInfo(std::ostream& a_stream);
};

struct Player {
public:
    bool player;
    int32_t id;
    int32_t lives;
    int32_t lastHurt;
    Personality personality;
};

struct GameState {
public:
    std::vector<Player> players;
    std::vector<ShellType> rounds;
    int32_t currentTurn;
    int32_t startingRoundCount;
    int32_t liveCount;
    std::ofstream logFile;

    GameState(int32_t a_playerCount) {
        this->logFile = std::ofstream("Logs/BuckshotLogs.log");
        this->players = std::vector<Player>();
        this->rounds = std::vector<ShellType>();
        this->currentTurn = 0;
        this->startingRoundCount = 0;
        this->liveCount = 0;
        logFile << ">>> NEW BUCKSHOT INSTANCE <<<\n";

        this->players.push_back(Player{true, 0, 4, -1, 
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
        logFile << "> Player 0\n";
        for (int32_t i = 1; i < a_playerCount; ++i) {
            this->players.push_back(Player{false, i, 4, -1, 
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
            logFile << "> Player " << (i) << "\n";
            players.back().personality.DebugInfo(logFile);
        }
    }

    void LogRounds(void) {
        logFile << "> Rounds [";
        for (size_t i = 0; i < rounds.size() - 1; ++i) {
            logFile << (rounds[i] == ShellType::BLANK ? "o" : "•");
        }
        logFile << std::format("\u032D\u030C{}]\n", rounds.back() == ShellType::BLANK ? "o" : "•");
    }

    void LoadRounds(void) {
        rounds.clear();
        startingRoundCount = 5 + (rand() % 6);
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
                players[a_target].lastHurt = a_shooter;
                players[a_target].lives -= 1;
                logFile << std::format("> Player {} shot Player {} (LIVE)\n", a_shooter, a_target);
                LogRounds();
                return ShootResult::LIVE;
        }
        return ShootResult::ERROR;
    }

    void NextTurn(void) {
        if (players[(currentTurn = (currentTurn + 1) % players.size())].lives <= 0) {
            NextTurn();
        }
    }
};

int32_t Personality::EvaluateSituation(const GameState& a_gameState, const Player& a_player) {
    float chanceLive = (float)a_gameState.liveCount / (float)a_gameState.rounds.size();
    if (chanceLive <= 0.0f) {
        return a_player.id;
    } 

    bool isLive = chanceLive >= 1.0f;
    chanceLive *= caution;

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
    a_stream << std::format("Caution:     {:5.3f}\nParanoia:    {:5.3f}\nAggression:  {:5.3f}\nSadism:      {:5.3f}\nBloodlust:   {:5.3f}\nRetribution: {:5.3f}\nAntagonism:  {:5.3f}\n", caution, paranoia, aggression, sadism, bloodlust, retribution, antagonism);
}

int main(int argc, char** argv) {
    srand(time(0));
    int32_t participants = 2;
    BoundedInput<int32_t>("Number of players: ", participants, 2, std::numeric_limits<int32_t>::max());
    GameState game = GameState(participants);
    game.LoadRounds();
    std::println("{} participants - {} rounds: {} live, {} blank", game.players.size(), game.startingRoundCount, game.liveCount, game.startingRoundCount - game.liveCount);
    bool running = true;
    while (running) {
        game.logFile.flush();
        Player& currentPlayer = game.players[game.currentTurn];
        if (currentPlayer.player) {
            std::cout << "Player Turn" << std::endl;
            for (const Player& player : game.players) {
                std::cout << (player.id + 1) << ") [";
                for (int32_t i = 0; i < player.lives; ++i) {
                    std::cout << "█";
                }
                for (int32_t i = 0; i < 4 - player.lives; ++i) {
                    std::cout << "░";
                }
                std::cout << "] " << (player.player ? "Player" : std::format("AI {}", player.id)) << std::endl;
            }
            int32_t target = 0;
            BoundedInput<int32_t>("Target: ", target, 1, (int32_t)game.players.size());
            if (--target == currentPlayer.id) {
                std::println("Player puts the barrel to their chin...");
            } else {
                std::println("Player points the barrel at AI {}...", target);
            }
            switch (game.Shoot(currentPlayer.id, target)) {
                case ShootResult::BLANK_OTHER:
                    game.NextTurn();
                case ShootResult::BLANK_SELF:
                    std::println("*click*");
                    break;
                case ShootResult::LIVE:
                    game.NextTurn();
                    std::println("*BANG*");
                    break;
            }
        } else {
            std::println("AI {} Turn", game.currentTurn);
            int32_t target = currentPlayer.personality.EvaluateSituation(game, currentPlayer);
            currentPlayer = game.players[game.currentTurn];
            if (target == currentPlayer.id) {
                std::println("AI {} puts the barrel to their chin...", currentPlayer.id);
            } else {
                std::println("AI {} points the barrel at {}...", currentPlayer.id, (game.players[target].player ? std::format("Player") : std::format("AI {}", target)));
            }
            switch (game.Shoot(currentPlayer.id, target)) {
                case ShootResult::BLANK_OTHER:
                    game.NextTurn();
                case ShootResult::BLANK_SELF:
                    std::println("*click*");
                    break;
                case ShootResult::LIVE:
                    game.NextTurn();
                    std::println("*BANG*");
                    break;
            }
        }
    }
    game.logFile.close();
    return 0;
}
