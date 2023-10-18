/*
 * Paedar Rader
 * COMP 218 Fall 2023
 * File: praderDS2.cpp
 * 
 * (main)
 * Allows the user to input a .grd file, and create a PPM/PGM object for their file. For this
 * operation the user has a variety of options:
 * - Create a new PGM object, which inherits DEMGrid class (g)
 * - Create a new PPM object, which inherits DEMGrid class (p)
 * - Display number of rows & columns for current object file (d)
 * - Choose starting row for pathing options (s)
 * - Draw the easiest west to east travel path on existing PPM/PGM object array (l)
 * - Draw the hardest west to east travel path on existing PPM/PGM object array (m)
 * - Write current PGM/PPM array to user-chosen filename (q)
 * 
 * Input: One of the chars listed above to select a menu item operation
 * Output: Dependent upon selected operation.
 *
 * Contains all class method definitions, function definitions,
 * main functionality, and library additions. All descriptions
 * for everything below are present in the 'praderDS2.h' file.
 */

#include <fstream>      //file I/O
#include <sstream>      //to manipulate getline floats/data
#include <string.h>     //to manipulate header lines w/ data
#include <string>       //for manipulation of file input strings
#include "praderDS2.h"  //header file with all method declarations, class declarations, descriptions, etc


/*
 * -- Call Menu Function --
 * Prints out the menu whenever function is called.
*/
void callMenu(){
    cout << "Menu ~~~~" << endl;
    cout << "g - Create new P(G)M file" << endl;
    cout << "p - Create new P(P)M file" << endl;
    cout << "d - (D)isplay number of rows/columns" << endl;
    cout << "s - Enter (s)tarting position row" << endl;
    cout << "l - Calculate path with (l)east elevation gain" << endl;
    cout << "m - Calculate path with (m)ost elevation gain" << endl;
    cout << "w - (W)rite output file" << endl;
    cout << "q - (Q)uit" << endl;
    cout << "---------------------↓↓↓↓---------------------" << endl;
    cout << "--> ";
}


/*
* -- DEMGrid Constructor --
* Initializes a DEMGrid object based on the provided file type.
* Prompts the user to enter a filename for data input.
* Reads the header information from the specified file and extracts key parameters.
* Allocates memory for the DEM array based on the extracted dimensions.
* Reads the elevation data from the file and populates the DEM array.
* Calculates and stores the maximum and minimum elevation values.
*/
DEMGrid :: DEMGrid(fTypeDec f){
    this->fType = f;   //sets the type of 'fTypeDesc' to type passed through parameter

    cout << "Please enter filename (with extension [.grd] ): " << endl;
    cin >> this->filename;

    ifstream gridFile;
    gridFile.open(filename);

    if (gridFile.is_open()) {

    string line; 

    for (int i = 0; i < 5; i++){          //loops through header elements of .grd file                    
        getline(gridFile, line);          //gets the actual line from gridFile, and stores it in line
        char str[line.length() + 1];      //creates a char array with the size of the line + 1

        strcpy(str, line.c_str());        //copies chars from line, and copies them to str array
        char * split = strtok(str, " ");  //splits str char arr in tokens, seperated by spaces, and assigns first token to split pointer

        split = strtok(nullptr, " ");     //cuts off the left side of the space, effectively just giving the number value
        
        if (i == 0){
            this->width = stoi(split);
        }
        else if (i == 1){
            this->height = stoi(split);
        }
        else if (i == 2){
            this->xllcrn = stoi(split);
        }
        else if (i == 3){
            this->yllcrn = stoi(split);
        }
        else if (i == 4){
            this->cellsize = stof(split);
        }
    }

    demArr = new float *[this->width];
    for (int i = 0; i < this->width; ++i) {
        demArr[i] = new float[this->height];
    }

    int rows = this->height;   //x axis
    int cols = this->width;  //y axis

    //fills the demArr with the data in the .grd file
    for (int r = 0; r < rows; r++){
        getline(gridFile, line);
        istringstream ss(line);
        for (int c = 0; c < cols; c++){
            ss >> demArr[r][c];
        }
    }

    string disType = "";
    if(this->fType == _PGM_){
        disType = "PGM";
    }
    else if (this->fType == _PPM_){
        disType = "PPM";
    }

    this->maxVal = findMaxInArr(demArr);
    this->minVal = findMinInArr(demArr);

    clearScreen();
    cout << "-------------------" << endl;
    cout << disType << " Object created." << endl;
    cout << "-------------------" << endl;

    }
    else{
        cout << "!! File failed to open. Try again. !!" << endl;
        return;
    }
    gridFile.close();
};

