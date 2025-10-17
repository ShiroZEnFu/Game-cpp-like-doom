Console FPS - 3D Shooter in the Console

A simple first-person 3D shooter that runs directly in the Windows command line. This project is a demonstration of the Ray Casting algorithm to create a 3D perspective in a 2D space using only ASCII characters.

📜 Description

This is a classic example of a pseudo-3D engine, similar to those used in early games like Wolfenstein 3D. The engine "casts" rays from the player's camera to determine the distance to walls and then renders vertical stripes on the screen whose height depends on this distance. This creates the illusion of depth and a three-dimensional space.

All rendering happens in a standard Windows console, and ASCII characters are used for the "graphics".

✨ Features

Pseudo-3D graphics in the console using C++.

Ray Casting algorithm for rendering.

Real-time player movement and rotation.

Collision detection with walls.

A mini-map in the corner of the screen for navigation.

An FPS counter to track performance.

🛠️ Building and Running

The project is written in C++ and uses the WinAPI for console operations, so it is intended to be run on Windows.

Requirements:

A C++ compiler (e.g., MinGW/g++ or Visual Studio).

Building with g++:

Open a terminal (Command Prompt, PowerShell, or Git Bash).

Navigate to the folder containing the main.cpp file.

Execute the compilation command:

g++ main.cpp -o ConsoleFPS.exe


Run the compiled file:

./ConsoleFPS.exe


Building with Visual Studio:

Create a new empty "Windows Console Application" project.

Copy the code from main.cpp into your project's main file.

Compile and run the project (F5 key).

🎮 Controls

W - Move forward

S - Move backward

A - Turn left

D - Turn right

🏗️ Code Structure (OOP)

The code has been refactored using an object-oriented approach for better readability and scalability.

Player Class: Manages the player's state, including coordinates (x, y) and viewing angle (angle). Contains methods for movement and rotation.

Map Class: Stores the level map as a string. Provides methods for checking collisions (e.g., whether a given cell is a wall).

Game Class: The main class that ties everything together. It contains the game loop, handles user input, manages rendering, and holds the Player and Map objects.

This structure makes the code more organized and allows for new functionality to be added easily.

🚀 Potential Improvements

[ ] Add "textures" for walls (different characters for different wall types).

[ ] Implement loading maps from text files.

[ ] Add simple enemies or other interactive objects.

[ ] Improve the rendering of the floor and ceiling.

[ ] Add sound effects.
