#include <dominos.h>
#include <players.h>
#include <cstdlib>

#ifdef HAVE_READLINE
#include <readline/readline.h>

std::string rline(const std::string &s)
{
	const char *r = readline(s.c_str());

	if (!r)
		return "";
	else
		return std::string(r);
}

#else

std::string rline(const std::string &s)
{
	std::string r;

	getline(std::cin, r);

	return r;
}

#endif

void split(std::list<std::string> &lst, const std::string &s)
{
	size_t i;
	size_t start;
	size_t l = s.length();

	lst.clear();
	for (i = 0; i < l; i++) {
		while (i < l && isspace(s[i])) // skip spaces
			i++;
		if (i == l)
			break;

		start = i;

		while (i < l && !isspace(s[i]))
			i++;

		std::string ss = s.substr(start, i - start);

		lst.push_back(ss);
	}
}

struct exit_game { };

#include <sys/time.h>

namespace dominos {

	class cmd_player: public player {
		public:
			cmd_player()
			{
			}

			virtual ~cmd_player()
			{
			}

			virtual void play(game *g)
			{
				std::vector<tile>::iterator til;
std::cout << "You can play: " << g->get_board()->begin()->second << ", " << g->get_board()->rbegin()->second << std::endl;
				for (;;) {
					std::cout << "Your tiles: ";
					for (til = tiles.begin(); til != tiles.end(); til++) {
						std::cout << til->first << til->second << " ";
					}
					std::cout << std::endl;

					std::string cmd = rline("-> ");
					std::list<std::string> cmdl;

					split(cmdl, cmd);

					if (cmdl.empty())
						continue;

					// Command:
					std::string cmd_name = *cmdl.begin();
					if (cmd_name == "exit") {
						throw exit_game();
					} else if (cmd_name == "l" || cmd_name == "r") {
						bool front = (cmd_name == "l");
						std::list<std::string>::iterator arg = cmdl.begin();
						arg++;

						if (arg->length() != 2 || arg->at(0) < '0' || arg->at(0) > '6' ||
								arg->at(1) < '0' || arg->at(1) > '6') {
							std::cerr << "Error: use 'l DD', where D is digit from 0 to 6\n";
							continue;
						}

						// Check if we have this tile:
						int num;
						tile t(arg->at(0) - '0', arg->at(1) - '0');
						for (num = tiles.size() - 1; num >= 0; num--) {
							if (tiles[num] ^ t) {
								break;
							}
						}

						if (num < 0) {
							std::cerr << "Error: You have no this tile!\n";
							continue;
						}

						int can = g->get_board()->can_push(t);
						std::cout << "CAN: " << can << std::endl;
						if (front && (can == -1 || can == 2)) {
							g->play_front(this, num);
							return;
						} else if (!front && can > 0) {
							g->play_back(this, num);
							return;
						} else {
							std::cerr << "Error: You can not push this tile here\n";
							continue;
						}
					} else if (cmd_name == "take" || cmd_name == "t") {
						if (g->take(this))
							continue;
						std::cout << "The bank is empty and you skip your step" << std::endl;
						return;
					} else {
						std::cerr << "Error: unknown command <" << cmd_name << ">\n";
					}
				}

			}
	};

}

using namespace dominos;

int main(int argc, char *argv[])
{
	int i, j;
	int prev = 6;
	struct timeval tv;
	game::round_result res;

	gettimeofday(&tv, 0);

	srand(tv.tv_usec);

	board b;
	cmd_player pl_1;
	stupid_computer_player pl_2, pl_3;

	game g;

	g.set_board(&b);
	g.set_player(1, &pl_1);
	g.set_player(2, &pl_2);
	g.set_player(3, &pl_3);

	g.start();

	for (;;) {
		b.draw();

		res = g.next_step();

		if (res.result == game::round_result::finished) {
			std::cout << "Player " << res.winner << " win!";
			break;
		} else if (res.result == game::round_result::fish) {
			std::cout << "Player " << res.winner << " made fish!";
			break;
		}
	}

	std::cout << "Points: |";
	for (std::vector<int>::const_iterator it = res.points.begin(); it != res.points.end(); it++)
		std::cout << (*it) << " | ";
	std::cout << std::endl;


	return 0;
}

