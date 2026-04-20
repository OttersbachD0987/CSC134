// CSC 134
// M6LAB1
// Daley Ottersbach
// 4/20/2026

#include <iostream>
#include <string>

// Define constants for directions
enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NUM_DIRECTIONS
};

// Direction strings for output
const std::string DIRECTION_NAMES[Direction::NUM_DIRECTIONS] = {
    "north", "east", "south", "west"
};

// Define constants for ROOMS
enum Room {
    NONE = -1,
    FOYER,
    LIBRARY,
    KITCHEN,
    DINING_HALL,
    BALL_ROOM,
    GARDEN,
    STAIRWELL,
    BASEMENT,
    NUM_ROOMS
};

struct RoomData {
public:
    std::string name;
    std::string description;
};

/*
   L
GFDDDsB
   K b

*/

const RoomData ROOMS[Room::NUM_ROOMS] = {
    {"Foyer"      , "A rather spacious room with rather sparse furnishings, nothing of importance seems to be here." },
    {"Library"    , "The smell of rotting wood is pungent, and the sight is no better. Sagging oak bookshelves, water damaged tomes and scrolls; naught of value can be found here." },
    {"Kitchen"    , "The remains of a timber stove are laying in a heap, crushed by a granite counter that has seen better days."},
    {"Dining Hall", "The once great table now is but a pile of splinters, the twisted remains of a cast iron chandelier buried in the center, likely haven fallen when the roof above caved in."},
    {"Ball Room"  , "To call this a room is a stretch, the entire east wall is gone, and the sounds of the ocean hitting the edge of the cliffside below fill the room."},
    {"Garden"     , "The thick bramble of plants is practically a jungle, it's a wonder that these plants can thrive in an environment so far from their native one."},
    {"Stairwell"  , "You're not sure if you trust these stairs, they creak and groan under the slightest movement, and flakes of the dark stone that comprises them seem to be falling without end."},
    {"Basement"   , "The darkness seems all consuming, with only a flickering candle in the center of the room providing any light."},
};

int connections[NUM_ROOMS][NUM_DIRECTIONS];

void DisplayRoom(const RoomData& a_room, int a_currentRoom) {
    std::cout << "\nYou are in the " << a_room.name << std::endl;
    std::cout << a_room.description << std::endl;
    
    // Show available exits
    std::cout << "Exits: ";
    bool hasExits = false;
    for (int dir = 0; dir < NUM_DIRECTIONS; dir++) {
        if (connections[a_currentRoom][dir] != -1) {
            std::cout << DIRECTION_NAMES[dir] << " ";
            hasExits = true;
        }
    }
    if (!hasExits) {
        std::cout << "none";
    }
    std::cout << std::endl;
}

int main() {    
    // Initialize all connections to -1 (no connection)
    for (int i = 0; i < NUM_ROOMS; i++) {
        for (int j = 0; j < NUM_DIRECTIONS; j++) {
            connections[i][j] = Room::NONE;
        }
    }
    
    // Define the connections between ROOMS using the Room enum
    // Entrance Hall connections
    connections[Room::FOYER][Direction::EAST] = Room::DINING_HALL;
    connections[Room::FOYER][Direction::WEST] = Room::GARDEN;

    connections[Room::DINING_HALL][Direction::NORTH] = Room::LIBRARY;
    connections[Room::DINING_HALL][Direction::EAST]  = Room::STAIRWELL;
    connections[Room::DINING_HALL][Direction::SOUTH] = Room::KITCHEN;
    connections[Room::DINING_HALL][Direction::WEST]  = Room::FOYER;

    connections[Room::GARDEN][Direction::EAST] = Room::FOYER;
    
    connections[Room::KITCHEN][Direction::NORTH] = Room::DINING_HALL;

    connections[Room::LIBRARY][Direction::SOUTH] = Room::DINING_HALL;
    
    connections[Room::STAIRWELL][Direction::EAST]  = Room::BALL_ROOM;
    connections[Room::STAIRWELL][Direction::SOUTH] = Room::BASEMENT;
    connections[Room::STAIRWELL][Direction::WEST]  = Room::DINING_HALL;
    
    connections[Room::BASEMENT][Direction::NORTH] = Room::STAIRWELL;

    connections[Room::BALL_ROOM][Direction::WEST]  = Room::STAIRWELL;
    
    // Game state
    int currentRoom = Room::FOYER; // Start in the Entrance Hall
    RoomData currentRoomData = ROOMS[currentRoom];
    bool gameRunning = true;
    
    DisplayRoom(currentRoomData, currentRoom);

    // Game loop
    while (gameRunning) {
        currentRoomData = ROOMS[currentRoom];
        
        // Get player input
        std::string command;
        std::cout << "\nWhat would you like to do: " << std::endl;
        std::getline(std::cin, command);
        std::transform(command.begin(),  command.end(), command.begin(), tolower);

        // Process movement commands
        if (command == "look" || command == "look around") {
            DisplayRoom(currentRoomData, currentRoom);
        } else if (command == "north" || command == "n") {
            if (connections[currentRoom][NORTH] != -1) {
                currentRoom = connections[currentRoom][NORTH];
                currentRoomData = ROOMS[currentRoom];
                DisplayRoom(currentRoomData, currentRoom);
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "east" || command == "e") {
            if (connections[currentRoom][EAST] != -1) {
                currentRoom = connections[currentRoom][EAST];
                currentRoomData = ROOMS[currentRoom];
                DisplayRoom(currentRoomData, currentRoom);
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "south" || command == "s") {
            if (connections[currentRoom][SOUTH] != -1) {
                currentRoom = connections[currentRoom][SOUTH];
                currentRoomData = ROOMS[currentRoom];
                DisplayRoom(currentRoomData, currentRoom);
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "west" || command == "w") {
            if (connections[currentRoom][WEST] != -1) {
                currentRoom = connections[currentRoom][WEST];
                currentRoomData = ROOMS[currentRoom];
                DisplayRoom(currentRoomData, currentRoom);
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "quit" || command == "q") {
            gameRunning = false;
        } else {
            std::cout << "I don't understand that command." << std::endl;
        }
    }
    
    std::cout << "Thanks for playing!" << std::endl;
    return 0;
}