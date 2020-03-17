/*
E: (T B) | (T A B) | (T C B) | (T A C B) | e
B: S E B | E B | b
A: {N}
N: T: T N | T: T | T: S N | T: S
C: [M]
M: T M | T
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define INSIZE 128

char inbuf[INSIZE];
uint16_t pos = 0;
FILE *in;
FILE *out;

void error(char *s) {
	fprintf(stderr, "E: %s\n", s);
	fflush(NULL);
	exit(1);
}

	
void refresh() {
	memcpy(inbuf, inbuf+(INSIZE-8), 8);
	for(int i = 8; i < INSIZE; i++) {
		char c = fgetc(in);
		if (c == EOF) {
			inbuf[i] = c;
			pos = 8;
			return;
		}
		inbuf[i] = c;
	}
	pos = 8;
}

char _next() {
	if (pos >= INSIZE) {
		refresh();
	}
	char c = inbuf[pos++];
	if(c == EOF) {
		fflush(NULL);
		exit(0);
	}
	return c;
}

char next() {
	char c = _next();
	if (c == '/') {
		char d = _next();
		if (d == '/') {
			while(_next() != '\n');
			return _next();
		}
		pos--;
	}
	return c;
}

char eatspace() {
	char c;
	do {
		c = next();
	} while(isspace(c));
	return c;
}

uint8_t accept(char a) {
	char c = eatspace();
	if(c == a) {
		return 1;
	}
	pos--;
	return 0;
}

uint8_t string() {
	if(!accept('"')) {
		return 0;
	}
	while(1) {
		char c = _next();
		if (c == '"') {
			break;
		}
		if (c == '\\') {
			c = _next();
			switch(c) {
			case '"':
				fprintf(out, "\"");
				continue;
			default:
				fprintf(out, "%c", c);
				continue;
			}
		}
		fprintf(out, "%c", c);
	}
	return 1;
}

uint8_t token() {
	char c = eatspace();
	if(strchr("]})", c)) {
		pos--;
		return 0;
	}
	while(!isspace(c) && !strchr("]})", c)) {
		fprintf(out, "%c", c);
		c = next();
	}
	pos--;
	return 1;
}

uint8_t stoken(char *s) {
	char c = eatspace();
	if(strchr("]})", c)) {
		pos--;
		return 0;
	}
	while(!isspace(c) && !strchr("]})", c)) {
		*(s++) = c;
		fprintf(out, "%c", c);
		c = next();
	}
	pos--;
	return 1;
}


uint8_t tag() {
	char c = eatspace();
	if(strchr("]}):", c)) {
		pos--;
		return 0;
	}
	while(!isspace(c) && !strchr("]}):", c)) {
		fprintf(out, "%c", c);
		c = next();
	}
	pos--;
	if(!accept(':')) {
		return 0;
	}
	fprintf(out, "=\"");
	return 1;
}

void attrs() {
	if(!accept('{')) {
		return;
	}
	while(!accept('}')) {
		fprintf(out, " ");
		if(tag()) {
			if(!string()) {
				token();
			}
			fprintf(out, "\"");
		}
	}
}

void classes() {
	if(!accept('[')) {
		return;
	}
	fprintf(out, " class=\"");
	token();
	while(!accept(']')) {
		fprintf(out, " ");
		token();
	}
	fprintf(out, "\"");
}
	
uint8_t element();

void body() {
	while(!accept(')')) {
		uint8_t a = string();
		uint8_t b = element();
		if(!a && !b) {
			return;
		}
	}
}

uint8_t streql(char *s, char *t) {
	while(1) {
		if(!*s && !*t) {
			return 1;
		}
		if(*s != *t) {
			return 0;
		}
		s++;
		t++;
	}
	return 1;
}

uint8_t isempty(char *e) {
	char *empties[15] = { "area","base","br","col",
						"embed","hr","img","input",
						"keygen","link","meta","param",
						"source","track","wbr"};

	for(uint8_t i = 0; i < 15; i++) {
		if(streql(e, empties[i])) {
			return 1;
		}
	}
	return 0;
}
	
			
	
uint8_t element() {
	if(!accept('(')) {
		return 0;
	}
	fprintf(out, "<");
	char buf[32] = {0};
	stoken(buf);
	if(isempty(buf)) {
		while(next() != ')');
		fprintf(out, ">");
		return 1;
	}
	attrs();
	classes();
	fprintf(out, ">");
	body();
	fprintf(out, "</%s>", buf);
	return 1;
}

size_t xstrlen;
size_t xdot;

void strstat(char *s) {
	size_t i;
	xdot = -1;
	for(i = 0; s[i]; i++) {
		if(s[i] == '.') {
			xdot = i;
		}
	}
	xstrlen = i+1;
	if(xdot == -1) {
		xdot = xstrlen;
	}
}

char *npath(char *p) {
	char *ext = ".html";
	strstat(p);
	char *np = malloc(xstrlen+6);
	memcpy(np, p, xdot);
	memcpy(np+xdot, ext, 6);
	return np;
}

int main(int argc, char ** argv) {
	in = stdin;
	out = stdout;
	if(argc > 1) {
		if (streql("-h", argv[1])) {
			printf("Usage:\tlml\n\tlml <input.lml>\n");
			return 0;
		}
		in = fopen(argv[1], "r");
		out = fopen(npath(argv[1]), "w");
	}
	fprintf(out, "<!DOCTYPE HTML>");
	refresh();
	element();
	fprintf(out, "\n");
	return 0;
}
