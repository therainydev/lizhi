/* introduction -------------------------------------------------------------------------------------------------------

lizhi shatranj engine by rainy

you can contact me at: therainy.dev@tutanota.de

-----------------------------------------------------------------------------------------------------------------------

copyright (c) 2024 rainy (mit license)

permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files ("the software"), to deal in the software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the software, and to permit
persons to whom the software is furnished to do so, subject to the following conditions:

the above copyright notice and this permission notice shall be included in all copies or substantial portions of the
software.

the software is provided "as is", without warranty of any kind, express or implied, including but not limited to the
warranties of merchantability, fitness for a particular purpose, and noninfringement. in no event shall the authors or
copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or
otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.

ᓚᘏᗢ

*/


/* preprocessor stuff -----------------------------------------------------------------------------------------------*/

#define LIZHI_VERSION "shatranj prototype"

//#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* non-shatranj-specific functions ----------------------------------------------------------------------------------*/

const uint64_t
	popcount_c1  = UINT64_C(0x5555555555555555),
	popcount_c2  = UINT64_C(0x3333333333333333),
	popcount_c4  = UINT64_C(0x0f0f0f0f0f0f0f0f),
	popcount_c8  = UINT64_C(0x00ff00ff00ff00ff),
	popcount_c16 = UINT64_C(0x0000ffff0000ffff),
	popcount_c32 = UINT64_C(0x00000000ffffffff),
	popcount_m   = UINT64_C(0x0101010101010101);

uint64_t popcount(uint64_t n) {
	size_t count;
	for (count=0; n; count++) {
		n &= n - 1;
	}
	return count;
}


/* board ------------------------------------------------------------------------------------------------------------*/

enum pieces { WK, WF, WR, WA, WN, WP, BK, BF, BR, BA, BN, BP };

struct position {
	uint64_t piece[12];
	uint8_t reversible_plies;
	int8_t mover;
};

const struct position startpos = {
	.piece = {
		UINT64_C(0x0000000000000008), // WK
		UINT64_C(0x0000000000000010), // WF
		UINT64_C(0x0000000000000081), // WR
		UINT64_C(0x0000000000000024), // WA
		UINT64_C(0x0000000000000042), // WN
		UINT64_C(0x000000000000ff00), // WP
		UINT64_C(0x0800000000000000), // BK
		UINT64_C(0x1000000000000000), // BF
		UINT64_C(0x8100000000000000), // BR
		UINT64_C(0x2400000000000000), // BA
		UINT64_C(0x4200000000000000), // BN
		UINT64_C(0x00ff000000000000)  // BP
	},
	.reversible_plies = UINT8_C(0),
	.mover = 0
};

const struct position empty = {
	.piece = { 0 },
	.reversible_plies = UINT64_C(0),
	.mover = 0
};

void print_bitboard(uint64_t bitboard, FILE *stream) {
	fprintf(stream, "bitboard = 0x%016" PRIx64 "\n", bitboard);
	size_t i = 64;
	while (i --> 0) {
		putchar(bitboard >> i & 1 ? '*':'.');
		if(!(i & 7)) {
			putchar('\n');
		}
	}
}

void print_position(struct position p, FILE *stream) {
	fprintf(
		stream,
		"struct position p = {\n"
		"	piece = {\n"
		"		UINT64_C(0x%016" PRIx64 "), // WK\n"
		"		UINT64_C(0x%016" PRIx64 "), // WF\n"
		"		UINT64_C(0x%016" PRIx64 "), // WR\n"
		"		UINT64_C(0x%016" PRIx64 "), // WA\n"
		"		UINT64_C(0x%016" PRIx64 "), // WN\n"
		"		UINT64_C(0x%016" PRIx64 "), // WP\n"
		"		UINT64_C(0x%016" PRIx64 "), // BK\n"
		"		UINT64_C(0x%016" PRIx64 "), // BF\n"
		"		UINT64_C(0x%016" PRIx64 "), // BR\n"
		"		UINT64_C(0x%016" PRIx64 "), // BA\n"
		"		UINT64_C(0x%016" PRIx64 "), // BN\n"
		"		UINT64_C(0x%016" PRIx64 "), // BP\n"
		"	};\n"
		"	reversible_plies = UINT8_C(%" PRIu8 ");\n"
		"};\n",
		p.piece[0], p.piece[1], p.piece[2], p.piece[3], p.piece[4], p.piece[5],
		p.piece[6], p.piece[7], p.piece[8], p.piece[9], p.piece[10], p.piece[11],
		p.reversible_plies
	);
}

