#include "game.h"
#include <vector>
#include <fstream>
#include <algorithm>

namespace {

	template<typename To, typename From>
	constexpr To narrow_cast(From&& value) noexcept {
		return static_cast<To>(std::forward<From>(value));
	}

	float lineLength(Vector2 A, Vector2 B) noexcept
	{
		const float dx = B.x - A.x; 
		const float dy = B.y - A.y; 
		return sqrtf(dx * dx + dy * dy);
	}

	bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept
	{
		return lineLength(circlePos, point) < radius;
	}

}

void Game::Start()
{
	// creating walls 
	const float window_width = static_cast<float>(GetScreenWidth()); 
	const float window_height = static_cast<float>(GetScreenHeight()); 
	const float wall_distance = window_width / (wallCount + 1); 
	for (int i = 0; i < wallCount; i++)
	{
		const float x = wall_distance * (i + 1);
		const float y = window_height - Wall::Y_OFFSET; 

		Walls.emplace_back(Vector2{ x, y }, Wall::DEFAULT_RADIUS);
	}

	//creating player
	player = Player(GetScreenWidth());

	//creating aliens
	SpawnAliens();

	//creating background
	background = Background(600);

	//reset score
	score = 0;

	gameState = State::GAMEPLAY;

}

void Game::End() noexcept
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	Projectiles.clear();
	Walls.clear();
	Aliens.clear();
	name.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
}


void Game::Continue() noexcept
{
	gameState = State::STARTSCREEN;
}

void Game::Update()
{
	switch (gameState)
	{
	case State::STARTSCREEN:
		UpdateStartScreen();
		break;
	case State::GAMEPLAY:
		UpdateGameplay();
		break;
	case State::ENDSCREEN:
		UpdateEndScreen();
		break;
	}
}

void Game::UpdateStartScreen()
{
	if (IsKeyReleased(KEY_SPACE))
	{
		Start();
	}
}

void Game::UpdateGameplay()
{
	if (IsKeyReleased(KEY_Q))
	{
		End();
		return;
	}

	UpdateEntities();
	HandleCollisions();
	HandlePlayerShooting();
	HandleAlienShooting();
	RemoveInactiveEntities();
	CheckGameOver();
}

void Game::UpdateEntities() noexcept
{
	player.Update();

	for (auto& projectile : Projectiles) {
		projectile.Update();
	}

	for (auto& wall : Walls) {
		wall.Update();
	}

	for (auto& alien : Aliens) {
		alien.Update();
	}

	const Vector2 playerPos = { player.GetX(), static_cast<float>(player.GetBaseHeight()) }; 
	const Vector2 cornerPos = { 0, static_cast<float>(player.GetBaseHeight()) }; 
	const float offset = lineLength(playerPos, cornerPos) * -1; 
	background.Update(offset / BACKGROUND_SCROLL_DIVISOR);
}

void Game::HandleCollisions() noexcept
{
	// Player projectiles vs aliens
	for (auto& projectile : Projectiles) {
		if (projectile.type == EntityType::PLAYER_PROJECTILE && projectile.active) {
			for (auto& alien : Aliens) {
				if (CheckCollision(alien.GetPosition(), alien.GetRadius(),
					projectile.lineStart, projectile.lineEnd)) {
					projectile.active = false;
					alien.Deactivate();
					score += Alien::POINTS;
				}
			}
		}
	}

	// Enemy projectiles vs player
	const Vector2 playerPos = { player.GetX(), static_cast<float>(GetScreenHeight()) - player.GetBaseHeight() }; 
	for (auto& projectile : Projectiles) {
		if (projectile.type == EntityType::ENEMY_PROJECTILE && projectile.active) {
			if (CheckCollision(playerPos, player.GetRadius(),
				projectile.lineStart, projectile.lineEnd)) {
				projectile.active = false;
				player.TakeDamage();
			}
		}
	}

	// All projectiles vs walls
	for (auto& projectile : Projectiles) {
		if (projectile.active) {
			for (auto& wall : Walls) {
				if (CheckCollision(wall.GetPosition(), wall.GetRadius(),
					projectile.lineStart, projectile.lineEnd)) {
					projectile.active = false;
					wall.Hit();
				}
			}
		}
	}
}

