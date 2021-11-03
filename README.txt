#gobang.cpp

A program that plays gobang with the user using terminal. Outputs board after the opponent makes a move. User can specify board size and color. 
User must type in desired move to place piece with the format (charater)(number). Ex: a1 to place in board space (1,1). 

Use: 'make' to generate gobang.exe file. 
    use -n flag followed by a number to specify board size (nxn). Default board size is 11x11
    use -l flag to play as light(O). Default is playing as black(X). Black(X) goes first.
    Example use to play a 10x10 board as second player: ./gobang.exe -n 10 -l 