#include "gobang.h"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]){   
    Gobang g;
    g.initialize(argc,argv);
}

void Gobang::initialize(int num, char *input[]){
    //reads in input of main
    for(int i = 0; i < num; i++){
        if(std::strcmp(input[i], "-n") == 0){
            boardSize = std::atoi(input[i+1]);
        }else if(std::strcmp(input[i], "-l") == 0){
            isLight = true;   
        }
    }
    //Initialize the board to 0
    board.resize(boardSize);
    for(int i = 0; i < boardSize; i++){
        board[i].resize(boardSize);
        for(int j = 0; j < boardSize; j++){
            board[i][j] = 0;
        }
    }

    //Initialize Eval Structures
    oneBlock.insert({0, 0});
    oneBlock.insert({1, 10});
    oneBlock.insert({2, 20});
    oneBlock.insert({3, 30});
    oneBlock.insert({4, 1000});
    oneBlock.insert({5, 10000000});

    noBlocks.insert({0, 0});
    noBlocks.insert({1, 100});
    noBlocks.insert({2, 200});
    noBlocks.insert({3, 300});
    noBlocks.insert({4, 5000});
    noBlocks.insert({5, 10000000});

    //Go first if first. Read input if last
    if(!isLight){
        IOcontroller();
    }else{
        //start first and places the piece at the middle of the board. 
        if(boardSize % 2 == 0){
            consider.insert(std::pair<int, int>((boardSize/2),(boardSize/2)-1));
            std::cout << "program's move\nMove played: " << convertChar(boardSize/2) << boardSize/2 << std::endl;
            curBoardScore = calcTotScore(convertChar(boardSize/2) + std::to_string(boardSize/2), 1);
            board[(boardSize/2)][(boardSize/2)-1] = 1;
        }else{ //odd number insertion
            consider.insert(std::pair<int, int>((boardSize/2) + 1,boardSize/2));
            std::cout << "program's move\nMove played: " << convertChar((boardSize/2) + 1) << (boardSize/2) + 1 << std::endl;
            curBoardScore = calcTotScore(convertChar((boardSize/2) + 1) + std::to_string((boardSize/2) + 1), 1);
            board[(boardSize/2) + 1][(boardSize/2)] = 1;
        }
        printBoard();
        IOcontroller();
    }
}

void Gobang::IOcontroller(){
    std::string move;
    bool notMoved = true;

    while(!gameOver){
        if(std::abs(curBoardScore) > 1000000){
            gameOver = true;
            if(curBoardScore > 0)
                std::cout << "You lose" << std::endl;
            else
                std::cout << "You win" << std::endl;
            break;
        }
        if(moveCount+1 >= boardSize*boardSize){
            gameOver = true;
            std::cout << "Tie" << std::endl; 
            break;
        }

        //read opponent's move
        std::cout << "Your Turn! Type in format (character)(number). Ex: a1 " << std::endl;
        std::cin >> move;
        while(notMoved){
            if(isValid(move)){
                std::cout << "opponent's move\nMove played: " << move << std::endl;
                curBoardScore = calcTotScore(move, -1);
                consider.insert(std::pair<int, int>(convertInt(move[0]), std::stoi(move.substr(1))-1));
                board[convertInt(move[0])][std::stoi(move.substr(1))-1] = -1;
                notMoved = false;
            }else{
                std::cout << "please enter a valid move" << std::endl;
                std::cin >> move;
            }
        }
        moveCount++;
        notMoved = true;
        //decide own based off opponent's move
        decideMove();
        moveCount++; 
        
        //output move
        consider.insert(decidedMove);
        std::cout << "program's move\nMove played: " << convertChar(decidedMove.first) << decidedMove.second + 1 << std::endl;
        curBoardScore = calcTotScore(convertChar(decidedMove.first) + std::to_string(decidedMove.second + 1), 1);

        board[decidedMove.first][decidedMove.second] = 1;

        printBoard();
        //printConsider();
    }
}