void Game::HandlePlayerShooting()
{
	if (!IsKeyPressed(KEY_SPACE)) return;

	const float y = static_cast<float>(GetScreenHeight()) 
		- player.GetBaseHeight()
		- Projectile::SPAWN_OFFSET;

	Projectiles.emplace_back(
		Vector2{ player.GetX(), y },
		EntityType::PLAYER_PROJECTILE
	);
}

void Game::HandleAlienShooting()
{
	shootTimer += 1;
	if (shootTimer <= FRAMES_PER_ALIEN_SHOT) return;

	shootTimer = 0;

	if (Aliens.empty()) return;

	const int randomIndex = GetRandomValue(0, narrow_cast<int>(Aliens.size()) - 1);

	Vector2 spawnPos = Aliens.at(randomIndex).GetPosition();
	spawnPos.y += 40;

	Projectiles.emplace_back(spawnPos, EntityType::ENEMY_PROJECTILE, -15);
}

void Game::RemoveInactiveEntities()
{
	std::erase_if(Projectiles, [](const Projectile& p) {
		return !p.active;
		});

	std::erase_if(Aliens, [](const Alien& a) noexcept {
		return !a.IsActive();
		});

	std::erase_if(Walls, [](const Wall& w) noexcept {
		return !w.IsActive();
		});
}

void Game::CheckGameOver()
{
	if (!player.IsAlive()) {
		End();
		return;
	}

	if (std::any_of(Aliens.begin(), Aliens.end(),
		[this](const Alien& alien) noexcept {
			return alien.GetPosition().y > GetScreenHeight() - player.GetBaseHeight();
		})) {
		End();
		return;
	}

	if (Aliens.empty()) {
		SpawnAliens();
	}
}

void Game::UpdateEndScreen()
{
	// Exit endscreen
	if (IsKeyReleased(KEY_ENTER) && !newHighScore)
	{
		Continue();
		return;
	}

	if (!newHighScore) return;

	if (CheckCollisionPointRec(GetMousePosition(), textBox))
	{
		mouseOnText = true;
	}
	else
	{
		mouseOnText = false;
	}

	if (mouseOnText)
	{
		SetMouseCursor(MOUSE_CURSOR_IBEAM);

		int key = GetCharPressed();
		while (key > 0)
		{
			if ((key >= 32) && (key <= 125) && (name.size() < 9))
			{
				name += narrow_cast<char>(key);
			}
			key = GetCharPressed();
		}

		if (IsKeyPressed(KEY_BACKSPACE))
		{
			if (!name.empty()) {
				name.pop_back();
			}
		}
	}
	else
	{
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	if (mouseOnText)
	{
		framesCounter++;
	}
	else
	{
		framesCounter = 0;
	}

	if (!name.empty() && name.size() <= 9 && IsKeyReleased(KEY_ENTER))
	{
		InsertNewHighScore(name);
		newHighScore = false;
		name.clear();
	}
}

void Game::Render() const
{
	switch (gameState)
	{
	case State::STARTSCREEN:

		DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);

		DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);

		break;
	case State::GAMEPLAY:

		//background render LEAVE THIS AT TOP
		background.Render();

		DrawText(TextFormat("Score: %i", score), 50, 20, 40, YELLOW);
		DrawText(TextFormat("Lives: %i", player.GetLives()), 50, 70, 40, YELLOW);

		//player rendering 
		player.Render(resources.getShipTexture(player.GetActiveTexture()));

		//projectile rendering
		for (const auto& projectile : Projectiles) {
			projectile.Render(resources.getLaserTexture());
		}

		// wall rendering 
		for (const auto& wall : Walls) {
			wall.Render(resources.getBarrierTexture());
		}

		//alien rendering  
		for (const auto& alien : Aliens) {
			alien.Render(resources.getAlienTexture());
		}

		break;
	case State::ENDSCREEN:

		if (newHighScore)
		{
			DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);



			// BELOW CODE IS FOR NAME INPUT RENDER
			DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

			DrawRectangleRec(textBox, LIGHTGRAY);
			if (mouseOnText)
			{
				// HOVER CONFIRMIATION
				DrawRectangleLines(narrow_cast<int>(textBox.x), narrow_cast<int>(textBox.y), narrow_cast<int>(textBox.width), narrow_cast<int>(textBox.height), RED);
			}
			else
			{
				DrawRectangleLines(narrow_cast<int>(textBox.x), narrow_cast<int>(textBox.y), narrow_cast<int>(textBox.width), narrow_cast<int>(textBox.height), DARKGRAY);
			}

			//Draw the name being typed out
			DrawText(name.c_str(), narrow_cast<int>(textBox.x + 5), narrow_cast<int>(textBox.y + 8), 40, MAROON);

			//Draw the text explaining how many characters are used
			DrawText(TextFormat("INPUT CHARS: %i/%i", narrow_cast<int>(name.size()), 9), 600, 600, 20, YELLOW);

			if (mouseOnText)
			{
				if (name.size() < 9)
				{
					// Draw blinking underscore char
					if (((framesCounter / 20) % 2) == 0)
					{
						DrawText("_", narrow_cast<int>(textBox.x + 8) + MeasureText(name.c_str(), 40), narrow_cast<int>(textBox.y + 12), 40, MAROON);
					}

				}
				else
				{
					//Name needs to be shorter
					DrawText("Press BACKSPACE to delete chars...", 600, 650, 20, YELLOW);
				}

			}

			// Explain how to continue when name is input
			if (!name.empty())
			{
				DrawText("PRESS ENTER TO CONTINUE", 600, 800, 40, YELLOW);
			}

		}
		else {
			// If no highscore or name is entered, show scoreboard and call it a day
			DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);

			DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

			for (size_t i = 0; i < Leaderboard.size(); i++)
			{
				const int yPos = 140 + narrow_cast<int>(i) * 40;
				DrawText(Leaderboard.at(i).name.c_str(), 50, yPos, 40, YELLOW);
				DrawText(TextFormat("%i", Leaderboard.at(i).score), 350, yPos, 40, YELLOW);
			}
		}




		break;
	default:
		//SHOULD NOT HAPPEN
		break;
	}
}

