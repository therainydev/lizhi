use std::ops::Not;
use cozy_chess;

const MIN_MATE_VALUE:i32 =  900000;
const MATE_VALUE:i32     = 1000000;

pub fn evaluate(position: cozy_chess::Board) -> i32 {
	1000 * (
		position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Pawn).len() as i32
		+ 3 * position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Knight).len() as i32
		+ 3 * position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Bishop).len() as i32
		+ 5 * position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Rook).len() as i32
		+ 9 * position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Queen).len() as i32
		- position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Pawn).len() as i32
		- 3 * position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Knight).len() as i32
		- 3 * position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Bishop).len() as i32
		- 5 * position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Rook).len() as i32
		- 9 * position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Queen).len() as i32
	)
}

fn negamax(
	node: cozy_chess::Board,
	depth: u64,
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
	if depth == 0 {
		return evaluate(node);
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
				break;
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

pub fn bestmove(node: cozy_chess::Board, depth: u64, evaluate: fn(cozy_chess::Board)->i32) -> (i32, cozy_chess::Move) {
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
