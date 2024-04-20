use std::ops::Not;
use cozy_chess;

pub fn material(position: cozy_chess::Board) -> i32 {
	100 * (
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
