#include "PPU466.hpp"
#include "Mode.hpp"
#include "load_save_png.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>
#include <list>

enum Direction : int32_t {
	UP = 1, // x = 0, y = 1
	DOWN = -1, // x = 0, y = -1
	LEFT = -2, // x = -1, y = 0
	RIGHT = 2 // x = 1, y = 0
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
	uint32_t min_len = 5;
	float x_velocity = 0.0f;
	float y_velocity = 0.0f;
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
	
	float snake_tick = 0.0f; // used for fixed time update of the snakes
	float snake_speed_tick = 0.0f; // used to increase snakes' moving speed
	float snake_speed_unit = 0.2f;
	virtual bool update_snake(Snake *snake, uint32_t y_bound_up, uint32_t y_bound_down);

	virtual std::pair<uint32_t, uint32_t> load_asset(std::string data_path, bool with_palette);
	virtual void initialize_game_state();
	Asset background, bar, snake_head, snake_body;
	Ball ball;
	Snake player_1;
	Snake player_2;

	//----- game state -----
	enum GameState {
		PAUSE,
		IN_PROGRESS,
		PLAYER_1_WON,
		PLAYER_2_WON,
		DRAW
	};
	GameState game_state = PAUSE;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	// snakes (paddles)
	Snake snake_1, snake_2;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