/*
 * -- Get DEM Array Getter -- 
 * Getter method for accessing the DEM array.
 * Returns a pointer to the 2D float array representing the DEM data.
*/
float ** DEMGrid :: getdemArr(){
    return demArr;
};

/*
 * -- DEMGrid Destructor. --
 * Releases the memory allocated for the DEM array when the DEMGrid object is destroyed.
*/
DEMGrid :: ~DEMGrid(){
    //deletes demArr as destructor
    if (demArr != nullptr) {
        for (int i = 0; i < this->width; ++i) {
            delete[] demArr[i];
        }
        delete[] demArr;
    }
};

/*
 * -- Find Max Value in Array -- 
 * Finds and returns the maximum value in a 2D float array.
 * Iterates through the entire array to locate the maximum value.
 * Returns the maximum value found in the array.
*/
float DEMGrid :: findMaxInArr(float ** arr){
    float maxVal = arr[0][0]; 
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (arr[i][j] > maxVal) {
                maxVal = arr[i][j];
            }
        }
    }
    return maxVal;
};

/*
 * -- Find Min Value in Array -- 
 * Finds and returns the minimum value in a 2D float array.
 * Iterates through the entire array to locate the minimum value.
 * Returns the minimum value found in the array.
*/
float DEMGrid :: findMinInArr(float ** arr){
    float minVal = arr[0][0]; 
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (arr[i][j] < minVal) {
                minVal = arr[i][j];
            }
        }
    }
    return minVal;
};

/*
 * -- Assemble Color Green Method -- 
 * Assembles the green component of an RGB color based on the given elevation value.
 * Calculates the intensity of green based on the elevation's position between the minimum and maximum values.
 * Returns an integer representing the green component of the color (0-255).
*/
int DEMGrid :: assembleColorGreen(float f){
    
    float colorIntensity = (f - this->minVal) / (this->maxVal - this->minVal);
    float greenVal = colorIntensity * 255.0;
    int green = int(greenVal);

    return green;
};

/*
 * -- Assemble Color Red Method -- 
 * Assembles the red component of an RGB color based on the given elevation value.
 * Calculates the intensity of red based on the elevation's position between the minimum and maximum values.
 * Returns an integer representing the red component of the color (0-255).
*/
int DEMGrid :: assembleColorRed(float f){
    float threshold = 0.5; // 50% threshold
    float range = maxVal - minVal;
    
    if (f >= minVal + threshold * range) {
        // calculate the amount of blue based on the elevation value
        float colorIntensity = (f - (minVal + threshold * range)) / ((1.0 - threshold) * range);
        float redVal = colorIntensity * 255.0; // Shades of blue
        int red = int(redVal);
        return red;
    } else {
        return 0; // No blue
    }
};


/*  ---   PPM CLASS   ---   */
/*
 * -- PPM Constructor -- 
 * Initializes a PPM object, inheriting from DEMGrid, and processes data for PPM output.
 * Adjusts the dimensions of the PPM array based on DEMGrid dimensions.
 * Fills the PPM array with RGB values calculated from DEM data using the `assembleColorRed` 
 * and `assembleColorGreen` methods.
*/
PPM :: PPM() : DEMGrid(_PPM_){
    float ** getArr = getdemArr();
    ppmWidth = (DEMGrid::width) * 3;
    ppmHeight = (DEMGrid::height);
    
    //One cell in PPM Array has 3 values, RGB - so, three locations are being used for one cell
    ppmArr = new float*[ppmHeight]; //creates new 2d matrix using the adjusted width & height for ppm
    for (int i = 0; i < ppmHeight; ++i) {
        ppmArr[i] = new float[ppmWidth];
    }



    for (int rows = 0; rows < this->ppmHeight; rows++){
        for (int cols = 0; cols < this->ppmWidth; cols+= 3){ 
            float num = getArr[rows][cols / 3]; 
                    //red val
                    ppmArr[rows][cols] = assembleColorRed(num);
                    //green val
                    ppmArr[rows][cols + 1] = assembleColorGreen(num);
                    //blue val
                    ppmArr[rows][cols + 2] = 0.0;
                    //doesn't need cols++, since prev for loop already indexes it
        }
    }

};

/*
 * -- PPM Destructor -- 
 * Cleans up resources associated with a PPM object.
 * Deallocates memory used by the PPM array.
*/
PPM :: ~PPM(){
    //deletes ppmArr as destructor
    for (int i = 0; i < ((DEMGrid::height)); ++i) {
        delete[] this->ppmArr[i];
    }
    delete[] this->ppmArr; 
};
/*  ---   ---------   ---   */


