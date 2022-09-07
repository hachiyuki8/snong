#include "PPU466.hpp"
#include "Mode.hpp"
#include "load_save_png.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>
#include <list>

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

// tile and palette indices of assets
struct Asset {
	uint32_t tile_index = -1;
	uint32_t palette_index = -1;
};

struct SnakeBody {
	glm::vec2 position = glm::vec2(0.0f);
	Asset tile;
};

struct Snake {
	std::list<SnakeBody> bodies = {};
	uint32_t max_len = 20;
	uint32_t min_len = 7;
	Direction direction;
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

	virtual std::pair<uint32_t, uint32_t> load_asset(std::string data_path, bool with_palette);
	virtual void initialize_game_state();
	Asset background, bar, snake_head, snake_body;

	//----- game state -----
	enum GameState {
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
	float snake_speed_tick = 0.0f; // for increasing moving speed of snakes and balls
	float snake_speed_unit = 0.5f;
	virtual bool update_snake(Snake *snake, uint32_t y_bound_up, uint32_t y_bound_down);
	virtual void increase_snake(Snake *snake);
	virtual void decrease_snake(Snake *snake);

	// ball
	Ball ball;
	int ball_speed_base = 50;
	int ball_speed_range = 50;
	virtual bool check_ball_collision(Snake *snake);
	virtual int update_ball(float elapsed);
	virtual void reset_ball();

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
