#include "game.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

// TODO: Magic number
// TODO: Move math functions to MathUtils namespace
// TODO: pointInCircle() can be simplified: return (distance < radius);

// MATH FUNCTIONS
float lineLength(Vector2 A, Vector2 B) //Uses pythagoras to calculate the length of a line
{
	float length = sqrtf(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));

	return length;
}

bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) // Uses pythagoras to calculate if a point is within a circle or not
{
	float distanceToCentre = lineLength(circlePos, point);

	if (distanceToCentre < radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// TODO: Replace C-style casts with static_cast;

void Game::Start()
{
	// creating walls 
	float window_width = (float)GetScreenWidth(); 
	float window_height = (float)GetScreenHeight(); 
	float wall_distance = window_width / (wallCount + 1); 
	for (int i = 0; i < wallCount; i++)
	{
		float x = wall_distance * (i + 1);
		float y = window_height - Wall::Y_OFFSET;

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

void Game::End()
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	Projectiles.clear();
	Walls.clear();
	Aliens.clear();
	name.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
}

void Game::Continue()
{
	gameState = State::STARTSCREEN;
}

void Game::Launch()
{
	//LOAD SOME RESOURCES HERE
	resources.Load();
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

void Game::UpdateEntities()
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

	Vector2 playerPos = { player.x_pos, static_cast<float>(player.player_base_height) };
	Vector2 cornerPos = { 0, static_cast<float>(player.player_base_height) };
	float offset = lineLength(playerPos, cornerPos) * -1; 
	background.Update(offset / BACKGROUND_SCROLL_DIVISOR);
}

void Game::HandleCollisions()
{
	// Player projectiles vs aliens
	for (auto& projectile : Projectiles) {
		if (projectile.type == EntityType::PLAYER_PROJECTILE && projectile.active) {
			for (auto& alien : Aliens) {
				if (CheckCollision(alien.position, alien.radius,
					projectile.lineStart, projectile.lineEnd)) {
					projectile.active = false;
					alien.active = false;
					score += Alien::POINTS;
				}
			}
		}
	}

	// Enemy projectiles vs player
	Vector2 playerPos = { player.x_pos, static_cast<float>(GetScreenHeight()) - player.player_base_height };
	for (auto& projectile : Projectiles) {
		if (projectile.type == EntityType::ENEMY_PROJECTILE && projectile.active) {
			if (CheckCollision(playerPos, player.radius,
				projectile.lineStart, projectile.lineEnd)) {
				projectile.active = false;
				player.lives -= 1;
			}
		}
	}

	// All projectiles vs walls
	for (auto& projectile : Projectiles) {
		if (projectile.active) {
			for (auto& wall : Walls) {
				if (CheckCollision(wall.position, wall.radius,
					projectile.lineStart, projectile.lineEnd)) {
					projectile.active = false;
					wall.health -= 1;
				}
			}
		}
	}
}

void Game::HandlePlayerShooting()
{
	if (!IsKeyPressed(KEY_SPACE)) return;

	float y = static_cast<float>(GetScreenHeight())
		- player.player_base_height
		- Projectile::SPAWN_OFFSET;

	Projectiles.emplace_back(
		Vector2{ player.x_pos, y },
		EntityType::PLAYER_PROJECTILE
	);
}

void Game::HandleAlienShooting()
{
	shootTimer += 1;
	if (shootTimer <= FRAMES_PER_ALIEN_SHOT) return;

	shootTimer = 0;

	if (Aliens.empty()) return;

	int randomIndex = (Aliens.size() > 1) ? rand() % Aliens.size() : 0;

	Vector2 spawnPos = Aliens[randomIndex].position;
	spawnPos.y += 40;

	Projectiles.emplace_back(spawnPos, EntityType::ENEMY_PROJECTILE);
	Projectiles.back().speed = -15; 
}

void Game::RemoveInactiveEntities()
{
	std::erase_if(Projectiles, [](const Projectile& p) {
		return !p.active;
		});

	std::erase_if(Aliens, [](const Alien& a) {
		return !a.active;
		});

	std::erase_if(Walls, [](const Wall& w) {
		return !w.active;
		});
}

void Game::CheckGameOver()
{
	if (player.lives < 1) {
		End();
		return;
	}

	if (std::any_of(Aliens.begin(), Aliens.end(),
		[this](const Alien& alien) {
			return alien.position.y > GetScreenHeight() - player.player_base_height;
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
				name += static_cast<char>(key);
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

	if (name.size() > 0 && name.size() <= 9 && IsKeyReleased(KEY_ENTER))
	{
		InsertNewHighScore(name);
		newHighScore = false;
		name.clear();
	}
}

// TODO: Maybe break into RenderStartScreen(), RenderGameplay(), etc.
void Game::Render() const noexcept
{
	switch (gameState)
	{
	case State::STARTSCREEN:
		//Code
		DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);

		DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);


		break;
	case State::GAMEPLAY:
		//Code


		//background render LEAVE THIS AT TOP
		background.Render();

		//DrawText("GAMEPLAY", 50, 30, 40, YELLOW);
		DrawText(TextFormat("Score: %i", score), 50, 20, 40, YELLOW);
		DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

		//player rendering 
		player.Render(resources.shipTextures[player.activeTexture]);

		//projectile rendering
		for (auto& projectile : Projectiles) {
			projectile.Render(resources.laserTexture);
		}

		// wall rendering 
		for (auto& wall : Walls) {
			wall.Render(resources.barrierTexture);
		}

		//alien rendering  
		for (auto& alien : Aliens) {
			alien.Render(resources.alienTexture);
		}






		break;
	case State::ENDSCREEN:
		//Code
		//DrawText("END", 50, 50, 40, YELLOW);


		

		


		if (newHighScore)
		{
			DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);



			// BELOW CODE IS FOR NAME INPUT RENDER
			DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

			DrawRectangleRec(textBox, LIGHTGRAY);
			if (mouseOnText)
			{
				// HOVER CONFIRMIATION
				DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
			}
			else
			{
				DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
			}

			//Draw the name being typed out
			DrawText(name.c_str(), (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

			//Draw the text explaining how many characters are used
			DrawText(TextFormat("INPUT CHARS: %i/%i", (int)name.size(), 9), 600, 600, 20, YELLOW);

			if (mouseOnText)
			{
				if (name.size() < 9)
				{
					// Draw blinking underscore char
					if (((framesCounter / 20) % 2) == 0)
					{
						DrawText("_", (int)textBox.x + 8 + MeasureText(name.c_str(), 40), (int)textBox.y + 12, 40, MAROON);
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

			for (int i = 0; i < Leaderboard.size(); i++)
			{
				DrawText(Leaderboard[i].name.c_str(), 50, 140 + (i * 40), 40, YELLOW);
				DrawText(TextFormat("%i", Leaderboard[i].score), 350, 140 + (i * 40), 40, YELLOW);
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
            Aliens.emplace_back();
            auto& alien = Aliens.back();
            alien.position.x = FORMATION_START_X + FORMATION_HORIZONTAL_OFFSET + (col * ALIEN_SPACING);
            alien.position.y = FORMATION_START_Y + (row * ALIEN_SPACING);
        }
    }
}

bool Game::CheckNewHighScore()
{
	return score > Leaderboard[4].score;
}

void Game::InsertNewHighScore(const std::string& name)
{
	PlayerData newData;
	newData.name = name;
	newData.score = score;

	auto it = std::find_if(Leaderboard.begin(), Leaderboard.end(),
		[&newData](const PlayerData& entry) {
			return newData.score > entry.score;
		});

	if (it != Leaderboard.end()) {
		Leaderboard.insert(it, newData);
		Leaderboard.pop_back();
	}
}

// TODO: Optimize CheckCollision (const, abs diff) and add unit tests

bool Game::CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd)  const noexcept
{
	// our objective is to calculate the distance between the closest point on the line to the centre of the circle, 
	// and determine if it is shorter than the radius.

	// check if either edge of line is within circle
	if (pointInCircle(circlePos, circleRadius, lineStart) || pointInCircle(circlePos, circleRadius, lineEnd))
	{
		return true;
	}

	// simplify variables
	Vector2 A = lineStart;
	Vector2 B = lineEnd;
	Vector2 C = circlePos;

	// calculate the length of the line
	float length = lineLength(A, B);
	
	// calculate the dot product
	float dotP = (((C.x - A.x) * (B.x - A.x)) + ((C.y - A.y) * (B.y - A.y))) / pow(length, 2);

	// use dot product to find closest point
	float closestX = A.x + (dotP * (B.x - A.x));
	float closestY = A.y + (dotP * (B.y - A.y));

	//find out if coordinates are on the line.
	// we do this by comparing the distance of the dot to the edges, with two vectors
	// if the distance of the vectors combined is the same as the length the point is on the line

	//since we are using floating points, we will allow the distance to be slightly innaccurate to create a smoother collision
	float buffer = 0.1;

	float closeToStart = lineLength(A, { closestX, closestY }); //closestX + Y compared to line Start
	float closeToEnd = lineLength(B, { closestX, closestY });	//closestX + Y compared to line End

	float closestLength = closeToStart + closeToEnd;

	if (closestLength == length + buffer || closestLength == length - buffer)
	{
		//Point is on the line!

		//Compare length between closest point and circle centre with circle radius

		float closeToCentre = lineLength(A, { closestX, closestY }); //closestX + Y compared to circle centre

		return closeToCentre < circleRadius;
	}
	else
	{
		// Point is not on the line, line is not colliding
		return false;
	}

}

// TODO: Implement frame-independent movement using deltaTime
// TODO: Use IsActive(), TakeDamage() instead of direct access
Player::Player(int screenWidth)
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
	std::cout << "Player created at X:" << x_pos
		<< " Y:" << GetScreenHeight() - player_base_height
		<< std::endl;
}

void Player::Update() noexcept
{

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

void Player::Render(Texture2D texture) const noexcept
{
	float window_height = GetScreenHeight(); 

	DrawTexturePro(texture,
		{
			0,
			0,
			352,
			352,
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
	lineEnd.y   = position.y + 15;

	lineStart.x = position.x;
	lineEnd.x   = position.x;

	if (position.y < 0 || position.y > 1500)
	{
		active = false;
	}
}

void Projectile::Render(Texture2D texture) const noexcept
{
	//DrawCircle((int)position.x, (int)position.y, 10, RED);
	DrawTexturePro(texture,
		{
			0,
			0,
			176,
			176,
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

Wall::Wall(Vector2 pos, int rad)
	: position(pos)
	, radius(rad)
	, active(true)
	, health(INITIAL_HEALTH)
{
}

void Wall::Render(Texture2D texture) const noexcept
{
	DrawTexturePro(texture,
		{
			0,
			0,
			704,
			704,
		},
		{
			position.x,
			position.y,
			200,
			200,
		}, { 100 , 100 },
		0,
		WHITE);


	DrawText(TextFormat("%i", health), position.x-21, position.y+10, 40, RED);
	
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
	int window_width = GetScreenWidth(); 

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

void Alien::Render(Texture2D texture) const noexcept
{
	//DrawRectangle((int)position.x - 25, (int)position.y, 30, 30, RED);
	//DrawCircle((int)position.x, (int)position.y, radius, GREEN);
	
	

	DrawTexturePro(texture,
		{
			0,
			0,
			352,
			352,
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

// TODO: Fix inconsistent color comments

//BACKGROUND
void Star::Update(float starOffset) noexcept
{
	position.x = initPosition.x + starOffset;
	position.y = initPosition.y;

}

void Star::Render() const noexcept
{
	DrawCircle((int)position.x, (int)position.y, size, color);
}

Background::Background(int starAmount) {
	for (int i = 0; i < starAmount; i++) {
		Star newStar;
		newStar.initPosition.x = GetRandomValue(-150, GetScreenWidth() + 150);
		newStar.initPosition.y = GetRandomValue(0, GetScreenHeight());

		//random color?
		newStar.color = SKYBLUE;
		newStar.size = GetRandomValue(1, 4) / 2;
		Stars.push_back(newStar);
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
	for (auto& star : Stars) {
		star.Render();
	}
}


