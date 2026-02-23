#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>

// TODO: Make members private
// TODO: Magic numbers

// Formation layout
constexpr int FORMATION_WIDTH = 8;
constexpr int FORMATION_HEIGHT = 5;
constexpr int ALIEN_SPACING = 80;
constexpr int FORMATION_START_X = 100;
constexpr int FORMATION_START_Y = 50;

// Shooting timing 
constexpr int FRAMES_PER_ALIEN_SHOT = 59;  // ~1 second at 60 FPS

// Background
constexpr int STAR_COUNT = 600;
constexpr float BACKGROUND_SCROLL_DIVISOR = 15.0f;

enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType
{
	PLAYER,
	ENEMY,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
};

struct PlayerData
{
	std::string name;
	int score = 0;
};

// TODO: Two step initialization - add constructor Player(int screenWidth)
// TODO: Remove Initialize() method
// TODO: Add const to Render()
// TODO: Add getters: GetX(), GetY(), GetLives(), IsAlive()
struct Player
{
	static constexpr float DEFAULT_SPEED = 7.0f;
	static constexpr float BASE_HEIGHT = 70.0f;
	static constexpr float DEFAULT_RADIUS = 50.0f;
	static constexpr int INITIAL_LIVES = 3;
	static constexpr float ANIMATION_INTERVAL = 0.4f;
	static constexpr int MAX_TEXTURE_INDEX = 2;

	float x_pos = 0;
	float speed = DEFAULT_SPEED;                  
	float player_base_height = BASE_HEIGHT;      
	float radius = DEFAULT_RADIUS;                
	int lives = INITIAL_LIVES;                    
	int direction = 0;
	int activeTexture = 0;
	float timer = 0;

	EntityType type = EntityType::PLAYER;

	void Initialize();
	void Render(Texture2D texture);
	void Update();
	
};

// TODO: Add constructor
// TODO: Add const to Render()
// TODO: Add getters: IsActive(), Deactivate()

struct Projectile
{
	static constexpr int DEFAULT_SPEED = 15;
	static constexpr float HALF_LENGTH = 15.0f;

	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = { 0,0 };
	int speed = DEFAULT_SPEED;
	bool active = true;
	EntityType type = {};

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	void Update();
	void Render(Texture2D texture);
};

// TODO: Add defaults or constructor
// TODO: Check if 'rec' and 'color' are used, delete if not
// TODO: Add const to Render()
// TODO: Add getters: IsActive(), GetHealth()
struct Wall 
{
	static constexpr float Y_OFFSET = 250.0f;
	static constexpr int INITIAL_HEALTH = 50;
	static constexpr int DEFAULT_RADIUS = 60;

	Vector2 position = { 0, 0 };
	Rectangle rec{};
	bool active = true;
	Color color{};
	int health = INITIAL_HEALTH;                  
	int radius = DEFAULT_RADIUS;          

	void Render(Texture2D texture);
	void Update();
};

// TODO: Delete x and y (use position instead)
// TODO: Check if 'color' is used - delete if not
// TODO: Make moveRight and active private
// TODO: Add const to Render()
// TODO: Add parameter to Update(int screenWidth)
// TODO: Add getters: IsActive(), GetY(), GetPosition()
struct Alien
{
	static constexpr float DEFAULT_SPEED = 2.0f;
	static constexpr float DEFAULT_RADIUS = 30.0f;
	static constexpr float DESCENT_AMOUNT = 50.0f;
	static constexpr int POINTS = 100;

	Color color = WHITE;
	Vector2 position = { 0, 0 };
	int x = 0;
	int y = 0;
	float radius = DEFAULT_RADIUS;
	bool active = true;
	bool moveRight = true;

	EntityType type = EntityType::ENEMY;

	int speed = DEFAULT_SPEED;

	void Update();
	void Render(Texture2D texture);
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;
	void Update(float starOffset);
	void Render();
};

//TODO: similiar as before
struct Background
{
	

	std::vector<Star> Stars;

	void Initialize(int starAmount);
	void Update(float offset);
	void Render();

};

// TODO: 'rec' is unused
// TODO: PlayerPos, alienPos, cornerPos, offset only used in Update() - make local
// TODO: C-style array 'name' - use std::string instead
// TODO: Break Update() into smaller functions
// TODO: Pass string by const reference: InsertNewHighScore(const std::string& name)
// TODO: Add const to CheckCollision()
// TODO: LoadLeaderboard() is empty
// TODO: SaveLeaderboard() is incomplete
struct Game
{
	// Gamestate
	State gameState = State::STARTSCREEN;

	// Score
	int score = 0;

	// for later, make a file where you can adjust the number of walls (config file) 
	int wallCount = 5;

	//Aliens shooting
	float shootTimer = 0;

	//Aliens stuff? (idk cause liv wrote this)
	Rectangle rec = { 0, 0 ,0 ,0 }; 

	int formationWidth = FORMATION_WIDTH;
	int formationHeight = FORMATION_HEIGHT;
	int alienSpacing = ALIEN_SPACING;
	int formationX = FORMATION_START_X; 
	int formationY = FORMATION_START_Y;

	bool newHighScore = false;
	

	void Start();
	void End();

	void Continue();
	void Launch();

	void Update();
	void Render();

	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineTop, Vector2 lineBottom);

	bool CheckNewHighScore();

	void InsertNewHighScore(std::string name);

	void LoadLeaderboard();
	void SaveLeaderboard();


	// Entity Storage and Resources
	Resources resources;

	Player player;

	std::vector<Projectile> Projectiles;

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };
	
	Background background;



	Vector2 playerPos;
	Vector2 alienPos; 
	Vector2 cornerPos;
	float offset;



	//TEXTBOX ENTER
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'
	int letterCount = 0;

	Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;

	int framesCounter = 0;

};