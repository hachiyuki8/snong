#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

// for glm::to_stirng();
#include <glm/gtx/string_cast.hpp>

#include <glm/gtx/io.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

const uint32_t TILE_SIZE = 8;
const uint32_t COLOR_LIMIT = 4;
const uint32_t PALETTE_LIMIT = 8;
const uint32_t TILE_LIMIT = 256;

const std::string DATA_PATH = "/Users/hachiyuki/Desktop/15466/game1/dist/"; // TODO: don't hardcode
const std::string SNAKE_HEAD_DATA = DATA_PATH + "snake_head.dat";
const std::string SNAKE_BODY_DATA = DATA_PATH + "snake_body.dat";
const std::string BALL_DATA = DATA_PATH + "ball.dat";
const std::string BACKGROUND_DATA = DATA_PATH + "background.dat";
const std::string BAR_DATA = DATA_PATH + "bar.dat";

const bool DEBUG = true;

std::pair<uint32_t, uint32_t> PlayMode::load_asset(std::string data_path, bool with_palette) {
	static uint32_t tile_index = 0;
	static uint32_t palette_index = 0;

	// https://stackoverflow.com/questions/43765881/c-reading-binary-files
	std::ifstream file(data_path, std::ios::ate | std::ios::binary);
    if(!file.good()) {
		throw std::runtime_error("Failed to load asset " + data_path);
    }
    auto size = file.tellg();
	if (with_palette) {
		assert(size == 32 && "each asset file with palette contains 32 bytes");
	} else {
		assert(size == 16 && "each asset file without palette contains 32 bytes");
	}
    std::vector<uint8_t> data(size);
    file.seekg(0);
    file.read((char *)data.data(), size);

	PPU466::Tile tile;
	for (uint32_t i = 0; i < TILE_SIZE; i++) {
		tile.bit0[i] = data[i];
		tile.bit1[i] = data[TILE_SIZE+i];
	}
	assert(tile_index < TILE_LIMIT && "PPU stores at most 256 tiles");
	ppu.tile_table[tile_index] = tile;
	tile_index += 1;

	PPU466::Palette palette;
	if (with_palette) {
		uint32_t offset = TILE_SIZE * 2;
		for (uint32_t i = 0; i < COLOR_LIMIT; i++) {
			palette[i] = glm::u8vec4(
				(uint8_t)data[offset + i * 4],
				(uint8_t)data[offset + i * 4 + 1],
				(uint8_t)data[offset + i * 4 + 2],
				(uint8_t)data[offset + i * 4 + 3]
			);
		}
		assert(tile_index < PALETTE_LIMIT && "PPU stores at most 8 palettes");
		ppu.palette_table[palette_index] = palette;
		palette_index += 1;
	}

	if (DEBUG) {
		std::cout << "Loaded " << data_path << std::endl;
		std::cout << "- Tile " << +(tile_index - 1) << ": " << std::endl;
		for (uint32_t i = 0; i < TILE_SIZE; i++) {
			std::cout << "  ";
			for (uint32_t j = 0; j < TILE_SIZE; j++) {
				auto bit0 = (tile.bit0[i] >> j) & 1;
				auto bit1 = (tile.bit1[i] >> j) & 1;
				auto color_index = (bit1 << 1) | bit0;
				std::cout << +color_index;
			}
			std::cout << std::endl;
		}
		if (with_palette) {
			std::cout << "- Palette " << +(palette_index - 1) << ": " << std::endl;
			for (uint32_t i = 0; i < COLOR_LIMIT; i++) {
				std::cout << "  " << glm::to_string(palette[i]) << std::endl;
			}
		}
	}

	return std::make_pair(tile_index-1, palette_index-1);
}

void PlayMode::initialize_game_state() {
	{ // ball
		ball.position.x = (PPU466::ScreenWidth - TILE_SIZE) / 2;
		ball.position.y = (PPU466::ScreenHeight - TILE_SIZE) / 2;
		ball.x_velocity = 30.0f; // TODO: randomize
		ball.y_velocity = 40.0f;
	}

	{ // player 1
		glm::vec2 pos = glm::vec2(
			PPU466::ScreenWidth / 2 - TILE_SIZE * (int(snake_2.min_len/2)+1), 
			PPU466::ScreenHeight - TILE_SIZE);
		SnakeBody head;
		head.position = pos;
		head.tile = snake_head;
		snake_1.bodies.push_back(head);
		for (uint32_t i = 0; i < snake_1.min_len; i++) {
			pos.x += TILE_SIZE;
			SnakeBody body;
			body.position = pos;
			body.tile = snake_body;
			snake_1.bodies.push_back(body);
		}
		snake_1.x_velocity = -8.0f;
		snake_1.direction = LEFT;
	}

	{ // player 2
		glm::vec2 pos = glm::vec2(
			PPU466::ScreenWidth / 2 + TILE_SIZE * (int(snake_2.min_len/2)), 
			0);
		SnakeBody head;
		head.position = pos;
		head.tile = snake_head;
		snake_2.bodies.push_back(head);
		for (uint32_t i = 0; i < snake_1.min_len; i++) {
			pos.x -= TILE_SIZE;
			SnakeBody body;
			body.position = pos;
			body.tile = snake_body;
			snake_2.bodies.push_back(body);
		}
		snake_2.x_velocity = 8.0f;
		snake_2.direction = RIGHT;
	}
}