void Gobang::printBoard(){
    std::cout << "    ";
    for(int i = 0; i < boardSize; i++){
        std::cout << convertChar(i) << "  ";
    }
    std::cout << "\n";
    std::cout << "  ";
    for(int i = 0; i < boardSize; i++){
        std::cout << "---";
    }
    std::cout << "-\n";
    for(int i = 0; i < boardSize;i++){
        if(i+1 < 10){
            std::cout << i + 1 << " |";
        }else{
            std::cout << i + 1 << "|";
        }
        for(int j = 0; j < boardSize; j++){
            if(board[j][i] == 0){
                std::cout << " " << "-" << " ";
            }else if(board[j][i] == 1){
                std::cout << " " << "O" << " ";
            }else if(board[j][i] == -1){
                std::cout << " " << "X" << " ";
            }
        }
        std::cout << "\n";
    }
}
char Gobang::convertChar(int num){
    return (char)(num + 97);
}
int Gobang::convertInt(char c){
    return ((int)c)-97;
}
int Gobang::toInput(int n){
    return n-1;
}
int Gobang::toOutput(int n){
    return n+1;
}
bool Gobang::isValid(std::string move){ //checks if the position has already been taken
    //doesn't factor in when the opponent inputs an invalid input
    try{
        int first = convertInt(move[0]);
        int second = toInput(std::stoi(move.substr(1)));
            if(first >= boardSize || second >= boardSize || first < 0 || second < 0){
            std::cerr << "move is out of range" << std::endl;
        return false;
        }
        if(board[first][second] == 0){
            return true;
        }else{
            std::cout << "another piece is already at this space" << std::endl;
            return false;
        }
    }
    catch(const std::invalid_argument& ia) {
        std::cerr << "Please enter a valid input (letter + number). Ex:'a1'";
        return false;
    }

}
bool Gobang::isValid(int first, int second){ //checks if the position has already been taken
    //doesn't factor in when the opponent inputs an invalid input
    if(first >= boardSize || second >= boardSize || first < 0 || second < 0){
        return false;
    }
    if(board[first][second] == 0){
        return true;
    }else{
        return false;
    }
}

int Gobang::calcTotScore(int first, int second, int n, int prevScore, int prevFirst, int prevSecond){
    int totScore = prevScore;
    //int totScore = 0;
    std::string move = convertChar(first) + std::to_string(second + 1);
        
    totScore -= calcHoriz(move, n);
    totScore -= calcVert(move, n);
    totScore -= calcUpDownDiag(move, n);
    totScore -= calcDownUpDiag(move, n);

    board[first][second] = n;

    totScore += calcVert(move, n);
    totScore += calcHoriz(move, n);
    totScore += calcUpDownDiag(move, n);
    totScore += calcDownUpDiag(move, n);


    board[first][second] = 0;

    return totScore;
}

int Gobang::calcTotScore(std::string move, int n){ //calculate the score of a board given a potential move
    int totScore = curBoardScore;
    //std::cout << "Move from calcTotScore: " << move <<std::endl;
    int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1))); 

    //calculate score horizontal from inserted move

    totScore -= calcHoriz(move, n);
    totScore -= calcVert(move, n);
    totScore -= calcUpDownDiag(move, n);
    totScore -= calcDownUpDiag(move, n);

    board[first][second] = n;

    totScore += calcVert(move, n);
    totScore += calcHoriz(move, n);
    totScore += calcUpDownDiag(move, n);
    totScore += calcDownUpDiag(move, n);

    board[first][second] = 0;

    return totScore;
}

