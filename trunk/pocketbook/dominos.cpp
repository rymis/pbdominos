#include <dominos.h>
#include <players.h>
#include <cstdlib>

#include <inkview.h>

#include <sys/time.h>

#include "dom.h"
#include <sstream>

namespace dominos {
	class pocketbook_board;
	class pocketbook_player;

	int screenWidth = 600;
	int screenHeight = 800;

	pocketbook_player *player_1 = 0;
	stupid_computer_player *player_2 = 0;
	random_computer_player *player_3 = 0;
	pocketbook_board *brd = 0;
	game *ggame = 0;

	void draw_game();
	int iv_select_tile(int type, int par1, int par2);

	/* For some reasons, that I can't understand pbres don't want process my images.
	 * So, I will work with bitmaps. Later.
	 */
	void FreeBitmap(ibitmap *bm)
	{
		free(bm->data);
		free(bm);
	}

	const int tWidth = 30;
	const int tHeight = 60;
	void draw_points(int x, int y, bool v, int n)
	{
		// Like in gen_all.py:
		int h2, w2, h3, w3;
		if (v) {
			h3 = tWidth / 3;
			w3 = tHeight / 6;
			h2 = tWidth / 2;
			w2 = tHeight / 4;
		} else {
			w3 = tWidth / 3;
			h3 = tHeight / 6;
			w2 = tWidth / 2;
			h2 = tHeight / 4;
		}

#define P(x, y) \
		do { \
			DrawPixel(x, y-2, 0); \
			DrawPixel(x, y+2, 0); \
			DrawPixel(x+2, y, 0); \
			DrawPixel(x+2, y, 0); \
			DrawRect(x-1, y-1, 3, 3, 0); \
		} while (0)

		if (n % 2)
			P(x + w2, y + h2);

		if (n >= 2) { // \-diag
			P(x + w2 + w3, y + h2 + h3);
			P(x + w2 - w3, y + h2 - h3);
		}

		if (n >= 4) { // /-diag
			P(x + w2 - w3, y + h2 + h3);
			P(x + w2 + w3, y + h2 - h3);
		}

		if (n == 6) {
			if (v) {
				P(x + w2 - w3, y + h2);
				P(x + w2 + w3, y + h2);
			} else {
				P(x + w2, y + h2 - h3);
				P(x + w2, y + h2 + h3);
			}
		}
#undef P
	}

	void draw_htile(int x, int y, const tile &t)
	{
		FillArea(x, y, tHeight, tWidth, 0x00FFFFFF);
		DrawRect(x, y, tHeight, tWidth, 0);
		DrawLine(x + tHeight/2, y, x + tHeight/2, y + tWidth - 1, 0);
		DrawLine(x + tHeight/2 + 1, y, x + tHeight/2 + 1, y + tWidth - 1, 0);
		draw_points(x, y, false, t.first);
		draw_points(x + tHeight/2, y, false, t.second);
	}

	void draw_vtile(int x, int y, const tile &t)
	{
		FillArea(x, y, tWidth, tHeight, 0x00FFFFFF);
		DrawRect(x, y, tWidth, tHeight, 0);
		DrawLine(x, y + tHeight/2, x + tWidth, y + tHeight/2, 0);
		DrawLine(x, y + tHeight/2 + 1, x + tWidth - 1, y + tHeight/2 + 1, 0);
		draw_points(x, y, true, t.first);
		draw_points(x, y + tHeight/2, true, t.second);
	}

	void draw_tile(int x, int y, int d, const tile &t)
	{
		if (d == 0)
			draw_htile(x - tHeight/2, y - tWidth/2, t);
		else if (d == 1)
			draw_vtile(x - tWidth/2, y - tHeight/2, rtile(t));
		else if (d == 2)
			draw_htile(x - tHeight/2, y - tWidth/2, rtile(t));
		else
			draw_vtile(x - tWidth/2, y - tHeight/2, t);
	}