void Game::SpawnAliens()
{
	for (int row = 0; row < FORMATION_HEIGHT; row++) {
		for (int col = 0; col < FORMATION_WIDTH; col++) {
			const float x = static_cast<float>(FORMATION_START_X + FORMATION_HORIZONTAL_OFFSET + col * ALIEN_SPACING); 
			const float y = static_cast<float>(FORMATION_START_Y + row * ALIEN_SPACING);
			Aliens.emplace_back(Vector2{ x, y });
		}
	}
}

bool Game::CheckNewHighScore() noexcept
{
	return score > Leaderboard.back().score;
}

void Game::InsertNewHighScore(const std::string& playerName)
{
	PlayerData newData{ playerName, score };

	auto it = std::find_if(Leaderboard.begin(), Leaderboard.end(),
		[&newData](const PlayerData& entry) {
			return newData.score > entry.score;
		});

	if (it != Leaderboard.end()) {
		Leaderboard.insert(it, newData);
		Leaderboard.pop_back();
	}
}

bool Game::CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd)  const noexcept
{
	// check if either edge of line is within circle
	if (pointInCircle(circlePos, circleRadius, lineStart) || pointInCircle(circlePos, circleRadius, lineEnd))
	{
		return true;
	}

	const Vector2 A = lineStart;
	const Vector2 B = lineEnd;
	const Vector2 C = circlePos;

	const float length = lineLength(A, B);

	const float dotP = (((C.x - A.x) * (B.x - A.x)) + ((C.y - A.y) * (B.y - A.y))) / (length * length);

	const float closestX = A.x + (dotP * (B.x - A.x));
	const float closestY = A.y + (dotP * (B.y - A.y));

	constexpr float buffer = 0.1f;

	const float closeToStart = lineLength(A, { closestX, closestY });
	const float closeToEnd = lineLength(B, { closestX, closestY });

	const float closestLength = closeToStart + closeToEnd;

	if (std::abs(closestLength - length) <= buffer)
	{
		const float closeToCentre = lineLength(C, { closestX, closestY });
		return closeToCentre < circleRadius;
	}
	else
	{
		return false;
	}

}

Player::Player(int screenWidth) noexcept
	: x_pos(screenWidth / 2.0f)
	, speed(DEFAULT_SPEED)
	, player_base_height(BASE_HEIGHT)
	, radius(DEFAULT_RADIUS)
	, lives(INITIAL_LIVES)
	, direction(0)
	, activeTexture(0)
	, timer(0.0f)
	, type(EntityType::PLAYER)
{
}

