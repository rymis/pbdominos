#ifndef DOMINOS____H
#define DOMINOS____H

#include <algorithm>
#include <list>
#include <vector>
#include <string>
#include <iostream>

using std::max;

namespace dominos {
	const int SUM_EMPTY = 10;

	class rules {
		public:
			static const int SUM_EMPTY = 10;
			static const int LOOSER_POINTS = 120;
			static const int MIN_POINTS = 13;
	};

	struct error: public std::exception {
		std::string message;
		error(const std::string &s)
		{
			message = s;
			std::cerr << "ERROR: " << message << std::endl;
		}

		virtual ~error() throw ()
		{
		}
	};

	//! tile is representation of dominos tile
	class player;
	class game;
	class board;

	//! mathematical vector representation
	struct vector {
		int x, y; // coordinates

		vector(int x_in, int y_in)
		{
			x = x_in;
			y = y_in;
		}

		vector()
		{
			x = 1;
			y = 0;
		}

		vector operator * (int a)
		{
			return vector(x*a, y*a);
		}

		vector operator / (int a)
		{
			return vector(x / a, y / a);
		}

		void operator /= (int a)
		{
			x /= 2;
			y /= 2;
		}

		vector operator + (const vector &v)
		{
			return vector(x + v.x, y + v.y);
		}

		void rotate(int d = 1)
		{
			d = d % 4;
			for (int i = 0; i < d; i++) {
				int nx = y;
				y = -x;
				x = nx;
			}
		}

		bool is_colliniar(const vector& v)
		{
			return (x*v.y - y*v.x == 0);
		}

		void print()
		{
			std::cout << "{ " << x << ", " << y << " }";
		}
	};

	inline std::ostream& operator << (std::ostream& S, const vector &v)
	{
		S << "{ " << v.x << ", " << v.y << " }";
		return S;
	}

	//! tile representation:
	typedef std::pair<int, int> tile;
	inline tile rtile(const tile& t)
	{
		return tile(t.second, t.first);
	}

	inline std::ostream& operator << (std::ostream& S, const tile& t)
	{
		S << "[" << t.first << "|" << t.second << "]";
		return S;
	}

	inline bool operator ^ (const tile &t1, const tile & t2)
	{
		return (t1.first == t2.first && t1.second == t2.second) ||
				(t1.first == t2.second && t1.second == t2.first);
	}

	//! base class for board:
	class board {
		protected:
			struct tile_pos {
				tile til;   // tile
				int deg; // degree/90
				vector position;

				tile_pos(const tile &t)
				{
					til = t;
					deg = 0;
				}

				std::ostream& print(std::ostream &S)
				{
					S << "TILE " << position << " " << deg << " [" << til.first << "|" << til.second << "]\n";
					return S;
				}
			};

			typedef std::list<tile_pos> tiles_t;
			std::list<tile_pos> tiles;
			std::list<tile_pos>::iterator first_tile;
			std::list<tile_pos>::reverse_iterator rfirst_tile;

			int max_width, max_height; // Board size characteristics
			int b_deg, f_deg;

		public:
			//! public tiles iterator
			template <typename ITER>
			class base_iterator {
				ITER it;
				public:
					base_iterator()
					{
					}

					base_iterator(const base_iterator<ITER> &iter)
					{
						it = iter.it;
					}

					base_iterator(const ITER& i)
					{
						it = i;
					}

					~base_iterator()
					{
					}

					void operator ++ ()
					{
						it++;
					}

					void operator ++ (int)
					{
						++it;
					}

					void operator () () {
						return it->til;
					}

					const tile* operator -> ()
					{
						return &(it->til);
					}

					bool operator == (const base_iterator<ITER> &iter)
					{
						return iter.it == it;
					}

					bool operator != (const base_iterator<ITER> &iter)
					{
						return iter.it != it;
					}

					base_iterator<ITER> operator = (const base_iterator<ITER> &iter)
					{
						it = iter.it;
						return *this;
					}
			};

			typedef base_iterator<tiles_t::const_iterator> iterator;
			typedef base_iterator<tiles_t::const_reverse_iterator> reverse_iterator;

			iterator begin() const
			{
				return iterator(tiles.begin());
			}

			iterator end() const
			{
				return iterator(tiles.end());
			}

			reverse_iterator rbegin() const
			{
				return reverse_iterator(tiles.rbegin());
			}

			reverse_iterator rend() const
			{
				return reverse_iterator(tiles.rend());
			}

			board()
			{
				max_width = 40;
				max_height = 12;
				b_deg = 0;
				f_deg = 2;
			}

