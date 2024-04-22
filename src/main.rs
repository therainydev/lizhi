use cozy_chess;
use std::io::IsTerminal;

pub mod evaluate;
pub mod search;
pub mod ui;

const NAME   :&str = env!("CARGO_PKG_NAME");
const VERSION:&str = env!("CARGO_PKG_VERSION");
const AUTHOR :&str = env!("CARGO_PKG_AUTHORS");

fn main() {
	print!(concat!(
		"\x1b[0;3;31m{} {}\x1b[0m by \x1b[32mthe\x1b[36mrainy\x1b[34mdev\x1b[0m\n",
		"This program is a text-only chess/shatranj engine, but tries to be nice to work with even without a GUI.\n",
		"If you need to work directly with UCI, enter 'uci'.\n"
	), NAME, VERSION);

	let mut is_tty = std::io::stdout().is_terminal();
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
					"id name {} {}\n",
					"id author {}\n",
					"option name UCI_Variant type combo default chess var chess var shatranj\n",
					"uciok"
				), NAME, VERSION, AUTHOR),
			Some("ucinewgame") => (),

			// toggling is_tty
			Some("tty") => match command.next() {
				Some("yes") => is_tty = true,
				Some("no")  => is_tty = false,
				_ => println!("info error \x1b[0;31minvalid command\x1b[0m"),
			},

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
									match cozy_chess::util::parse_uci_move(&position, move_str.unwrap()) {
										Ok(v) => position.play(v),
										Err(_) => println!("info error \x1b[0;31minvalid move\x1b[0m"),
									}
									move_str = command.next();
								}
							},
							None => (),
							_ => println!("info error \x1b[0;31minvalid command\x1b[0m"),
						}
					},
					None => {
						println!("info error \x1b[0;31mno position specified\x1b[0m");
						continue;
					},
					_ => {
						println!("info error \x1b[0;31minvalid command\x1b[0m");
					},
				}
			},
			Some("moves") => {
				let mut move_str = command.next();
				while move_str != None {
					match cozy_chess::util::parse_uci_move(&position, move_str.unwrap()) {
						Ok(v) => position.play(v),
						Err(_) => println!("info error \x1b[0;31minvalid move\x1b[0m"),
					}
					move_str = command.next();
				}
			},

			// searching
			Some("go") => {
				let (evaluation, best) = search::bestmove(position.clone(), 6, evaluate::material);
				if is_tty {
					println!("Evaluation: {} centipawns", evaluation);
					println!("Best move : {}", best);
				} else {
					println!("info depth 6 score cp {}", evaluation);
					println!("bestmove {}", best);
				}
			},
			Some("play") => {
				let (evaluation, best) = search::bestmove(position.clone(), 6, evaluate::material);
				position.play_unchecked(best);
				println!("evaluation: {} centipawns", evaluation);
				println!("my move is {}", best);
				ui::show(position.clone());
			},

			// quitting
			Some("quit") => break,
			Some("exit") => break,

			// invalid command
			_ => println!("info error \x1b[0;31minvalid command\x1b[0m"),
		}
	}
}
