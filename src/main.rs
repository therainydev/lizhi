fn main() {
	print!(concat!(
		"\x1b[0;3;31mlizhi 0.1.0\x1b[0m by \x1b[32mthe\x1b[36mrainy\x1b[34mdev\x1b[0m\n",
		"This program is a text-only shatranj engine, but tries to be nice to work with even without a GUI.\n",
		"If you want to know how to use this program, enter 'help'.\n",
		"If you need to work directly with UCI, enter 'uci'.\n"
	));

	loop {
		print!("");

		let mut input = String::new();
		std::io::stdin().read_line(&mut input).expect("failed to read input");

		match input.trim() {
			"cat" => println!("ᓚᘏᗢ"),
			"isready" => println!("readyok"),
			"quit" => break,
			"uci" =>
				println!(concat!(
					"uci\n",
					"id name lizhi 0.1.0\n",
					"option name UCI_Variant type combo default chess var chess var shatranj\n",
					"uciok"
				)),
			_ => println!("\x1b[0;31minvalid command\x1b[0m"),
		}
	}
}
