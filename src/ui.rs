use cozy_chess;

pub fn show(board: cozy_chess::Board) {
	match board.side_to_move() {
		cozy_chess::Color::White => println!("white to move"),
		cozy_chess::Color::Black => println!("black to move"),
	}

	for rank in cozy_chess::Rank::ALL.iter().rev() {
		for file in cozy_chess::File::ALL.iter() {
			let square = cozy_chess::Square::new(*file, *rank);
			let mut to_print = board.piece_on(square).map(|p| p.into()).unwrap_or('.');
			if board.color_on(square) == Some(cozy_chess::Color::White) {
				to_print = to_print.to_ascii_uppercase();
			}
			print!("{}", to_print);
		}
		print!("\n");
	}
}
