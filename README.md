
# About This Project

This is a small demo I made in Unreal Engine using C++ with the goal of improving my Unreal Engine and C++ skills. The focus of the demo is on the enemy lock on targeting camera system which is inspired by the 3D Zelda games.

---

## How it Works

When the targeting input is held, the camera spring arm automatically adjusts its position and length to keep the player and the nearest target in focus, while also allowing the player to control how the camera spring arm rotates. Players can switch to the nearest target by quickly releasing and then holding the targeting input, and if the nearest target is the same as last time, it will target the second nearest target if there is one in range. An additional feature added that is not present in the Zelda games is the ability to switch to the left or right target with a single input. Other important systems developed to support or showcase the targeting system include the player controller, targeting arrow UI, player melee combat system, and enemy controller/AI.

---

## Why This Feature is Important in Games

An enemy targeting camera system is used in games to make combat for controller players much easier, since the player can hold the targeting input (usually left trigger) to aim instead of using the right stick constantly. This frees up their right thumb for other actions with the buttons on the right of the controller. That way the player doesn't have to constantly move their thumb back and forth between the right stick and the buttons. This makes combat easier for players and makes the controller more comfortable to use.

---

## Keyboard and Mouse Controls

Move: WASD

Jump: Space

Attack: Left Mouse Button

Target Enemy: Hold Right Mouse Button

Switch Targets: Q (Left) or E (Right) While in Targeting Mode

Spawn Enemy: R

## Controller Controls

Move: Left Stick

Jump: A (Bottom Button)

Attack: X (Left Button)

Target Enemy: Left Trigger

Switch Targets: D-pad Left or D-pad Right While in Targeting Mode

Spawn Enemy: Y (Top Button)

---

## Where to Play

Download the executable to play the demo yourself here:
[https://eyan11.itch.io/camera-lock-on-targeting-demo](https://eyan11.itch.io/camera-lock-on-targeting-demo)
