use std::cmp;
use cozy_chess;

fn negamax(node: Board, depth: u64, evaluate: fn(position)->u64) -> u64 {
	if (depth == 0 /* || terminal node */) {
		evaluate(node)
	}
	let mut evaluation = -2 * mate_value;
	for each child of node {
		evaluation = cmp::max(evaluation, -negamax(child, depth-1, evaluate));
	}
	evaluation
}
