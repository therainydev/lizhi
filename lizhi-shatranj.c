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

*/

/* preprocessor stuff -----------------------------------------------------------------------------------------------*/

#define LIZHI_VERSION "prototype"
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

uint64_t popcount_u64_slowmul(uint64_t n) {
	abort();
}

uint64_t popcount_u64_fastmul(uint64_t n) {
	abort();
}

uint64_t popcount_u64_smalltotal(uint64_t n) {
	uint64_t count;
	for (count=0; n; count++) {
		n &= n - 1;
	}
	return count;
}

#define popcount(x) popcount_u64_smalltotal(x)


/* board ------------------------------------------------------------------------------------------------------------*/

enum pieces { WK, WQ, WR, WB, WN, WP, BK, BQ, BR, BB, BN, BP };
const uint64_t piece_values = 

struct position {
	uint64_t piece[12];
	uint64_t en_passant;
	uint8_t reversible_plies;
	uint8_t castlewq;
	uint8_t castlewk;
	uint8_t castlebq;
	uint8_t castlebk;
};

// starting position
// rnbqkbnr xx bbbbbbbb
// pppppppp xx bbbbbbbb
// ******** xx bbbbbbbb
// ******** xx bbbbbbbb
// ******** xx bbbbbbbb
// ******** xx bbbbbbbb
// PPPPPPPP xx bbbbbbbb
// RNBQKBNR xx bbbbbbbb

const struct position startpos = {
	.piece = {
		UINT64_C(0x0000000000000008), // WK
		UINT64_C(0x0000000000000010), // WQ
		UINT64_C(0x0000000000000081), // WR
		UINT64_C(0x0000000000000024), // WB
		UINT64_C(0x0000000000000042), // WN
		UINT64_C(0x000000000000ff00), // WP
		UINT64_C(0x0800000000000000), // BK
		UINT64_C(0x1000000000000000), // BQ
		UINT64_C(0x8100000000000000), // BR
		UINT64_C(0x2400000000000000), // BB
		UINT64_C(0x4200000000000000), // BN
		UINT64_C(0x00ff000000000000)  // BP
	},
	.en_passant = UINT64_C(0),
	.reversible_plies = UINT8_C(0),
	.castlewq = UINT8_C(1),
	.castlewk = UINT8_C(1),
	.castlebq = UINT8_C(1),
	.castlebk = UINT8_C(1)
};

const struct position empty = {
	.piece = {
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000),
		UINT64_C(0x0000000000000000)
	},
	.en_passant = UINT64_C(0),
	.reversible_plies = UINT64_C(0),
	.castlewq = UINT8_C(0),
	.castlewk = UINT8_C(0),
	.castlebq = UINT8_C(0),
	.castlebk = UINT8_C(0)
}

void print_position(struct position p) {
	fprintf(
		stderr,
		"struct position p = {\n"
		"	piece = {\n"
		"		UINT64_C(0x%016" PRIx64 "), // WK\n"
		"		UINT64_C(0x%016" PRIx64 "), // WQ\n"
		"		UINT64_C(0x%016" PRIx64 "), // WR\n"
		"		UINT64_C(0x%016" PRIx64 "), // WB\n"
		"		UINT64_C(0x%016" PRIx64 "), // WN\n"
		"		UINT64_C(0x%016" PRIx64 "), // WP\n"
		"		UINT64_C(0x%016" PRIx64 "), // BK\n"
		"		UINT64_C(0x%016" PRIx64 "), // BQ\n"
		"		UINT64_C(0x%016" PRIx64 "), // BR\n"
		"		UINT64_C(0x%016" PRIx64 "), // BB\n"
		"		UINT64_C(0x%016" PRIx64 "), // BN\n"
		"		UINT64_C(0x%016" PRIx64 "), // BP\n"
		"	};\n"
		"	en_passant = UINT64_C(%" PRIu64 ");\n"
		"	reversible_plies = UINT8_C(%" PRIu8 ");\n"
		"	castlewq = UINT8_C(%" PRIu8 ");\n"
		"	castlewk = UINT8_C(%" PRIu8 ");\n"
		"	castlebq = UINT8_C(%" PRIu8 ");\n"
		"	castlebk = UINT8_C(%" PRIu8 ");\n"
		"};\n",
		p.piece[0], p.piece[1], p.piece[2], p.piece[3], p.piece[4], p.piece[5],
		p.piece[6], p.piece[7], p.piece[8], p.piece[9], p.piece[10], p.piece[11],
		p.en_passant, p.reversible_plies, p.castlewq, p.castlewk, p.castlebq, p.castlebk
	);
}

struct position parsefen(char *fen) {
	uint64_t sq = UINT64_C(0x8000000000000000)
	struct position result;
	result.piece = empty.piece;
	size_t i = 0;
	while (sq) {
		if (isalpha(fen[i])) {
			switch (fen[i]) {
				case 'K': result.piece[WK] |= sq; break;
				case 'Q': result.piece[WQ] |= sq; break;
				case 'R': result.piece[WR] |= sq; break;
				case 'B': result.piece[WB] |= sq; break;
				case 'N': result.piece[WN] |= sq; break;
				case 'P': result.piece[WP] |= sq; break;
				case 'k': result.piece[BK] |= sq; break;
				case 'q': result.piece[BQ] |= sq; break;
				case 'r': result.piece[BR] |= sq; break;
				case 'b': result.piece[BB] |= sq; break;
				case 'n': result.piece[BN] |= sq; break;
				case 'p': result.piece[BP] |= sq; break;
				default:
					// something's wrong
			}
			sq >>= 1;
		} else if (isdigit(fen[i])) {
			sq >>= fen[i] - '0';
		} else if (fen[i] != '/') {
			// something's wrong
		}
	}
	// parse turn, en passant square, castling, halfmove clock
	return position;
}

char *to_fen(struct position p) {
	char *fen = malloc(90*sizeof(char));
	size_t char_to_modify = 0;
	
	return fen;
}

struct position move(struct position p, uint64_t start_square, uint64_t end_square) {
	
}

/* evaluation -------------------------------------------------------------------------------------------------------*/

const int64_t piece_values[] = { 1000000, 900, 500, 300, 300, 100 };

int64_t eval_material(struct position position) {
	int64_t eval = 0;
	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			// add material value of piece on position.board[i][j]
		}
	}
}

/* search -----------------------------------------------------------------------------------------------------------*/

/* uci --------------------------------------------------------------------------------------------------------------*/
void debug_interface(void) {
	puts("lizhi-engine " LIZHI_VERSION " debug interface");
	puts("printing start position");
	print_position(startpos);
}

void uci(void) {
	puts(
		"uci\n"
		"id name lizhi-shatranj prototype\n"
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
	#ifndef NDEBUG
		fputs("debug enabled - running assertions\n", stderr);
	#endif
	assert(popcount(UINT64_C(0)) == 0);
	assert(popcount(UINT64_MAX) == 64);
	assert(popcount(popcount_c1) == 32);
	assert(popcount(popcount_c2) == 32);
	assert(popcount(popcount_c4) == 32);
	assert(popcount(popcount_c16) == 32);
	assert(popcount(popcount_c32) == 32);
	assert(popcount(UINT64_C(1127298329768902696)) == 31);
	assert(popcount(UINT64_C(12157665459056928801)) == 28);

	assert(parsefen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") == startpos);

	uci();

	return 0;
}