/*  ---   PGM CLASS   ---   */
/*
 * -- PGM Constructor -- 
 * Initializes a PGM object, inheriting from DEMGrid, and processes data for PGM output.
 * Adjusts the dimensions of the PGM array based on DEMGrid dimensions.
 * Fills the PGM array with RGB values calculated from DEM data using the `assembleGrayscale` method
*/
PGM :: PGM() : DEMGrid(_PGM_){
        float ** getArr = getdemArr();
        pgmWidth = (DEMGrid::width);
        pgmHeight = (DEMGrid::height);
        
        //One cell in PPM Array has 1 value, grayscale - so, three locations are being used for one cell
        pgmArr = new float*[pgmWidth]; //creates new 2d matrix using the adjusted width & height for ppm
        for (int i = 0; i < pgmWidth; ++i) {
            pgmArr[i] = new float[pgmHeight];
        }



        for (int rows = 0; rows < this->pgmWidth; rows++){
            for (int cols = 0; cols < this->pgmHeight; cols++){ 
                float num = getArr[rows][cols]; 
                for (int n = 0; n < 3; n++){
                    if (n == 0){
                        pgmArr[rows][cols] = assembleGrayscale(num);
                }
            }
        }
    }
};

/*
 * -- PGM Destructor -- 
 * Cleans up resources associated with a PGM object.
 * Deallocates memory used by the PGM array.
*/
PGM :: ~PGM(){
    //deletes pgmArr as destructor
    for (int i = 0; i < ((DEMGrid::height)); ++i) {
        delete[] this->pgmArr[i];
    }
    delete[] this->pgmArr; 
};

/*
 * -- Assemble Grayscale Method -- 
 * Assembles the grayscale value based on the given elevation value.
 * Calculates the grayscale intensity based on the elevation's position between the minimum and maximum values.
 * If the elevation is below 30.0, it assigns a minimum grayscale value of 30 to leave room for a black path.
 * Returns an integer representing the grayscale value (0-255).
*/
int DEMGrid :: assembleGrayscale(float f){
    int gray;
    if (f > 30.0){
        float grayIntensity = (f - this->minVal) / (this->maxVal - this->minVal);
        float grayVal = grayIntensity * 255.0;
        gray = int(grayVal);
    }
    else{
        gray = 30; //sets all values lower than 30, automatically to 30, so there is room for black path
    }
    return gray;
};

/*  ---   ---------   ---   */

/*
 * -- Get Width Getter -- 
 * Getter method to retrieve the width of the DEM grid.
 * Returns the width of the DEM grid.
*/
int DEMGrid :: getWidth(){
    return this->width;
};

/*
 * -- Get Height Getter -- 
 * Getter method to retrieve the height of the DEM grid.
 * Returns the height of the DEM grid.
*/
int DEMGrid :: getHeight(){
    return this->height;
};

/*
 * -- Write File --
 * Writes the DEM data to a file in PGM or PPM format based on the specified type.
 * The 'type' parameter determines whether to write PGM or PPM data.
 * If 'type' is "PGM," it prompts the user for a filename, appends ".pgm" to it, and writes PGM data to the file.
 * If 'type' is "PPM," it prompts the user for a filename, appends ".ppm" to it, and writes PPM data to the file.
 * For PPM, it writes the width, height, and color intensity (255) as the header and then the pixel values.
 * For PGM, it writes the width, height, and color intensity (255) as the header and then the grayscale pixel values.
*/
void DEMGrid :: writeFile(string type){
    if (type == "PGM"){
        string filename;
        cout << "Please enter the name of the file to write to (don't include file extension, like .pgm): " << endl;
        cin >> filename;

        filename = filename + ".pgm";

        ofstream ppmFile;
        ppmFile.open(filename);

        ppmFile << "P2" << endl;
        ppmFile << this->width << " " << this->height << endl;
        ppmFile << 255 << endl;
        for (int r = 0; r < this->width; r++){
            for (int c = 0; c < this->height; c++){
                int pixelVal = pgmArr[r][c];
                ppmFile << pixelVal << " ";
            }
            ppmFile << endl;
        }
    }
    else if (type == "PPM"){

    string filename;
    cout << "Please enter the name of the file to write to (don't include file extension, like .ppm): " << endl;
    cin >> filename;

    filename = filename + ".ppm";

    ofstream ppmFile;
    ppmFile.open(filename);

    ppmFile << "P3" << endl;
    ppmFile << this->width << " " << this->height << endl;
    ppmFile << 255 << endl;
    for (int r = 0; r < this->width; r++){
        for (int c = 0; c < (this->height * 3); c++){
            int pixelVal = ppmArr[r][c];
            ppmFile << pixelVal << " ";
        }
        ppmFile << endl;
    }

    }
};

