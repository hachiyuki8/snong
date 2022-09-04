#include "PPU466.hpp"
#include "Mode.hpp"
#include "load_save_png.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

// tile and palette indices of assets
struct Asset {
	uint32_t tile_index = -1;
	uint32_t palette_index = -1;
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	virtual std::pair<uint32_t, uint32_t> load_asset(std::string data_path, bool with_palette);
	Asset snake_head;
	Asset snake_body;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
