#include <iostream>
#include <string>
#include <algorithm>  // for min and max
#include <vector>

class Weapon {
public:
    std::string name;
    int damage;
    int weight;
    int durability;

    Weapon(std::string a_name, int a_damage, int a_weight, int a_durability) {
        this->name = a_name;
        this->damage = a_damage;
        this->weight = a_weight;
        this->durability = a_durability;
    }

    void DisplayInfo(void) {
        std::cout << name << " - " << damage << " dmg, " << weight << " kg, " << durability << " durability";
    }

    int GetDamage(void) {
        return durability > 0 ? damage : 0;
    }

    void Use(void) {
        if (--durability < 0) {
            durability = 0;
        }
    }
};

class Character
{
private:
    std::string name;
    int currentHP;
    int maxHP;
    int strength;
    int dexterity;
    int intelligence;
    int level;
    int experience;
    int gold;
    int weaponEquipped;
    std::vector<Weapon> inventory;
    
    // Private helper - only used internally
    void checkLevelUp()
    {
        int xpNeeded = level * 100;  // 100 XP per level
        while (experience >= xpNeeded)
        {
            level++;
            experience -= xpNeeded;
            maxHP += 10;
            currentHP = maxHP;  // Full heal on level up!
            strength += 2;
            dexterity += 2;
            intelligence += 2;
            std::cout << name << " leveled up to " << level << "!" << std::endl;
            xpNeeded = level * 100;
        }
    }

public:
    // Constructor
    Character(std::string n, int hp, int str, int dex, int intel, Weapon weapon)
        : name(n), maxHP(hp), currentHP(hp), 
          strength(str), dexterity(dex), intelligence(intel),
          level(1), experience(0), gold(0)
    {
        // The initializer list above handles everything
        this->weaponEquipped = 0;
        this->inventory = std::vector<Weapon>();
        this->inventory.push_back(weapon);
    }
    
    // --- Combat Methods ---
    void takeDamage(int damage)
    {
        if (damage <= 0) return;
        currentHP = std::max(0, currentHP - damage);
    }
    
    void heal(int amount)
    {
        if (amount <= 0) return;
        currentHP = std::min(maxHP, currentHP + amount);
    }
    
    // --- Progression Methods ---
    void gainExperience(int xp)
    {
        if (xp <= 0) return;
        experience += xp;
        std::cout << name << " gained " << xp << " XP!" << std::endl;
        checkLevelUp();
    }
    
    void addGold(int amount)
    {
        if (amount <= 0) return;
        gold += amount;
    }
    
    bool spendGold(int amount)
    {
        if (amount <= 0 || amount > gold) return false;
        gold -= amount;
        return true;
    }
    
    // --- Getters (const = promises not to modify) ---
    std::string getName() const { return name; }
    int getHP() const { return currentHP; }
    int getMaxHP() const { return maxHP; }
    int getStrength() const { return strength; }
    int getDexterity() const { return dexterity; }
    int getIntelligence() const { return intelligence; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getGold() const { return gold; }
    std::vector<Weapon> GetInventory() const { return inventory; }
    Weapon GetWeapon() const { return inventory[weaponEquipped]; }
    int GetWeaponIndex() const { return weaponEquipped; }
    void EquipWeapon(int a_weapon) { weaponEquipped = a_weapon; }
    void AddItem(Weapon a_weapon) { inventory.push_back(a_weapon); }
    
    bool isAlive() const { return currentHP > 0; }
    
    // --- Display ---
    void display() const
    {
        std::cout << "\n=== " << name << " (Level " << level << ") ===" << std::endl;
        std::cout << "HP: " << currentHP << "/" << maxHP << std::endl;
        std::cout << "STR: " << strength << " | DEX: " << dexterity 
             << " | INT: " << intelligence << std::endl;
        std::cout << "XP: " << experience << "/" << (level * 100) << std::endl;
        std::cout << "Gold: " << gold << std::endl;
    }
};

int main()
{
    Character hero("Aldric", 100, 15, 12, 10, Weapon("Mace", 5, 2, 16));
    hero.display();
    
    std::cout << "\n--- Battle with Goblin! ---" << std::endl;
    hero.takeDamage(25);
    std::cout << "Took 25 damage. HP: " << hero.getHP() << std::endl;
    
    std::cout << "\n--- Victory! ---" << std::endl;
    hero.gainExperience(50);
    hero.addGold(30);
    
    std::cout << "\n--- Heal at Inn ---" << std::endl;
    if (hero.spendGold(10))
    {
        hero.heal(50);
        std::cout << "Healed! HP: " << hero.getHP() << std::endl;
    }
    
    std::cout << "\n--- Grinding XP ---" << std::endl;
    hero.gainExperience(60);  // This should trigger level up!
    
    hero.display();
    
    return 0;
}