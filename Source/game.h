#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>

// Formation layout
constexpr int FORMATION_WIDTH = 8;
constexpr int FORMATION_HEIGHT = 5;
constexpr int ALIEN_SPACING = 80;
constexpr int FORMATION_START_X = 100;
constexpr int FORMATION_START_Y = 50;
constexpr int FORMATION_HORIZONTAL_OFFSET = 450;

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

struct Player
{
	static constexpr float DEFAULT_SPEED = 7.0f;
	static constexpr float BASE_HEIGHT = 70.0f;
	static constexpr float DEFAULT_RADIUS = 50.0f;
	static constexpr int INITIAL_LIVES = 3;
	static constexpr float ANIMATION_INTERVAL = 0.4f;
	static constexpr int MAX_TEXTURE_INDEX = 2;

	Player() = default;
	explicit Player(int screenWidth);
	void Render(const std::vector<Texture2D>& textures) const noexcept;
	void Update() noexcept;

	bool IsAlive() const noexcept { return lives > 0; }
	void TakeDamage() noexcept { --lives; }
	int GetLives() const noexcept { return lives; }
	float GetX() const noexcept { return x_pos; }
	float GetBaseHeight() const noexcept { return player_base_height; }
	float GetRadius() const noexcept { return radius; }

private:
	float x_pos = 0;
	float speed = DEFAULT_SPEED;
	float player_base_height = BASE_HEIGHT;
	float radius = DEFAULT_RADIUS;
	int lives = INITIAL_LIVES;
	int direction = 0;
	int activeTexture = 0;
	float timer = 0;

	EntityType type = EntityType::PLAYER;
};

struct Projectile
{
	static constexpr int DEFAULT_SPEED = 15;
	static constexpr float HALF_LENGTH = 15.0f;
	static constexpr float SPAWN_OFFSET = 60.0f;

	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = { 0,0 };
	int speed = DEFAULT_SPEED;
	bool active = true;
	EntityType type = {};

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	Projectile(Vector2 pos, EntityType t, int spd = DEFAULT_SPEED)
		: position(pos), type(t), speed(spd) {
	}

	void Update() noexcept;
	void Render(Texture2D texture) const noexcept;
};

struct Wall 
{
	static constexpr float Y_OFFSET = 250.0f;
	static constexpr int INITIAL_HEALTH = 50;
	static constexpr float DEFAULT_RADIUS = 60.0f;	

	Wall() = default;
	Wall(Vector2 pos, float rad);

	void Render(Texture2D texture) const noexcept;
	void Update() noexcept;

	void Hit() noexcept { --health; }
	bool IsActive() const noexcept { return active; }
	Vector2 GetPosition() const noexcept { return position; }
	float GetRadius() const noexcept { return radius; }

private:
	Vector2 position = { 0, 0 };
	bool active = true;
	int health = INITIAL_HEALTH;
	float radius = DEFAULT_RADIUS;
};

struct Alien
{
	static constexpr float DEFAULT_SPEED = 2.0f;
	static constexpr float DEFAULT_RADIUS = 30.0f;
	static constexpr float DESCENT_AMOUNT = 50.0f;
	static constexpr int POINTS = 100;

	Alien() = default; 
	explicit Alien(Vector2 pos) : position(pos) {}

	void Update() noexcept;
	void Render(Texture2D texture) const noexcept;

	bool IsActive() const noexcept { return active; }
	void Deactivate() noexcept { active = false; }
	Vector2 GetPosition() const noexcept { return position; }
	float GetRadius() const noexcept { return radius; }

private:
	Vector2 position = { 0, 0 };
	float radius = DEFAULT_RADIUS;
	bool active = true;
	bool moveRight = true;

	EntityType type = EntityType::ENEMY;

	float speed = DEFAULT_SPEED;
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;

	Star() = default;
	Star(Vector2 initPos, Color col, float sz)
		: initPosition(initPos)
		, position(initPos)
		, color(col)
		, size(sz)
	{
	}

	void Update(float starOffset) noexcept;
	void Render() const noexcept;
};

struct Background
{
	std::vector<Star> Stars;

	Background() = default;
	explicit Background(int starAmount);

	void Update(float offset) noexcept;
	void Render() const noexcept;
};

struct Game
{
	Game();
	void Update();
	void Render() const noexcept;

private:
	State gameState = State::STARTSCREEN;

	int score = 0;

	int wallCount = 5;

	float shootTimer = 0;

	bool newHighScore = false;


	void Start();
	void End();

	void Continue();



	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) const noexcept;

	bool CheckNewHighScore();

	void InsertNewHighScore(const std::string& name);

	// Entity Storage and Resources
	Resources resources;

	Player player;

	std::vector<Projectile> Projectiles;

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

	Background background;

	std::string name;

	Rectangle textBox = { 600, 500, 225, 50 };
	bool mouseOnText = false;

	int framesCounter = 0;

	// Update
	void UpdateStartScreen();
	void UpdateGameplay();
	void UpdateEndScreen();

	// Gameplay
	void UpdateEntities();
	void HandleCollisions();
	void HandlePlayerShooting();
	void HandleAlienShooting();
	void RemoveInactiveEntities();
	void CheckGameOver();
};