struct position parsefen(char *fen) {
	uint64_t sq = UINT64_C(0x8000000000000000);
	struct position result;
	size_t i;
	for (i=0; i<12; i++) {
		result.piece[i] = empty.piece[i];
	}
	result.reversible_plies = empty.reversible_plies;
	for (i=0; sq; i++) {
		if (isalpha(fen[i])) {
			switch (fen[i]) {
				case 'K': result.piece[WK] |= sq; break;
				case 'Q': result.piece[WF] |= sq; break;
				case 'R': result.piece[WR] |= sq; break;
				case 'B': result.piece[WA] |= sq; break;
				case 'N': result.piece[WN] |= sq; break;
				case 'P': result.piece[WP] |= sq; break;
				case 'k': result.piece[BK] |= sq; break;
				case 'q': result.piece[BF] |= sq; break;
				case 'r': result.piece[BR] |= sq; break;
				case 'b': result.piece[BA] |= sq; break;
				case 'n': result.piece[BN] |= sq; break;
				case 'p': result.piece[BP] |= sq; break;
				default:
					return empty;
			}
			sq >>= 1;
		} else if (isdigit(fen[i])) {
			sq >>= fen[i] - '0';
		} else if (fen[i] != '/') {
			return empty;
		}
	}
	if (fen[i++] != ' ') {
		return empty;
	}
	switch (fen[i++]) {
		case 'w': result.mover = 0; break;
		case 'b': result.mover = 1; break;
		default:
			return empty;
	}
	if (!(fen[i++] == ' ' && fen[i++] == '-' && fen[i++] == ' ' && fen[i++] == '-' && fen[i++] == ' ')) {
		return empty;
	}
	while(isdigit(fen[i])) {
		result.reversible_plies *= 10;
		result.reversible_plies += fen[i++] - '0';
	}
	return result;
}


/* evaluation -------------------------------------------------------------------------------------------------------*/

const int64_t MATE_VALUE = INT64_C(1000000);

const int64_t piece_values[] = { MATE_VALUE, 144, 657, 77, 415, 78 };

int64_t eval_material(struct position position) {
	int64_t total = 0;
	for (size_t i=0; i<6; i++) {
		total += (popcount(position.piece[i]) - popcount(position.piece[i+6])) * piece_values[i];
	}
	return total;
}


/* move generation --------------------------------------------------------------------------------------------------*/

const uint64_t A_FILE = UINT64_C(0x8080808080808080);
const uint64_t B_FILE = UINT64_C(0x4040404040404040);
const uint64_t G_FILE = UINT64_C(0x0202020202020202);
const uint64_t H_FILE = UINT64_C(0x0101010101010101);

const uint64_t A_OR_B_FILE = A_FILE | B_FILE;
const uint64_t G_OR_H_FILE = G_FILE | H_FILE;

const uint64_t CENTER_4_FILES = UINT64_C(0x3c3c3c3c3c3c3c3c);
const uint64_t EDGE_FILES     = A_FILE | H_FILE;

const size_t MAX_MOVES = 80;

struct move {
	uint64_t start;
	uint64_t end;
};

uint64_t get_ferz_attacks(uint64_t ferz) {
	uint64_t ferz_a = ferz & A_FILE;
	ferz = ferz ^ ferz_a;
	uint64_t ferz_h = ferz & H_FILE;
	ferz = ferz ^ ferz_h;
	return
		ferz_a << 7 | ferz_a >> 9 |
		ferz_h << 9 | ferz_h >> 7 |
		ferz << 9 | ferz << 7 | ferz >> 7 | ferz >> 9;
}

uint64_t get_rook_attacks(uint64_t rook, uint64_t obstructions) {
	uint64_t up, left = 0, right = 0, down, last = 0;
	switch (popcount(rook)) {
		case 0:
			return 0;
		case 1:
			up    = rook >> 8;
			down  = rook << 8;
			while (up && !(up & obstructions) && up != last) {
				last = up;
				up |= up >> 8;
			}
			if (!(rook & H_FILE)) {
				right = rook >> 1;
				while (right && !(right & (obstructions | EDGE_FILES)) && right != last) {
					last = right;
					right |= right >> 1;
				}
			}
			if (!(rook & A_FILE)) {
				left = rook << 1;
				while (left && !(left & (obstructions | EDGE_FILES)) && left != last) {
					last = left;
					left |= left << 1;
				}
			}
			while (down && !(down & obstructions) && down != last) {
				last = down;
				down |= down << 8;
			}
			return up | left | right | down;
		case 2:
			return get_rook_attacks(rook&(rook-1), obstructions) | get_rook_attacks(rook&(~rook+1), obstructions);
		default:
			abort();
	}
}