			virtual ~board()
			{
			}

			tile get_bound_values() const
			{
				int n1 = tiles.begin()->til.second;
				int n2 = tiles.rbegin()->til.second;
				if (tiles.begin() == first_tile) {
					n1 = tiles.begin()->til.first;
				}
				return tile(n1, n2);
			}
			//! returns if we can push tile.
			// If can push into front - returns -1
			// if push back returns 1
			// If can not - returns 0
			// If both - returns 2
			int can_push(const tile &t) const
			{
				int res = 0;

				if (tiles.empty())
					return 2;

				if (tiles.begin() == first_tile) {
					if (first_tile->til.first == t.first || first_tile->til.first == t.second)
						res = -1;
				} else {
					if (tiles.begin()->til.second == t.second) { // Ok, we can set it
						res = -1;
					} else if (tiles.begin()->til.second == t.first) { // Ok, we can set reverse of it
						res = -1;
					}
				}

				if (tiles.rbegin()->til.second == t.first) { // Ok, we can set it
					if (res)
						res = 2;
					else
						res = 1;
				} else if (tiles.rbegin()->til.second == t.second) { // Ok, we can set reverse of it
					if (res)
						res = 2;
					else
						res = 1;
				}

				return res;
			}

			//! push tile to back (right)
			bool push_tile_back(const tile &t)
			{
				if (tiles.empty()) { // No tiles, no check!
					tiles.push_back(tile(t));
					first_tile = tiles.begin();
					rfirst_tile = tiles.rbegin();
					first_tile->position.x = 0;
					first_tile->position.y = 0;
					if (first_tile->til.first == first_tile->til.second) {
						first_tile->deg = 1;
					} else {
						first_tile->deg = 0;
					}
					return true;

				} else if (tiles.rbegin()->til.second == t.first) {
					tiles.push_back(tile(t));
				} else if (tiles.rbegin()->til.second == t.second) { // Ok, we can set reverse of it
					tiles.push_back(rtile(t));
				} else {
					return false;
				}

				// Now check position:
				tiles_t::reverse_iterator cur = tiles.rbegin();
				tiles_t::reverse_iterator prev = tiles.rbegin(); prev++;
				set_tile_position(cur, prev, b_deg);

				return true;
			}

			template <typename IT>
			void set_tile_position(IT &cur, IT &prev, int &deg)
			{
				vector prev_v = prev->position;
				vector deg_v;
				vector pdeg_v;
				int max_d = deg % 2 ? max_height: max_width;

				if (prev->deg % 2 != deg % 2) {
					pdeg_v = vector(1, 0);
				} else {
					pdeg_v = vector(2, 0);
				}

				// set cur_v:
				if (cur->til.first == cur->til.second) // double
					deg_v = vector(1, 0);
				else
					deg_v = vector(2, 0);
				
				// Rotate prev_v:
				prev_v.rotate((4 - deg) % 4);

				vector new_pos = prev_v + pdeg_v + deg_v;

				// Can we simple push tile?
				if (new_pos.x >= max_d) { // We must rotate
					if (prev->deg % 2 == deg % 2) {
						pdeg_v = vector(1, -1);
					} else {
						pdeg_v = vector(0, -2);
					}
					deg_v = vector(0, -2);
					new_pos = prev_v + deg_v + pdeg_v;

					new_pos.rotate(deg);
					deg = (deg + 1) % 4;
					cur->deg = deg;
				} else {
					new_pos.rotate(deg);
					if (cur->til.first == cur->til.second)
						cur->deg = (deg + 1) % 4;
					else
						cur->deg = deg;
				}
				cur->position = new_pos;
				return;
			}

			//! push tile to front (left)
			bool push_tile_front(const tile &t)
			{
				if (tiles.empty()) { // No tiles, no check!
					tiles.push_back(tile(t));
					first_tile = tiles.begin();
					rfirst_tile = tiles.rbegin();
					first_tile->position.x = 0;
					first_tile->position.y = 0;
					if (first_tile->til.first == first_tile->til.second) {
						first_tile->deg = 1;
					} else {
						first_tile->deg = 0;
					}
					return true;
				} else if (tiles.begin() == first_tile) {
					if (tiles.begin()->til.first == t.second) { // Ok, we can set it
						tiles.push_front(rtile(t));
					} else if (tiles.begin()->til.first == t.first) { // Ok, we can set reverse of it
						tiles.push_front(tile(t));
					} else
						return false;
				} else if (tiles.begin()->til.second == t.second) { // Ok, we can set it
					tiles.push_front(rtile(t));
				} else if (tiles.begin()->til.second == t.first) { // Ok, we can set reverse of it
					tiles.push_front(tile(t));
				} else {
					return false;
				}

				tiles_t::iterator cur = tiles.begin();
				tiles_t::iterator next = tiles.begin(); next++;

				set_tile_position(cur, next, f_deg);

				return true;
			}

