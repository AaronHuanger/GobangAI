#ifndef GOBANG_H
#define GOBANG_H
#include <unordered_map> 
#include <map>
#include <vector>
#include <set>
#include <queue>
struct Node{
    bool isOpp = false;
    std::pair<int, int> move = std::pair<int, int>(0,0);
    int depth = 0;
    int score = 0;
    std::set<Node> children;
};
//
class Gobang{
    public:
        void initialize(int num,char *input[]);

        void IOcontroller(); 

        void decideMove();
        
        void createTree();

        int getMinLeaf(std::pair<int, int> move);

        //where n is the value of the move. AKA whether or not the move belongs to opponent or program
        int calcTotScore(int first, int second, int n, int prevScore, int prevFirst, int prevSecond);
        int calcTotScore(std::string move, int n);
        int calcHoriz(std::string move, int n);
        int calcVert(std::string move, int n);
        int calcUpDownDiag(std::string move, int n);
        int calcDownUpDiag(std::string move, int n);

        //Note: convertChar and convertInt already converts to input/output format
        //      no need to add or subtract an additional 1 to match the board.
        char convertChar(int num);
        int convertInt(char c);
        int toOutput(int n);
        int toInput(int n);
        int getScore(int blocks, int consc);
        void printConsider();
        bool isValid(std::string move);
        bool isValid(int first, int second);
        void printBoard();
        
    private:
        int boardSize = 11;
        int curBoardScore = 0;

        int depth = 2;
        /*std::queue<Node> tree; 
        
        int beta;*/
        int alpha;


        bool isLight = false;
        bool gameOver = false;
        std::vector<std::vector<int>>board;
        //std::vector<std::pair<int, int>> consider;
        std::set<std::pair<int, int>> consider;

        std::pair<int, int> decidedMove;

        //{key, value} = {#consecutive tiles, resulting score}
        std::unordered_map<int, int> noBlocks;
        std::unordered_map<int, int> oneBlock;
        
};
#endif