/*
 * -- Clear Screen Function --
 * Prints out 25 lines of spaces, with endlines at the end, to keep the
 * user interface clean.
*/
void clearScreen(){
    for (int i = 0; i < 25; i++){
        cout << " " << endl;
    }
}

/*
 * -- Calculate and Mark Easiest Path -- 
 * Grabs the type of the DEM grid ("PPM" or "PGM") as a parameter
 * Calculates and marks the easiest path in the DEM grid based on elevation.
 * The path is marked with colors (blue for PPM, black for PGM).
*/
void DEMGrid::easiestPath(string type) {
    int nextCol;
    int currentRow;
    int minRowIndex;

    if (type == "PPM") {
        float ** _ppmArr = ppmArr;
        if (startRow >= 1 && startRow <= this->height) {
            currentRow = startRow - 1; 
            int currentCol = 0;

            while (currentCol < this->width) {
                nextCol = currentCol + 1;

                float minElevation = getdemArr()[currentRow][currentCol];
                minRowIndex = currentRow;

                for (int i = -1; i <= 1; i++) {
                    int neighborRow = currentRow + i;
                    if (neighborRow >= 0 && neighborRow < this->height) {
                        float neighborElevation = getdemArr()[neighborRow][nextCol];
                        if (neighborElevation < minElevation) {
                            minElevation = neighborElevation;
                            minRowIndex = neighborRow;
                        }
                    }
                }
                
                _ppmArr[currentRow][currentCol * 3] = 0;
                _ppmArr[currentRow][currentCol * 3 + 1] = 0;
                _ppmArr[currentRow][currentCol * 3 + 2] = 255;

                currentCol = nextCol;
                currentRow = minRowIndex;
            }
            clearScreen();
            cout << "------------------------------------" << endl;
            cout << "Easiest path calculated and colored." << endl;
            cout << "------------------------------------" << endl;
        }
        else {
            clearScreen();
            cout << "Invalid starting row, or no starting row. Please enter a valid row." << endl;
        }
    }
    else if (type == "PGM") {
        float** _pgmArr = pgmArr;
        if (startRow >= 1 && startRow <= this->height) {
            currentRow = startRow - 1;
            int currentCol = 0;

            while (currentCol < this->width) {
                nextCol = currentCol + 1;

                float minElevation = getdemArr()[currentRow][currentCol];
                minRowIndex = currentRow;

                for (int i = -1; i <= 1; i++) {
                    int neighborRow = currentRow + i;
                    if (neighborRow >= 0 && neighborRow < this->height) {
                        float neighborElevation = getdemArr()[neighborRow][nextCol];
                        if (neighborElevation < minElevation) {
                            minElevation = neighborElevation;
                            minRowIndex = neighborRow;
                        }
                    }
                }
                
                _pgmArr[currentRow][currentCol] = 0; 

                currentCol = nextCol;
                currentRow = minRowIndex;
            }
            clearScreen();
            cout << "------------------------------------" << endl;
            cout << "Easiest path calculated and colored." << endl;
            cout << "------------------------------------" << endl;
        }
        else {
            clearScreen();
            cout << "Invalid starting row, or no starting row. Please enter a valid row." << endl;
        }
    }
    else {
        clearScreen();
        cout << "Cannot calculate easiest path without PGM/PPM Object." << endl;
        return;
    }
};

