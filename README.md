# battleship-tty

Everyone remembers the popular game Battleship. If not, here's a youtube video talking about the game and how to play it: https://www.youtube.com/watch?v=4gHJlYLomrs&t=6s. This is a close-to-faithful rendition of the old battleship game, but with multiple changes and implementations to get it as up with the times as possible. 

## Installation

As of right now, battleship only supports tty mode and only supports linux-based systems. The game is designed to be run in a linux terminal and can be built from source using the following steps.

### Clone the repository

```
git clone https://github.com/KillingJoke42/battleship-tty.git
cd battleship-tty
```

You may also download the .zip file directory from the git repository.

### Build from cloned source

Building is simple: once you fire a linux terminal and cd into the cloned source, simply execute

```
make
```

The executable will be generated in the <src>/exec directory by the name of battleship

### Start executable

The executable can be executed simply from the root directory with the following command

```
./exec/battleship
```

## How to play
Once you execute the program, two options are presented to you
  - Couch multiplayer
  - Local game server
Local game server is yet to be implemented.
Setup a server by following the prompts presented to you
Once you specify number of players in server and names of each player, the game begins
  
### Preparation phase
This phase is where each player gets to place their ships on their maps. There are 5 total ships to place
  - Carrier: 5 blocks long
  - Battleship: 4 blocks long
  - Destroyer: 3 blocks long
  - Submarine: 3 blocks long
  - Patrol Boat: 2 blocks long
  
The Map is a 10x10 grid, with rows denoted by capital letters (A-Z) and columns denoted by digits (0-9)
For obvious reasons, one can only place ships vertically or horizontally.
  
### Firing phase
Once ships are placed, we may start firing. Each player gets a turn at firing and they get to choose which player they wish to attack. They select the cell (row, col) they want to attack and the game responds with a hit or a miss depending on whether the opponent had a ship on the cell or not. Once the shot has been fired, your notes are automatically updated.
  
### Notes
Each player maintains note maps for each of the other players. You may view your notes for any player when it is your turn. The note maps are essentially keeping tracks of all your shots, so you can strategize your next shot. 
  - It is a 10x10 grid
  - Each successful hit marked with an 'X'
  - Each miss is marked with an '-'
  - Cells not yet hit are marked with 'O'

NOTE: If you get a successful hit on an enemy cell, and you shoot again at the same cell, the game prompts a miss, but your notes will still show the cell as a hit.
  
### Endgame
The game ends when one survivor remains and all other players have their players sunk. Note that once you sink an enemy ship completely, the game prompts you with which ship you have sunk.
  
## Current Status
As of 8/6/22, the tty version of battleship-tty supports couch multiplayer free-for-all. Only base game is supported
  
## Future Scope
  - Implement a Local game server network
  - Introduce abilities that can be picked up from random points on the map or maybe chaining shots earns abilities. For example:
    - Move one ship to another location
    - Grant immunity to one cell on the map for one hit
    - Revive a hit cell
    - Reveal enemy positions for a few seconds
    - Get an extra shot
  Any suggestions from people visiting this repo are welcome. Simply raise an issue with the enhancement label with your suggestion and how to implement.
  - Implement more game modes: zen mode, a battle royale mode (maybe? haven't thought this through)
  - Implement server configuration files. Currently server asks admin to set each and every parameter manually via prompts. Would be better to be able to export and     import a config file to load servers faster. Need a parser for that as well.