uint64_t get_alfil_attacks(uint64_t alfil) {
	uint64_t alfil_ab = alfil & A_OR_B_FILE;
	alfil = alfil ^ alfil_ab;
	uint64_t alfil_gh = alfil & G_OR_H_FILE;
	alfil = alfil ^ alfil_gh;
	return
		alfil_ab << 14 | alfil_ab >> 18 |
		alfil_gh << 18 | alfil_gh >> 14 |
		alfil << 18 | alfil << 14 | alfil >> 14 | alfil >> 18;
}

uint64_t get_knight_attacks(uint64_t knight) {
	uint64_t
		knight_a = knight & A_FILE,
		knight_b = knight & B_FILE,
		knight_g = knight & G_FILE,
		knight_h = knight & H_FILE;
	knight &= CENTER_4_FILES;
	return
		knight_a << 15 | knight_a <<  6 | knight_a >> 10 | knight_a >> 17 |
		knight_h << 17 | knight_h << 10 | knight_h >>  6 | knight_h >> 15 |
		knight_b << 17 | knight_b << 15 | knight_b <<  6 | knight_b >> 10 | knight_b >> 15 | knight_b >> 17 |
		knight_g << 17 | knight_g << 15 | knight_g << 10 | knight_g >>  6 | knight_g >> 15 | knight_g >> 17 |
		knight << 17 | knight << 15 | knight << 10 | knight <<  6 |
		knight >>  6 | knight >> 10 | knight >> 15 | knight >> 17;
}

uint64_t get_pawn_attacks(uint64_t pawn, int8_t mover) {
	uint64_t pawn_a = pawn & A_FILE;
	pawn = pawn ^ pawn_a;
	uint64_t pawn_h = pawn & H_FILE;
	pawn = pawn ^ pawn_h;
	return mover
		? pawn_a >> 9 | pawn_h >> 7 | pawn >> 9 | pawn >> 7
		: pawn_a << 7 | pawn_h << 9 | pawn << 7 | pawn << 9;
}

uint64_t checked(struct position position) {
	uint64_t obstructions = 0;
	for (size_t i=0; i<12; i++) {
		obstructions |= position.piece[i];
	}
	if ((
		get_ferz_attacks(position.piece[WF+6*!position.mover]) |
		get_rook_attacks(position.piece[WR+6*!position.mover], obstructions) |
		get_alfil_attacks(position.piece[WA+6*!position.mover]) |
		get_knight_attacks(position.piece[WN+6*!position.mover]) |
		get_pawn_attacks(position.piece[WP+6*!position.mover], !position.mover)
	) & position.piece[WK]) {
		return 1;
	} else {
		return 0;
	}
}

struct move *get_moves(struct position position) {
	if (checked(position)) {
		// check movegen
	} else {
		// no-check movegen
	}
}


/* search -----------------------------------------------------------------------------------------------------------*/


/* self-testing */

void fail_test(size_t line) {
	fprintf(stderr, "self-test \33[0;1;31mfailed\33[0m @ line \33[0;1;31m%zd\33[0m!\n", line);
	abort();
}

#define TEST(x) if (!(x)) fail_test(__LINE__)

void self_test(void) {
	puts("running self-tests");

	TEST(popcount(UINT64_C(0)) == 0);
	TEST(popcount(UINT64_MAX) == 64);
	TEST(popcount(popcount_c1) == 32);
	TEST(popcount(popcount_c2) == 32);
	TEST(popcount(popcount_c4) == 32);
	TEST(popcount(popcount_c16) == 32);
	TEST(popcount(popcount_c32) == 32);
	TEST(popcount(UINT64_C(1127298329768902696)) == 31);
	TEST(popcount(UINT64_C(12157665459056928801)) == 28);

	struct position test;
	uint64_t obstructions;

	test = parsefen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
	for (size_t i=0; i<12; i++) {
		TEST(test.piece[i] == startpos.piece[i]);
	}
	TEST(test.reversible_plies == startpos.reversible_plies);
	TEST(test.mover == startpos.mover);

	test = parsefen("nr6/p1p1p1p1/P1P1P1PR/1B1q1N2/2bp1bkN/Bp1PQprp/RP3P1P/1n5K w - - 34 1");
	obstructions = 0;
	for (size_t i=0; i<12; i++) {
		obstructions |= test.piece[i];
	}
	TEST(test.reversible_plies == 34);
	TEST(test.mover == 0);
	TEST(get_ferz_attacks(test.piece[WF]) == 0x0000000014001400);
	TEST(get_ferz_attacks(test.piece[BF]) == 0x0000280028000000);
	TEST(get_rook_attacks(test.piece[WR], obstructions) == 0x0101020101804080);
	TEST(get_rook_attacks(test.piece[BR], obstructions) == 0xbf40404002050202);
	TEST(get_alfil_attacks(test.piece[WA]) == 0x0010002000100020);
	TEST(get_alfil_attacks(test.piece[BA]) == 0x0000990000009900);
	TEST(get_knight_attacks(test.piece[WN]) == 0x000a1304110e0200);
	TEST(get_knight_attacks(test.piece[BN]) == 0x0020400000a01000);
	TEST(get_pawn_attacks(test.piece[WP], 0) == 0x0055000028aa0000);
	TEST(get_pawn_attacks(test.piece[BP], 1) == 0x000055000028aa00);

	test = parsefen("rnbQKbnr/8/8/pppppppp/PPPPPPPP/8/8/RNBqkBNR b - - 26 1");
	TEST(test.reversible_plies == 26);
	TEST(test.mover == 1);

	puts("\33[0;32mall self-tests passed\33[0m");
}


