use std::ops::Not;
use cozy_chess;

const MATE_VALUE:i64 = 1000000;

pub fn evaluate(position: cozy_chess::Board) -> i64 {
	1000 * (
		i64::from(position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Pawn).len())
		+ 3 * i64::from(position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Knight).len())
		+ 3 * i64::from(position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Bishop).len())
		+ 5 * i64::from(position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Rook).len())
		+ 9 * i64::from(position.colored_pieces(position.side_to_move(), cozy_chess::Piece::Queen).len())
		- i64::from(position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Pawn).len())
		- 3 * i64::from(position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Knight).len())
		- 3 * i64::from(position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Bishop).len())
		- 5 * i64::from(position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Rook).len())
		- 9 * i64::from(position.colored_pieces(position.side_to_move().not(), cozy_chess::Piece::Queen).len())
	)
}

fn negamax(
	node: cozy_chess::Board,
	depth: u64,
	mut alpha: i64,
	beta: i64,
	evaluate: fn(cozy_chess::Board)->i64
) -> i64 {
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
	evaluation
}

pub fn bestmove(node: cozy_chess::Board, depth: u64, evaluate: fn(cozy_chess::Board)->i64) -> (i64, cozy_chess::Move) {
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
