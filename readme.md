<div align="center">

# lizhi chess and shatranj engine

![License](https://img.shields.io/github/license/therainydev/lizhi)

![lgbtqia+ friendly](https://pride-badges.pony.workers.dev/static/v1?label=lgbtqia%2B%20friendly&stripeWidth=6&stripeColors=E40303,FF8C00,FFED00,008026,24408E,732982)
![trans rights](https://pride-badges.pony.workers.dev/static/v1?label=trans%20rights&stripeWidth=6&stripeColors=5BCEFA,F5A9B8,FFFFFF,F5A9B8,5BCEFA)

</div>

lizhi (荔枝) is a work-in-progress chess and shatranj engine, now written in Rust.

## A note on building
lizhi benefits extensively from compiler optimizations; as a result, you are advised to build as follows:
`RUSTFLAGS='-C target-cpu=native' cargo build --release`

## Features
- Minimal UCI support.
- Material-counting (1, 3, 3, 5, 7) evaluation.
- Negamax search (no pruning).

## Todo
- Make it play shatranj.
- Support mostly complete UCI.
- Make it stronger. (^-^)
