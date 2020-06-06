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
    oneBlock.insert({4, 40});
    oneBlock.insert({5, 10000});

    noBlocks.insert({0, 0});
    noBlocks.insert({1, 100});
    noBlocks.insert({2, 200});
    noBlocks.insert({3, 300});
    noBlocks.insert({4, 1000});
    noBlocks.insert({5, 10000});

    //Go first if first. Read input if last
    if(isLight){
        IOcontroller();
    }else{
        //start first and places the piece at the middle of the board. 
        if(boardSize % 2 == 0){
            consider.insert(std::pair<int, int>((boardSize/2),(boardSize/2)-1));
            std::cout << "Move played: " << convertChar(boardSize/2) << boardSize/2 << std::endl;
            curBoardScore = calcTotScore(convertChar(boardSize/2) + std::to_string(boardSize/2), 1);
            //std::cout << "curBoardScore = " << curBoardScore <<std::endl;
            board[(boardSize/2)][(boardSize/2)-1] = 1;
        }else{ //odd number insertion
            consider.insert(std::pair<int, int>((boardSize/2) + 1,boardSize/2));
            std::cout << "Move played: " << convertChar((boardSize/2) + 1) << (boardSize/2) + 1 << std::endl;
            curBoardScore = calcTotScore(convertChar((boardSize/2) + 1) + std::to_string((boardSize/2) + 1), 1);
            board[(boardSize/2) + 1][(boardSize/2)] = 1;
        }
        IOcontroller();
    }
}

void Gobang::IOcontroller(){
    std::string move;
    bool notMoved = true;

    while(!gameOver){
        if(std::abs(curBoardScore) > 5000){
            gameOver = true;
            if(curBoardScore > 0)
                std::cout << "You lose" << std::endl;
            else
                std::cout << "You win" << std::endl;
        }
        //read opponent's move
        std::cin >> move;
        while(notMoved){
            if(isValid(move)){
                std::cout << "Move played: " << move << std::endl;
                curBoardScore = calcTotScore(move, -1);
                std::cout << "After Opponent: " << curBoardScore << std::endl;
                consider.insert(std::pair<int, int>(convertInt(move[0]), std::stoi(move.substr(1))-1));
                board[convertInt(move[0])][std::stoi(move.substr(1))-1] = -1;
                notMoved = false;
                
                //testing purposes
                //std::cout << "curBoardScore: " << curBoardScore << std::endl;
                printBoard();
                printConsider();
            }else{
                std::cout << "please enter a valid move" << std::endl;
                std::cin >> move;
            }
        }
        notMoved = true;
        //decide own based off opponent's move
        decideMove();
        
        //output move
        consider.insert(decidedMove);
        std::cout << "Move played: " << convertChar(decidedMove.first) << decidedMove.second + 1 << std::endl;
        curBoardScore = calcTotScore(convertChar(decidedMove.first) + std::to_string(decidedMove.second + 1), 1);
        board[decidedMove.first][decidedMove.second] = 1;
        printBoard();
        printConsider();
        std::cout << "After Program: " << curBoardScore << std::endl;
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
            if(board[j][i] >= 0){
                std::cout << " " << board[j][i] << " ";
            }else{
                std::cout << board[j][i] << " ";
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
    int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1)));
    if(first >= boardSize || second >= boardSize || first < 0 || second < 0){
        std::cerr << "move is out of range" << std::endl;
        return false;
    }
    if(board[first][second] == 0){
        return true;
    }else{
        std::cerr << "another piece is already at this space" << std::endl;
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
    board[prevFirst][prevSecond] = -n;

    totScore += calcVert(move, n);
    totScore += calcHoriz(move, n);
    totScore += calcUpDownDiag(move, n);
    totScore += calcDownUpDiag(move, n);

    /*for(int i = 0; i < boardSize; i++){
        totScore += calcHoriz("a" + to_string(i),n);
        totScore += calcVert("a" + to_string(i),n);
        totScore += calcUpDownDiag("a" + to_string(i),n);
        totScore += calcDownUpDiag("a" + to_string(i),n);
    }*/

    //std::cout << "totalSCore at the end: " << totScore << std::endl;

    board[first][second] = 0;
    board[prevFirst][prevSecond] = 0;

    //std::cout << "totScore: " << totScore << std::endl;
    return totScore;
}

