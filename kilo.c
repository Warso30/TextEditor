/*** includes ***/

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void enableRawMode();
void disableRawMode();
void die();

void die(const char *s){
	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enableRawMode(){
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
	// disableRawMode function gets called when the program exits
	atexit(disableRawMode);

	struct termios raw = orig_termios;

	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= ~(CS8);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw))
		die("tcsetattr");
}

/*** init ***/

int main(){
	enableRawMode();

	while (1){
		char c = '\0';
		// Won't treat EAGAIN as an error for Cygwin
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("read");
		if (iscntrl(c)){
			printf("%d\r\n", c);
		}
		else{
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') break;
	}

	return 0;
}