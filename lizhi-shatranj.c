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
//#define NDEBUG

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* function prototypes ----------------------------------------------------------------------------------------------*/


/* non-chess-specific functions -------------------------------------------------------------------------------------*/

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

void print_position(struct position p) {
	fprintf(
		stderr,
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
					fputs("invalid fen", stderr);
					exit(1);
			}
			sq >>= 1;
		} else if (isdigit(fen[i])) {
			sq >>= fen[i] - '0';
		} else if (fen[i] != '/') {
			fputs("invalid fen", stderr);
			exit(1);
		}
	}
	if (fen[i++] != ' ') {
		fputs("invalid fen", stderr);
		exit(1);
	}
	switch (fen[i++]) {
		case 'w': result.mover = 0; break;
		case 'b': result.mover = 1; break;
		default:
			fputs("invalid fen", stderr);
			exit(1);
	}
	if (!(fen[i++] == ' ' && fen[i++] == '-' && fen[i++] == ' ' && fen[i++] == '-' && fen[i++] == ' ')) {
		fputs("invalid fen", stderr);
		exit(1);
	}
	while(isdigit(fen[i])) {
		result.reversible_plies *= 10;
		result.reversible_plies += fen[i++] - '0';
	}
	return result;
}


/* evaluation -------------------------------------------------------------------------------------------------------*/

const int64_t MATE_VALUE 1000000

const int64_t piece_values[] = { MATE_VALUE, 144, 657, 77, 415, 78 };

int64_t eval_material(struct position position) {
	int64_t total = 0;
	for (size_t i=0; i<6; i++) {
		total += (popcount(position.piece[i]) - popcount(position.piece[i+6])) * piece_values[i];
	}
	return total;
}


/* move generation --------------------------------------------------------------------------------------------------*/

const size_t MAX_MOVES = 80;

struct move {
	uint64_t start;
	uint64_t end;
};

uint64_t get_checkers(struct position position) {
}

struct move *get_moves(struct position position) {
	if (popcount(get_checkers(position))) {
		// check movegen
	} else {
		// no-check movegen
	}
}


/* search -----------------------------------------------------------------------------------------------------------*/


/* uci --------------------------------------------------------------------------------------------------------------*/
void debug_interface(void) {
	puts(" * debug interface.");
	puts("printing start position");
	print_position(startpos);

	char input[100];
	struct position position;

	while (1) {
		fputs("fen> ", stdout);
		gets(input);
		position = parsefen(input);
		print_position(position);
		fprintf(stderr, "evaluation: %" PRId64 " cp\n", eval_material(position));
	};
}

void uci(void) {
	puts(
		"uci\n"
		"id name lizhi " LIZHI_VERSION "\n"
		"option name UCI_Variant type combo default shatranj var shatranj\n"
		"uciok\n"
	);
	debug_interface();
	return;
	while (1) {
		
	}
}


/* Main function ----------------------------------------------------------------------------------------------------*/

int main(void) {
	assert(popcount(UINT64_C(0)) == 0);
	assert(popcount(UINT64_MAX) == 64);
	assert(popcount(popcount_c1) == 32);
	assert(popcount(popcount_c2) == 32);
	assert(popcount(popcount_c4) == 32);
	assert(popcount(popcount_c16) == 32);
	assert(popcount(popcount_c32) == 32);
	assert(popcount(UINT64_C(1127298329768902696)) == 31);
	assert(popcount(UINT64_C(12157665459056928801)) == 28);

	struct position test = parsefen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
	for (size_t i=0; i<12; i++) {
		assert(test.piece[i] == startpos.piece[i]);
	}
	assert(test.reversible_plies == startpos.reversible_plies);

	uci();

	return 0;
}