/*
 * -- Calculate and Mark Hardest Path -- 
 * Grabs the type of the DEM grid ("PPM" or "PGM") as a parameter
 * Calculates and marks the hardest path in the DEM grid based on elevation.
 * The path is marked with colors (red for PPM, black for PGM).
*/
void DEMGrid::hardestPath(string type) {
    int nextCol;
    int currentRow;
    int maxRowIndex;

    if (type == "PPM") {
        float** _ppmArr = ppmArr;
        if (startRow >= 1 && startRow <= this->height) {
            currentRow = startRow - 1; 
            int currentCol = 0;

            while (currentCol < this->width) {
                nextCol = currentCol + 1;

                float maxElevation = getdemArr()[currentRow][currentCol];
                maxRowIndex = currentRow;

                for (int i = -1; i <= 1; i++) {
                    int neighborRow = currentRow + i;
                    if (neighborRow >= 0 && neighborRow < this->height) {
                        float neighborElevation = getdemArr()[neighborRow][nextCol];
                        if (neighborElevation > maxElevation) {
                            maxElevation = neighborElevation;
                            maxRowIndex = neighborRow;
                        }
                    }
                }
                
                _ppmArr[currentRow][currentCol * 3] = 255;
                _ppmArr[currentRow][currentCol * 3 + 1] = 0;
                _ppmArr[currentRow][currentCol * 3 + 2] = 0; 

                currentCol = nextCol;
                currentRow = maxRowIndex;
            }
            clearScreen();
            cout << "------------------------------------" << endl;
            cout << "Hardest path calculated and colored." << endl;
            cout << "------------------------------------" << endl;
        }
        else {
            clearScreen();
            cout << "Invalid starting row, or no starting row. Please enter a valid row." << endl;
        }
    }
    else if (type == "PGM") {
        float** _pgmArr = pgmArr; //2d arr copy
        if (startRow >= 1 && startRow <= this->height) {
            currentRow = startRow - 1; 
            int currentCol = 0;

            while (currentCol < this->width) {
                nextCol = currentCol + 1;

                float maxElevation = getdemArr()[currentRow][currentCol];
                maxRowIndex = currentRow;

                for (int i = -1; i <= 1; i++) {
                    int neighborRow = currentRow + i;
                    if (neighborRow >= 0 && neighborRow < this->height) {
                        float neighborElevation = getdemArr()[neighborRow][nextCol];
                        if (neighborElevation > maxElevation) {
                            maxElevation = neighborElevation;
                            maxRowIndex = neighborRow;
                        }
                    }
                }
                
                _pgmArr[currentRow][currentCol] = 0; 

                currentCol = nextCol;
                currentRow = maxRowIndex;
            }
            clearScreen();
            cout << "------------------------------------" << endl;
            cout << "Hardest path calculated and colored." << endl;
            cout << "------------------------------------" << endl;
        }
        else {
            clearScreen();
            cout << "Invalid starting row, or no starting row. Please enter a valid row." << endl;
        }
    }
    else {
        clearScreen();
        cout << "Cannot calculate hardest path without PGM/PPM Object." << endl;
        return;
    }
}

/*
 * -- Adjust Starting Row -- 
 * Prompts the user to enter the starting row for the path, taken from West to East.
 * Clears the console screen after input and displays the chosen starting position.
 */
void DEMGrid :: adjustSRow(){
    cout << "Please enter the starting row for the path, to be taken from West to East. :" << endl;
    cin >> this->startRow;
    clearScreen();
    cout << "Your starting position is row " << this->startRow << endl;
};

int main(){

    char entry;
    DEMGrid * selectedObject = nullptr;
    string type = "";

    callMenu();
    cin >> entry;

    while (entry != 'q'){
        if (entry == 'g'){
            selectedObject = new PGM();
            type = "PGM";
        }
        else if (entry == 'p'){
            selectedObject = new PPM();
            type = "PPM";
        }
        else if (entry == 'd'){
            float ** arr = selectedObject->getdemArr();
            int width = selectedObject->getWidth();
            int height = selectedObject->getHeight();
            if (type == "PGM"){
                //rows by cols
                clearScreen();
                cout << "--------------------------------" << endl;
                cout << "| Rows : " << width << " | Cols : " << height << " |" << endl;
                cout << "--------------------------------" << endl;
            }
            else if (type == "PPM"){
                clearScreen();
                cout << "--------------------------------" << endl;
                cout << "| Rows : " << (width * 3) << " | Cols : " << height << " |" << endl;
                cout << "--------------------------------" << endl;
            }
            else{
                clearScreen();
                cout << "----------------------------------------------------" << endl;
                cout << "No object has been created. Please create one first." << endl;
                cout << "----------------------------------------------------" << endl;
            }
        }
        else if (entry == 's'){
            selectedObject->adjustSRow();
        }
        else if (entry == 'l'){
            selectedObject->easiestPath(type);
        }
        else if (entry == 'm'){
            selectedObject->hardestPath(type);
        }
        else if (entry == 'w'){
            if (type ==  "PGM"){
                selectedObject->writeFile(type);
                clearScreen();
            }
            else if (type == "PPM"){
                selectedObject->writeFile(type);
                clearScreen();
            }
            else{
                cout << "----------------------------------------------------" << endl;
                cout << "No object has been created. Please create one first." << endl;
                cout << "----------------------------------------------------" << endl;
                clearScreen();
            }
        }
        else if (entry == 'q'){
            entry = 'q';
        }

        callMenu();
        cin >> entry;
    }

    return 0;
}