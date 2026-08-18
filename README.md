<a name="readme-top"></a>

<!-- GENERAL GAME INFO -->
<br />
<div align="center">

  <h2 align="center">Hotline Miami - The Metro (Prologue)</h2>

  <p align="center">
    Short description of the game.
    <br />
    <strong>Original game : </strong>
    <a href="https://en.wikipedia.org/wiki/Hotline_Miami"><strong>General info »</strong></a>
    ·
    <a href="https://www.youtube.com/watch?v=fJlvMHFsrr4&list=PLTaOHX8kEkD6q6_bo9MwwS0NfwBTu_0Vi&index=1"><strong>Youtube video »</strong></a>
    <br />
    <br />
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#my-version">My version</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
    </li>
    <li><a href="#how-to-play">How To Play</a></li>
    <li><a href="#class-structure">Class structure</a></li>
    <li><a href="#checklist">Checklist</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[Hotline_Miami_screenshot]<img width="1394" height="787" alt="Hotline_Miami_screenshot" src="https://github.com/user-attachments/assets/3cb20b3b-cfe8-4e7c-8471-9ffb3fee6ed8" />



Here's why:
TODO: describe why you chose this game 
* reason 1 I chose this game because it looked very interesting to me.
* reason 2 The NPC enemies and their behaviour make the gameplay exciting.
* reason 3 I also like the camera movement and the top-down view of the game.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## My version

This section gives a clear and detailed overview of which parts of the original game I planned to make.
I am going to recreate a part of the level "The Metro (Prologue)" from Hotline Miami.
The player controls a character from a top-down view and fights with enemies inside a building.
The goal is to kill enemies while navigating through rooms and obstacles.

### The minimum I will most certainly develop:
* Player movement with a top-down camera
* Basic combat mechanics
* NPC enemies
* Different weapon types(at least: bat, knife)
* Level layout with walls and obstacles
* Pickable weapons
  
### What I will probably make as well:
* Different types of enemies with different behaviour
*  I will add simple score popups when enemies are defeated
*  I will implement basic blood effects when enemies are hit (not on the floor)

### What I plan to create if I have enough time left:
* Additional enemy types
* Start menu
* Ranged weapon(pistol)
* Blood effects on the floor
* The player will be able to throw weapons
### Interaction:
* The player will be able to pick up gun from the ground
* The player will be able to finish off npc

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started
Detailed instructions on how to run your game project are in this section.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* Visual Studio 2022

### How to run the project

In Visual Studio 2022 and run the project in Debug or Release mode.
 
<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- HOW TO PLAY -->
## How to play

Use the mouse to point and move in one direction or another.

### Controls
* W, A, S, D - Move the player
* LMB - Attack
* E - Pick up a weapon
* Space - Perform a finisher off on stunned enemy(LMB hit)
* R - Restart Current Level

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CLASS STRUCTURE -->
## Class structure 

### Object composition 
Entity contains HitBox objects for body and combat detection. Player and Enemy also contain Animation objects to manage their animations.
Another example is EnemyNavigator, which contains a PathFinder.

### Inheritance 
Player and Enemy inherit from the Entity base class. Entity provides functionality shared by both classes, such as position, velocity, body and combat hitboxes,
active/alive states, and collision-related behaviour.
Entity also has virtual functions, which are implemented by the derived classes.
I also use the WorldObject base class for objects such as Door, Car, DroppedItem, and Sign.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CHECKLIST -->
## Checklist

- [x] Accept / set up github project
- [ ] week 01 topics applied
    - [ ] const keyword applied proactively (variables, functions,..)
    - [ ] static keyword applied proactively (class variables, static functions,..)
    - [ ] object composition (optional)
- [ ] week 02 topics applied
- [ ] week 03 topics applied
- [ ] week 04 topics applied
- [ ] week 05 topics applied
- [ ] week 06 topics applied
- [ ] week 07 topics applied
- [ ] week 08 topics applied
- [ ] week 09 topics applied (optional)
- [ ] week 10 topics applied (optional)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Hryhorii Bulyhin - hryhorii.bulyhin@student.howest.be

Project Link: [gd14-HryhoriiBulyhin](https://github.com/HowestDAE/gd14-HryhoriiBulyhin.git)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. 

* [Example 1: cpp reference on std::vector](https://en.cppreference.com/w/cpp/container/vector)
* [Spritesheet](https://www.dropbox.com/scl/fo/5h1i7fbnz9oc0gd86meaa/AOcQZMcmvk4GHjQppr6SEag?rlkey=ljao663e3smhpqvz5fmtixdrv&e=2&dl=0)
* [Lab07]
* [JsonExporter](https://github.com/Marcel-Rei/Prog-2-Unity-JSON-Exporter)
* [json.hpp](https://sourceforge.net/projects/json-for-modern-c.mirror/files/v3.11.1/json.hpp/download)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

