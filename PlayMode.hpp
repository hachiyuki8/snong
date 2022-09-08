#include "PPU466.hpp"
#include "Mode.hpp"
#include "load_save_png.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>
#include <list>

enum Direction : uint16_t {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

// tile and palette indices of assets
struct Asset {
	uint8_t tile_index = 0;
	uint8_t palette_index = 0;
};

struct SnakeBody {
	glm::vec2 position = glm::vec2(0.0f);
	Asset tile;
};

struct Snake {
	std::list<SnakeBody> bodies = {};
	uint32_t max_len = 100;
	uint32_t min_len = 20;
	Direction direction;
	Asset head_animation [4];
	uint8_t head_animation_idx = 0;
};

struct Ball {
	glm::vec2 position = glm::vec2(0.0f);
	Asset tile;
	float x_velocity = 0.0f;
	float y_velocity = 0.0f;
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	virtual std::pair<int, int> load_asset(std::string data_path, bool with_palette);
	virtual void initialize_game_state();
	Asset background, bar_top, bar_bottom, snake_head, snake_head_1, snake_head_2, snake_head_3, snake_head_4, snake_body;
	Asset up_arrow, down_arrow, left_arrow, right_arrow;
	Asset A, B, C, D, E, G, I, L, M, N, O, P, R, S, T, V, W, Y;

	//----- game state -----
	enum GameState : uint16_t {
		PAUSE,
		IN_PROGRESS,
		PLAYER_1_WON,
		PLAYER_2_WON,
		DRAW
	};
	GameState game_state = PAUSE;

	// snakes
	Snake snake_1, snake_2;
	float snake_tick = 0.0f; // for fixed time update of the snakes
	float speed_tick = 0.0f; // for increasing moving speed of snakes and balls
	float snake_speed_unit = 0.2f;
	virtual bool update_snake(Snake *snake, uint32_t y_bound_up, uint32_t y_bound_down);
	virtual bool check_snake_collision(float x, float y, Snake *snake);
	virtual void add_tail_to_snake(Snake *snake);

	// ball
	Ball ball;
	int ball_speed_base = 50;
	int ball_speed_range = 50;
	virtual bool check_ball_collision(Snake *snake);
	virtual void update_ball(float elapsed);
	virtual void reset_ball();
	virtual float randomize_ball_speed(float v);

	virtual void type_text(uint16_t y, uint16_t x, std::string text);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
