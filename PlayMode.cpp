#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/io.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "data_path.hpp"

const uint32_t TILE_SIZE = 8;
const uint32_t COLOR_LIMIT = 4;
const uint32_t PALETTE_LIMIT = 8;
const uint32_t TILE_LIMIT = 256;

const std::string SNAKE_HEAD_DATA = data_path("snake_head.dat");
const std::string SNAKE_HEAD_1_DATA = data_path("snake_head_1.dat");
const std::string SNAKE_HEAD_2_DATA = data_path("snake_head_2.dat");
const std::string SNAKE_HEAD_3_DATA = data_path("snake_head_3.dat");
const std::string SNAKE_HEAD_4_DATA = data_path("snake_head_4.dat");
const std::string SNAKE_BODY_DATA = data_path("snake_head_5.dat");
const std::string BALL_DATA = data_path("ball.dat");
const std::string BACKGROUND_DATA = data_path("background.dat");
const std::string BAR_TOP_DATA = data_path("bar_top.dat");
const std::string BAR_BOTTOM_DATA = data_path("bar_bottom.dat");
const std::string UP_ARROW_DATA = data_path("up_arrow.dat");
const std::string DOWN_ARROW_DATA = data_path("down_arrow.dat");
const std::string LEFT_ARROW_DATA = data_path("left_arrow.dat");
const std::string RIGHT_ARROW_DATA = data_path("right_arrow.dat");
const std::string A_DATA = data_path("A.dat");
const std::string B_DATA = data_path("B.dat");
const std::string C_DATA = data_path("C.dat");
const std::string D_DATA = data_path("D.dat");
const std::string E_DATA = data_path("E.dat");
const std::string G_DATA = data_path("G.dat");
const std::string I_DATA = data_path("I.dat");
const std::string L_DATA = data_path("L.dat");
const std::string M_DATA = data_path("M.dat");
const std::string N_DATA = data_path("N.dat");
const std::string O_DATA = data_path("O.dat");
const std::string P_DATA = data_path("P.dat");
const std::string R_DATA = data_path("R.dat");
const std::string S_DATA = data_path("S.dat");
const std::string T_DATA = data_path("T.dat");
const std::string V_DATA = data_path("V.dat");
const std::string W_DATA = data_path("W.dat");
const std::string Y_DATA = data_path("Y.dat");

const float DIFFICULTY_INTERVAL = 5.0f;

const bool DEBUG = false;

void PlayMode::type_text(uint16_t y, uint16_t x, std::string text) {
	auto get_bg = [] (Asset a) { return a.palette_index << 8 | a.tile_index; };

	for (auto &letter : text) {
		if (x+PPU466::BackgroundWidth*y >= PPU466::BackgroundHeight * PPU466::BackgroundWidth) {
			return;
		}
		switch (letter) {
			case 'A':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(A);
				break;
			case 'B':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(B);
				break;
			case 'C':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(C);
				break;
			case 'D':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(D);
				break;
			case 'E':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(E);
				break;
			case 'G':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(G);
				break;
			case 'I':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(I);
				break;
			case 'L':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(L);
				break;
			case 'M':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(M);
				break;
			case 'N':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(N);
				break;
			case 'O':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(O);
				break;
			case 'P':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(P);
				break;
			case 'R':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(R);
				break;
			case 'S':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(S);
				break;
			case 'T':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(T);
				break;
			case 'V':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(V);
				break;
			case 'W':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(W);
				break;
			case 'Y':
				ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(Y);
				break;
			default:
				break;
		}
		x += 1;
	}
}