/* main function & user interface -----------------------------------------------------------------------------------*/

const size_t UCI_INPUT_SIZE = 10000;

int main(void) {
	fputs(
		"\33[0;3;31mlizhi " LIZHI_VERSION "\33[0m by \33[32mthe\33[36mrainy\33[34mdev\33[0m\n"
		"This program is a text-only shatranj engine, but tries to be nice to work with even without a GUI.\n"
		"If you want to know how to use this program, enter 'help'.\n"
		"If you need to work directly with UCI, enter 'uci'.\n",
		stdout
	);

	struct position position = startpos;
	char *orig, *input, *token;
	size_t i;

	while(1) {
		orig = input = malloc(UCI_INPUT_SIZE*sizeof(char));
		if (orig == NULL) {
			fputs("failed to allocate memory for reading input\n", stderr);
			exit(1);
		}
		fputs(" \33[3;31mlizhi \33[35m-> \33[34m", stdout);
		if (fgets(orig, UCI_INPUT_SIZE, stdin) == NULL) {
			fputs("\n\33[0;1mbye\33[0m\n", stdout);
			exit(0);
		}
		fputs("\33[0m", stdout);

		token = strsep(&input, " \n\t");
		if (token == NULL) {
			fputs("please enter a valid command\n", stdout);
			free(orig);
			continue;
		}

		if (!strcmp(token, "quit")) {
			fputs("\33[1mbye\33[0m\n", stdout);
			exit(0);
		}

		else if (!strcmp(token, "cat")) {
			fputs("ᓚᘏᗢ\n", stdout);
		}

		else if (!strcmp(token, "help")) {
			fputs(
				"\33[1;4;32mcommands\33[0m\n"
				" \33[1;32mquit\33[0m   - exit the engine\n"
				" \33[1;32mcat\33[0m    - print a cat\n"
				" \33[1;32mhelp\33[0m   - print this\n"
				" \33[1;32muci\33[0m    - enter UCI mode\n"
				" \33[1;34mprint\33[0m  - print the current position\n"
				" \33[1;34mfen\33[0m    - parse fen into current position\n"
				" \33[1;35mtest\33[0m   - run self-tests\n"
				" \33[1;35mferz\33[0m   - print ferz attacks\n"
				" \33[1;35mrook\33[0m   - print rook attacks\n"
				" \33[1;35malfil\33[0m  - print alfil attacks\n"
				" \33[1;35mknight\33[0m - print knight attacks\n"
				" \33[1;35mpawn\33[0m   - print pawn attacks\n",
				stdout
			);
		}

		else if (!strcmp(token, "uci")) {
			goto UCI;
		}

		else if (!strcmp(token, "print")) {
			print_position(position, stdout);
		}

		else if (!strcmp(token, "fen")) {
			position = parsefen(input);
		}

		else if (!strcmp(token, "test")) {
			self_test();
		}

		else if (!strcmp(token, "ferz")) {
			print_bitboard(get_ferz_attacks(position.piece[WF+6*position.mover]), stdout);
		}

		else if (!strcmp(token, "rook")) {
			uint64_t obstructions = 0;
			for (size_t i=0; i<12; i++) {
				obstructions |= position.piece[i];
			}
			print_bitboard(get_rook_attacks(position.piece[WR+6*position.mover], obstructions), stdout);
		}

		else if (!strcmp(token, "alfil")) {
			print_bitboard(get_alfil_attacks(position.piece[WA+6*position.mover]), stdout);
		}

		else if (!strcmp(token, "knight")) {
			print_bitboard(get_knight_attacks(position.piece[WN+6*position.mover]), stdout);
		}

		else if (!strcmp(token, "pawn")) {
			print_bitboard(get_pawn_attacks(position.piece[WP+6*position.mover], position.mover), stdout);
		}

		else {
			puts("please enter a valid command");
		}

		free(orig);
	}

	UCI:
	fputs(
		"uci\n"
		"id name lizhi " LIZHI_VERSION "\n"
		"option name UCI_Variant type combo default shatranj var shatranj\n"
		"uciok\n",
		stdout
	);
	exit(1);
	while(1) {

	}
}