int Gobang::calcTotScore(std::string move, int n){ //calculate the score of a board given a potential move
    int totScore = curBoardScore;
    //int totScore = 0;
    int first = convertInt(move[0]);
    int second = toInput(std::stoi(move.substr(1))); 

    //calculate score horizontal from inserted move

    totScore -= calcHoriz(move, n);
    //std::cout << "horiz: " << calcHoriz(move, n) << std::endl;
    totScore -= calcVert(move, n);
    //std::cout << "vert: " << calcVert(move, n) << std::endl;
    totScore -= calcUpDownDiag(move, n);
    //std::cout << "upDown: " << calcUpDownDiag(move, n) << std::endl;
    totScore -= calcDownUpDiag(move, n);
    //std::cout << "downUp: " << calcDownUpDiag(move, n) << std::endl;

    //std::cout << "totalSCore from middle: " << totScore << std::endl;
    board[first][second] = n;

    totScore += calcVert(move, n);
    totScore += calcHoriz(move, n);
    totScore += calcUpDownDiag(move, n);
    totScore += calcDownUpDiag(move, n);

    //std::cout << "totalSCore at the end: " << totScore << std::endl;

    board[first][second] = 0;

    //std::cout << "totScore: " << totScore << std::endl;
    return totScore;

    //test case for updown
    /*
    board[0][0] = 1;
    board[2][2] = 1;
    board[3][3] = -1;
    board[4][4] = -1;
    board[6][6] = 1;
    board[9][9] = 1;
    //test case for downup
    board[0][10] = 1;
    board[2][8] = 1;
    board[3][7] = -1;
    board[4][6] = -1;
    board[6][4] = 1;
    board[9][1] = 1;
    */
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
    //first part of the move is not used

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
    if(blocks == 2){
        score = 0;
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
    /*std::cout << "contents: " << std::endl;
    for(size_t i = 0; i < consider.size(); i++){
        std::cout << "   letter format: " << convertChar(consider[i].first) + std::to_string(consider[i].second + 1)<< "  Number format: ("<< consider[i].first << "," << consider[i].second << ")"  << std::endl;
    }
    std::cout << "size: " << consider.size() << std::endl;*/
    std::cout << "contents: " << std::endl;
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it)
        std::cout << "   letter format: " << convertChar(it->first) + std::to_string(it->second + 1)<< "  Number format: ("<< it->first << "," << it->second << ")"  << std::endl;
    std::cout << "\n";
}

void Gobang::decideMove(){
    /*bool isOpp = false;
    std::pair<int, int> alphaMove;
    std::pair<int, int> betaMove;
    int alpha = -9999999;
    int beta = 9999999;*/
    
    createTree();
    //std::cout << "queue size: " << tree.size()<<std::endl;
}

void Gobang::createTree(){
    std::vector<std::pair<int, int>> purge;
    int maxScore = -9999999;
    int minChild;

    int inValid = 0;
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it){
        if(isValid(it->first - 1, it->second + 1)){ // topleft
            //tree.push({false, std::pair<int,int>(it->first -1 , it->second + 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first - 1, it->second + 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first - 1, it->second + 1);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first, it->second + 1)){//top
            //tree.push({false, std::pair<int,int>(it->first , it->second + 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first, it->second + 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first, it->second + 1);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first + 1, it->second + 1)){//top right
            //tree.push({false, std::pair<int,int>(it->first +1 , it->second + 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first + 1, it->second + 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first + 1, it->second + 1);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first - 1, it->second)){//left
            //tree.push({false, std::pair<int,int>(it->first -1 , it->second), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first - 1, it->second));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first - 1, it->second);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first + 1, it->second)){//right
            //tree.push({false, std::pair<int,int>(it->first +1 , it->second), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first + 1, it->second));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first + 1, it->second);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first - 1, it->second - 1)){//bottomleft
            //tree.push({false, std::pair<int,int>(it->first -1 , it->second - 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first - 1, it->second - 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first - 1, it->second - 1);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first, it->second - 1)){//bottom
            //tree.push({false, std::pair<int,int>(it->first , it->second + 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first, it->second - 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first, it->second - 1);
            }
        }else{
            inValid++;
        }
        if(isValid(it->first + 1, it->second - 1)){//bottomright
            //tree.push({false, std::pair<int,int>(it->first +1 , it->second - 1), 1, 0});
            minChild = getMinLeaf(std::pair<int, int> (it->first + 1, it->second - 1));
            if(minChild > maxScore){
                maxScore = minChild;
                decidedMove = std::pair<int, int> (it->first + 1, it->second - 1);
            }
        }else{
            inValid++;
        }
        if(inValid == 8){
            purge.push_back(*it);
        }
    }
    //purge moves that are no longer needed to be considered
    for(size_t i = 0; i < purge.size() ; i++){
        consider.erase(consider.find(purge[i]));
    }
}

int Gobang::getMinLeaf(std::pair<int, int> move){ //make it so that it works for every iteration in consider. 
    int minScore = 9999999;
    consider.insert(move);
    int prev = calcTotScore(move.first, move.second, 1, curBoardScore,move.first, move.second);
    for (std::set<std::pair<int,int>>::iterator it=consider.begin(); it!=consider.end(); ++it){
        if(isValid(it->first + 1, it->second + 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first + 1, it->second + 1, -1, prev, move.first, move.second);
        if(isValid(it->first, it->second + 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first, it->second + 1, -1, prev, move.first, move.second));
        if(isValid(it->first - 1, it->second + 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first - 1, it->second + 1, -1, prev, move.first, move.second));
        if(isValid(it->first + 1, it->second - 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first + 1, it->second - 1, -1, prev, move.first, move.second));
        if(isValid(it->first, it->second - 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first, it->second - 1, -1, prev, move.first, move.second));
        if(isValid(it->first - 1, it->second - 1) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first - 1, it->second - 1, -1, prev, move.first, move.second));
        if(isValid(it->first + 1, it->second) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first + 1, it->second, -1, prev, move.first, move.second));
        if(isValid(it->first - 1, it->second) && minScore > alpha)
            minScore = std::min(minScore, calcTotScore(it->first - 1, it->second, -1, prev, move.first, move.second));
    }
    consider.erase(consider.find(move));
    return minScore;
}