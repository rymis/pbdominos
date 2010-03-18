#ifndef DOM_H___INC
#define DOM_H___INC

#include <dominos.h>

namespace dominos {
	const game::round_result& get_round_result();
	void play_round(int start_player = 0);
};

void ink_main(iv_handler h);
void ink_main_quit();

#endif
