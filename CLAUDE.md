# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Dial Pet is a Tamagotchi-style firmware for the M5Stack M5-Dial (ESP32-S3), built with
PlatformIO + Arduino framework. The pet has 0-100 stats (currently `hunger` and
`sleepiness`) that decay over time; the user acts on it by rotating the dial encoder,
pressing/double-pressing the single button, or touching the round screen. Built as a
4-day MVP (see `README.md`). `Dino` is still a scaffolding/placeholder scene, but `Pong`
is now a real single-player-vs-CPU game (dial-controlled paddle, CPU opponent, scoring
tied to happiness) — see the `Pong` note under Architecture.

All actual firmware code lives under `firmware/`. The repo root only has license/readme
files plus empty `assets/` and `docs/` placeholders.

## Commands

Run from the `firmware/` directory (that's where `platformio.ini` lives):

```bash
pio run                 # build
pio run -t upload       # build + flash to the board (port set in platformio.ini: /dev/ttyACM0)
pio device monitor      # serial monitor (115200 baud, exception decoder filter enabled)
pio run -t upload -t monitor   # flash then immediately open the monitor
pio run -t clean        # clean build artifacts (.pio/build)
```

There is no test suite (`firmware/test/` is the empty PlatformIO placeholder) and no
linter configured — validate changes by building and, when possible, flashing to
hardware.

`firmware/playground/encoderTest.cpp` is a scratch file for testing the rotary
encoder/button in isolation; per its header comment, its contents are meant to be
copied into `main.cpp` temporarily, not compiled as part of the normal build.

## Architecture

**Scene-based app loop.** `App` (`include/App.h` / `src/app/app.cpp`) owns one instance
of each `Scene` (`Menu`, `Pet`, `Dino`, `Pong`) and switches between them via
`App::requestScene(SceneId)`. Requests are deferred: `App::loop()` applies a pending
scene switch (calling `end()` on the old scene, `begin()` on the new one, clearing the
display) before delegating to the active scene's `update()`/`draw()`. Scenes never
switch themselves synchronously — they always go through `requestScene`.

Every scene implements the `Scene` interface (`include/Scene.h`): `begin()`/`end()`
lifecycle hooks plus per-frame `update(const Input::InputState&)` and `draw()`. Scenes
gate redraws behind a `redraw_` dirty flag to avoid flickering/needless SPI traffic —
follow that pattern (set `redraw_ = true` on any state change) rather than redrawing
unconditionally every frame.

**`main.cpp`'s `loop()` is a thin driver**: `M5Dial.update()` → `Input::update()` →
`App::loop()`. `App::loop()` itself drives the other singletons in order: `TimeService`
→ `GameState` → current scene. Global input/game/time state is read via each module's
free-function API (`Input::state()`, `GameState::pet()`, `TimeService::synced()`), not
passed around as objects — these are all header-declared namespaces with static state
in the corresponding `.cpp`, effectively process-wide singletons.

**Input** (`include/Input.h` / `src/input/input.cpp`) polls the M5Dial encoder, button
(single/double press), and touch panel once per frame into a single `InputState`
struct. `dialDelta` is the per-frame encoder change; scenes typically accumulate it and
consume it in fixed `step` increments (see `Menu::update`, `Pet::update`) so encoder
sensitivity is easy to retune per scene.

**GameState** (`include/GameState.h` / `src/state/gameState.cpp`) is a millisecond-based
decay simulation: separate time accumulators for hunger, sleepiness, and happiness so
each stat changes at its own fixed interval (`kHungerStepMs`, `kSleepStepMs`,
`kHappinessStepMs`) regardless of loop frequency. Hunger and sleepiness passively rise
over time; happiness passively *falls* over time and is nudged back up (or down) by
scene code — `feed()`/`rest()`/`play()`/`adjustHappiness()` mutate stats directly and
are called from the `Pet` scene (`feed`/`rest`/`play`) and the `Pong` scene
(`adjustHappiness()`, on scoring) in response to user actions. All three stats declared
in `PetState` (`hunger`, `sleepiness`, `happiness`) are implemented.

**TimeService** (`include/TimeService.h` / `src/time/timeService.cpp`) is a
non-blocking state machine (`Idle → WifiConnecting → NtpStarting → WaitingTime → Synced/Failed`)
that connects to WiFi and syncs the RTC via NTP. Credentials come from `include/Secrets.h`
(WIFI_SSID/WIFI_PASS), which is gitignored since is a public repo. A template
(`include/Secrets.h.example`) with placeholder values is commited instead, and a `SessionStart`
hook auto-creates `Secrets.h` from that template if it's missing - but the placeholders
still needs real WiFi credentials before `TimeService` can actually connect and sync.
It's polled every frame from `App::loop()` and never blocks
except for the deliberate one-second spin-wait in `WaitingTime` used to align to the
next whole second before setting the RTC.

**Pong** (`src/scenes/Pong.h` / `src/scenes/pong.cpp`) is a real playable
single-player-vs-CPU game, not a placeholder, and its structure is a useful template
for `Dino` (still a placeholder): a fixed-timestep tick accumulator
(`tickAccumulatorMs_`/`kTickMs`, the same pattern `GameState::tick()` uses for stat
decay) drives physics independently of frame rate, `stepPhysics()` sets
`redraw_ = true` every tick so the scene keeps animating without waiting on input, and
the HUD follows the same layout convention as `Pet` — a title at the same
position/size (`PONG`/`PET`) and a bottom hint line at the same (45, 205) spot.

**Adding a new scene**: create `include/scenes/Foo.h` + `src/scenes/foo.cpp`
implementing `Scene`, add it to `SceneId` in `App.h`, instantiate it as a static member
and add a case in `sceneFromId()` in `app.cpp`, then trigger it via
`App::requestScene(App::SceneId::Foo)` from wherever the user should be able to reach it
(usually `Menu::activateSelected()`).