PlayMode::PlayMode() {
	{ // load assets
		std::pair<int, int> indices;

		indices = load_asset(SNAKE_HEAD_1_DATA, true);
		snake_head_1.tile_index = (uint8_t) indices.first;
		snake_head_1.palette_index = (uint8_t) indices.second;

		indices = load_asset(SNAKE_HEAD_2_DATA, false);
		snake_head_2.tile_index = (uint8_t) indices.first;
		snake_head_2.palette_index = snake_head_1.palette_index;

		indices = load_asset(SNAKE_HEAD_3_DATA, false);
		snake_head_3.tile_index = (uint8_t) indices.first;
		snake_head_3.palette_index = snake_head_1.palette_index;

		indices = load_asset(SNAKE_HEAD_4_DATA, true);
		snake_head_4.tile_index = (uint8_t) indices.first;
		snake_head_4.palette_index = (uint8_t) indices.second;

		indices = load_asset(SNAKE_BODY_DATA, false);
		snake_body.tile_index = (uint8_t) indices.first;
		snake_body.palette_index = snake_head_4.palette_index;

		indices = load_asset(BALL_DATA, true);
		ball.tile.tile_index = (uint8_t) indices.first;
		ball.tile.palette_index = (uint8_t) indices.second;

		indices = load_asset(BACKGROUND_DATA, true);
		background.tile_index = (uint8_t) indices.first;
		background.palette_index = (uint8_t) indices.second;

		indices = load_asset(BAR_TOP_DATA, true);
		bar_top.tile_index = (uint8_t) indices.first;
		bar_top.palette_index = (uint8_t) indices.second;

		indices = load_asset(BAR_BOTTOM_DATA, true);
		bar_bottom.tile_index = (uint8_t) indices.first;
		bar_bottom.palette_index = (uint8_t) indices.second;

		indices = load_asset(A_DATA, true);
		A.tile_index = (uint8_t) indices.first;
		A.palette_index = (uint8_t) indices.second;

		indices = load_asset(B_DATA, false);
		B.tile_index = (uint8_t) indices.first;
		B.palette_index = A.palette_index;

		indices = load_asset(C_DATA, false);
		C.tile_index = (uint8_t) indices.first;
		C.palette_index = A.palette_index;

		indices = load_asset(D_DATA, false);
		D.tile_index = (uint8_t) indices.first;
		D.palette_index = A.palette_index;

		indices = load_asset(E_DATA, false);
		E.tile_index = (uint8_t) indices.first;
		E.palette_index = A.palette_index;

		indices = load_asset(G_DATA, false);
		G.tile_index = (uint8_t) indices.first;
		G.palette_index = A.palette_index;

		indices = load_asset(I_DATA, false);
		I.tile_index = (uint8_t) indices.first;
		I.palette_index = A.palette_index;

		indices = load_asset(L_DATA, false);
		L.tile_index = (uint8_t) indices.first;
		L.palette_index = A.palette_index;

		indices = load_asset(M_DATA, false);
		M.tile_index = (uint8_t) indices.first;
		M.palette_index = A.palette_index;

		indices = load_asset(N_DATA, false);
		N.tile_index = (uint8_t) indices.first;
		N.palette_index = A.palette_index;

		indices = load_asset(O_DATA, false);
		O.tile_index = (uint8_t) indices.first;
		O.palette_index = A.palette_index;

		indices = load_asset(P_DATA, false);
		P.tile_index = (uint8_t) indices.first;
		P.palette_index = A.palette_index;

		indices = load_asset(R_DATA, false);
		R.tile_index = (uint8_t) indices.first;
		R.palette_index = A.palette_index;

		indices = load_asset(S_DATA, false);
		S.tile_index = (uint8_t) indices.first;
		S.palette_index = A.palette_index;

		indices = load_asset(T_DATA, false);
		T.tile_index = (uint8_t) indices.first;
		T.palette_index = A.palette_index;

		indices = load_asset(V_DATA, false);
		V.tile_index = (uint8_t) indices.first;
		V.palette_index = A.palette_index;

		indices = load_asset(W_DATA, false);
		W.tile_index = (uint8_t) indices.first;
		W.palette_index = A.palette_index;

		indices = load_asset(Y_DATA, false);
		Y.tile_index = (uint8_t) indices.first;
		Y.palette_index = A.palette_index;

		indices = load_asset(UP_ARROW_DATA, false);
		up_arrow.tile_index = (uint8_t) indices.first;
		up_arrow.palette_index = A.palette_index;

		indices = load_asset(DOWN_ARROW_DATA, false);
		down_arrow.tile_index = (uint8_t) indices.first;
		down_arrow.palette_index = A.palette_index;

		indices = load_asset(LEFT_ARROW_DATA, false);
		left_arrow.tile_index = (uint8_t) indices.first;
		left_arrow.palette_index = A.palette_index;

		indices = load_asset(RIGHT_ARROW_DATA, false);
		right_arrow.tile_index = (uint8_t) indices.first;
		right_arrow.palette_index = A.palette_index;
	}

	{ // make background
		auto get_bg = [] (Asset a) { return a.palette_index << 8 | a.tile_index; };
		for (uint16_t y = 0; y < PPU466::BackgroundHeight; y++) {
			for (uint16_t x = 0; x < PPU466::BackgroundWidth; x++) {
				if (y == PPU466::BackgroundHeight / 4) {
					ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(bar_top);
				} else if (y == PPU466::BackgroundHeight / 4 - 1) {
					ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(bar_bottom);
				} else {
					ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(background);
				}
			}
		}
		// player instructions
		uint16_t y = PPU466::BackgroundHeight / 4;
		uint16_t x = PPU466::BackgroundWidth / 4;
		type_text(y+2, x-2, "SNONG");
		type_text(y+1, x-10, "BY A SMALL INDIE DEV");
		type_text(y-2, x-10, "PRESS SPACE TO START");

		y = PPU466::BackgroundHeight / 5 * 2;
		ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(W);
		ppu.background[x-1+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(A);
		ppu.background[x+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(S);
		ppu.background[x+1+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(D);

		y = PPU466::BackgroundHeight / 8;
		ppu.background[x+PPU466::BackgroundWidth*y] = (uint16_t) get_bg(up_arrow);
		ppu.background[x-1+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(left_arrow);
		ppu.background[x+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(down_arrow);
		ppu.background[x+1+PPU466::BackgroundWidth*(y-1)] = (uint16_t) get_bg(right_arrow);
	}

	initialize_game_state();
}

PlayMode::~PlayMode() {
}


std::pair<int, int> PlayMode::load_asset(std::string data_path, bool with_palette) {
	static int tile_index = 0;
	static int palette_index = 0;

	// https://stackoverflow.com/questions/43765881/c-reading-binary-files
	std::ifstream file(data_path, std::ios::ate | std::ios::binary);
    if(!file.good()) {
		throw std::runtime_error("Failed to load asset " + data_path);
    }
    auto size = file.tellg();
	assert(size == 32 && "each asset file contains 32 bytes");
    std::vector<uint8_t> data(size);
    file.seekg(0);
    file.read((char *)data.data(), size);

	assert((uint32_t) tile_index < TILE_LIMIT && "PPU stores at most 256 tiles");
	for (uint16_t i = 0; i < TILE_SIZE; i++) {
		ppu.tile_table[tile_index].bit0[i] = data[i];
		ppu.tile_table[tile_index].bit1[i] = data[TILE_SIZE+i];
	}
	tile_index += 1;

	if (with_palette) {
		assert((uint32_t) palette_index < PALETTE_LIMIT && "PPU stores at most 8 palettes");
		uint16_t offset = TILE_SIZE * 2;
		for (uint16_t i = 0; i < COLOR_LIMIT; i++) {
			ppu.palette_table[palette_index][i] = glm::u8vec4(
				(uint8_t)data[offset + i * 4],
				(uint8_t)data[offset + i * 4 + 1],
				(uint8_t)data[offset + i * 4 + 2],
				(uint8_t)data[offset + i * 4 + 3]
			);
		}
		palette_index += 1;
	}

	return std::make_pair(tile_index-1, palette_index-1);
}

// initial the ball and the snakes
void PlayMode::initialize_game_state() {
	reset_ball();

	{ // player 1
		snake_1.bodies.clear();
		snake_1.head_animation[0] = snake_head_1;
		snake_1.head_animation[1] = snake_head_2;
		snake_1.head_animation[2] = snake_head_3;
		snake_1.head_animation[3] = snake_head_4;
		glm::vec2 pos = glm::vec2(
			PPU466::ScreenWidth / 2 - TILE_SIZE * (int(snake_2.min_len/2)+1), 
			PPU466::ScreenHeight - TILE_SIZE);
		SnakeBody head;
		head.position = pos;
		head.tile = snake_1.head_animation[snake_1.head_animation_idx];
		snake_1.bodies.push_back(head);
		for (uint32_t i = 0; i < snake_1.min_len; i++) {
			pos.x += TILE_SIZE;
			SnakeBody body;
			body.position = pos;
			body.tile = snake_body;
			snake_1.bodies.push_back(body);
		}
		snake_1.direction = LEFT;
	}

	{ // player 2
		snake_2.bodies.clear();
		snake_2.head_animation[0] = snake_head_1;
		snake_2.head_animation[1] = snake_head_2;
		snake_2.head_animation[2] = snake_head_3;
		snake_2.head_animation[3] = snake_head_4;
		glm::vec2 pos = glm::vec2(
			PPU466::ScreenWidth / 2 + TILE_SIZE * (int(snake_2.min_len/2)), 
			0);
		SnakeBody head;
		head.position = pos;
		head.tile = snake_2.head_animation[snake_2.head_animation_idx];
		snake_2.bodies.push_back(head);
		for (uint32_t i = 0; i < snake_2.min_len; i++) {
			pos.x -= TILE_SIZE;
			SnakeBody body;
			body.position = pos;
			body.tile = snake_body;
			snake_2.bodies.push_back(body);
		}
		snake_2.direction = RIGHT;
	}
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
			if (game_state != IN_PROGRESS && game_state != PAUSE) {
				initialize_game_state();
			}
			game_state = IN_PROGRESS;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			if (snake_1.direction != RIGHT) {
				snake_1.direction = LEFT;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			if (snake_1.direction != LEFT) {
				snake_1.direction = RIGHT;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			if (snake_1.direction != DOWN) {
				snake_1.direction = UP;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			if (snake_1.direction != UP) {
				snake_1.direction = DOWN;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			if (snake_2.direction != RIGHT) {
				snake_2.direction = LEFT;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			if (snake_2.direction != LEFT) {
				snake_2.direction = RIGHT;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			if (snake_2.direction != DOWN) {
				snake_2.direction = UP;
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			if (snake_2.direction != UP) {
				snake_2.direction = DOWN;
			}
			return true;
		}
	} 

	return false;
}

// check if the given position collides with any chunk of the snake body
bool PlayMode::check_snake_collision(float x, float y, Snake *snake) {
	for (auto &b : snake->bodies) {
		if (b.position.x == x && b.position.y == y) {
			return true;
		}
	}
	return false;
}

// add a tail to the snake
void PlayMode::add_tail_to_snake(Snake *snake) {
	if (DEBUG) {
		int i = (snake == &snake_1) ? 1 : 2;
		std::cout << "[logger] Adding tail to snake " << i << std::endl;
	}
	if (snake->bodies.size() >= snake->max_len) {
		return;
	}

	float x = snake->bodies.back().position.x;
	float y = snake->bodies.back().position.y;
	SnakeBody body;
	body.tile = snake_body;

	// pick a direction to add the tail without colliding
	if (x - TILE_SIZE >= 0 && !check_snake_collision(x - TILE_SIZE, y, snake)) {
		body.position = glm::vec2(x - TILE_SIZE, y);
		snake->bodies.push_back(body);
	} else if (x < PPU466::ScreenWidth - TILE_SIZE && !check_snake_collision(x + TILE_SIZE, y, snake)) {
		body.position = glm::vec2(x + TILE_SIZE, y);
		snake->bodies.push_back(body);
	} else if (y - TILE_SIZE >= 0 && !check_snake_collision(x, y - TILE_SIZE, snake)) {
		body.position = glm::vec2(x, y - TILE_SIZE);
		snake->bodies.push_back(body);
	} else if (y < PPU466::ScreenHeight - TILE_SIZE && !check_snake_collision(x, y + TILE_SIZE, snake)) {
		body.position = glm::vec2(x, y + TILE_SIZE);
		snake->bodies.push_back(body);
	} else if (DEBUG) {
		std::cout << "[logger] Failed to add tail to snake" << std::endl;
	}
}

// fixed time update: move the snake by one chunk/tile
bool PlayMode::update_snake(Snake *snake, uint32_t y_bound_up, uint32_t y_bound_down) {
	snake->head_animation_idx = (snake->head_animation_idx + 1) % 4;

	SnakeBody head;
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
		default:
			break;
	}
	head.tile = snake->head_animation[snake->head_animation_idx];
	head.position = pos;

	// check if the new head is out of boundary or colliding with the snake
	if (head.position.x < 0 || head.position.x > PPU466::ScreenWidth - TILE_SIZE ||
		head.position.y > y_bound_up || head.position.y < y_bound_down ||
		check_snake_collision(head.position.x, head.position.y, snake)) {
		return false;
	} 

	snake->bodies.push_front(head);
	return true;
}

// check if the ball collides with any chunk of the snake body
// https://lazyfoo.net/tutorials/SDL/27_collision_detection/index.php
bool PlayMode::check_ball_collision(Snake *snake) {
	for (auto &body: snake->bodies) {
		if ((ball.position.x > body.position.x + TILE_SIZE) ||
			(ball.position.x + TILE_SIZE < body.position.x) ||
			(ball.position.y > body.position.y + TILE_SIZE) ||
			(ball.position.y + TILE_SIZE < body.position.y)) {
			continue;
		} else {
			return true;
		}
	}
	return false;
}

// randomize the speed but keep direction (sign) the same
float PlayMode::randomize_ball_speed(float v) {
	float new_velocity = float(rand() % ball_speed_base + ball_speed_range);
	return (v >= 0.0f) ? new_velocity : -new_velocity;
}

void PlayMode::update_ball(float elapsed) {
	ball.position.x += ball.x_velocity * elapsed;
	{
		bool flag_1 = check_ball_collision(&snake_1);
		bool flag_2 = check_ball_collision(&snake_2);
		if (flag_1 || flag_2 || ball.position.x > PPU466::ScreenWidth - TILE_SIZE || ball.position.x < 0) {
			ball.position.x -= ball.x_velocity * elapsed;
			ball.x_velocity *= -1.0f;
			ball.x_velocity = randomize_ball_speed(ball.x_velocity);
		}
		if (flag_1) {
			add_tail_to_snake(&snake_2);
		}
		if (flag_2) {
			add_tail_to_snake(&snake_1);
		}
	}
	
	ball.position.y += ball.y_velocity * elapsed;
	{
		bool flag_1 = check_ball_collision(&snake_1);
		bool flag_2 = check_ball_collision(&snake_2);
		if (flag_1 || flag_2 || ball.position.y > PPU466::ScreenHeight - TILE_SIZE || ball.position.y < 0) {
			ball.position.y -= ball.y_velocity * elapsed * 2;
			ball.y_velocity *= -1.0f;
			ball.y_velocity = randomize_ball_speed(ball.y_velocity);
		}
		if (flag_1) {
			add_tail_to_snake(&snake_2);
		}
		if (flag_2) {
			add_tail_to_snake(&snake_1);
		}
	}
}

// reset ball to screen centre, and generate a new random speed in range
void PlayMode::reset_ball() {
	if (DEBUG) {
		std::cout << "[logger] Resetting ball position and speed" << std::endl;
	}
	ball.position.x = (PPU466::ScreenWidth - TILE_SIZE) / 2;
	ball.position.y = (PPU466::ScreenHeight - TILE_SIZE) / 2;
	ball.x_velocity = float(rand() % ball_speed_base + ball_speed_range);
	ball.y_velocity = float(rand() % ball_speed_base + ball_speed_range);
}

void PlayMode::update(float elapsed) {
	if (game_state == IN_PROGRESS) {
		// increase moving speed of snake and ball every DIFFICULTY_INTERVAL
		// https://15466.courses.cs.cmu.edu/lesson/timing
		speed_tick += elapsed;
		while (speed_tick > DIFFICULTY_INTERVAL) {
			if (DEBUG) {
				std::cout << "[logger] Increasing moving speed" << std::endl;
			}
			speed_tick -= DIFFICULTY_INTERVAL;
			if (snake_speed_unit > 0.01f) {
				snake_speed_unit -= 0.01f;
			}
			ball_speed_base += 10;
			ball_speed_range += 10;
		}

		// variable time update
		update_ball(elapsed);

		// fixed time update
		snake_tick += elapsed;
		while (snake_tick > snake_speed_unit) {
			snake_tick -= snake_speed_unit;
			bool one = update_snake(&snake_1, 
				PPU466::ScreenHeight - TILE_SIZE, PPU466::ScreenHeight / 2 + TILE_SIZE);
			bool two = update_snake(&snake_2,
				PPU466::ScreenHeight / 2 - TILE_SIZE * 2, 0);

			// check game termination
			if (!one && !two) {
				game_state = DRAW;
			} else if (!one) {
				game_state = PLAYER_2_WON;
			} else if (!two) {
				game_state = PLAYER_1_WON;
			}
		}

		// if the snake moves into the ball, reset the ball
		if (check_ball_collision(&snake_1)) {
			reset_ball();
		} else if (check_ball_collision(&snake_2)) {
			reset_ball();
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
