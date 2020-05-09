# Card Game

This is a chinese card game to be played on console. Currently it only supports MacOS, but Windows and Linux versions will be released in future updates.

## Installation

For MacOS users, first run the following command in terminal:

```zsh
$ brew install ncurses
```

Then you can open the executable file "card-game" under the build folder. Make sure your terminal is large enough so that the game can be rendered properly. 

Or you can compile your own executable with source files using CMake. CMakeLists.txt is provided.

## How to Play

Use keyboard arrow keys to select buttons or cards, and hit enter to click on what you select. When selecting cards, use ALT + left/right to select multiple cards. 

## Rules

This card game uses a standard 54-card deck, with thirteen cards in four suits plus two jokers, one red and one black. 

There are three players and each player is randomly given 18 cards at the beginning. Players deal cards in a counterclockwise manner around the table, starting from the player who gets heart 3. Please refer to wikipedia [Winner (card game)](https://en.wikipedia.org/wiki/Winner_(card_game)) for specific rules on card combinations. 