int Gobang::calcVert(std::string move, int n){
    int state = 2;
    //state of 0 means that there are no consecutive pieces
    //state of 1 means that the program is calculating consecutive pieces of piece 1
    //state of -1 means that the progrma is calculating consecutive pieces of piece -1
    //state 2 means the program is currently looking at the top row or the left column
    int curScore = 0;
    int blocks = 0;
    int consc = 0;

    int first = convertInt(move[0]);
    //int second = toInput(std::stoi(move.substr(1))); 
    //second part of the move is not used

    blocks++; //accounts for the block at the edge of the leftmost wall 
    for(int i = 0; i < boardSize; i++){ // x state --> 0 state
        if(board[first][i] == 0){ //x state --> 0 state
            if(state == -1){
                curScore -= getScore(blocks, consc); //note opponent minimizes score
            }else if(state == 1){
                curScore += getScore(blocks, consc); //program maximizes score
            } 
            //if state = 0 then don't do anything
        
            //reset variables
            state = 0;
            blocks = 0;
            consc = 0;
        }else if(board[first][i] == 1){ // x state --> 1 state
            if(state == -1){
                blocks++; //add 1 to the current block count for -1 because theres a 1 piece
                curScore -= getScore(blocks, consc); //score is calculated because the route got cut off
                consc = 1;
                //reset information
                blocks = 1; // block for the next state is 1
            }else if(state == 1){
                consc++; // add to the consc count if 
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc = 1; // takes care of the cases at the top row/leftmost row. state 2-->1
            }
            state = 1; // the next state becomes 1
        }else if(board[first][i] == -1){ // x state --> -1 state
            if(state == -1){
                consc++;
            }else if(state == 1){
                blocks++;
                curScore += getScore(blocks, consc);
                consc = 1;

                blocks = 1;
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc =1; 
            }
            state = -1;
        }
    }
    blocks += 1; //accoutns for the block at the edge of the rightmost wall
    if(state == -1){
        curScore -= getScore(blocks, consc);
    }else if(state == 1){
        curScore += getScore(blocks, consc);
    }
    return curScore;
}
int Gobang::calcHoriz(std::string move, int n){
    int state = 2;
    //state of 0 means that there are no consecutive pieces
    //state of 1 means that the program is calculating consecutive pieces of piece 1
    //state of -1 means that the progrma is calculating consecutive pieces of piece -1
    //state 2 means the program is currently looking at the top row or the left column
    int curScore = 0;
    int blocks = 0;
    int consc = 0;

    //int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1))); 
    //second part of the move is not used

    blocks++; //accounts for the block at the edge of the leftmost wall 
    for(int i = 0; i < boardSize; i++){ // x state --> 0 state
        if(board[i][second] == 0){ //x state --> 0 state
            if(state == -1){
                curScore -= getScore(blocks, consc); //note opponent minimizes score
            }else if(state == 1){
                curScore += getScore(blocks, consc); //program maximizes score
            } 
            //if state = 0 then don't do anything
        
            //reset variables
            state = 0;
            blocks = 0;
            consc = 0;
        }else if(board[i][second] == 1){ // x state --> 1 state
            if(state == -1){
                blocks++; //add 1 to the current block count for -1 because theres a 1 piece
                curScore -= getScore(blocks, consc); //score is calculated because the route got cut off
                consc = 1;
                //reset information
                blocks = 1; // block for the next state is 1
            }else if(state == 1){
                consc++; // add to the consc count if 
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc = 1; // takes care of the cases at the top row/leftmost row. state 2-->1
            }
            state = 1; // the next state becomes 1
        }else if(board[i][second] == -1){ // x state --> -1 state
            if(state == -1){
                consc++;
            }else if(state == 1){
                blocks++;
                curScore += getScore(blocks, consc);
                consc = 1;

                blocks = 1;
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc =1; 
            }
            state = -1;
        }
    }
    blocks += 1; //accoutns for the block at the edge of the rightmost wall
    if(state == -1){
        curScore -= getScore(blocks, consc);
    }else if(state == 1){
        curScore += getScore(blocks, consc);
    }
    return curScore;
}
int Gobang::calcUpDownDiag(std::string move, int n){
    int state = 2;

    int curScore = 0;
    int blocks = 0;
    int consc = 0;
    

    int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1))); 

    int i = first;
    int j = second;
    //find the first position of the diagonal(upleft corner);
    while(i != 0 && j != 0){
        i--;
        j--;
    }

    blocks++; //accounts for the block at the edge of the leftmost wall 
    while(i != boardSize && j != boardSize){ // x state --> 0 state
        if(board[i][j] == 0){ //x state --> 0 state
            if(state == -1){
                curScore -= getScore(blocks, consc); //note opponent minimizes score
            }else if(state == 1){
                curScore += getScore(blocks, consc); //program maximizes score
            } 
            //if state = 0 then don't do anything
        
            //reset variables
            state = 0;
            blocks = 0;
            consc = 0;
        }else if(board[i][j] == 1){ // x state --> 1 state
            if(state == -1){
                blocks++; //add 1 to the current block count for -1 because theres a 1 piece
                curScore -= getScore(blocks, consc); //score is calculated because the route got cut off
                consc = 1;
                //reset information
                blocks = 1; // block for the next state is 1
            }else if(state == 1){
                consc++; // add to the consc count if 
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc = 1; // takes care of the cases at the top row/leftmost row. state 2-->1
            }
            state = 1; // the next state becomes 1
        }else if(board[i][j] == -1){ // x state --> -1 state
            if(state == -1){
                consc++;
            }else if(state == 1){
                blocks++;
                curScore += getScore(blocks, consc);
                consc = 1;

                blocks = 1;
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc =1; 
            }
            state = -1;
        }
        i++;
        j++;
    }
    blocks += 1; //accoutns for the block at the edge of the rightmost wall
    if(state == -1){
        curScore -= getScore(blocks, consc);
    }else if(state == 1){
        curScore += getScore(blocks, consc);
    }
    return curScore;
}
int Gobang::calcDownUpDiag(std::string move, int n){
    int state = 2;

    int curScore = 0;
    int blocks = 0;
    int consc = 0;
    

    int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1))); 

    int i = first;
    int j = second;
    //find the first position of the diagonal(upleft corner);
    while(i != 0 && j != boardSize -1){
        i--;
        j++;
    }
    

    blocks++; //accounts for the block at the edge of the leftmost wall 
    while(i != boardSize && j != -1){ // x state --> 0 state
        if(board[i][j] == 0){ //x state --> 0 state
            if(state == -1){
                curScore -= getScore(blocks, consc); //note opponent minimizes score
            }else if(state == 1){
                curScore += getScore(blocks, consc); //program maximizes score
            } 
            //if state = 0 then don't do anything
        
            //reset variables
            state = 0;
            blocks = 0;
            consc = 0;
        }else if(board[i][j] == 1){ // x state --> 1 state
            if(state == -1){
                blocks++; //add 1 to the current block count for -1 because theres a 1 piece
                curScore -= getScore(blocks, consc); //score is calculated because the route got cut off
                consc = 1;
                //reset information
                blocks = 1; // block for the next state is 1
            }else if(state == 1){
                consc++; // add to the consc count if 
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc = 1; // takes care of the cases at the top row/leftmost row. state 2-->1
            }
            state = 1; // the next state becomes 1
        }else if(board[i][j] == -1){ // x state --> -1 state
            if(state == -1){
                consc++;
            }else if(state == 1){
                blocks++;
                curScore += getScore(blocks, consc);

                consc = 1;
                blocks = 1;
            }else if(state == 0){
                blocks = 0;
                consc = 1;
            }else{
                consc =1; 
            }
            state = -1;
        }
        i++;
        j--;
    }
    blocks += 1; //accoutns for the block at the edge of the rightmost wall
    if(state == -1){
        curScore -= getScore(blocks, consc);
    }else if(state == 1){
        curScore += getScore(blocks, consc);
    }
    return curScore;
}
int Gobang::getScore(int blocks, int consc){
    int score = 0;
    if(consc > 5){ //added after realizing opponent could get more than 5 in a row.
        consc = 5;
    }
    if(blocks == 2){
        if(consc != 5){ // added after realizing opponent could get 5 in a row with 2 blocks
            score = 0;
        }else{
            score = 10000;
        }
    }else if(blocks == 1){
        score = oneBlock[consc];
    }else if(blocks == 0){
        score = noBlocks[consc];
    }else{
        std::cerr << "number of blocks isn't possible" << std::endl;
    }
    return score;
}