	void draw_back_tile(int x, int y)
	{
		FillArea(x, y, tWidth, tHeight, 0x00FFFFFF);
		DrawRect(x, y, tWidth, tHeight, 0);
		DrawLine(x, y, x + tWidth - 1, y + tHeight/2 - 1, 0);
		DrawLine(x, y + tHeight/2 - 1, x + tWidth - 1, y, 0);
		DrawLine(x, y + tHeight/2 + 1, x + tWidth - 1, y + tHeight - 1, 0);
		DrawLine(x, y + tHeight, x + tWidth - 1, y + tHeight/2 + 1, 0);
	}


	class pocketbook_board: public board {
		public:
			pocketbook_board()
			{
				max_width = 16;
				max_height = 18;
			}

			virtual ~pocketbook_board()
			{
			}

			virtual void draw_graphics(int min_x, int min_y, int max_x, int max_y)
			{
				if (tiles.empty())
					return;

				// Tile size:
				int x, y;

				for (std::list<tile_pos>::iterator it = tiles.begin(); it != tiles.end(); it++) {
					x = screenWidth / 2 - tWidth / 2 + (it->position.x * tHeight)/4;
					y = screenHeight / 2 - tHeight / 2 + (it->position.y * tHeight)/4;
					draw_tile(x, y, it->deg, it->til);
				}
			}
	};

	class pocketbook_player: public player {
		int selected_tile;
		game *tmp_game;
		public:
			pocketbook_player()
			{
				tmp_game = 0;
			}

			virtual ~pocketbook_player()
			{
			}

			virtual void play(game *g)
			{
				selected_tile = 0;
				tmp_game = g;

				while (selected_tile < tiles.size()) {
					if (g->get_board()->can_push(tiles[selected_tile]))
						break;
					selected_tile++;
				}
				if (selected_tile == tiles.size())
					selected_tile = 0;

				// TODO: if set in parameters - take tiles until you can not play
	
				draw_game();

				// Ok, now we must select tile for play:
				ink_main(iv_select_tile);
			}

			void key_left()
			{
				selected_tile--;
				if (selected_tile < 0)
					selected_tile = tiles.size() - 1;
				draw_selector(true);
			}

			void key_right()
			{
				selected_tile++;
				if (selected_tile >= tiles.size())
					selected_tile = 0;
				draw_selector(true);
			}

			bool key_ok()
			{
				int can = tmp_game->get_board()->can_push(tiles[selected_tile]);

				if (can == -1) {
					tmp_game->play_front(this, selected_tile);
					return true;
				} else if (can == 1) {
					tmp_game->play_back(this, selected_tile);
					return true;
				} else if (can == 2) {
					// TODO: write something :)
					return false;
				}

				return false;
			}

			bool key_up()
			{
				int can = tmp_game->get_board()->can_push(tiles[selected_tile]);
				if (can > 0) {
					tmp_game->play_back(this, selected_tile);
					return true;
				}
				return false;
			}

			bool key_down()
			{
				int can = tmp_game->get_board()->can_push(tiles[selected_tile]);
				if (can == 2 || can == -1) {
					tmp_game->play_front(this, selected_tile);
					return true;
				}
				return false;
			}

			void draw_selector(bool update = false)
			{
				unsigned l = tiles.size();
				int x = screenWidth / 2 - (tiles.size() * (tWidth + 10) - 10) / 2;

				FillArea(0, screenHeight - 9, screenWidth - 1, screenHeight - 1, 0x00FFFFFF); // Delete previously selected pointer
				FillArea(x + selected_tile * (tWidth + 10) - 3, screenHeight - 8,
						6, 6, 0);
				if (update) {
					PartialUpdate(0, screenHeight - 9, screenWidth - 1, screenHeight - 1);
				}
			}

			void draw_tiles()
			{
				int x = screenWidth/2 - (tiles.size() * (tWidth + 10) - 10)/2;
				unsigned i;
				unsigned l = tiles.size();

				for (i = 0; i < l; i++) {
					draw_tile(x + i*(tWidth + 10), screenHeight - 10 - tHeight, true, tiles[i]);
				}

				draw_selector();
			}

			bool take_all1(game *g)
			{
				bool r = take_all(g);
				if (r) {
					selected_tile = tiles.size() - 1;
				}

				return r;
			}

	};

