use std::cmp;
use cozy_chess;

fn evaluate(position: Board) -> u64 {
	1000 * (
		position.colored_pieces(position.side_to_move(), Piece::Pawn)
		+ 3 * position.colored_pieces(position.side_to_move(), Piece::Knight)
		+ 3 * position.colored_pieces(position.side_to_move(), Piece::Bishop)
		+ 5 * position.colored_pieces(position.side_to_move(), Piece::Rook)
		+ 9 * position.colored_pieces(position.side_to_move(), Piece::Queen)
		- position.colored_pieces(position.side_to_move.not(), Piece::Pawn)
		- 3 * position.colored_pieces(position.side_to_move.not(), Piece::Knight)
		- 3 * position.colored_pieces(position.side_to_move.not(), Piece::Bishop)
		- 5 * position.colored_pieces(position.side_to_move.not(), Piece::Rook)
		- 9 * position.colored_pieces(position.side_to_move.not(), Piece::Queen)
	)
}

fn negamax(node: Board, depth: u64, evaluate: fn(Board)->u64) -> u64 {
	let mate_value = 1000000;
	if (node.status() == GameStatus::Won) {
		-mate_value
	}
	if (node.status() == GameStatus::Drawn) {
		0
	}
	if (depth == 0) {
		evaluate(node)
	}
	let mut evaluation = -2 * mate_value;
	for each child of node {
		evaluation = cmp::max(evaluation, -negamax(child, depth-1, evaluate));
	}
	evaluation
}
