// CSC 134
// M6LAB1
// Daley Ottersbach
// 4/20/2026

#include <iostream>
#include <string>
#include <vector>
#include <map>

// Define constants for directions
enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NUM_DIRECTIONS
};

// Direction strings for output
const std::string DIRECTION_NAMES[NUM_DIRECTIONS] = {
    "north", "east", "south", "west"
};

// Define constants for rooms
enum Room {
    ENTRANCE_HALL,
    LIBRARY,
    KITCHEN,
    GARDEN,
    BASEMENT,
    NUM_ROOMS
};

// Room names array
const std::string roomNames[NUM_ROOMS] = {
    "Entrance Hall",
    "Library",
    "Kitchen",
    "Garden",
    "Basement"
};

// Room descriptions array
const std::string roomDescriptions[NUM_ROOMS] = {
    "A grand entrance hall with a dusty chandelier hanging above.",
    "Walls lined with ancient books. The air smells of old paper.",
    "A spacious kitchen with an old stove and wooden counter.",
    "An overgrown garden with stone pathways and a small fountain.",
    "A dark, damp basement with cobwebs in the corners."
};

int main() {
    
    // Adjacency list using a 2D array
    // connections[from_room][direction] = to_room or -1 if no connection
    int connections[NUM_ROOMS][NUM_DIRECTIONS];
    
    // Initialize all connections to -1 (no connection)
    for (int i = 0; i < NUM_ROOMS; i++) {
        for (int j = 0; j < NUM_DIRECTIONS; j++) {
            connections[i][j] = -1;
        }
    }
    
    // Define the connections between rooms using the Room enum
    // Entrance Hall connections
    connections[ENTRANCE_HALL][NORTH] = LIBRARY;    // Entrance Hall -> North -> Library
    connections[ENTRANCE_HALL][EAST] = KITCHEN;     // Entrance Hall -> East -> Kitchen
    connections[ENTRANCE_HALL][SOUTH] = -1;         // No connection south
    connections[ENTRANCE_HALL][WEST] = GARDEN;      // Entrance Hall -> West -> Garden
    
    // Library connections
    connections[LIBRARY][NORTH] = -1;               // No connection north
    connections[LIBRARY][EAST] = -1;                // No connection east
    connections[LIBRARY][SOUTH] = ENTRANCE_HALL;    // Library -> South -> Entrance Hall
    connections[LIBRARY][WEST] = -1;                // No connection west
    
    // Kitchen connections
    connections[KITCHEN][NORTH] = -1;               // No connection north
    connections[KITCHEN][EAST] = -1;                // No connection east
    connections[KITCHEN][SOUTH] = BASEMENT;         // Kitchen -> South -> Basement
    connections[KITCHEN][WEST] = ENTRANCE_HALL;     // Kitchen -> West -> Entrance Hall
    
    // Garden connections
    connections[GARDEN][NORTH] = -1;                // No connection north
    connections[GARDEN][EAST] = ENTRANCE_HALL;      // Garden -> East -> Entrance Hall
    connections[GARDEN][SOUTH] = -1;                // No connection south
    connections[GARDEN][WEST] = -1;                 // No connection west
    
    // Basement connections
    connections[BASEMENT][NORTH] = KITCHEN;         // Basement -> North -> Kitchen
    connections[BASEMENT][EAST] = -1;               // No connection east
    connections[BASEMENT][SOUTH] = -1;              // No connection south
    connections[BASEMENT][WEST] = -1;               // No connection west
    
    // Game state
    int currentRoom = ENTRANCE_HALL; // Start in the Entrance Hall
    bool gameRunning = true;
    
    // Game loop
    while (gameRunning) {
        // Display current room information
        std::cout << "\nYou are in the " << roomNames[currentRoom] << std::endl;
        std::cout << roomDescriptions[currentRoom] << std::endl;
        
        // Show available exits
        std::cout << "Exits: ";
        bool hasExits = false;
        for (int dir = 0; dir < NUM_DIRECTIONS; dir++) {
            if (connections[currentRoom][dir] != -1) {
                std::cout << DIRECTION_NAMES[dir] << " ";
                hasExits = true;
            }
        }
        if (!hasExits) {
            std::cout << "none";
        }
        std::cout << std::endl;
        
        // Get player input
        std::string command;
        std::cout << "\nWhat would you like to do? ";
        std::cin >> command;
        
        // Process movement commands
        if (command == "north" || command == "n") {
            if (connections[currentRoom][NORTH] != -1) {
                currentRoom = connections[currentRoom][NORTH];
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "east" || command == "e") {
            if (connections[currentRoom][EAST] != -1) {
                currentRoom = connections[currentRoom][EAST];
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "south" || command == "s") {
            if (connections[currentRoom][SOUTH] != -1) {
                currentRoom = connections[currentRoom][SOUTH];
            } else {
                std::cout << "You can't go that way." << std::endl;
            }
        } else if (command == "west" || command == "w") {
            if (connections[currentRoom][WEST] != -1) {
                currentRoom = connections[currentRoom][WEST];
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