			void draw()
			{
				int deg = 0;
				int max_x = 0;
				int max_y = 0;
				int min_x = 0;
				int min_y = 0;

				if (tiles.empty()) { // Nothing to draw
					draw_graphics(0, 0, 0, 0);
					return;
				}

				deg = 0;
				for (tiles_t::iterator i = tiles.begin(); i != tiles.end(); i++) {
					if (i->position.x > max_x) max_x = i->position.x;
					if (i->position.y > max_y) max_y = i->position.y;
					if (i->position.x < min_x) min_x = i->position.x;
					if (i->position.y < min_y) min_y = i->position.y;
				}

				std::cout << min_x << " - " << max_x << "|" << min_y << " - " << max_y << "\n";

				draw_graphics(min_x, min_y, max_x, max_y);
			}

		protected:
			//! draw board (default is console implementation :))
			virtual void draw_graphics(int min_x, int min_y, int max_x, int max_y)
			{
				if (tiles.empty())
					return;
				/* Render it to array of strings and after all print: */
				std::vector<std::string> p;
				int i;
				int w = max(max_x, -min_x) * 2 + 10;
				int h = max(max_y, -min_y) * 2 + 10;
				std::string split;
				split.resize(w + 2, '_');

std::cout << w << "x" << h << "\n";

				for (i = 0; i < h; i++) {
					std::string s;
					s.resize(w, ' ');
					p.push_back(s);
				}

				for (std::list<tile_pos>::iterator it = tiles.begin(); it != tiles.end(); it++) {
					int p_x = w/2 + it->position.x;
					int p_y = h/2 + it->position.y;

it->print(std::cout);

					if (it->deg == 3) {  // |
						p[p_y][p_x + 1] = it->til.first + '0';
						p[p_y][p_x] = it->til.first + '0';
						p[p_y - 1][p_x + 1] = it->til.first + '0';
						p[p_y - 1][p_x] = it->til.first + '0';
						p[p_y + 1][p_x + 1] = it->til.second + '0';
						p[p_y + 1][p_x] = it->til.second + '0';
						p[p_y + 2][p_x + 1] = it->til.second + '0';
						p[p_y + 2][p_x] = it->til.second + '0';
					} else if (it->deg == 1) { // |
						p[p_y][p_x + 1] = it->til.second + '0';
						p[p_y][p_x] = it->til.second + '0';
						p[p_y - 1][p_x + 1] = it->til.second + '0';
						p[p_y - 1][p_x] = it->til.second + '0';
						p[p_y + 1][p_x + 1] = it->til.first + '0';
						p[p_y + 1][p_x] = it->til.first + '0';
						p[p_y + 2][p_x + 1] = it->til.first + '0';
						p[p_y + 2][p_x] = it->til.first + '0';
					} else if (it->deg == 0) {  // -
						p[p_y][p_x] = it->til.first + '0';
						p[p_y + 1][p_x] = it->til.first + '0';
						p[p_y][p_x - 1] = it->til.first + '0';
						p[p_y + 1][p_x - 1] = it->til.first + '0';
						p[p_y][p_x + 2] = it->til.second + '0';
						p[p_y + 1][p_x + 2] = it->til.second + '0';
						p[p_y][p_x + 1] = it->til.second + '0';
						p[p_y + 1][p_x + 1] = it->til.second + '0';
					} else { // -
						p[p_y][p_x] = it->til.second + '0';
						p[p_y + 1][p_x] = it->til.second + '0';
						p[p_y][p_x - 1] = it->til.second + '0';
						p[p_y + 1][p_x - 1] = it->til.second + '0';
						p[p_y][p_x + 2] = it->til.first + '0';
						p[p_y + 1][p_x + 2] = it->til.first + '0';
						p[p_y][p_x + 1] = it->til.first + '0';
						p[p_y + 1][p_x + 1] = it->til.first + '0';
					}
				}

				std::cout << split << "\n";
				for (std::vector<std::string>::iterator s = p.begin(); s != p.end(); s++) {
					std::cout << "|" << *s << "|\n";
				}
				split[0] = '\\';
				split[w + 1] = '/';
				std::cout << split << "\n";
			}
		private:
	};

