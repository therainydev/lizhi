fn main() {
	print!(concat!(
		"uci\n",
		"id name lizhi 0.1.0\n",
		"option name UCI_Variant type combo default shatranj var shatranj\n",
		"uciok\n"
	));
}
