# 3dchess

# What is it?
![Screenshot of UI](https://raw.githubusercontent.com/hacst/chess/master/media/3dchess.png)

3dchess is a basic cross-platform chess application with a 3D GUI and basic AI. It was implemented using modern C++11 features and can be built and run on Windows and Linux with 32bit or 64bit architectures. The goal was to gain experience in architecting a small sized project with multiple participants from the ground up as well as getting an understanding of the approaches used in computer chess.

The end result is a playable chess program allowing player versus player, player vs AI or AI vs AI games. The engine mostly implements FIDE rules with some exceptions. While the AI only implements a limited subset of capabilities needed to compete in modern computer chess play strength is sufficient for human amateur players.

![Sample snippet of debug output](https://raw.githubusercontent.com/hacst/chess/master/media/3dchesslogsample.png)

# Features
The project was split into three big areas of interest and architected to allow as much independent work on each as possible.
## GUI
* 3D GUI using OpenGL 2
* Menu system
* Model loading using assimp
* Text rendering using freetype2
 
<small>Primarily worked on by Patrick Hillert:</small>
## Chess implementation

* FIDE rule based chess engine 
* Bitboard based move generation
* FEN in/output

<small>Primarily worked on by Max Stark</small>
## AI

* Negamax search
* Alpha-beta-pruning
* Basic scoring using material evaluation and piece square tables
* Zobrist hashing
* Polyglot format compatible opening book support
* Transposition tables
* Iterative deepening
* Configurable AI profiles

<small>Primarily worked on by Stefan Hacker</small>
## Build
### Windows
#### Steps
 * Checkout boost SVN into 3rdparty directory
 * Run buildboost.bat in the same directory from a
   Visual Studio 2013 command prompt (this will take
   a while)
 * Add all git submodules (before running runcmake.bat):
     * To setup run 'git submodule init' then 'git submodule update'
 * Run runcmake.bat in the root directory to generate project
   files.
 * Open .sln projectfile in build/ and build all
### Build requirements (Windows):
 * Visual Studio 2013
 * cmake 2.8.12 in path
 * Python 2.7 or later
 * Boost 1.55 (trunk)

### Linux
#### Steps
 * Add all git submodules (before running runcmake.sh):
     * To setup run 'git submodule init' then 'git submodule update'
 * Run runcmake.sh
 * Run make in bin/ subfolder
 
### Build requirements
 * gcc >= 4.8
 * pthreads
 * Python 2.7
 * Boost >= 1.54

#### Ubuntu < 13.10
* GCC >=4.8 from PPA needed: e.g. ppa:ubuntu-toolchain-r/test

#### Ubuntu < 14.04
* Boost >=1.54 from PPA needed: e.g. ppa:boost-latest/ppa

## Authors
In no specific order:
 * Max Stark
 * Patrick Hillert <<silent@gmx.biz>>
 * Stefan Hacker <<mail@hacst.net>>