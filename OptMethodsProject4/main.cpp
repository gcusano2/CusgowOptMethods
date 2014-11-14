//
//  main.cpp
//  OptMethodsProject4
//
//  Created by Greg Cusano on 11/6/14.
//  Copyright (c) 2014 Greg Cusano. All rights reserved.
//

#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  //  The number of cells in a small square
//  (usually 3).  The board has
//  SquareSize^2 rows and SquareSize^2
//  columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

int beginningArray[9][9];
int endingArray[9][9];

int total = 0;
int boardCounter = 0;

class board
// Stores the entire Sudoku board
{
public:
    board(int);
    void clear();
    void initialize(ifstream &fin);
    void print();
    bool isBlank(int, int);
    ValueType getCell(int, int);
    
    //Added Methods
    void setCell(int, int, int);
    void printConflicts();
    bool conflicts(int i, int j, int Val);
    void clearCell(int i, int j);
    bool solvedBoard();
    int squareNumber(int i, int j);
    
    //Added to 4B
    int getRecursionCount() {
        return recursionCounter;
    }
    void clearRecursionCount() {
        recursionCounter = 0;
    }
    int goodPositionCheck(int i, int j, int value);
    int redoCheck(int x, int y);
    void printArray();
    
    
private:
    
    //Added Methods
    void ConflictUpdate(int, int, int, bool);
    //Conflict Matrices
    matrix<int> Conflicts;
    matrix<bool> rows;
    matrix<bool> cols;
    matrix<bool> squares;
    
    // The following matrices go from 1 to BoardSize in each
    // dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)
    
    matrix<ValueType> value;
    matrix<int> output;
    int recursionCounter; //counts number of recursive calls
};

board::board(int sqSize)
: value(BoardSize+1,BoardSize+1),
rows(BoardSize+1,BoardSize+1),
cols(BoardSize+1,BoardSize+1),
squares(BoardSize+1,BoardSize+1)
// Board constructor
{
    clear();
}

bool board::conflicts(int i, int j, int Val){
    int SquareNum = squareNumber(i,j);
    return rows[i][Val] || cols[j][Val] || squares[SquareNum][Val];
}

void board::ConflictUpdate(int i, int j, int val, bool Bool){
    int SquareNum = squareNumber(i,j);
    rows[i][val] = Bool;
    cols[j][val] = Bool;
    squares[SquareNum][val] = Bool;
}

void board::clear()
// Mark all possible values as legal for each board entry
{
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            value[i][j] = Blank;
        }
}

bool board::solvedBoard(){
    bool Solved = true;
    for (int i = 1; i <= BoardSize; i++){
        for (int j = 1; j <= BoardSize; j++){
            Solved = Solved && !(isBlank(i,j));
            if(!Solved)
                return false;
        }
    }
    return true;
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
    char ch;
    
    clear();
    
    for (int i = 1; i <= BoardSize; i++){
        for (int j = 1; j <= BoardSize; j++){
            
            fin >> ch;
            
            if (ch != '.'){  // If the read char is not Blank
                if (conflicts(i,j,ch-'0'))
                    throw rangeError("Conflict, cannot Initialize");
                setCell(i,j,ch-'0');   // Convert char to int
            }
            else if (ch == '.')
                setCell(i,j,0);
        }
    }
}

int board::squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
    // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
    // coordinates of the square that i,j is in.
    
    return SquareSize * ((i-1)/SquareSize) + (j-1)/SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
    for (int i = 0; i < v.size(); i++)
        ostr << v[i] << " ";
    ostr << endl;
    return ostr;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
}

void board::setCell(int i, int j, int Val){
    //Sets the cell with given inputs. Updates conflicts.
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize){
        value[i][j] = Val;
        ConflictUpdate(i, j, Val, true);
    }
    else
        throw rangeError("Cell Value out of Range");
}

void board::clearCell(int i, int j){
    //Clears the cell at given coordinates. Updates conflicts.
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize){
        ConflictUpdate(i, j, value[i][j], false);
        value[i][j] = 0;
    }
    else
        throw rangeError("Cell Value out of Range");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in setCell");
    
    return (getCell(i,j) == Blank);
}

