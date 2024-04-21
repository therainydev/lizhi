use cozy_chess;


const MIN_MATE_VALUE:i32 = 10301;
const MATE_VALUE:i32     = 10500;

// note: this is the largest number of moves in a position that can be found from the normal starting position.
//       example with 218 moves: R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 b - - 0 1
//       positions with more moves may not occur in a real chess game and will cause a panic.
//       TODO: refuse such positions.
const MAX_MOVES:usize = 218;

enum MoveType {
	UnderPromotion,
	Quiet,
	Check,
	Capture,
	QueenPromotion
}

type TypeSortedMove = (cozy_chess::Move, MoveType);

enum MoveSortStage {
	QueenPromotions,
	Captures,
	Checks,
	Quiets,
	UnderPromotions,
	Done
}

struct SortedMoveGenerator {
	stage: MoveSortStage,
	queenpromotions: [TypeSortedMove; MAX_MOVES],
	captures       : [TypeSortedMove; MAX_MOVES],
	checks         : [TypeSortedMove; MAX_MOVES],
	quiets         : [TypeSortedMove; MAX_MOVES],
	underpromotions: [TypeSortedMove; MAX_MOVES],
}

impl SortedMoveGenerator {
}

fn negamax(
	node: cozy_chess::Board,
	depth: i64,
	mut alpha: i32,
	beta: i32,
	evaluate: fn(cozy_chess::Board)->i32
) -> i32 {
	if node.status() == cozy_chess::GameStatus::Won {
		return -MATE_VALUE;
	}
	if node.status() == cozy_chess::GameStatus::Drawn {
		return 0;
	}
	if depth <= 0 {
		if node.checkers().len() == 0 {
			return evaluate(node);
		}
	}

	let mut evaluation = -2 * MATE_VALUE;
	node.generate_moves(|moves| {
		for mv in moves {
			let mut new_node = node.clone();
			new_node.play_unchecked(mv);
			let new_evaluation = -negamax(new_node, depth-1, -beta, -alpha, evaluate);
			evaluation = std::cmp::max(evaluation, new_evaluation);
			alpha = std::cmp::max(alpha, new_evaluation);
			if alpha >= beta {
				return true;
			}
		}
		false
	});
	if evaluation >= MIN_MATE_VALUE {
		evaluation - 1
	} else {
		evaluation
	}
}

pub fn bestmove(node: cozy_chess::Board, depth: i64, evaluate: fn(cozy_chess::Board)->i32) -> (i32, cozy_chess::Move) {
	let mut evaluation = -2 * MATE_VALUE;
	let mut best = cozy_chess::Move {from: cozy_chess::Square::A1, to: cozy_chess::Square::A1, promotion: None};
	node.generate_moves(|moves| {
		for mv in moves {
			let mut new_node = node.clone();
			new_node.play_unchecked(mv);
			let node_evaluation = -negamax(new_node, depth-1, -2 * MATE_VALUE, 2 * MATE_VALUE, evaluate);
			if node_evaluation > evaluation {
				evaluation = node_evaluation;
				best = mv;
			}
		}
		false
	});
	(evaluation, best)
}
