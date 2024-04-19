use cozy_chess;

pub fn show(board: cozy_chess::Board) {
	match board.side_to_move() {
		cozy_chess::Color::White => println!("white to move"),
		cozy_chess::Color::Black => println!("black to move"),
	}

	// FIXME
	// normally i don't add little comments in my code
	// but this is pretty obviously the ABSOLUTELY WRONG way to do things.
	// (code generated programatically.)
	for square in [
		cozy_chess::Square::A8, cozy_chess::Square::B8, cozy_chess::Square::C8, cozy_chess::Square::D8, cozy_chess::Square::E8, cozy_chess::Square::F8, cozy_chess::Square::G8, cozy_chess::Square::H8,
		cozy_chess::Square::A7, cozy_chess::Square::B7, cozy_chess::Square::C7, cozy_chess::Square::D7, cozy_chess::Square::E7, cozy_chess::Square::F7, cozy_chess::Square::G7, cozy_chess::Square::H7,
		cozy_chess::Square::A6, cozy_chess::Square::B6, cozy_chess::Square::C6, cozy_chess::Square::D6, cozy_chess::Square::E6, cozy_chess::Square::F6, cozy_chess::Square::G6, cozy_chess::Square::H6,
		cozy_chess::Square::A5, cozy_chess::Square::B5, cozy_chess::Square::C5, cozy_chess::Square::D5, cozy_chess::Square::E5, cozy_chess::Square::F5, cozy_chess::Square::G5, cozy_chess::Square::H5,
		cozy_chess::Square::A4, cozy_chess::Square::B4, cozy_chess::Square::C4, cozy_chess::Square::D4, cozy_chess::Square::E4, cozy_chess::Square::F4, cozy_chess::Square::G4, cozy_chess::Square::H4,
		cozy_chess::Square::A3, cozy_chess::Square::B3, cozy_chess::Square::C3, cozy_chess::Square::D3, cozy_chess::Square::E3, cozy_chess::Square::F3, cozy_chess::Square::G3, cozy_chess::Square::H3,
		cozy_chess::Square::A2, cozy_chess::Square::B2, cozy_chess::Square::C2, cozy_chess::Square::D2, cozy_chess::Square::E2, cozy_chess::Square::F2, cozy_chess::Square::G2, cozy_chess::Square::H2,
		cozy_chess::Square::A1, cozy_chess::Square::B1, cozy_chess::Square::C1, cozy_chess::Square::D1, cozy_chess::Square::E1, cozy_chess::Square::F1, cozy_chess::Square::G1, cozy_chess::Square::H1,
	].iter() {
		match (board.color_on(*square), board.piece_on(*square)) {
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::King))   => print!("K"),
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::Queen))  => print!("Q"),
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::Rook))   => print!("R"),
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::Bishop)) => print!("B"),
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::Knight)) => print!("N"),
			(Some(cozy_chess::Color::White), Some(cozy_chess::Piece::Pawn))   => print!("P"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::King))   => print!("k"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::Queen))  => print!("q"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::Rook))   => print!("r"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::Bishop)) => print!("b"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::Knight)) => print!("n"),
			(Some(cozy_chess::Color::Black), Some(cozy_chess::Piece::Pawn))   => print!("p"),
			_ => print!("."),
		}
		if square.file() == cozy_chess::File::H {
			print!("\n");
		}
	}
}
