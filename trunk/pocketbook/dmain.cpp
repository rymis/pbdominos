#include <inkview.h>
#include <dominos.h>
#include <players.h>
#include "dom.h"

#include <list>
#include <sstream>

#define LOOSER_POINTS 120
#define WRITE_MIN     13

using namespace dominos;

static std::list<iv_handler> *S__handlers = 0;
void ink_main(iv_handler h)
{
	bool start = false;
	if (!S__handlers) {
		S__handlers = new std::list<iv_handler>;
		start = true;
	}

	if (h)
		S__handlers->push_back(h);
	else
		S__handlers->push_back(GetEventHandler());

	if (start)
		InkViewMain(h);
	else if (h)
		SetEventHandler(h);
}

void ink_main_quit()
{
	S__handlers->pop_back();
	
	if (S__handlers->empty()) {
		CloseApp();
		delete S__handlers;
		S__handlers = NULL;
	} else {
		SetEventHandler(*S__handlers->rbegin());
	}
}

void set_font()
{
	static ifont *fnt = OpenFont("DroidSans", 12, 1);

	SetFont(fnt, BLACK);
}

void draw_text(int x, int y, int w, int max_h, int font_size, const char *txt, int flags)
{
	ifont *fnt = OpenFont("DroidSans", font_size, 1);
	std::istringstream S(txt);
	int h;
	int max_y = y + max_h;

	if (!fnt)
		return;

	while (!S.eof()) {
		std::string s;

		std::getline(S, s);
		h = TextRectHeight(w, (char*)s.c_str(), flags);
		if (y + h > max_h)
			return;

		DrawTextRect(x, y, w, h, (char*)s.c_str(), flags);

		if (h == 0)
			h = font_size;

		y += h;
	}

	CloseFont(fnt);
}

static imenu main_menu[5] = {
	{ ITEM_HEADER,   0, "Dominos", NULL },
	{ ITEM_ACTIVE, 101, "Play", NULL },
	{ ITEM_ACTIVE, 102, "Help", NULL },
	{ ITEM_ACTIVE, 133, "Exit", NULL },
	{ 0, 0, 0, 0 }
};

static enum {
	select_game,
	show_points,
	show_help
} state = select_game;

static int points[3] = {0, 0, 0};
static int cur_player = 0;
void draw()
{
	std::cout << "DRAW!" << std::endl;
	ClearScreen();

	// TODO: draw nice KOZEL :)
	// TODO: write points, lets user know ones :)
	std::stringstream s;

	s << "Player 1: " << points[0] << ", player 2: " << points[1] << ", player 3: " << points[2] << ".";

	set_font();
	DrawString(20, ScreenHeight() - 20, s.str().c_str());

	std::cout << "Player 1: " << points[0] << ", player 2: " << points[1] << ", player 3: " << points[2] << std::endl;
}

extern char *help_message;
static bool wait_round_result = false;
static void menu_handler(int it)
{
	std::cout << "MENU[" << it << "]" << std::endl;

	if (it == 101) { // Play
		play_round(cur_player);
		wait_round_result = true;
	} else if (it == 133) { // Exit
		ink_main_quit();
	} else if (it == 102) { // Help
		set_font();
		draw_text(20, 20, ScreenWidth() - 40, ScreenHeight() - 40, 14, help_message, ALIGN_LEFT | VALIGN_TOP);
		FullUpdate();
		state = show_help;
	} else /* if (it < 0) */ {
		OpenMenu(main_menu, 0, 20, 20, menu_handler);
		FullUpdate();
	}
}

static int my_main(int type, int par1, int par2)
{
	std::cout << "EVENT " << type << "(" << par1 << ", " << par2 << ")" << std::endl;

	if (wait_round_result && type != 24) {
		const game::round_result& res = get_round_result();

		if (res.points[0] > WRITE_MIN)
			points[0] += res.points[0];
		if (res.points[1] > WRITE_MIN)
			points[1] += res.points[1];
		if (res.points[2] > WRITE_MIN)
			points[2] += res.points[2];

		cur_player = res.winner;

		wait_round_result = false;

		if (points[0] >= LOOSER_POINTS) {
			// Player is Kozel!
			Message(ICON_INFORMATION, T("KOZEL"), T("You are the looser!"), 10000);
			points[0] = points[1] = points[2] = 0;
		} else if (points[1] >= LOOSER_POINTS || points[2] >= LOOSER_POINTS) {
			Message(ICON_INFORMATION, T("Congradulations!"), T("You won!"), 10000);
			points[0] = points[1] = points[2] = 0;
		}

		draw();

		OpenMenu(main_menu, 0, 20, 20, menu_handler);
		FullUpdate();
	}

	if (type == EVT_INIT) {
		// Init code here
		OpenMenu(main_menu, 0, 20, 20, menu_handler);
	} else if (type == EVT_SHOW) {
	} else if (type == EVT_REPAINT) {
		draw();
		FullUpdate();
		OpenMenu(main_menu, 0, 20, 20, menu_handler);
	} else if (type = EVT_KEYDOWN && par1 == KEY_OK) {
		if (state == show_help) {
			state = select_game;
			draw();
			FullUpdate();
			OpenMenu(main_menu, 0, 20, 20, menu_handler);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int i, j;
	struct timeval tv;

	gettimeofday(&tv, 0);

	srand(tv.tv_usec);

	// Open pocketbook screen:
	OpenScreen();
	// Set portrait orientation:
	SetOrientation(0);

	ink_main(my_main);

	CloseApp();

	return 0;
}
