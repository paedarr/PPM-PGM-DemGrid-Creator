/*
 * Paedar Rader
 * COMP 218 Fall 2023
 * File: praderDS2.h
 *
 * Stores the class declaration, method prototypes,
 * data members of class, and inherited class declarations.
 * -Each method has a description on what it does, input/output, parameters, etc-
 * 
 */

#include <iostream>
using namespace std;


void callMenu();
void clearScreen();

class DEMGrid{
protected:
    int width, height;                //grid's width and height
    int xllcrn, yllcrn;               //xllcorner & yllcorner - where the bottom left of graph is
    float cellsize;                   //tells us cell size, refers to how much space one pixel takes up, in a width x height relationship
    string filename;                  //name of DEM grid file, as a string
    enum fTypeDec {_PPM_, _PGM_};     //either PPM (P3) or PGM (P2), called 'fTypeDec' for 'File Type Declaration'
    fTypeDec fType;                   //type of file 'fTypeDesc' variable
    float ** demArr;                  //2d DEMGrid array, which holds floats
    float maxVal, minVal;             //max and min values of the data within the graph
    int startRow, endRow;             //start and end of pathing row user selection
public:
    float ** pgmArr;                  //pgmArr is public, so it can be accessed via easiestPath and hardestPath methods (points to pgmArr)
    float ** ppmArr;                  //ppmArr is public, so it can be accessed via easiestPath and hardestPath methods (points to ppmArr)  
    DEMGrid(fTypeDec);
    ~DEMGrid();
    int assembleColorGreen(float);
    int assembleColorRed(float);
    int assembleGrayscale(float);
    float findMaxInArr(float ** arr);
    float findMinInArr(float ** arr);
    float ** getdemArr();
    void writeFile(string);
    int getWidth();
    int getHeight();
    void easiestPath(string);
    void hardestPath(string);
    void adjustSRow();
};

class PPM : public DEMGrid{
protected:
    int ppmWidth;   //represents width of PPM file
    int ppmHeight;  //represents height of PPM file
public:
    PPM();
    ~PPM();
};

class PGM : public DEMGrid{
protected:
    int pgmWidth;   //represents width of PGM file
    int pgmHeight;  //represents height of PGM file
public:
    PGM();
    ~PGM();
};