	//! public player interface:
	class player {
		friend class game;
		protected:
			//! Player tiles:
			std::vector<tile> tiles;
		public:
			player()
			{
				tiles.reserve(14);
			}

			virtual ~player() {}

			//! add tile to player:
			virtual void add_tile(const tile& t)
			{
				tiles.push_back(t);
				// TODO: sort?
			}

			//! play his step:
			virtual void play(game *g) = 0;

			size_t tiles_cnt() const
			{
				return tiles.size();
			}

			inline bool take_all(game *g);
			inline tile bound_values(game *g);

			bool can_play(game *g)
			{
				tile bnd = bound_values(g);
				int i;

				for (i = tiles.size() - 1; i >= 0; i--) {
					if (tiles[i].first == bnd.first || tiles[i].second == bnd.first) {
						return true;
					}
					if (tiles[i].first == bnd.second || tiles[i].second == bnd.second) {
						return true;
					}
				}
				return false;
			}
	};

	//! class that implements game
	// At this moment only classic "Kozel" game with 3 players implemented
	class game {
		board *brd;

		player *player1;
		player *player2;
		player *player3;

		int current_player;
		void next_player()
		{
			current_player = current_player % 3 + 1;
		}

		//! all tiles
		std::vector<tile> tiles;
		unsigned current_tile;
		player* get_player(int num)
		{
			switch (num) {
				case 1: return player1;
				case 2: return player2;
				case 3: return player3;
				default:
					throw error("Invalid value!");
			}
		}

		int get_player_num(player *pl)
		{
			if (pl == player1)
				return 1;
			if (pl == player2)
				return 2;
			if (pl == player3)
				return 3;
			return -1;
		}

		public:
			//! Representation of round result:
			struct round_result {
				enum { cont, finished, fish } result;
				int winner;
				std::vector<int> points;

				round_result()
				{
					winner = -1;
					result = cont;
				}

				void print()
				{
					if (result == cont) {
						std::cout << "RES: continue" << std::endl;
						return;
					}

					std::cout << "RES: " << winner;

					std::cout << (result == finished ? "finished": "made fish");
					std::cout << ". Points: |";
					for (std::vector<int>::const_iterator i = points.begin(); i != points.end(); i++) {
						std::cout << *i << "|";
					}

					std::cout << std::endl;
				}
			};
		protected:
			//! Construct round result by checking the board:
			round_result make_round_result(int num_player)
			{
				int j, k;
				round_result res;

				// 1. Check is player finished?
				for (j = 1; j <= 3; j++) {
					if (get_player(j)->tiles.size() == 0) {
						res.winner = j;
						res.result = round_result::finished;
					}
				}

				if (res.result != round_result::finished) { // Check fish:
					int cnt1 = 0;
					int cnt2 = 0;
					tile t = brd->get_bound_values();
					int n1 = t.first;
					int n2 = t.second;

					for (board::iterator t = brd->begin(); t != brd->end(); t++) {
						if (t->first == n1)
							cnt1++;
						if (t->second == n1)
							cnt1++;
						if (t->first == n2)
							cnt2++;
						if (t->second == n2)
							cnt2++;
					}

					if (cnt1 == 8 && cnt2 == 8) { // Fish!
						res.winner = num_player;
						res.result = round_result::fish;
					}
				}

				if (res.result != round_result::cont) {
					for (j = 1; j <= 3; j++) {
						int sum = 0;
						player *pl = get_player(j);

						for (std::vector<tile>::const_iterator it = pl->tiles.begin(); it != pl->tiles.end(); it++) {
							if (it->first == 0 && it->second == 0) {
								sum += SUM_EMPTY;
							} else {
								sum += (it->first + it->second);
							}
						}

						res.points.push_back(sum);
					}
				}

				std::cout << "MAKE_ROUND_RESULT[" << num_player << "] ";
				res.print();

				return res;
			}
		public:

			game()
			{
				int i, j, k;

				player1 = 0;
				player2 = 0;
				player3 = 0;
				brd = 0;
				current_tile = 0;
				current_player = 0;

				for (i = 0; i < 7; i++) {
					for (j = 0; j <= i; j++) {
						tiles.push_back(tile(i, j));
					}
				}

				// We need shuffle them:
				for (i = 0; i < 1000; i++) {
					tile tmp;
					j = rand() % tiles.size();
					while ((k = rand() % tiles.size()) == j) ;

					tmp = tiles[j];
					tiles[j] = tiles[k];
					tiles[k] = tmp;
				}
			}

			~game()
			{
			}

