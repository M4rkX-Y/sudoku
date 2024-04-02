// Declarations and functions for project #4
#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <vector>
#include <fstream>
using namespace std;
typedef int ValueType;    // The type of the value in a cell
const int Blank = -1;     // Indicates that a cell is blank
const int SquareSize = 3; // The number of cells in a small square
// (usually 3). The board has
// SquareSize^2 rows and SquareSize^2
// columns.
const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;
int numSolutions = 0;

class board
// Stores the entire Sudoku board
{
public:
    board(int);
    void clear();
    void initialize(ifstream &fin);
    void print();
    void printConflict();
    bool isBlank(int, int);
    bool isConflict(int, int, int);
    bool isSolved();
    void setCell(int, int, int);
    void clearCell(int, int);
    void setConflict(int, int, int);
    void updateConflict(int, int);
    ValueType getCell(int, int);

private:
    // The following matrices go from 1 to BoardSize in each
    // dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)
    matrix<ValueType> value;
    matrix<vector<bool>> conflict;
};

board::board(int sqSize)
    : value(BoardSize + 1, BoardSize + 1), conflict(BoardSize + 1, BoardSize + 1)
// Board constructor
{
    clear();
}

void board::clear()
// Mark all possible values as legal for each board entry
{
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            value[i][j] = Blank;
            conflict[i][j].resize(BoardSize);
            for (int k = 0; k < BoardSize; k++)
            {
                conflict[i][j][k] = false;
            }
        }
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
    char ch;
    clear();
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            fin >> ch;
            // If the read char is not Blank
            if (ch != '.')
            {
                setCell(i, j, ch - '0'); // Convert char to int
            }
        }
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom. Note that i and j each go from 1 to BoardSize
{
    // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
    // coordinates of the square that i,j is in.
    return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
    for (int i = 0; i < v.size(); i++)
        ostr << v[i] << " ";
    cout << endl;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell. Throws an exception
// if bad values are passed.
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
}

void board::setCell(int i, int j, int k)
// Set the value of a cell
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
    {
        value[i][j] = k;
        setConflict(i, j, k);
    }
    else
        throw rangeError("bad value in setCell");
}

bool board::isConflict(int i, int j, int k)
{
    return conflict[i][j][k - 1];
}

void board::clearCell(int i, int j)
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
    {
        value[i][j] = Blank;

        int sqrNum = squareNumber(i, j);
        for (int x = 1; x <= BoardSize; x++)
        {
            for (int y = 1; y <= BoardSize; y++)
            {
                if (squareNumber(x, y) == sqrNum || x == i || y == j)
                    updateConflict(x, y);
            }
        }
    }
    else
        throw rangeError("bad value in clearCell");
}

void board::setConflict(int i, int j, int k)
// Set the conflict vector of all the cells in the same roll/column/square based on the number
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
    {
        int sqrNum = squareNumber(i, j);
        for (int x = 1; x <= BoardSize; x++)
        {
            for (int y = 1; y <= BoardSize; y++)
            {
                if (squareNumber(x, y) == sqrNum || x == i || y == j)
                    conflict[x][y][k - 1] = true;
            }
        }
    }
    else
        throw rangeError("bad value in setConflict");
}

void board::updateConflict(int i, int j)
// Update the specific cell based on the number occurance of same roll/column/square cells
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
    {
        for (int k = 0; k < BoardSize; k++)
        {
            conflict[i][j][k] = false;
        }
        int sqrNum = squareNumber(i, j);
        for (int x = 1; x <= BoardSize; x++)
        {
            for (int y = 1; y <= BoardSize; y++)
            {
                if (!isBlank(x, y) && (squareNumber(x, y) == sqrNum || x == i || y == j))
                {
                    int k = getCell(x, y);
                    conflict[i][j][k - 1] = true;
                }
            }
        }
    }
    else
        throw rangeError("bad value in updateConflict");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in isBlank");
    return (getCell(i, j) == Blank);
}

bool board::isSolved()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            if (isBlank(i, j))
                return false;
        }
    }
    return true;
}

void board::print()
// Prints the current board.
{
    for (int i = 1; i <= BoardSize; i++)
    {
        if ((i - 1) % SquareSize == 0)
        {
            cout << " -";
            for (int j = 1; j <= BoardSize; j++)
                cout << "---";
            cout << "-";
            cout << endl;
        }
        for (int j = 1; j <= BoardSize; j++)
        {
            if ((j - 1) % SquareSize == 0)
                cout << "|";
            if (!isBlank(i, j))
                cout << " " << getCell(i, j)
                     << " ";
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

void board::printConflict()
// Prints the conflict vectors.
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            cout << "conflict[" << i << "][" << j << "] = {";
            for (int k = 0; k < BoardSize; k++)
            {
                if (conflict[i][j][k])
                    cout << "T";
                else
                    cout << "F";
            }
            cout << "}" << endl;
        }
    }
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
            b1.initialize(fin);
            cout << "Print Board:" << endl;
            b1.print();
            cout << "Print Conflict Vecters: " << endl;
            b1.printConflict();
            for (int i = 1; i <= BoardSize; i++)
            {
                if (b1.isBlank(1, i))
                {
                    for (int k = 1; k <= BoardSize; k++)
                    {
                        cout << i << " " << k << " " << b1.isConflict(1, i, k) << endl;
                        if (!b1.isConflict(1, i, k))
                        {
                            b1.setCell(1, i, k);
                            break;
                        }
                    }
                    break;
                }
            }
            for (int i = 1; i <= BoardSize; i++)
            {
                if (!b1.isBlank(1, i))
                {
                    b1.clearCell(1, i);
                    break;
                }
            }
            cout << "Print Updated Board:" << endl;
            b1.print();
            cout << "Print Updated Conflict Vecters: " << endl;
            b1.printConflict();
            cout << "Is the board solved: ";
            if (b1.isSolved())
                cout << "True";
            else
                cout << "False";
            cout << endl;
        }
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl;
        exit(1);
    }
}
