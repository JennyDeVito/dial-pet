# Dial Pet

Dial Pet is a tamagochi developed for a M5-Stack M5-Dial ESP32-S3 Stamp Controller.

## Goal

The goal was to test the functionalities, to train programming skills and to develop a
MVP.

## The project

Dial Pet will function as a tamagochi, having 3 0-to-100 status: hunger, sleep and happiness.
Those status will decay over time. User can perform three actions: eat, sleep, and play.
Eating restores hunger and consumes energy, playing improves happiness consumes energy, and
will be possible to play two games: Dino Game and Pong.

## Setup

`include/Secrets.h` holds WiFi credentials (`WIFI_SSID`/`WIFI_PASS`) and is gitignored
since this is a public repo. `include/Secrets.h.example` is the committed template - copy
it to `include/Secrets.h` and fill in your own credentials before building.

## License and credits

This project is licenced under _Apache License 2.0_ - read the file [LICENSE](./LICENSE)

### What you can do

Under this license you can:

- use the code
- modify it
- redistribute it
- use it commercial projects

### This license requires you to

When using or redistributing this code, you must:

- keep the copyright and the Apache 2.0 notices
- point out relevant modifications you made to the code
- keep the [NOTICE](./NOTICE) file

### Credits

When using this project - specially in something public - give proper credit (this isn't demand beyond the Apache 2.0 license)

- **Project:** dial-pet
- **Author:** Jenny DeVito
- **License:** Apache 2.0

This project is developed with AI assistance (Claude Code).

---

[![wakatime](https://wakatime.com/badge/user/1d9ba0a2-6c8a-4dcd-99dd-46aae04b90e0/project/6324e7c0-1b25-49c5-a442-d1b3531a40e2.svg)](https://wakatime.com/badge/user/1d9ba0a2-6c8a-4dcd-99dd-46aae04b90e0/project/6324e7c0-1b25-49c5-a442-d1b3531a40e2)