void board::print()
// Prints the current board.
{
    for (int i = 1; i <= BoardSize; i++)
    {
        if ((i-1) % SquareSize == 0)
        {
            cout << " -";
            for (int j = 1; j <= BoardSize; j++)
                cout << "---";
            cout << "-";
            cout << endl;
        }
        for (int j = 1; j <= BoardSize; j++)
        {
            if ((j-1) % SquareSize == 0)
                cout << "|";
            if (!isBlank(i,j))
                cout << " " << getCell(i,j) << " ";
            else
                cout << "   ";
        }
        cout << "|";
        cout << endl;
    }
    
    cout << " -";
    for (int j = 1; j <= BoardSize; j++)
        cout << "---";
    cout << "-";
    cout << endl;
}

void board::printConflicts(){
    cout << "Conflicts: Rows    ";
    cout << "Columns             ";
    cout << "Squares" << endl;
    for (int i=1; i < value.rows(); i++){
        for (int j=1; j < value.cols(); j++){
            cout << rows[i][j] << " ";
        }
        for (int j=1; j < value.cols(); j++){
            cout << cols[i][j] << " ";
        }
        for (int j=1; j < value.cols(); j++){
            cout << squares[i][j] << " ";
        }
        cout << endl;
    }
}

//*********Added for 4B***********

int board::goodPositionCheck(int i, int j, int value){
    
    int x = (i/3)*3;
    int y = (j/3)*3;
    
    for (int a=x; a< x+3; a++){
        for (int b=y; b < y+3; b++){
            if (endingArray[x][y] == value){
                return 0;
            }
        }
    }
    
    for (int m=0; m < 9; m++){
        if(endingArray[m][j] == value || endingArray[i][m] == value){
            return 0;
        }
    }
    
    return value;
}

int board::redoCheck(int x, int y){
    
    recursionCounter++;
    int i;
    
    if (endingArray[x][y] == 0){
        for(i=1; i<10;i++){
            int valueAtSpace = goodPositionCheck(x, y, i);
            
            if (valueAtSpace != 0){
                endingArray[x][y] = valueAtSpace;
                
                if (x==9 && y==8){ return 1; }
                else if (x==8){
                    if (redoCheck(0, y+1)){
                        return 1;
                    }
                }
                else{
                    if (redoCheck(x+1, y)){
                        return 1;
                    }
                }
            }
        }
        
        if (i == 10)
        {
            if (endingArray[x][y] != beginningArray[x][y]){
                endingArray[x][y] = 0;
            }
            return 0;
        }
    }
    else{
        if (x==8 && y==8){
            return 1;
        }
        else if (x==8){
            if (redoCheck(0, y+1))
                return 1;
        }
        else{
            if (redoCheck(x+1, y))
                return 1;
        }
        return 0;
    }
}

void board::printArray(){
    cout << "+-:";
    for (int j = 1; j <= BoardSize; j++){
        cout << "---";
    }
    cout << "-+" << endl;
    
    for (int i =1; i<= BoardSize; i++){
        if (i == 4 || i == 7){
            cout << "|";
            for (int j=1; j < SquareSize; j++){
                cout << "---------+";
            }
            cout << "---------|" << endl;
        }
        for (int j = 1; j <= BoardSize; j++){
            if ((j-1) % SquareSize == 0){
                cout << "|";
            }
            cout << " " << endingArray[i-1][j-1] << " ";
        }
        cout << "|";
        cout << endl;
    }
    
    cout << "+-";
    for (int j=1; j <= BoardSize; j++){
        cout<< "---";
    }
    cout << "-+" << endl;
}

int main()
{
    ifstream fin;
    
    // Read the sample grid from the file.
    string fileName = "sudoku.txt";
    
    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }
    
    try
    {
        board b1(SquareSize);
        
        while (fin && fin.peek() != 'Z')
        {
            boardCounter++;
            b1.clearRecursionCount();
            b1.initialize(fin);
            
            for( int i=0; i < 9; i++){
                for (int j=0; j<9; j++){
                    endingArray[i][j] = beginningArray[i][j];
                }
            }
            cout << "\nBoard " << boardCounter << endl;
            b1.print();
            
            b1.redoCheck(0,0);
            cout << "\nSolved Board " << boardCounter << endl;
            b1.printArray();
            
            cout << "Number of calls: " << b1.getRecursionCount() - 1 << endl;
            total += b1.getRecursionCount()-1;
        }
        cout << "Average number of recursive calls: " << total/ boardCounter << endl;
        
        
        
        cout << boardCounter << " boards have been solved." << endl;
    }
    catch (rangeError &ex){
        cout << ex.what() << endl;
        exit(1);
    }
}