PlayMode::PlayMode() {
	// load assets
	{
		std::pair<uint32_t, uint32_t> indices;

		indices = load_asset(SNAKE_HEAD_DATA, true);
		snake_head.tile_index = indices.first;
		snake_head.palette_index = indices.second;

		indices = load_asset(SNAKE_BODY_DATA, true);
		snake_body.tile_index = indices.first;
		snake_body.palette_index = indices.second;

		indices = load_asset(BALL_DATA, true);
		ball.tile.tile_index = indices.first;
		ball.tile.palette_index = indices.second;

		indices = load_asset(BACKGROUND_DATA, true);
		background.tile_index = indices.first;
		background.palette_index = indices.second;

		indices = load_asset(BAR_DATA, true);
		bar.tile_index = indices.first;
		bar.palette_index = indices.second;
	}

	// make background
	{
		uint32_t bg = background.palette_index << 8 | background.tile_index;
		uint32_t b = bar.palette_index << 8 | bar.tile_index;
		for (uint32_t y = 0; y < PPU466::BackgroundHeight; y++) {
			for (uint32_t x = 0; x < PPU466::BackgroundWidth; x++) {
				if (y == PPU466::BackgroundHeight / 4 || y == PPU466::BackgroundHeight / 4 - 1) {
					ppu.background[x+PPU466::BackgroundWidth*y] = b;
				} else {
					ppu.background[x+PPU466::BackgroundWidth*y] = bg;
				}
			}
		}
	}

	initialize_game_state();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
			game_state = IN_PROGRESS;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			snake_1.direction = LEFT;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			snake_1.direction = RIGHT;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			snake_1.direction = UP;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			snake_1.direction = DOWN;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			snake_2.direction = LEFT;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			snake_2.direction = RIGHT;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			snake_2.direction = UP;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			snake_2.direction = DOWN;
			return true;
		}
	} 

	return false;
}

// fixed time update: move the snake by one chunk/tile
bool PlayMode::update_snake(Snake *snake, uint32_t y_bound_up, uint32_t y_bound_down) {
	// remove tail and add a new block in front of the new head
	snake->bodies.pop_back();
	snake->bodies.front().tile = snake_body;
	glm::vec2 pos = snake->bodies.front().position;
	switch (snake->direction) {
		case UP:
			pos.y += TILE_SIZE;
			break;
		case DOWN:
			pos.y -= TILE_SIZE;
			break;
		case LEFT:
			pos.x -= TILE_SIZE;
			break;
		case RIGHT:
			pos.x += TILE_SIZE;
			break;
	}
	SnakeBody head;
	head.tile = snake_head;
	head.position = pos;
	snake->bodies.push_front(head);

	// check collision / out of boundary
	pos = snake->bodies.front().position;
	if (pos.x < 0 || pos.x > PPU466::ScreenWidth - TILE_SIZE ||
		pos.y > y_bound_up || pos.y < y_bound_down) {
		snake->bodies.pop_front();
		return false;
	} 

	return true;
}

void PlayMode::update(float elapsed) {
	if (game_state == IN_PROGRESS) {
		{ // snake (fixed time update)
			// https://15466.courses.cs.cmu.edu/lesson/timing
			snake_speed_tick += elapsed;
			while (snake_speed_tick > 10.0f) {
				snake_speed_tick -= 10.0f;
				if (snake_speed_unit > 0.01f) {
					snake_speed_unit -= 0.01f;
				}
			}

			snake_tick += elapsed;
			while (snake_tick > snake_speed_unit) {
				snake_tick -= snake_speed_unit;
				bool one = update_snake(&snake_1, 
					PPU466::ScreenHeight - TILE_SIZE, PPU466::ScreenHeight / 2 + TILE_SIZE);
				bool two = update_snake(&snake_2,
					PPU466::ScreenHeight / 2 - TILE_SIZE * 2, 0);
				if (!one && !two) {
					game_state = DRAW;
				} else if (!one) {
					game_state = PLAYER_2_WON;
				} else if (!two) {
					game_state = PLAYER_1_WON;
				}
			}
		}

		{ // ball
			ball.position.x += ball.x_velocity * elapsed;
			if (ball.position.x > PPU466::ScreenWidth - TILE_SIZE) {
				ball.position.x = PPU466::ScreenWidth - TILE_SIZE;
				ball.x_velocity *= -1.0f; // TODO: randomize
			}
			if (ball.position.x < 0) {
				ball.position.x = 0;
				ball.x_velocity *= -1.0f;
			}

			ball.position.y += ball.y_velocity * elapsed;
			if (ball.position.y > PPU466::ScreenHeight - TILE_SIZE) {
				ball.y_velocity = 0.0f;
				game_state = PLAYER_1_WON;
			}
			if (ball.position.y < 0) {
				ball.y_velocity = 0.0f;
				game_state = PLAYER_2_WON;
			}
		}

		if (DEBUG && game_state != IN_PROGRESS) {
			std::cout << "Game state updated to: " << game_state << std::endl;
		}
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	uint32_t idx = 0;
	// ball sprite
	{
		ppu.sprites[idx].x = int8_t(ball.position.x);
		ppu.sprites[idx].y = int8_t(ball.position.y);
		ppu.sprites[idx].index = ball.tile.tile_index;
		ppu.sprites[idx].attributes = ball.tile.palette_index;
	}

	// snakes sprite
	{
		for (auto &body : snake_1.bodies) {
			idx += 1;
			ppu.sprites[idx].x = int8_t(body.position.x);
			ppu.sprites[idx].y = int8_t(body.position.y);
			ppu.sprites[idx].index = body.tile.tile_index;
			ppu.sprites[idx].attributes = body.tile.palette_index;

		}
		for (auto &body : snake_2.bodies) {
			idx += 1;
			ppu.sprites[idx].x = int8_t(body.position.x);
			ppu.sprites[idx].y = int8_t(body.position.y);
			ppu.sprites[idx].index = body.tile.tile_index;
			ppu.sprites[idx].attributes = body.tile.palette_index;

		}
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
