#ifndef DOM_H___INC
#define DOM_H___INC

#include <dominos.h>
#include "messages.h"

namespace dominos {
	const game::round_result& get_round_result();
	void play_round(int start_player = 0);
};

void ink_main(iv_handler h);
void ink_main_quit();
void draw_text(int x, int y, int w, int max_h, int font_size, const char *txt, int flags);

#endif