void Player::Update() noexcept
{
	timer += GetFrameTime();
	//Movement
	direction = 0;
	if (IsKeyDown(KEY_LEFT))
	{
		direction--;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		direction++;
	}

	x_pos += speed * direction;

	if (x_pos < 0 + radius)
	{
		x_pos = 0 + radius;
	}
	else if (x_pos > GetScreenWidth() - radius)
	{
		x_pos = GetScreenWidth() - radius;
	}


	//Determine frame for animation
	if (timer > Player::ANIMATION_INTERVAL &&
		activeTexture == Player::MAX_TEXTURE_INDEX)
	{
		activeTexture = 0;
		timer = 0;
	}
	else if (timer > Player::ANIMATION_INTERVAL)
	{
		activeTexture++;
		timer = 0;
	}


}

void Player::Render(const Texture2D& texture) const noexcept
{
	const float window_height = static_cast<float>(GetScreenHeight());

	DrawTexturePro(texture,
		{
			0,
			0,
			static_cast<float>(texture.width),
			static_cast<float>(texture.height),
		},
		{
			x_pos, window_height - player_base_height,
			100,
			100,
		}, { 50, 50 },
		0,
		WHITE);
}



void Projectile::Update() noexcept
{
	position.y -= speed;

	// UPDATE LINE POSITION
	lineStart.y = position.y - 15;
	lineEnd.y = position.y + 15;

	lineStart.x = position.x;
	lineEnd.x = position.x;

	if (position.y < 0 || position.y > static_cast<float>(GetScreenHeight()))
	{
		active = false;
	}
}

void Projectile::Render(const Texture2D& texture) const noexcept
{
	DrawTexturePro(texture,
		{
			0,
			0,
			static_cast<float>(texture.width),
			static_cast<float>(texture.height),
		},
		{
			position.x,
			position.y,
			50,
			50,
		}, { 25 , 25 },
		0,
		WHITE);
}

Wall::Wall(Vector2 pos, float rad) noexcept
	: position(pos)
	, radius(rad)
	, active(true)
	, health(INITIAL_HEALTH)
{
}

void Wall::Render(const Texture2D& texture) const noexcept
{
	DrawTexturePro(texture,
		{
			0,
			0,
			static_cast<float>(texture.width),
			static_cast<float>(texture.height),
		},
		{
			position.x,
			position.y,
			200,
			200,
		}, { 100 , 100 },
		0,
		WHITE);


	DrawText(TextFormat("%i", health), static_cast<int>(position.x - 21), static_cast<int>(position.y + 10), 40, RED);

}

void Wall::Update() noexcept
{

	// set walls as inactive when out of health
	if (health < 1)
	{
		active = false;
	}


}

void Alien::Update() noexcept
{
	if (moveRight)
	{
		position.x += speed;

		if (position.x >= GetScreenWidth())
		{
			moveRight = false;
			position.y += DESCENT_AMOUNT;
		}
	}
	else
	{
		position.x -= speed;

		if (position.x <= 0)
		{
			moveRight = true;
			position.y += DESCENT_AMOUNT;
		}
	}
}

void Alien::Render(const Texture2D& texture) const noexcept
{
	DrawTexturePro(texture,
		{
			0,
			0,
			static_cast<float>(texture.width),
			static_cast<float>(texture.height),
		},
		{
			position.x,
			position.y,
			100,
			100,
		}, {50 , 50},
		0,
		WHITE);
}

//BACKGROUND
void Star::Update(float starOffset) noexcept
{
	position.x = initPosition.x + starOffset;
	position.y = initPosition.y;

}

void Star::Render() const noexcept
{
	DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), size, color);
}

Background::Background(int starAmount)
{
	for (int i = 0; i < starAmount; i++) {
		Vector2 pos = {
			static_cast<float>(GetRandomValue(-150, GetScreenWidth() + 150)),
			static_cast<float>(GetRandomValue(0, GetScreenHeight()))
		};
		float size = static_cast<float>(GetRandomValue(1, 4)) / 2.0f;

		Stars.emplace_back(pos, SKYBLUE, size);
	}
}

void Background::Update(float offset) noexcept
{
	for (auto& star : Stars) {
		star.Update(offset);
	}

}

void Background::Render() const noexcept
{
	for (const auto& star : Stars) {
		star.Render();
	}
}