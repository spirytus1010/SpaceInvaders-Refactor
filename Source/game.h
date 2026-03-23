#pragma once

#include "RaylibWrapper.h"

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

// Alien shooting offset
constexpr float ALIEN_SHOOT_Y_OFFSET = 40.0f;

enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
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
	static constexpr float RENDER_SIZE = 100.0f;
	static constexpr int INITIAL_LIVES = 3;
	static constexpr float ANIMATION_INTERVAL = 0.4f;
	static constexpr int MAX_TEXTURE_INDEX = 2;

	Player() = default;
	explicit Player(int screenWidth) noexcept;
	void Render(const Texture2D& texture) const noexcept;
	void Update() noexcept;

	bool IsAlive() const noexcept { return lives > 0; }
	void TakeDamage() noexcept { --lives; }
	int GetLives() const noexcept { return lives; }
	float GetX() const noexcept { return x_pos; }
	float GetBaseHeight() const noexcept { return player_base_height; }
	int GetActiveTexture() const noexcept { return activeTexture; }

	Rectangle Hitbox() const noexcept
	{
		return {
			x_pos - BASE_HEIGHT / 2.0f,
		    static_cast<float>(GetScreenHeight()) - player_base_height - BASE_HEIGHT / 2.0f,
			BASE_HEIGHT,
			BASE_HEIGHT
		};
	}

private:
	float x_pos = 0;
	float speed = DEFAULT_SPEED;
	float player_base_height = BASE_HEIGHT;
	int lives = INITIAL_LIVES;
	int direction = 0;
	int activeTexture = 0;
	float timer = 0;
};

struct Projectile
{
	static constexpr int DEFAULT_SPEED = 15;
	static constexpr float HITBOX_WIDTH = 10.0f;
	static constexpr float HITBOX_HEIGHT = 40.0f;
	static constexpr float RENDER_SIZE = 50.0f;
	static constexpr float SPAWN_OFFSET = 60.0f;

	Vector2 position = { 0, 0 };
	int speed = DEFAULT_SPEED;
	bool active = true;

	Projectile(Vector2 pos, int spd = DEFAULT_SPEED) noexcept
		: position(pos), speed(spd) {
	}

	void Update() noexcept;
	void Render(const Texture2D& texture) const noexcept;

	Rectangle Hitbox() const noexcept
	{
		return { position.x - HITBOX_WIDTH / 2.0f, position.y - HITBOX_HEIGHT / 2.0f, HITBOX_WIDTH, HITBOX_HEIGHT };
	}
};

struct Wall
{
	static constexpr float Y_OFFSET = 250.0f;
	static constexpr int INITIAL_HEALTH = 50;
	static constexpr float HITBOX_WIDTH = 150.0f;
	static constexpr float HITBOX_HEIGHT = 80.0f;
	static constexpr float RENDER_SIZE = 200.0f;

	Wall() = default;
	explicit Wall(Vector2 pos) noexcept;

	void Render(const Texture2D& texture) const noexcept;
	void Update() noexcept;

	void Hit() noexcept { --health; }
	bool IsActive() const noexcept { return active; }
	Vector2 GetPosition() const noexcept { return position; }

	Rectangle Hitbox() const noexcept
	{
		return { position.x - HITBOX_WIDTH / 2.0f, position.y - HITBOX_HEIGHT / 2.0f, HITBOX_WIDTH, HITBOX_HEIGHT };
	}

private:
	Vector2 position = { 0, 0 };
	bool active = true;
	int health = INITIAL_HEALTH;
};

struct Alien
{
	static constexpr float DEFAULT_SPEED = 2.0f;
	static constexpr float HITBOX_WIDTH = 80.0f;
	static constexpr float HITBOX_HEIGHT = 60.0f;
	static constexpr float RENDER_SIZE = 100.0f;
	static constexpr float DESCENT_AMOUNT = 50.0f;
	static constexpr int POINTS = 100;

	Alien() = default;
	explicit Alien(Vector2 pos) noexcept : position(pos) {}

	void Update() noexcept;
	void Render(const Texture2D& texture) const noexcept;

	bool IsActive() const noexcept { return active; }
	void Deactivate() noexcept { active = false; }
	Vector2 GetPosition() const noexcept { return position; }

	Rectangle Hitbox() const noexcept
	{
		return { position.x - HITBOX_WIDTH / 2.0f, position.y - HITBOX_HEIGHT / 2.0f, HITBOX_WIDTH, HITBOX_HEIGHT };
	}

private:
	Vector2 position = { 0, 0 };
	bool active = true;
	bool moveRight = true;
	float speed = DEFAULT_SPEED;
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;

	Star() = default;
	Star(Vector2 initPos, Color col, float sz) noexcept
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
	void Update();
	void Render() const;

private:
	State gameState = State::STARTSCREEN;

	int score = 0;

	int wallCount = 5;

	int shootTimer = 0;

	bool newHighScore = false;

	void Start();
	void End() noexcept;

	void Continue() noexcept;

	void SpawnAliens();

	bool CheckNewHighScore() noexcept;

	void InsertNewHighScore(const std::string& name);

	// Entity Storage and Resources
	Resources resources;

	Player player;

	std::vector<Projectile> playerProjectiles;
	std::vector<Projectile> enemyProjectiles;

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
	void UpdateEntities() noexcept;
	void HandleCollisions() noexcept;
	void HandlePlayerShooting();
	void HandleAlienShooting();
	void RemoveInactiveEntities();
	void CheckGameOver();
};