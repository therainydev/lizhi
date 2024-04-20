use std::str::FromStr;
use cozy_chess;

pub mod search;
pub mod ui;

fn main() {
	print!(concat!(
		"\x1b[0;3;31mlizhi 0.1.0\x1b[0m by \x1b[32mthe\x1b[36mrainy\x1b[34mdev\x1b[0m\n",
		"This program is a text-only chess/shatranj engine, but tries to be nice to work with even without a GUI.\n",
		"If you need to work directly with UCI, enter 'uci'.\n"
	));

	let mut position = cozy_chess::Board::default();

	loop {
		print!("");

		let mut input = String::new();
		if std::io::stdin().read_line(&mut input).expect("failed to read input") == 0 {
			break;
		}

		let mut command = input.split_whitespace();

		match command.next() {
			// outputting constant
			Some("cat")     => println!("ᓚᘏᗢ"),
			Some("isready") => println!("readyok"),
			Some("uci")     => println!(concat!(
					"uci\n",
					"id name lizhi 0.1.0\n",
					"option name UCI_Variant type combo default chess var chess var shatranj\n",
					"uciok"
				)),

			// displaying state
			Some("show") => {
				ui::show(position.clone());
			},
			Some("showfen") => {
				todo!();
			},

			// mutating state
			Some("position") => {
				match command.next() {
					Some("startpos") => {
						position = cozy_chess::Board::default();
						match command.next() {
							Some("moves") => {
								let mut move_str = command.next();
								while move_str != None {
									match cozy_chess::Move::from_str(move_str.unwrap()) {
										Ok(v) => position.play(v),
										Err(_) => println!("\x1b[0;31minvalid move\x1b[0m"),
									}
									move_str = command.next();
								}
							},
							None => (),
							_ => println!("\x1b[0;31minvalid command\x1b[0m"),
						}
					},
					None => {
						println!("\x1b[0;31mno position specified\x1b[0m");
						continue;
					},
					_ => {
						println!("\x1b[0;31minvalid command\x1b[0m");
					},
				}
			},

			// searching
			Some("go") => {
				let (evaluation, best) = search::bestmove(position.clone(), 4, search::evaluate);
				println!("info depth 4 score cp {}", evaluation/10);
				println!("bestmove {}", best);
			},

			// quitting
			Some("quit") => break,
			Some("exit") => break,

			// invalid command
			_ => println!("\x1b[0;31minvalid command\x1b[0m"),
		}
	}
}
