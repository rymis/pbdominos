#ifndef PLAYERS____H
#define PLAYERS____H

#include "dominos.h"

namespace dominos {

	class computer_player: public player {
		public:
			computer_player()
			{
			}

			virtual ~computer_player()
			{
			}
	};

	class stupid_computer_player: public computer_player {
		public:
			stupid_computer_player()
			{
			}

			virtual ~stupid_computer_player()
			{
			}

			virtual void play(game *g)
			{
				int max_sum = 0;
				int max_num = 0;
				int i;
				tile bnd = bound_values(g);

				if (!take_all(g)) {
					return; // Can not play and can not take
				}

				// Find max tile:
				for (i = tiles.size() - 1; i >= 0; i--) {
					if (tiles[i].first == bnd.first || tiles[i].second == bnd.first ||
							tiles[i].first == bnd.second || tiles[i].second == bnd.second) {
						int sum = tiles[i].first + tiles[i].second;
						if (!sum)
							sum = SUM_EMPTY;
						// Lets doubles play frequently, otherise this is not player, this is fisher!
						if (tiles[i].first == tiles[i].second)
							sum *= 2;
						if (sum > max_sum) {
							max_sum = sum;
							max_num = i;
						}
					}
				}

				// And play!
				int can = g->get_board()->can_push(tiles[max_num]);
				if (can > 0) {
					g->play_back(this, max_num);
				} else {
					g->play_front(this, max_num);
				}
			}
	};

	class random_computer_player: public computer_player {
		public:
			random_computer_player()
			{
			}

			virtual ~random_computer_player()
			{
			}

			virtual void play(game *g)
			{
				std::vector<std::pair<int, bool> > idx;
				int i;

				if (!take_all(g))
					return;

				for (i = tiles.size() - 1; i >= 0; i--) {
					int can = g->get_board()->can_push(tiles[i]);
					if (can == -1) {
						idx.push_back(std::pair<int, bool>(i, true));
					} else if (can == 1) {
						idx.push_back(std::pair<int, bool>(i, false));
					} else if (can == 2) {
						idx.push_back(std::pair<int, bool>(i, false));
						idx.push_back(std::pair<int, bool>(i, true));
					}
				}

				i = rand() % idx.size();

				if (idx[i].second) {
					g->play_front(this, idx[i].first);
				} else {
					g->play_back(this, idx[i].first);
				}
			}
	};

}

#endif