void Gobang::printConsider(){
    std::cout << "contents: " << std::endl;
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it)
        std::cout << "   letter format: " << convertChar(it->first) + std::to_string(it->second + 1)<< "  Number format: ("<< it->first << "," << it->second << ")"  << std::endl;
    //std::cout << "\n";
}

void Gobang::decideMove(){
    std::vector<std::pair<int, int>> purge;
    alpha = -999999999;
    int minChild;

    int inValid = 0;

    //CREATES BRANCHES OF DEPTH 1
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(isValid(it->first + i, it->second + j)){
                    minChild = getMinLeaf(std::pair<int, int> (it->first + i, it->second + j));
                    if(minChild > alpha){
                        alpha = minChild;
                        decidedMove = std::pair<int, int> (it->first + i, it->second + j);
                    }
                }else{
                    inValid++;
                }
            }
        }
        if(inValid == 8){
            purge.push_back(*it);
        }
        inValid = 0;
    }
    //purge moves that are no longer needed to be considered
    for(size_t i = 0; i < purge.size() ; i++){
        consider.erase(consider.find(purge[i]));
    }
}

int Gobang::getMinLeaf(std::pair<int, int> move){ //make it so that it works for every iteration in consider. 
    int minScore = 999999999;
    consider.insert(move);

    std::string moveString = convertChar(move.first) + std::to_string(move.second + 1);
    int prev = calcTotScore(moveString, 1);
    
    int tempScore;
    std::string minMove;
    board[move.first][move.second] = 1;
    //CREATE BRANCHES OF DEPTH 2
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(isValid(it->first + i, it->second + j) && minScore > alpha){//ALPHA PRUNING IMPLEMENTED HERE
                    tempScore = calcTotScore(it->first + i, it->second + j, -1, prev, move.first, move.second);
                    if(tempScore < minScore){
                        minScore = tempScore;
                    }
                }
            }
        }
    }
    board[move.first][move.second] = 0;

    consider.erase(consider.find(move));
    return minScore;
}
