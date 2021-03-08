# Nyan Pong Ball

IS1200 Advanced project

Demo video #1: [https://youtu.be/GnXHa3-83lQ](https://youtu.be/GnXHa3-83lQ)

---

![Hardware setup and main menu](https://user-images.githubusercontent.com/54029719/110346726-20ae2000-8030-11eb-8b38-94ccc8a1311b.png)

---

## Harware required

- ChipKIT Uno32
- ChipKIT Basic I/O Shield

Optionally, connect external buttons to 3.3V and pins 37, 36, 34 and 4.

## Environment

Follow the instructions at [is1200-example-projects/mcb32tools/releases/](http://github.com/is1200-example-projects/mcb32tools/releases/).

## Rules and controls

The objective of the game is to get as many points as possible! Set a nice highscore.

For normal gameplay, set all switches to down (away from display).

The game starts off with Pong. Fly up and down with buttons 3 and 2 to hit the ball.

After reaching the required score, the game moves on to Dodgeball.
Avoid getting hit by balls. Jump with button 3, fast fall with button 2, turn around with buttons 1 and 4.
Turning the same direction as you are already facing fires an arrow instead. The arrow kills incoming balls.
You can also jump on balls to kill them.

At the next score threshold, the game continues with Pong, but *faster*.

When the game is over, use the potentiometer and buttons 1 and 4 to set your scoreboard name. Submit with button 2.

## Cheat mode and debugging

If switch 4 is up, *cheat mode* is enabled. This makes you invulnerable. <sup><sup>epic hacks</sup></sup>

If switch 3 is up, you get lots of free points. <sup><sup>legit highscore</sup></sup>

On a positive edge on switch 1, *debug mode* is toggled.
While enabled, use button 1 to step one gametick at a time, displaying the address and value of `int *DEBUG_ADDR`. Set this variable to debug something.

## Contributions

The following files and functions were taken or adapted from the lab files (by Axel Isaksson & F Lundevall):

- `vectors.S`
- `stubs.c`
- `main()`
- `quicksleep()`
- `spi_send_recv()`
- `display_init()`
- `screen_render()`
- `print_debug()`
- `font[]`
- `num32asc()`
- `itoaconv()`

The potentiometer code was taken from [is1200-example-projects/hello-adc](https://github.com/is1200-example-projects/hello-adc/blob/master/main.c) (by Axel Isaksson).

Implementation of xorshift was adapted from [Wikipedia](https://en.wikipedia.org/wiki/Linear-feedback_shift_register).

Everything else made by me.

## TODO

- Fix the "class hierarchy" of `Unit` and `AnimUnit`.
- Make the `screen` buffer more efficient by storing bytes instead of bits.
- Add disclaimers for using cheats.
