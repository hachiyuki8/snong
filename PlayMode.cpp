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

PlayMode::PlayMode() {
	{
		std::pair<uint32_t, uint32_t> indices;

		indices = load_asset(SNAKE_HEAD_DATA, true);
		snake_head.tile_index = indices.first;
		snake_head.palette_index = indices.second;

		indices = load_asset(SNAKE_BODY_DATA, true);
		snake_body.tile_index = indices.first;
		snake_body.palette_index = indices.second;
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	// //background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	// //tilemap gets recomputed every frame as some weird plasma thing:
	// //NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		//TODO: make weird plasma thing
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
	// 	}
	// }

	// player sprite:
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = snake_head.tile_index;
	ppu.sprites[0].attributes = snake_head.palette_index;

	// //some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	//--- actually draw ---
	ppu.draw(drawable_size);
}