	void draw_game()
	{
		ClearScreen();

		brd->draw();
		int i, j;
		int cnt = 0;

		// Now we must draw players:
		cnt = player_2->tiles_cnt();
		for (i = 0; i < cnt; i++) {
			draw_back_tile(10 + i * (tWidth + 10), 10);
		}

		cnt = player_3->tiles_cnt();
		for (i = 0; i < cnt; i++) {
			draw_back_tile(310 + i * (tWidth + 10), 10);
		}

		player_1->draw_tiles();

		// Draw bank at right side of board:
		cnt = ggame->tiles_in_bank();
		for (i = 0; i < cnt; i++) {
			draw_back_tile(screenWidth - tWidth - 10, tHeight + 30 + i * (tHeight + 10));
		}

		FullUpdate();
	}

	int iv_select_tile(int type, int par1, int par2)
	{
		std::cout << "inkmain(" << type << ", " << par1 << ", " << par2 << ")" << std::endl;

		if (type == EVT_KEYPRESS) {
			if (par1 == KEY_RIGHT) {
				player_1->key_right();
			} else if (par1 == KEY_LEFT) {
				player_1->key_left();
			} else if (par1 == KEY_UP) {
				if (player_1->key_up()) {
					ink_main_quit();
				}
			} else if (par1 == KEY_DOWN) {
				if (player_1->key_down()) {
					ink_main_quit();
				}
			} else if (par1 == KEY_OK) {
				if (player_1->can_play(ggame)) {
					if (player_1->key_ok()) {
						ink_main_quit();
					}
				} else {
					if (player_1->take_all1(ggame)) {
						draw_game();
						return 0;
					} else {
						ink_main_quit(); // Player can not put the tile
					}
				}
			} else if (par1 == KEY_MENU) {
				// TODO: menu
			}
		} else if (type == EVT_REPAINT) {
			draw_game();
		} else if (type == EVT_SHOW) {
			//FullUpdate();
		}

		return 0;
	}

	static game::round_result g_rresult;
	static void dont_sleep_baby();
	static int my_main(int type, int par1, int par2)
	{
		std::cout << "my_main(" << type << ", " << par1 << ", " << par2 << ")" << std::endl;

		return 0;
	}

	static void dont_sleep_baby()
	{
		if (GetEventHandler() != my_main) {
			SetWeakTimer("NEXT STEP", dont_sleep_baby, 100);
			return;
		}

		std::cout << "g_rresult = ";
		g_rresult.print();

		if (g_rresult.result == game::round_result::finished) {
			std::cout << "Player " << g_rresult.winner << " win!";
			draw_game();

			std::stringstream s;
			s << "Player " << g_rresult.winner << " win!";
			Message(ICON_INFORMATION, "Win!", (char*)s.str().c_str(), 10000);

			ink_main_quit();
		} else if (g_rresult.result == game::round_result::fish) {
			std::cout << "Player " << g_rresult.winner << " made fish!";
			draw_game();

			std::stringstream s;
			s << "Player " << g_rresult.winner << " made fish!";
			Message(ICON_INFORMATION, "Fish!", (char*)s.str().c_str(), 10000);

			ink_main_quit();
		} else {
			g_rresult = ggame->next_step();
			SetWeakTimer("NEXT STEP", dont_sleep_baby, 100);
		}

		//draw_game();
	}

	void play_round(int start_player)
	{
		screenWidth = ScreenWidth();
		screenHeight = ScreenHeight();

		// Set globals:
		g_rresult.result = game::round_result::cont;

		brd = new pocketbook_board();
		player_1 = new pocketbook_player();
		player_2 = new stupid_computer_player();
		player_3 = new random_computer_player();
		ggame = new game();

		ggame->set_board(brd);
		ggame->set_player(1, player_1);
		ggame->set_player(2, player_2);
		ggame->set_player(3, player_3);

		// Choose player for first step and start the game:
		ggame->start(start_player);

		// Clear screen:
		ClearScreen();

		ink_main(my_main);

		SetWeakTimer("NEXT STEP", dont_sleep_baby, 100);
	}

	const game::round_result& get_round_result()
	{
		return g_rresult;
	}
}