			void set_board(board *b)
			{
				if (brd)
					delete brd;
				brd = b;
			}

			const board* get_board()
			{
				return brd;
			}

			void set_player(int num, player *p)
			{
				player **pp = 0;
				switch (num) {
					case 1: pp = &player1; break;
					case 2: pp = &player2; break;
					case 3: pp = &player3; break;
				};

				if (*pp)
					delete *pp;
				*pp = p;
			}

			void prepare()
			{
				int i;
				if (!brd || !player1 || !player2 || !player3) {
					throw error("Invalid values");
				}

				for (i = 0; i < 7; i++) {
					player1->add_tile(tiles[current_tile++]);
				}

				for (i = 0; i < 7; i++) {
					player2->add_tile(tiles[current_tile++]);
				}

				for (i = 0; i < 7; i++) {
					player3->add_tile(tiles[current_tile++]);
				}
			}

			//! Start the game:
			void start(int player_num = 0)
			{
				player *pl = 0;
				int i, j;

				if (!current_tile)
					prepare();

				switch (player_num) {
					case 1:
					case 2:
					case 3: pl = get_player(player_num); break;
					default:
					   // Choose player with min double:
					   int max_d = -1;
					   int maxp = 0;

					   for (i = 1; i <= 3; i++) {
						pl = get_player(i);

						for (j = pl->tiles.size() - 1; j >= 0; j--) {
							if (pl->tiles[j].first == pl->tiles[j].second) { // double
								if (pl->tiles[j].first > max_d) {
									max_d = pl->tiles[j].first;
									maxp = i; // player
								}
							}
						}
					   }
					   if (maxp == 0) {
						//! Choose player with max tile:
						int max_tile = 0;

						for (i = 1; i <= 3; i++) {
							pl = get_player(i);

							for (j = pl->tiles.size() - 1; j >= 0; j--) {
								if (pl->tiles[j].first + pl->tiles[j].second > max_tile) {
									maxp = i;
									max_tile = pl->tiles[j].first + pl->tiles[j].second;
								}
							}
						}
					   }

					   // Now we have choosen player.
					   pl = get_player(maxp);
					   player_num = maxp;
				}

				current_player = player_num;

				// We have player. Search tile for first move
				int max_d = -1;
				int max_i = -1;
				int max_t = 0;
				int max_ti = -1;

				for (i = pl->tiles.size() - 1; i >= 0; i--) {
					if (pl->tiles[i].first == pl->tiles[i].second) {
						if (pl->tiles[i].first > max_d) {
							max_d = pl->tiles[i].first;
							max_i = i;
						}
					} else {
						if (pl->tiles[i].first + pl->tiles[i].second > max_t) {
							max_t = pl->tiles[i].first + pl->tiles[i].second;
							max_ti = i;
						}
					}
				}

				if (max_d >= 0) {
					play_back(pl, max_i);
				} else {
					play_back(pl, max_ti);
				}

				next_player();
			}

			//! Down tile from player
			round_result play_back(player *pl, int tile_num)
			{
				int can = brd->can_push(pl->tiles[tile_num]);
				if (can != 1 && can != 2) {
					throw error("Can not play this tile");
				}

				brd->push_tile_back(pl->tiles[tile_num]);
				pl->tiles.erase(pl->tiles.begin() + tile_num);

				return make_round_result(get_player_num(pl));
			}

			//! Down tile from player
			round_result play_front(player *pl, int tile_num)
			{
				int can = brd->can_push(pl->tiles[tile_num]);
				if (can != -1 && can != 2) {
					throw error("Can not play this tile");
				}

				brd->push_tile_front(pl->tiles[tile_num]);
				pl->tiles.erase(pl->tiles.begin() + tile_num);

				return make_round_result(get_player_num(pl));
			}

			round_result next_step()
			{
				player *pl = get_player(current_player);
				round_result res;

				pl->play(this);

				res = make_round_result(current_player);

				if (res.result == round_result::cont)
					next_player();

				return res;
			}

			bool take(player *pl)
			{
				if (current_tile < tiles.size()) {
					pl->add_tile(tiles[current_tile++]);
					return true;
				}
				return false;
			}

			int tiles_in_bank() const
			{
				return tiles.size() - current_tile;
			}

	};

	// Utility functions:
	inline tile player::bound_values(game *g)
	{
		const board* b = g->get_board();

		return b->get_bound_values();
	}

	inline bool player::take_all(game *g)
	{
		for (;;) {
			if (can_play(g))
				return true;

			if (!g->take(this))
				return false;
		}

		return false;
	}

};

#endif
