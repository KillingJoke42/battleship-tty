#include <ncurses.h>

int ncurses_hello_world(void)
{
    initscr();
    printw("Hello World!");
    refresh();
    getch();
    endwin();

    return 0;
}

int ncurses_hello_2(void)
{
    char ch;
    
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    printw("Type in a character to see it in bold\n");
    ch = getch();

    if (ch == KEY_F(1))
        printw("F1 key pressed");
    else
    {
        printw("The pressed key is: ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }

    refresh();
    getch();
    endwin();

    return 0;
}