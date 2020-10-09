
#include <stdio.h>
#include <stdlib.h> //for calloc
#include <time.h>       /* time */           
#include <string.h>
#include <math.h>       //used in option 6 

//macro K defines the numeric accuracy.
#define K 30
#define SPEED 60
#define LETSPLAY "letsPlay.txt"
#define BARCODEFILE "package.bmp"
#define NEWBARCODE "newbarcode.bmp"
#define MAP "map.txt"
#define ROUTE "route.txt"
#define FUEL "fuel.txt"

 



//-----------------------------------------
typedef struct stores { // This structure helps represent a map
    int num; // store/factory number 0-10
    int exists; // 1 - this store exists in map.txt, 0 - does not exists. start of route
    int r1, r2, r3;  //routes
    struct stores *c1;
    struct stores *c2;
    struct stores *c3;
} store;

typedef struct packages { // This structure helps representing a list of barcode
    int gift;
    int dest;
    int id;
    struct packages *next;
} package_t;

typedef struct newbarcode { // This structure helps representing a list of barcode
    int gift[3];
    int dest[2];
    int id[6];
    int checksum;
} newbarcode_t;

typedef struct nextstore { // This structure helps representing a path for a driver
    int distTotal;  //accumulating distance in a path
    int packCount;  //packages for a store
    int packTotal;  //accumulating packages in a path
    int store;      //a number 0-10
    struct nextstore *next;
}path_t;

typedef struct inventory {
    int num;
    struct packages *head;
}current_store;

//*********** functions declaration - prototypes ************************//
int getOption();        // this function return a digit number entered or 0 if something else than a digit was enterd

//Function #1
int option1ReadUPC();   // this option triggers functions that read a barcode image file and stores it if it is valid.
int getOffset(FILE * fp);       // get the data begining position of the barcode image
int getHight(FILE * fp);            // get the hight of the barcode image
int getWidth(FILE * fp);            // get the width of the barcode image
int read_UPC_data_From_Bitmap(int[]);           // read a UPC barcode BMP file to a 0s (represent white) and 1s (represent black) array.
int Convert_BW_UPC_2_Number(char*, int*);   // decode the UPC black and white int array to a string of barcode digits
int Add_Barcode_2_File(char*);              // write the barcode to the bottom of the storage file and a message to the screen
int Validate_CheckSum(char* sn);        // return 0 if checksum correct else 1
int Check_ID_Duplicates(char*);             // return 0 if ID is unique in LETSPLAY, else returns 1

//Function #2
int option2GenUPC();                    // this option generates new barcode.bmp file
int genChecksum(newbarcode_t barcode1);//v
int randomize(int num, int random[]);//change this name
void barcodeHeader(int hight, int width, FILE* barcode);//v
int createBMPfile(int upc[]);//v
int genUPCfromBarcode(newbarcode_t bar, int* upc);//v
void genPixelArray(int hight, int width, FILE* barcode, int upc[]);//v
                                                                   //int getDestFromUser();     //get destination input store function, to be changed later by the almighty Anton
//Function 3                                                                  //Function #3
void option3ListPacks();
int get_dest_from_user();
void free_list_pack(package_t* head);
package_t* read_letsplay_fun();
package_t* reverse_order_of_list(package_t* head);
void print_packages(package_t *head, int inp);
int check_gift_number(package_t *head, int gift, int inp, int id);
int check_package_amount(package_t *head, int inp);



//Function #4
void option4PrintsPathes();     //This option prints all the paths that exist in map.txt originating at the factory store (num 0)
void free_store_tree(store* head);// free tree
store* build_tree(store* stores, int visits[]);//build tree
int build_paths(store* stores);//build paths
void print_paths(store* head, int path[], int counter);//prints paths


//Function #5
void option5DriverPath();   /*this 5th option uses 4th readstores,create_EmptyStoreNode,map2tree,freeStoreTree*/
                            //store* createLinkedPaths(int num, int storesMat[33][3], store* existingStores[11]); //replace
int findAllFactoryRoutes(store* head, int ValidRoutes[][12], int validRoutesDist[][12]);
path_t* createPathFromArray(int * path, int* dist, FILE* letsPlay);
void printBestPath(path_t* path);
int comparePaths(path_t* p1, path_t* p2);
int getPackCountTo(int storeNum, FILE* LetsPlay);
void findAllFactoryRoutesRec(store* head, int currentDist, int* tmp, int* distTmp, int tmpIndex, int ValidRoutes[][12], int * i, int* hasBeen, int ValidRoutesDist[][12]);
void copyToArray(int dest[], int source[], int size);
void freePath(path_t*);     // frees the path

//Function #6
void option6FuelConsumption();  /*this 6th option use 4th readstores,create_EmptyStoreNode,map2tree,freeStoreTree,insert2path....*/
void fuelConsumption(path_t *path);
int getTotalDistance(path_t * path);
int getTotalPackages(path_t * path);
int getFuel();
float getFuelAtTimet(float t, int c, int packs);

//Function #7
void option7();
int fill_inventory(current_store *store, int inp);
void print_inventory(package_t *head, int inp, package_t *other_head, package_t *sold_head);
void ask_name(char* user_name);
int sort_packages(current_store *store, int inp, current_store *other_stores, int total_pack);
void check_sales(current_store *store, current_store *sold, int average, int total_pack);
int exists_in_sold(package_t *head, package_t* sold_list);
int get_total_packages7(package_t *head);
void print_logo();

//*********** functions declaration - End ************************//


// Main function gets 
int main(int argc, char** argv)
{
    int choice = 0;
    do {
        printf("--------------------------\nBoNessahek delivery services\n--------------------------\nMenu:\n");
        printf("1. Scan barcode.\n2. Generate barcode.\n3. List Store packages.\n4. Genrate all routes\n5. Generate driver route.\n6. Fuel consumption report.\n7. Students addition\n9. Exit.\nEnter choice: ");
        choice = getOption();
        printf("\n");
        switch (choice) {
        case 1: option1ReadUPC(); break;
        case 2: option2GenUPC(); break;
        case 3: option3ListPacks(); break;
        case 4: option4PrintsPathes(); break;
        case 5: option5DriverPath(); break;
        case 6: option6FuelConsumption(); break;
        case 7: option7(); break;
        case 9: printf("Good bye!\n"); break;
        default: printf("Bad input, try again\n\n");
        }
    } while (choice != 9);
}

//-----------------------------------------

/*this function returns the number entered
@return - a digit choice, if the input is not in the correct format #\n than return 0
*/
int getOption() //gets option from user
{
    
    char inputStr[10000] = { 0 };   
    gets(inputStr);

    int digit = atoi(inputStr);
    return digit;
}


/* option 1 read a upc barcode from a bmp file and store it in a file if:
1) its id is unique
2) it has a correct checksum digit
@return - 0 if success, else return 1
*/

int option1ReadUPC()
{
    int BW_UPC[95];                 // black and white upc code black=1 ; white=0 
    char* UPC_12 = calloc(13, 1);   // a string for a 12 digit barcode serial numbers  (12 + 1 string) 
    char ID[6];                     // holds the ID
    int i = 0;

    // return 1 upon failure to read the BMP file barcode
    if (read_UPC_data_From_Bitmap(BW_UPC))
        return 1;

    //return 1 upon failure to decod BW_UPC array to a valid barcode str
    if (Convert_BW_UPC_2_Number(UPC_12, BW_UPC))
        return 1;

    //read ID from UPC_12
    for (i = 0; i < 6; i++)
    {
        ID[i] = UPC_12[i + 5];
    }

    //check if ID already exists in letsplay.txt
    if (Check_ID_Duplicates(ID))
        printf("Barcode %s is already in %s.\n\n", UPC_12, LETSPLAY);

    //if barcode ID number is not already in letsplay.txt insert to letsplay.txt
    else if (!Add_Barcode_2_File(UPC_12))
        printf("%s was inserted to the warehouse.\a\n\n", UPC_12);

    free(UPC_12);   //free memory block of barcode's Serial Number
    return 0;
}

/* read a UPC barcode BMP file.
@param upc[95] - array of 1s and 0s that holds sequenses of
alternating barcode black and white lines' representation
@returns 0 on success 1 if fail
*/
int read_UPC_data_From_Bitmap(int upc[])
{

    FILE* barcode;      //pointer to package.bmp
    int i;
    int success;        //flag - checks if file opening was successful

    success = fopen_s(&barcode, BARCODEFILE, "rb");
    if (success != 0)   //return 1 on failure to open package.bmp  
    {
        printf("can't open barcode file\n");
        return 1;
    }

    int offset = getOffset(barcode);    //get file offset to pixel array
    int width = getWidth(barcode);      //bitmap width in pixels 
    int height = getHight(barcode);     //bitmap hight in pixels
    unsigned char r, g, b;
    int bmp_middle = height / 2;
    int Pixel_Array[80][150] = { 0 };   //saves sequences of three 0's and 1's as pixels (from packages.bmp) 
    int j, k, rows = 0, cols = 0;       //cols and rows of mat

    fseek(barcode, offset, SEEK_SET);   //set the file position of the stream to the beginning of the file

                                        //creat pixel array
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            //read from bmp and get r,g,b values
            r = fgetc(barcode);
            g = fgetc(barcode);
            b = fgetc(barcode);

            if (r == 0 && g == 0 && b == 0)     //assign 1 to all black pixels 
            {
                Pixel_Array[rows][cols++] = 1;
            }
            else
            {
                Pixel_Array[rows][cols++] = 0;      //assign 0 to all white pixels
            }
        }

        rows++;
        cols = 0;

        //padding the bmp if width does not devide by 4
        if (width % 4 != 0)
        {
            for (k = 1; k <= (width % 4); k++)
            {
                fgetc(barcode);
            }
        }
    }



    //loop untill first black pixel
    for (i = 0; i < width; i++)
    {

        if (Pixel_Array[bmp_middle][i] != 1)
        {
            continue;
        }

        break;
    }

    //initialize upc[95] with 0's and 1's
    for (j = 0; j < 95; j++)
    {

        upc[j] = Pixel_Array[bmp_middle][i + j];
    }

    //check that the barcode guards are in right place
    if ((upc[0] != 1) ||
        (upc[1] != 0) ||
        (upc[2] != 1) ||
        (upc[92] != 1) ||
        (upc[93] != 0) ||
        (upc[94] != 1) ||
        (upc[45] != 0) ||
        (upc[46] != 1) ||
        (upc[47] != 0) ||
        (upc[48] != 1) ||
        (upc[49] != 0))
    {
        return 1;
    }


    //close the file
    fclose(barcode);

    return 0;
}


/*according to:
0 = 3211, 1 = 2221, 2 = 2122, 3 = 1411, 4 = 1132, 5 = 1231, 6 = 1114, 7 = 1312, 8 = 1213, 9 = 3112
convert binary to decimal
@param str - holds the 12 digits of the UPC string
@param upc[95] - array space of 0s and 1s
@return - 0 if success, else returns 1
*/
int Convert_BW_UPC_2_Number(char* str, int* upc) {

    //standardized convention for converting binary to decimal 
    int sequences[10][4] =
    {
    { 3,2,1,1 },
    { 2,2,2,1 },
    { 2,1,2,2 },
    { 1,4,1,1 },
    { 1,1,3,2 },
    { 1,2,3,1 },
    { 1,1,1,4 },
    { 1,3,1,2 },
    { 1,2,1,3 },
    { 3,1,1,2 }
    };

    int tmpSeq[4] = { 0 };
    int tempSeqIndex = 0;

    int count = 1; //counts the amount of pixels of same color in a row 
    int i, j;
    char* strTracker = str;

    //loop from first guard till last guard is encountered 
    for (i = 3; i < 92; ++i)
    {

        // i goes from bmp_middle to last gurad of the barcode
        if (i == 45)
        {
            i += 5;
        }

        if (upc[i] == upc[i + 1])   //count how many pixels of same color in a row
        {
            count++;
            continue;
        }

        tmpSeq[tempSeqIndex++] = count;
        count = 1;          //reinitialize the counter

                            //convert to decimal numbers
        if (tempSeqIndex == 4)
        {
            for (j = 0; j < 10; j++)
            {
                if (tmpSeq[0] == sequences[j][0] && tmpSeq[1] == sequences[j][1] && tmpSeq[2] == sequences[j][2] && tmpSeq[3] == sequences[j][3])
                {
                    *strTracker++ = j + '0';    //'0' converts int to char because *strTracker must get a pointer to char
                    break;
                }
            }

            if (j == 10)
            {
                return 1;
            }

            tempSeqIndex = 0;
        }
    }

    //return - 0 if success, else returns 1
    return !Validate_CheckSum(str) ? 0 : 1;

}


/* gets the offset to the pixel data
return value - offset to pixel data
-no data validation is done here
*/
int getOffset(FILE * fp)
{
    int offset = 0;
    fseek(fp, 10, SEEK_SET);    //set file position to offset from beginning of stream
    int i;
    for (i = 0; i<4; i++) offset |= fgetc(fp) << (i * 8);
    return offset;
}

/* gets the image width
return value - image width
-no data validation is done here
*/
int getWidth(FILE * fp)     //gets pointer to package.bmp   
{
    int width = 0;
    fseek(fp, 18, SEEK_SET);    //set file position to offset from beginning of stream
    int i;
    for (i = 0; i<4; i++) width |= fgetc(fp) << (i * 8);    //reads binary file in little endian 
    return width;
}

/* get the image hight
return value- image hight
-no data validation is done here
*/
int getHight(FILE * fp)
{
    int hight = 0;
    fseek(fp, 22, SEEK_SET);        //set file position to offset from beginning of stream
    int i;
    for (i = 0; i<4; i++) hight |= fgetc(fp) << (i * 8);
    return hight;
}

/*Enter the barcode number to the bottom of the file
@param upcNumber - the barcode string
@return 0 on success
*/
int Add_Barcode_2_File(char* upcNumber)
{


    FILE* letsPlay;     //pointer to file
    int success = fopen_s(&letsPlay, LETSPLAY, "a");    //flag for sucsseful opening of file

                                                        //returns 1 on failur to open letsplay.txt
    if (success != 0)
    {
        return 1;
    }

    fprintf(letsPlay, "%s\n", upcNumber);       //write barcode SN to letsplay.txt
    fclose(letsPlay);   //close the file
    return 0;           //returns 0 upon success 
}

/* check if there is an identicle ID already in file
@PARAM upc - holds the package ID (6 digits)
@RETURN - 0 if UPC ID is unique, else returns 1
*/
int Check_ID_Duplicates(char* upc)
{
    char barcode[13] = { 0 };
    int i, isEqual = 0;
    FILE* letsPlay;     //pointer to file
    int success;    //flag - checks if file opening was successful 
    success = fopen_s(&letsPlay, LETSPLAY, "r");

    //flag for sucsseful opening of file
    if (success != 0)
    {
        printf("letsPlay.txt was not found and will be created.\n");
        fopen_s(&letsPlay, LETSPLAY, "w");      //open a new file for letsplay.txt
        fclose(letsPlay);       //close the file letsplay.txt
        return 0;
    }

    while (fgets(barcode, 13, letsPlay))    //loops until EOF is encountered 
    {                                       //scans all existing barcodes in letsplay.txt 

        if (barcode[0] == '\n')             //if end of barcode's SN in encountered - continue
        {
            continue;
        }

        //check duplications od barcode's ID
        for (i = 0; i < 6; i++)
        {
            if (barcode[i + 5] != upc[i])
            {
                break;
            }
        }

        //close file when in end of barcode SN
        if (i == 6)
        {
            fclose(letsPlay);
            return 1;
        }
    }

    //close file 
    fclose(letsPlay);
    return 0;
}

/* check if checksum correct
@param sn - holds the upc number (12 digits)
@return - 0 if checksum is correct, else returns 1
*/
int Validate_CheckSum(char* sn)
{

    int decimalDigits[12] = { 0 };
    int i, sum = 0, tenMultiple = 0;

    //validates barcode serial number according to formula on attached link provided by you
    for (i = 0; i < 11; i++)
    {
        if (i % 2 == 0)
        {
            decimalDigits[i] = (sn[i] - '0') * 3;
        }
        else
        {
            decimalDigits[i] = sn[i] - '0';
        }

        sum += decimalDigits[i];
    }

    tenMultiple = sum;

    while (tenMultiple % 10 != 0)
    {
        tenMultiple++;
    }

    sum = tenMultiple - sum;

    int store = (sn[3] - '0') * 10 + sn[4] - '0';
    if (sum == (sn[11] - '0') && store<9 && store>0) {
        return 0;
    }
    return 1;
}

//---------------------------------------

//---------------------------------------

/* option 2 gets a desination Store and create a upc barcode bmp file with:
1) unique id among the barcodes in the store file
2) a correct checksum digit
and also store it at the buttom of the sore file.
@return - 0 if success, else returns 1
*/
int option2GenUPC()
{

    int dest = get_dest_from_user();//get store number from user

    newbarcode_t barcode1;//generated the barcode number
    int gift[3], id[6];
    /* Intializes random number generator */
    srand(time(0));
    int i;

    randomize(3, gift); // get random numbers for gift
                        //copy gift to newbarcode struct
    for (i = 0; i < 3; i++) {
        barcode1.gift[i] = gift[i];
    }

    char idStr[7] = { 0 };
    //loop until id is unique
    do {
        int i;
        randomize(6, id); // get random numbers for id
        for (i = 0; i<6; i++) {
            idStr[i] = id[i] + '0';
        }
        idStr[i] = '\0';

    } while (Check_ID_Duplicates(idStr));//check if id exists already

    for (i = 0; i < 6; i++) {//insert id into the barcode
        barcode1.id[i] = id[i];
    }

    //copy the store number to newbarcode struct
    if (dest == 10) {
        barcode1.dest[0] = 1;
        barcode1.dest[1] = 0;
    }
    else {
        barcode1.dest[0] = 0;
        barcode1.dest[1] = dest;
    }

    //calc the check digit
    int check = genChecksum(barcode1);
    barcode1.checksum = check;

    //make a barcode color array - initialized at 0
    int upc[95] = { 0 };

    //update the upc array of 95 binary digits
    if (genUPCfromBarcode(barcode1, upc))
    {
        return 1;
    }


    return 0;
}


/*gets the 12 digit barcode struct
returns upc array of 95 binary digits
*/
int genUPCfromBarcode(newbarcode_t bar, int* upc)
{
    //sequences of bits representing 1 digit - left side
    int fromFirstGuard[10][7] = { { 0,0,0,1,1,0,1 },{ 0,0,1,1,0,0,1 },{ 0,0,1,0,0,1,1 },{ 0,1,1,1,1,0,1 },{ 0,1,0,0,0,1,1 },{ 0,1,1,0,0,0,1 },{ 0,1,0,1,1,1,1 },{ 0,1,1,1,0,1,1 },{ 0,1,1,0,1,1,1 },{ 0,0,0,1,0,1,1 } };

    //sequences of bits representing 1 digit - right side
    int fromMiddleGuard[10][7] = { { 1,1,1,0,0,1,0 },{ 1,1,0,0,1,1,0 },{ 1,1,0,1,1,0,0 },{ 1,0,0,0,0,1,0 },{ 1,0,1,1,1,0,0 },{ 1,0,0,1,1,1,0 },{ 1,0,1,0,0,0,0 },{ 1,0,0,0,1,0,0 },{ 1,0,0,1,0,0,0 },{ 1,1,1,0,1,0,0 } };
    int i = 0, currentDigit, counter = 0, barcodeIndex = 0, j;

    int barcode[12] = { 0 };

    for (i = 0; i < 3; i++) {//fill the whole number from the gift, dest, id and checksum variables
        barcode[i] = bar.gift[i];
    }

    for (i = 3; i < 5; i++) {
        barcode[i] = bar.dest[i - 3];
    }

    for (i = 5; i < 11; i++) {
        barcode[i] = bar.id[i - 5];
    }

    barcode[i] = bar.checksum;

    //first divider
    upc[0] = 1; upc[1] = 0; upc[2] = 1;

    //last divider
    upc[92] = 1; upc[93] = 0; upc[94] = 1;

    //middle divider
    upc[45] = 0; upc[46] = 1; upc[47] = 0; upc[48] = 1; upc[49] = 0;

    //first half of the array
    for (i = 3; i < 92; i += 7)
    {

        if (i == 45) {
            i += 5;
        }
        currentDigit = barcode[barcodeIndex++]; //convert char to int

                                                //if we are on the left side of the barcode
        if (i < 45) {
            //copy the sequence to upc from fromFirstGuard array of bits
            for (j = i; j <= i + 7; j++) {
                upc[j] = fromFirstGuard[currentDigit][j - i];
            }
        }
        //if we are on the right side of the barcode
        else {
            //copy the sequence to upc from fromMiddleGuard array of bits
            for (j = i; j <= i + 7; j++) {
                upc[j] = fromMiddleGuard[currentDigit][j - i];
            }
        }
    }

    //call function to create an image from the upc array
    if (createBMPfile(upc))
    {
        return 1;
    }

    char barcodeStr[13] = { 0 };
    for (i = 0; i < 12; i++)
    {
        barcodeStr[i] = barcode[i] + '0';//turn number into number in string format
    }
    barcodeStr[i] = '\0';//end of string array

    if (!Add_Barcode_2_File(barcodeStr))//adds the barcode to the file
        printf("%s was inserted to the warehouse.\a\n\n", barcodeStr);

    return 0;
}


/*this function creates bmp file out of upc array
0 - success
1 - failure
*/
int createBMPfile(int upc[])
{
    FILE* newbarcode;
    if (fopen_s(&newbarcode, NEWBARCODE, "wb") != 0) //if cant open the file
    {
        printf("Can't create file newbarcode.bmp\n");
        return 1;
    }
    int width = 140;
    int hight = 71;

    barcodeHeader(hight, width, newbarcode);// create the header of the image
    genPixelArray(hight, width, newbarcode, upc);//puts the pixels into the file


    fclose(newbarcode);
    return 0;
}

/*write header and header info to new bmp file
*/

void barcodeHeader(int hight, int width, FILE* barcode)
//must be long cause you have to write at leat 54 bits
//must write each bit by itself, and add comments so thats why its long
{

    int filesize = 54 + 3 * width*hight;  //define file size by width and hight

                                          //signature, BM
    fputc(0x42, barcode);
    fputc(0x4d, barcode);

    //file size to header
    fprintf(barcode, "%x", filesize);

    //reserved
    fputc('h', barcode);
    fputc('i', barcode);
    fputc('d', barcode);
    fputc('e', barcode);

    //offset to pixel array
    fputc(54, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //file header info
    fputc(0x28, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //width in pixel
    fputc(width, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //hight in pixels
    fputc(hight, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //number of colors to be used - 1
    fputc(0x1, barcode);
    fputc(0, barcode);

    //number of bits per pixels (24)
    fputc(0x18, barcode);
    fputc(0, barcode);

    //compression being used, 0
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //image size RAW image size
    fprintf(barcode, "%x", filesize - 54);

    //horizontal resolution
    fputc(0x0, barcode);
    fputc(0x0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //vertical resolution
    fputc(0x0, barcode);
    fputc(0x0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //number or color palette 0
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

    //number of important colors - 0 is all are important
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);
    fputc(0, barcode);

}

/*
This funtion writes the pixel array to new barcode bmp file
*/
void genPixelArray(int hight, int width, FILE* barcode, int upc[]) {

    int i, j, upcCounter = 0;
    unsigned char whitePixel = 255;
    unsigned char blackPixel = 0;

    //set the file cursor to offset 54
    fseek(barcode, 54, SEEK_SET);

    //pixel is represented by 3 bytes

    //fill the 12 first rows with white pixels
    for (i = 0; i < 12 * width; i++) {
        fputc(whitePixel, barcode);
        fputc(whitePixel, barcode);
        fputc(whitePixel, barcode);
    }


    for (i = 0; i < 4; i++) {//generate the 3 long seperators, 4 pixels longer then the rest
        for (j = 0; j < width; j++) {
            if (j<20 || (j>22 && j<66) || (j>68 && j<112) || j>114) {//puts a white inbetween the black seperators
                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);
            }
            else {//puts the long seperators black-white-black
                fputc(blackPixel, barcode);
                fputc(blackPixel, barcode);
                fputc(blackPixel, barcode);

                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);

                fputc(blackPixel, barcode);
                fputc(blackPixel, barcode);
                fputc(blackPixel, barcode);

                j += 2;//jumps 2 widths up cause we put 2 more colors per width
            }
        }

    }


    for (i = 0; i < 46; i++) {
        for (j = 0; j < width; j++) {
            if (j<20 || j>114) {
                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);
                fputc(whitePixel, barcode);
            }
            else {
                if (upc[upcCounter] == 0) {
                    fputc(whitePixel, barcode);
                    fputc(whitePixel, barcode);
                    fputc(whitePixel, barcode);
                }
                else {
                    fputc(blackPixel, barcode);
                    fputc(blackPixel, barcode);
                    fputc(blackPixel, barcode);
                }

                upcCounter++;

                if (upcCounter == 95) {
                    upcCounter = 0;
                }
            }
        }
    }

    for (i = 0; i < 9 * width; i++) {
        fputc(whitePixel, barcode);
        fputc(whitePixel, barcode);
        fputc(whitePixel, barcode);
    }

}


/*calc the check digit
*/
int genChecksum(newbarcode_t barcode1) {// generate checksum
    int check[11];
    for (int i = 0; i < 3; i++) {//get the first 3 digits of the number
        check[i] = barcode1.gift[i];
    }

    for (int i = 3; i < 5; i++) {//get the 3-5 digits of the number
        check[i] = barcode1.dest[i - 3];
    }

    for (int i = 5; i < 11; i++) {//get the 5-11 digits of the number
        check[i] = barcode1.id[i - 5];
    }

    int finalCheck = 0, oddCheck = 0, evenCheck = 0;
    int checksum;
    for (int i = 0; i < 11; i++) {//checks if a number is even or odds
        if (i % 2 == 0) {//for even number, add the number value to the even check
            evenCheck = evenCheck + check[i];
        }
        else {//add the number value to odd check
            oddCheck = oddCheck + check[i];
        }
    }
    finalCheck = oddCheck + evenCheck * 3;//triples even check, and 1 time the odd check
    checksum = finalCheck;

    if (checksum % 10 == 0) {//check if the number is dividable by 10
        return 0;
    }

    return 10 - (checksum % 10);//if not, deduce the extra from 10 and then thats the checksum
}



int randomize(int num, int random[]) {
    int i;

    /* Print num random numbers from 0 to 9 */
    for (i = 0; i < num; i++) {
        random[i] = rand() % 10;
    }
}




//-----------------------------------------

/* extruct packages for a specific destination and print them sorted by id
*/

//-----------------------------------------------
//function #3 main function and its subfunctions
//-----------------------------------------------

// extruct packages for a specific destination and print them sorted by id
void option3ListPacks()
//Written by Anton Nicolaevsky, 309327930
{
    package_t *head = NULL;                 // pointer to head of linked list of packages
    int inp;                                // input variable

    inp = get_dest_from_user();             // gets destination store from user, 1-10 are valid destinations
    head = read_letsplay_fun();             // gets pointer to the linked list of all factory packages
    if (head)                               //checks if head is a null pointer due to some error or no map.txt existing
    {
        head = reverse_order_of_list(head); //reverse order of list to be as in the map.txt
        print_packages(head, inp);          // prints all relevant 
        free_list_pack(head);               // free list
    }
}

//a standard input function
int get_dest_from_user()
//Written and amended by Ben Ayash, Lee Joe Barak, Anton Nicolaevsky
{
    char str[100] = { 0 };                                      //initializing an array for input string
    int destination = -1;                                       //initializing destination variable
                                                                //in order to get a valid input, make user's life hell until he/she puts a valid input.
    do
    {

        printf_s("Enter destination store (1-10)\n");           //prints request for input unto the screen
        fgets(str, 100, stdin);                                 //gets the actual input from user
        if (str[0] == 10)                                       //checks wether the user just entered '\n', new line (hit enter)
        {
            printf_s("Too Short!\n");                           //prints a scolding message unto the screen
            continue;                                           //continues to next itteration of asking the user to give a valid input
        }
        destination = atoi(str);                                //gets the integer value of the input string
        if (destination <1 || destination > 10)                 //checks wether the input was of valid integers
        {
            printf_s("Not valid!\n");                           //prints a scolding message unto the screen
        }

    } while (destination < 1 || destination > 10);              //while input is invalid, continue to next itteration

    return destination;                                         //return destination store number
}


//Free package linked list, this function is called whenever the linked list is has no use anylonger
void free_list_pack(package_t* head)
//Written by Anton Nicolaevsky, 309327930
{
    package_t *ptr = NULL;                      //a helping pointer
    int i;                                      //a counter
    for (i = 0; head != NULL; i++)              //a for loop that continues until all linked list is free
    {
        ptr = head->next;                       //saves the address of the next item on the list
        free(head);                             //frees the current item of the list
        head = ptr;                             //progresses the head of list pointer to the next item of the list
    }
}

//The actual working horse of function #3, opens and reads the map.txt and builds linkd list of packages
package_t* read_letsplay_fun()
//Written by Anton Nicolaevsky, 309327930
{
    package_t *pack = NULL, *head = NULL;               //pointers to be used for creating the linked list
    FILE *fptr = NULL;                                  //pointer for file stream
    int i;//a counter
    char ch, str[14] = { 0 };                           //ch: char variable to read from file, str[14]: a string to 

    fopen_s(&fptr, "letsPlay.txt", "rt");               //opens map.txt file in text read mode
    if (!fptr)                                          //if openning failed due to missing file
    {
        printf_s("can't find file letsPlay.txt\n");     //prints a warnning
        return NULL;                                    //return null pointer if no file exists
    }
    //a loop thath in each itteration a barcode string is read from the file and puts it in an item of a linked list
    do
    {
        ch = 48;                                        //reinitialising the variable character as a 0
        for (i = 0; ch != 10; i++)
            //a loop that reads characters from the file and puts in a string to be used in fields of the list item, each bardode ends with '\n'
        {
            if ((ch = fgetc(fptr)) == EOF)              //reads from file and puts in ch and also checks if it's end of file
            {
                fclose(fptr);                           //closes file
                return head;                            // returns the head of the linked list
            }
            str[i] = ch;                                //puts character in the barcode array
        }
        pack = (package_t*)calloc(1, sizeof(package_t));
        //dynamically allocate an initialised structure of type package, all fields are either int 0 or null pointer in respect of the fields
        if (pack == NULL)
            //if the allocation failed due to overflow
        {
            printf_s("Overflow!\n");
            exit(0);
        }
        pack->gift = ((int)str[0] - 48) * 100 + ((int)str[1] - 48) * 10 + ((int)str[2] - 48);
        //gives the field gift number the value of the first 3 digits
        pack->dest = ((int)str[3] - 48) * 10 + ((int)str[4] - 48);
        //gives the field dest number the value of the next 2 digits
        pack->id = ((int)str[5] - 48) * 1000000 + ((int)str[6] - 48) * 100000 + ((int)str[7] - 48) * 10000 + ((int)str[8] - 48) * 1000 + ((int)str[9] - 48) * 100 + ((int)str[10] - 48) * 10 + ((int)str[11] - 48);
        //gives the field id number the value of the next and last 7 digits
        pack->next = head;
        //gives the next field the address of the previously allocated item
        head = pack;
        //gives the head of the linked list the currently allocated item
    } while (1);
}

//changes the order of the linked list, to represent the order in map.txt
package_t* reverse_order_of_list(package_t* head)
//Written by Anton Nicolaevsky, 309327930
{
    package_t* curr = head, *prev = NULL, *next = NULL;
    //curr: currently pointed to item, prev: the previous item's address, next: next item's address
    while (curr != NULL)            //while not pointing at end of list
    {
        next = curr->next;          //saves next item's address
        curr->next = prev;          //gives previous item's address 
        prev = curr;                //sets prev to curr
        curr = next;                //sets curr to next address
    }
    head = prev;                    //gives head the completly reversed list
    return head;
}

//prints all packages that belong to this store
void print_packages(package_t *head, int inp)
//Written by Anton Nicolaevsky, 309327930
{
    int i, counter = 0;                                                 //a counter
    package_t *pack = head;                                             //saves the untouched list
    if (check_package_amount(pack, inp))                                //check if there are any packages for requested store
    {
        printf_s("\n\n----------------------------------------------------------\n\n\tList of packages for store number %d:\n\n----------------------------------------------------------\n\n\tID\t\tgift\n\t------\t\t-------\n", inp);
        for (i = 0; head != NULL; i++)
            //a loop which circles through all items in inventory and prints only relevant ones
        {
            if (head->dest == inp)                                      //checks if the package belongs to the store
            {
                printf_s("\n\t%07d\t\t%03d", head->id, head->gift);     //prints id number and gift number
                if (check_gift_number(pack, head->gift, inp, head->id))         //checks if there are the same gift number
                {
                    printf_s("\t\tDouble gift");                         //prints double gift message
                }
                counter++;
            }
            head = head->next;                                          //advances item on list
        }
        printf_s("\n\n----------------------------------------------------------\n\n\tTotal %d packages\n\n----------------------------------------------------------\n\n", counter);
    }
    else                                                                //if there are no packages to requested store
        printf_s("\n\n----------------------------------------------------------\n\n\tNo packages here!\n\n----------------------------------------------------------\n\n");
}

//checks wether two or more packages have the same gift number
int check_gift_number(package_t *head, int gift, int inp, int id)
//gets the head of the package list, gets the requested store number
{
    int i;                              // a counter
    for (i = 0; head != NULL; i++)      //a loop cycling through the package list
    {
        if (head->gift == gift && head->dest == inp && head->id != id)
            //if the gift  number matches and it's to the same store 
        {
            return 1;
        }
        head = head->next;              //advances the head
    }
    return 0;
}

//get the total amount of the packages
int check_package_amount(package_t *head, int inp)
{
    int i, counter = 0;                 //counters
    for (i = 0; head != NULL; i++)      //a loop which advances the counter until it cycled the whol list
    {
        if (head->dest == inp)
        {
            counter++;                  //advance counter
        }
        head = head->next;              //advance the head
    }
    return counter;                     //return the counter
}

//-----------------------------------------


//-----------------------------------------------
//function #4 main function and its subfunctions
//-----------------------------------------------

//This function prints all the pathes that exist in the map starting at 0
void option4PrintsPathes()
//Written and amended by Anton Nicolaevsky & Lee Joe Barak
{

    store *head = NULL;  // tree that spans all pathes from 0
    store stores[11] = { { 0 },{ 1 },{ 2 },{ 3 },{ 4 },{ 5 },{ 6 },{ 7 },{ 8 },{ 9 },{ 10 } };
    //array of structs - each individual store->num is intialized with a given int that's inside the curly brackets;
    //all other fields of each individual struct store is initialized with: NULL - for fields of type struct, 0 - for fields of type 
    int visits[11] = { 0 }, path[12] = { 0 }, path_counter = 0;
    //array of visits per store, array of the path, path counter

    if (build_paths(stores))                        //build all possible connections between the stores 
                                                    //checks if map.txt exists, if not exits the function
    {
        head = build_tree(&stores[0], visits);      //head gets factory address     
        print_paths(head, path, path_counter);      //print all paths
        free_store_tree(head);                      //free the tree
    }
}

//frees the built tree in post-order
void free_store_tree(store* head)
//written by Anton Nicolaevsky
{
    if (!head) return;                                  //returns when gets to the end of path
    free_store_tree(head->c1);                          //continues to c1 connection chain
    free_store_tree(head->c2);                          //continues to c2 connection chain
    free_store_tree(head->c3);                          //continues to c3 connection chain
    if (head->num == 0 && head->exists == 1) return;    //main head is stores[0]
    free(head);
}


store* build_tree(store* stores, int visits[])
//written by Anton Nicolaevsky
{
    store *new_store = NULL;
    //pointer for tree
    if (!stores)
        //null pointer
    {
        return NULL;
    }
    if (!stores->num && visits[0] == 1)
        //path returned to factory
    {
        new_store = (store*)calloc(1, sizeof(store));
        new_store->exists = 1;
        return new_store;
    }

    if (visits[stores->num] == 1)
        //already visited
    {
        return NULL;
    }

    visits[stores->num] = 1; //mark visits array

    new_store = (store*)calloc(1, sizeof(store));
    new_store->num = stores->num;
    new_store->exists = 1;
    new_store->r1 = stores->r1;
    new_store->r2 = stores->r2;
    new_store->r3 = stores->r3;

    new_store->c1 = build_tree(stores->c1, visits);
    new_store->c2 = build_tree(stores->c2, visits);
    new_store->c3 = build_tree(stores->c3, visits);

    visits[stores->num] = 0; //unmark visits array on the way back
    return new_store;
}


int build_paths(store* stores)
//Written by Anton Nicolaevsky
/*builds map of stores gets file pointer and array of stores with
the size of 11 initialized in the main subfunction
stores[11] = { {0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}},
it is done this way to not dynamically allocate new store.
when there are only 11 possibilities:
[0]-factory, [1-10] - stores, each element of that array is struct store.*/
{
    int inp[3];//this array saves the origin, destination, and distance given in each row in map.txt (for ease of use)
    FILE* map;                                                      //pointer to file map

    fopen_s(&map, "map.txt", "rt");                                 //openning the file
    if (!map)                                                       //if the file failed to open
    {
        printf("file map.txt not found\n\n");                       //print an error
        return 0;                                                   //exit function
    }

    do
    {
        if (fscanf_s(map, "%d", &inp[0], 1) == EOF) return; //saves origin store input and also functions as a exit condition 


        fscanf_s(map, "%d", &inp[1], 1);    // saves destination store input
        fscanf_s(map, "%d", &inp[2], 1);    //saves distance between source (origin) to dest.
        stores[inp[0]].exists = 1;  //flags from store's existence
        stores[inp[1]].exists = 1;  //flags to store's existence


                                    /*these "ifs" check whether a connection (c1, c2, or c3)
                                    is empty (meaning: hasn't been initialized with anything other than NULL)
                                    and if c1, c2, or c3, still hold a NULL value - the if's body initializes them with their respective
                                    connection (nest dest) according to given data in map.txt*/

        if (stores[inp[0]].c1 == NULL)                      //checks whether first connection's value is NULL 
        {
            stores[inp[0]].c1 = &stores[inp[1]];            //initialize store's c1 field with first possible destination (according to map.txt) 
            stores[inp[0]].r1 = inp[2];                     //initialize store's r1 field with distance between stores[inp[0]] to stores[inp[1]] (according to map.txt)
        }
        else
            if (stores[inp[0]].c2 == NULL)                  //checks whether second connection's value is NULL
            {
                stores[inp[0]].c2 = &stores[inp[1]];        //initialize store's c2 field with second possible destination (according to map.txt) 
                stores[inp[0]].r2 = inp[2];                 //initialize store's r2 field with distance between stores[inp[0]] to stores[inp[1]] (according to map.txt)
            }
            else
                if (stores[inp[0]].c3 == NULL)              //checks whether second connection's value is NULL
                {
                    stores[inp[0]].c3 = &stores[inp[1]];    //initialize store's c3 field with third possible destination (according to map.txt) 
                    stores[inp[0]].r3 = inp[2];             //initialize store's r3 field with distance between stores[inp[0]] to stores[inp[1]] (according to map.txt)
                }

    } while (fgetc(map) != EOF);                            //move cursor one char ahead to skip the new line character
    fclose(map);                                            //close the file
    return 1;
}

//prints all possible paths
void print_paths(store* head, int path[], int counter)
//written and amended by Anton Nicolaevsky & Lee Joe Barak
{
    int i;//a counter

    if (!head) return;
    //if end of connection

    if (!head->c1 && !head->c2 && !head->c3)
        //if returned to factory or end path
    {
        path[counter] = head->num;
        //gives path array's item current store number

        //print curent route
        for (i = 0; i < counter; i++) {
            printf("%d->", path[i]);
        }
        printf("%d\n", path[i]);
        return;
    }


    path[counter] = head->num;
    //gives path array's item current store number

    //send to next recursions
    print_paths(head->c1, path, counter + 1);
    print_paths(head->c2, path, counter + 1);
    print_paths(head->c3, path, counter + 1);
}

//-----------------------------------------

/*this function bundle option 5 - finding a route for the driver
*/

/* create route.txt.
route.txt shall include:
1)legal driving route
2)total amount of packages to deliver throughout route
3)total distance in KM
*/


/* a legal and OPTIMAL driving route:
1) starts and ends at factory
2)allows delivery of maximum amount of packages
3) if two or more routes qualify, choose the shortest one (smallest accumulated distance from factory and back)*/

/* Output to screen shall include:
1) the number of packages destined to each individual store
2)accumulated distance from factory and back in KM*/



void option5DriverPath()
{
    store *head = NULL; //the root of the stores tree
                        //path_t *path = NULL;
    path_t* bestPath = NULL;
    path_t* arrayOfPaths[100] = { 0 };

    int ValidRoutesDistances[1000][12] = { 0 }; //array contains all routes starting and ending in factory distances
    int ValidRoutes[1000][12] = { 0 }; //array contains all routes starting and ending in factory
    int i, j, numOfFactoryRoutes;
    store stores[11] = { { 0 },{ 1 },{ 2 },{ 3 },{ 4 },{ 5 },{ 6 },{ 7 },{ 8 },{ 9 },{ 10 } };
    //array of structs - each individual store->num is intialized with a given int that's inside the curly brackets;
    //all other fields of each individual struct store is initialized with: NULL - for fields of type struct, 0 - for fields of type 

    FILE* letsPlay = NULL;


    //inserts values to fields c1, c2, c3 of stores[i]
    if (build_paths(stores))//build all possible connections between the stores 
                            //checks wether map.txt exists, if it does continues the program, if it does not, exits the function
    {
        fopen_s(&letsPlay, LETSPLAY, "r");
        if (!letsPlay)
        {
            printf("Cant find file letsPlay.txt\n");
        }
        int visits[11] = { 0 };

        head = build_tree(&stores[0], visits);

        //construct array of arrays representing paths ending at factory
        numOfFactoryRoutes = findAllFactoryRoutes(head, ValidRoutes, ValidRoutesDistances);

        //build path for every array representing path of factoryEndingRoutes
        for (i = 0; i < numOfFactoryRoutes; i++) {
            arrayOfPaths[i] = createPathFromArray(ValidRoutes[i], ValidRoutesDistances[i], letsPlay);
        }


        //find best path out of all factory ending paths
        bestPath = arrayOfPaths[0];
        for (i = 1; i < numOfFactoryRoutes; i++) {
            if (comparePaths(bestPath, arrayOfPaths[i]) < 0) {
                bestPath = arrayOfPaths[i];
            }
        }

        //print best path to output window and write to file route.txt
        printBestPath(bestPath);

        //release memory of all paths
        for (i = 0; i < numOfFactoryRoutes; i++) {
            freePath(arrayOfPaths[i]);
        }

        if (letsPlay) //checks wether the letsplay.txt was openned
            fclose(letsPlay);
    }
}


//function to print best path and create new file
void printBestPath(path_t* path)
{
    FILE* out; //out file pointer, to print route file
    path_t* tmp = path;
    fopen_s(&out, ROUTE, "w");

    printf("Driver route:\n");
    fprintf(out, "Driver route:\n");

    while (tmp->next != NULL) {
        printf("%d->", tmp->store);
        fprintf(out, "%d->", tmp->store);
        tmp = tmp->next;
    }

    //for 0 in the end
    printf("%d\n", tmp->store);
    fprintf(out, "%d\n", tmp->store);

    //for file route.txt
    fprintf(out, "Store  Packages Distance so far[km]\n");
    fprintf(out, "----    --------  -------------------\n");

    tmp = path->next;

    while (tmp->next != NULL)
    {
        fprintf(out, "%-7d %-9d %d\n", tmp->store, tmp->packCount, tmp->distTotal);
        tmp = tmp->next;
    }

    //for last 0
    fprintf(out, "%-7d %-9d %d\n", tmp->store, tmp->packCount, tmp->distTotal);


    printf("The total packages available for this route is %d\n", tmp->packTotal);
    printf("The total suggested route distance is %d\n", tmp->distTotal);

    fprintf(out, "The total packages available for this route is %d\n", tmp->packTotal);
    fprintf(out, "The total suggested route distance is %d\n", tmp->distTotal);

    fclose(out);

}

//function to compare routes using a deduction method
int comparePaths(path_t* p1, path_t* p2)
{
    path_t* tmp1 = p1;
    path_t* tmp2 = p2;

    while (tmp1->next != NULL)//checks if the last node of route is empty
    {
        tmp1 = tmp1->next;
    }

    while (tmp2->next != NULL)
    {
        tmp2 = tmp2->next;
    }


    if (tmp1->packTotal > tmp2->packTotal) //comparison of packages for each route
    {
        return 1;
    }

    else if (tmp1->packTotal < tmp2->packTotal)//comparison of total distance
    {
        return -1;
    }

    else
    {
        if (tmp1->distTotal > tmp2->distTotal)
        {
            return -1;
        }

        else
        {
            return 1;
        }
    }
    return 1;
}


path_t* createPathFromArray(int * path, int* dist, FILE* letsPlay)
{
    int i, count = 0;
    path_t* head = (path_t*)malloc(sizeof(path_t));
    //initializes all feild data with zero, meaning non-initialized
    head->distTotal = 0;
    head->packCount = getPackCountTo(0, letsPlay);
    head->packTotal = getPackCountTo(0, letsPlay);
    head->store = 0;
    head->next = NULL;

    path_t* tmp = head; //creates a temporary pointer for the route

                        //for loop to initialize all the stops on the route with corresponding data
    for (i = 1; count != 1; i++) {

        if (path[i] == 0) {   //break for loop if ValidRoutes[i] == 0
            count++;
        }

        path_t* newNode = (path_t*)malloc(sizeof(path_t));
        newNode->distTotal = tmp->distTotal + dist[i];//adds appropriate amount of distance to field
        newNode->packCount = getPackCountTo(path[i], letsPlay);
        newNode->packTotal = tmp->packTotal + newNode->packCount;//adds appropriate amount of packages to field
        newNode->store = path[i];
        newNode->next = NULL;
        tmp->next = newNode;
        tmp = tmp->next;
    }

    return head;
}

//function to retrieve all data on package amount from file
int getPackCountTo(int storeNum, FILE* letsPlay) {
    char store[3] = { 0 };
    int countPacks = 0;

    if (letsPlay)
    {
        fseek(letsPlay, 3, SEEK_SET);
        while (fgets(store, 3, letsPlay) != NULL)
        {
            if (atoi(store) == storeNum)
            {
                countPacks++;
            }
            fseek(letsPlay, 12, SEEK_CUR);
        }
    }

    return countPacks;
}


//function to house all of the data needed for recursive function
int findAllFactoryRoutes(store* head, int ValidRoutes[][12], int validRoutesDist[][12]) {

    int tmp[12] = { 0 };
    int tmpIndex = 0;
    int totalIndex = 0;
    int hasBeen[11] = { 0 };
    int distTmp[12] = { 0 };

    findAllFactoryRoutesRec(head, 0, tmp, distTmp, tmpIndex, ValidRoutes, &totalIndex, hasBeen, validRoutesDist);

    return totalIndex;
}

//Recursive function to identify all routes starting and ending in factory
void findAllFactoryRoutesRec(store* head, int currentDist, int* tmp, int* distTmp, int tmpIndex, int ValidRoutes[][12], int * i, int* hasBeen, int ValidRoutesDist[][12]) {

    static int counRec;
    if (head == NULL) {
        return;
    }

    //check if the route has passed through the same destination more than once and if it has returned to the factory
    if (head->num == 0 && hasBeen[0] == 1) {

        tmp[tmpIndex] = 0;
        distTmp[tmpIndex] = currentDist;

        copyToArray(ValidRoutes[*i], tmp, tmpIndex + 1);
        copyToArray(ValidRoutesDist[*i], distTmp, tmpIndex + 1);


        counRec++;

        *i = *i + 1;
        return;
    }

    //initializes the hasbeen indicator
    hasBeen[head->num] = 1;
    tmp[tmpIndex] = head->num;
    distTmp[tmpIndex] = currentDist;

    //calls function with all existing routes to this store
    findAllFactoryRoutesRec(head->c1, head->r1, tmp, distTmp, tmpIndex + 1, ValidRoutes, i, hasBeen, ValidRoutesDist);
    findAllFactoryRoutesRec(head->c2, head->r2, tmp, distTmp, tmpIndex + 1, ValidRoutes, i, hasBeen, ValidRoutesDist);
    findAllFactoryRoutesRec(head->c3, head->r3, tmp, distTmp, tmpIndex + 1, ValidRoutes, i, hasBeen, ValidRoutesDist);
    hasBeen[head->num] = 0;


}

//copy and paste the results from the best route into an array
void copyToArray(int dest[], int source[], int size) {

    int i;
    for (i = 0; i < size; i++) {
        dest[i] = source[i];
    }
}



//-----------------------------------

/*free path*/
void freePath(path_t* path)
{

    if (path == NULL) {
        return;
    }

    freePath(path->next);
    free(path);

}


/*This function calculates fuel consumption.
It reuses path finding functions
*/
void option6FuelConsumption()
{

    store *head = NULL; //the root of the stores tree
                        //path_t *path = NULL;
    path_t* bestPath = NULL;
    path_t* arrayOfPaths[100] = { 0 };

    int ValidRoutesDistances[1000][12] = { 0 }; //array contains all routes starting and ending in factory distances
    int ValidRoutes[1000][12] = { 0 }; //array contains all routes starting and ending in factory
    int i, j, numOfFactoryRoutes;
    store stores[11] = { { 0 },{ 1 },{ 2 },{ 3 },{ 4 },{ 5 },{ 6 },{ 7 },{ 8 },{ 9 },{ 10 } };
    //array of structs - each individual store->num is intialized with a given int that's inside the curly brackets;
    //all other fields of each individual struct store is initialized with: NULL - for fields of type struct, 0 - for fields of type 
    int visits[11] = { 0 };
    //an array which saves all the visits
    FILE* letsplay = NULL;

    if (build_paths(stores))//build all possible connections between the stores 
                            //checks wether map.txt exists, if it does continues the program, if it does not, exits the function
    {
        head = build_tree(&stores[0], visits);
        //insert values to fields c1, c2, c3 of stores[i]
        fopen_s(&letsplay, LETSPLAY, "r");

        if (letsplay)
            //checks wether letsplay.txt is missing, and prints a message
            //the function can continue without its existence
        {
            printf("can't find file letsPlay.txt\n");
        }

        //insert values to fields c1, c2, c3 of stores[i]

        //construct array of arrays representing paths ending at factory
        numOfFactoryRoutes = findAllFactoryRoutes(head, ValidRoutes, ValidRoutesDistances, letsplay);

        if (ValidRoutes[0] == NULL)
            //checks wethere there are no valid roots
        {
            printf("No best path found\n");
            return;
        }

        //build path for every array representing path of factoryEndingRoutes
        for (i = 0; i < numOfFactoryRoutes; i++) {
            arrayOfPaths[i] = createPathFromArray(ValidRoutes[i], ValidRoutesDistances[i], letsplay);
        }

        //find best path out of all factory ending paths
        bestPath = arrayOfPaths[0];
        for (i = 1; i < numOfFactoryRoutes; i++) {
            if (comparePaths(bestPath, arrayOfPaths[i]) < 0) {
                bestPath = arrayOfPaths[i];
            }
        }
        fuelConsumption(bestPath);
        //calculates the feul consumption of the best path

        if (letsplay)
            //if the file was openned closes it
            fclose(letsplay);
    }
}


//
/* this function calculates the fuel consumption at eash time interval dt and prints to output screen*/
void fuelConsumption(path_t *path)
{
    //float fc[K][3];   // holds numeric fuel consumption calculations. K is predefined for the needed accuracy
    int initialFuel = getFuel(); //get fuel from user
    int i, dtPerSection, totalDistance = getTotalDistance(path);
    //counter, dt per each section of the path, calculates the total distance of the path
    float dt = ((float)totalDistance / (float)SPEED) / (float)K; //dt
    path_t* tmp = path; // holder for path
    float currentTime = 0.0; // initialises the current time
    int currentAmountOfPackages = getTotalPackages(path); //packages at start
    float c = initialFuel;// initialises c

    if (currentAmountOfPackages != 0)
        //reinitialises c if current amout of packages is not 0
    {
        c = initialFuel + 300.0 / (float)currentAmountOfPackages; //constant for fuel equation
    }

    float currentFuel = getFuelAtTimet(0.0, c, currentAmountOfPackages); //get fuel at time 0
    int intervalsCounter = 0; //runs until reaches K


                              //loops till end of path
    while (tmp->next != NULL)

    {
        //calc the rows per this route
        dtPerSection = (float)(tmp->next->distTotal - tmp->distTotal)*K / (float)totalDistance;
        int mod = (tmp->next->distTotal - tmp->distTotal)*K % totalDistance;

        if (mod < 50)
        {
            dtPerSection++;
        }
        printf("driving to %d\n", tmp->next->store);
        for (i = 0; i < dtPerSection && intervalsCounter < K; i++)
        {
            printf("%.3f    ", currentTime);
            printf("%.1f        ", currentFuel);
            printf("%d\n", currentAmountOfPackages);

            currentTime = currentTime + dt;
            currentFuel = getFuelAtTimet(currentTime, c, currentAmountOfPackages);
            intervalsCounter++;
        }


        tmp = tmp->next;
        //if we are on our last route and didnt reach K prints - complete to K
        if (intervalsCounter < (K - 1) && tmp->next == NULL)
        {
            for (i = intervalsCounter; i < K; i++)
            {
                printf("%.3f    ", currentTime);
                printf("%.1f    ", currentFuel);
                printf("%d\n", currentAmountOfPackages);

                currentTime = currentTime + dt;
                currentFuel = getFuelAtTimet(currentTime, c, currentAmountOfPackages);
            }
        }

        else
        {
            currentAmountOfPackages = currentAmountOfPackages - tmp->packCount;
            if (currentAmountOfPackages == 0)
            {
                c = initialFuel;
            }
            else
            {
                c = initialFuel + 300.0 / (float)currentAmountOfPackages;
            }
        }
    }

}

int getTotalPackages(path_t * path) {
    path_t* tmp = path;

    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    return tmp->packTotal;
}

int getTotalDistance(path_t * path) {
    path_t* tmp = path;

    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    return tmp->distTotal;
}

float getFuelAtTimet(float t, int c, int packs) {

    if (packs == 0) {
        return (-30) * t + c;
    }

    return (-300) / packs + pow(exp(1), -0.1*packs*t)*c;
}


/*get input from user to set truck fuel and cleare stdin buffer
@return - users input in the required range
*/
int getFuel() {

    char tankInitial[1000] = { 0 };
    int fuel;

    while (1) {
        printf("Set fuel in the truck's tank. Enter an integer number from 0-100\n");
        gets(tankInitial);

        int i;

        if (!isLegalNumber(tankInitial)) {
            printf("Illegal input for fuel\n");
            continue;
        }

        fuel = atoi(tankInitial);
        if (fuel > 100) {
            printf("Illegal input for fuel\n");
            continue;
        }
        else {
            break;
        }
    }

    return fuel;
}


//check if there are any chars on str other than 0-9
int isLegalNumber(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }

    return 1;
}


//-------------------------------------------
//function #7 and its subfunctions
//-------------------------------------------

/*Function 07, or "Manager's inventory app", is responsible to inform the store's manager about the current the inventory,
The app checks if there are any packages that were dropped off by mistake to the current store, and sends them to the correct store.
Furthermore, the app checks which packages were sold, based on this week's sales.
At last the app prints all the inventory sorted by fields.*/
void option7()
//Written and amended by Anton Nicolaevsky and Lee Joe Barak
{
    current_store this_store = { 0 }, other_stores = { 0 }, sold = { 0 };
    //store structures, for this store, other stores and sold items
    int total_packages = 0;
    //total packages count
    char user_name[50] = { 0 };
    //user name sting
    time_t rawtime = time(0);
    //current time seed
    char time_str[50] = { 0 };
    //current time string


    ctime_s(time_str, 27, &rawtime);
    //builds current time string
    time_str[strlen(time_str) - 1] = 0;
    //seting a null for last place to make it a string
    print_logo();
    //print the letsplay logo
    printf_s("\n\n----------------------------------------------------------\n\n\tWelcome to store manager inventory app.\n\n----------------------------------------------------------\n\n");
    //welcomes user
    this_store.num = get_dest_from_user();
    //gets current store's number
    ask_name(user_name);
    //asks user for his name, politely
    if (fill_inventory(&this_store, this_store.num))
        //fills store's inventory and if there are no packages, then it says goodbye
    {
        printf_s("\n\n----------------------------------------------------------\n\n\tGoodday %s\n\tStore's inventory as of: %s\n\n----------------------------------------------------------\n\n", user_name, time_str);
        //prints a greeting with the user's name and the date
        total_packages = get_total_packages7(this_store.head);
        //gets total packages 
        total_packages = sort_packages(&this_store, this_store.num, &other_stores, total_packages);
        //sort packages
        check_sales(&this_store, &sold, calculate_average(), total_packages);
        //check sales
        print_inventory(this_store.head, this_store.num, other_stores.head, sold.head);
        //print existing packages end of the day
        printf_s("\n\n----------------------------------------------------------\n\n\tGoodbye\n\n----------------------------------------------------------\n\n");
        //prints farewell
        if (this_store.head)
            //frees this store list, if it's still there
            free_list_pack(this_store.head);
        if (other_stores.head)
            //frees other store list if any items were sent
            free_list_pack(other_stores.head);
        if (sold.head)
            //frees the sold list if any items were sold
            free_list_pack(sold.head);
    }
}

//builds a linked list and fills this store's inventory (randomly)
int fill_inventory(current_store *store, int inp)
//Written and amended by Anton Nicolaevsky & Lee Joe Barak
{
    srand(time(0));
    //initialises the time seed
    int i, total_packages = rand() % 50, mismatch_dest_num, counter = 0;
    // i is a counter
    //total packages initialised randomly
    //mismatch destination number: how many mistakes are possible, as in packages which destination weren't of this store
    package_t* pack = NULL, *head = store->head;
    //pointers to linked lists

    do
    {
        //initialising the possible mistake until the mistakes number is lower than the total packages
        //it's illogical that all sent packages to this store were sent wrongly
        mismatch_dest_num = rand() % 10;
    } while (total_packages <= mismatch_dest_num);

    if (!total_packages)
    {
        //inventory is empty
        printf_s("\n\n----------------------------------------------------------\n\n\tThe inventory is empty, please contact the quartermaster\n\n----------------------------------------------------------\n\n");
        return 0;
    }
    for (i = 0; i < total_packages; i++)
        //a loop which builds the packages list
    {
        pack = (package_t*)calloc(1, sizeof(package_t));
        if (pack == NULL)
        {
            printf_s("Overflow!\n");//something went wrong
            exit(0);
        }
        pack->gift = rand() % 999 + 1;//randomise a gift number
        if (rand() % 2 && counter < mismatch_dest_num && i != total_packages && i)
            //gives a semi random chance of giving the package a wrong destination number
        {
            pack->dest = rand() % 10 + 1;
            counter++;
        }
        else
            //gives the current store's number
        {
            pack->dest = inp;
        }
        pack->id = (rand() % 999 + 1)*(rand() % 9999 + 1) + 1;
        //randomises a 7 digit id number
        pack->next = head;
        //links the next item
        head = pack;
        //adds item to list
    }
    store->head = head;//gives the store the now built list
    return 1;
}

//counts total packages number
int get_total_packages7(package_t *head)
//Written by Anton Nicolaevsky
{
    int i;

    for (i = 0; head != NULL; i++)
    {
        head = head->next;//advances the head
    }

    return i;//returns the total number of total packages
}

//prints the inventory
void print_inventory(package_t *head, int inp, package_t *other_head, package_t *sold_head)
//Written by Anton Nicolaevsky
{
    int i;//a counter
    if (other_head)
        //if the other store list is filled, prints it
    {
        printf_s("\n\n----------------------------------------------------------\n\n\tPackages sent to other stores:\n\n----------------------------------------------------------\n\n");
        for (i = 0; other_head != NULL; i++)
        {
            printf_s("\nPackage Serial:\t%03d%02d%07d\t\tPackage was sent to store %d", other_head->gift, other_head->dest, other_head->id, other_head->dest);
            other_head = other_head->next;
        }
        printf_s("\n\n----------------------------------------------------------\n\n\tA total of %d packages were sent\n\n----------------------------------------------------------\n\n", i);
    }
    if (sold_head)
        //if the sold list is filled, prints it
    {
        printf_s("\n\n----------------------------------------------------------\n\n\tPackages sold today:\n\n----------------------------------------------------------\n\n");
        for (i = 0; sold_head != NULL; i++)
        {
            printf_s("\nPackage Serial:\t%03d%02d%07d\t\tPackage was sold today", sold_head->gift, sold_head->dest, sold_head->id);
            sold_head = sold_head->next;
        }
        printf_s("\n\n----------------------------------------------------------\n\n\tA total of %d packages were sold\n\n----------------------------------------------------------\n\n", i);
    }
    if (head)
        //if current store list is filled, prints it
    {
        printf_s("\n\n----------------------------------------------------------\n\n\tCurrent inventory:\n\n----------------------------------------------------------\n\n");
        for (i = 0; head != NULL; i++)
        {
            printf_s("\nPackage Serial:\t%03d%02d%07d", head->gift, head->dest, head->id);
            head = head->next;
        }
    }
    printf_s("\n\n----------------------------------------------------------\n\n\tThere are a total of %d packages in inventory\n\n----------------------------------------------------------\n\n", i);
}

//asks user for his name
void ask_name(char* user_name)
//Written by Anton Nicolaevsky
{
    char str[62] = { 0 }, first_name[30] = { 0 }, last_name[30] = { 0 };
    //string for bulding the full first and last name
    int i, j, flag;
    //i & j are counters, flag is a flag
    do
    {
        flag = 0;
        printf_s("Please enter your First name: ");
        if (!fgets(first_name, 30, stdin))
            //gets first and maybe middle name
        {
            printf_s("Sorry, invalid input!\n");
            continue;
        }
        printf_s("Please enter your Last name: ");
        if (!fgets(last_name, 30, stdin))
            //gets last name and any royal titles and/or titular titles
        {

            printf_s("Sorry, invalid input!\n");
            flag = 1;
        }
    } while (flag);

    //builds the User name string
    /*
    for (i = 0; i < 30; i++)
    {
    if (first_name[i] == 32) continue;
    if (first_name[i] == 10) break;
    str[i] = first_name[i];
    }
    str[i] = 32;//put a space between first name and surname
    for (j = 0; j < 30; j++)
    {
    if (last_name[j] == 32) continue;
    if (last_name[j] == 10) break;
    str[i+j+1] = last_name[j];
    }*/
    strcat_s(str, 30, first_name);
    i = strlen(str);
    str[i - 1] = 32;
    str[i] = 0;
    strcat_s(str, 30, last_name);
    strcpy_s(user_name, 40, str);
    //copy the string to the user name
}

//sort the list into a list of packages which belong to the store and those who don't
int sort_packages(current_store *store, int inp, current_store *other_stores, int total_pack)
//written by Anton Nicolaevsky
{
    package_t *pack = NULL, *second_head = NULL, *head = store->head;
    //pointers to lists
    int i, counter = total_pack;
    //counters

    for (i = 0; head != NULL; i++)
        //a loop which builds the other store packages list
    {
        if (head->dest != inp)
        {
            pack = (package_t*)calloc(1, sizeof(package_t));
            if (pack == NULL)
            {
                printf_s("Overflow!\n");
                exit(0);
            }
            pack->gift = head->gift;
            pack->dest = head->dest;
            pack->id = head->id;
            pack->next = second_head;
            second_head = pack;
        }
        head = head->next;
    }
    other_stores->head = second_head;//builds other store list

                                     //removes unneeded items from the store inventory
    pack = NULL;
    head = store->head;
    for (i = 0; head->next != NULL; i++)
        //a loop which checks the next item on the list, if the next item's store destination remove next item
    {
        if (head->next->dest != inp)
            //if next item's destination is wrong
        {
            pack = head->next;                  //saves next item's address
            head->next = head->next->next;      //save the address of the item after the next into current item's pointer
            free(pack);                         //free the wrong item
            counter--;
        }
        else
            head = head->next;
    }
    return counter;
}

//calculates the average base on the previous five days' sales, to calculate how many packages were sold today
int calculate_average()
//written by Anton Nicolaevsky
{
    srand(time(0));//randomises the seed for rand()
    int i, sum = 0;
    for (i = 0; i < 5; i++)
    {
        sum += rand() % 40 + 20;
    }
    return sum;
}

//checks how many packages were sold today
void check_sales(current_store *store, current_store *sold, int average, int total_pack)
//Written by Anton Nicolaevsky
{
    int i, pack_sold = (int)(((float)average / 5)*0.25), counter = 0;
    //i & counter are counters, pack_sold is the number 
    package_t *head = store->head, *pack = NULL, *second_head = sold->head;
    //pointer to head of lists

    for (i = 0; head != NULL; i++)
        //a loop which creates a list of sold items
    {
        if (rand() % 2 && counter < pack_sold && i && i != total_pack)
            //gives a chance for the package to be sold
            //allocates a linked list and puts it in sold
        {
            pack = (package_t*)calloc(1, sizeof(package_t));
            if (pack == NULL)
            {
                printf_s("Overflow!\n");
                exit(0);
            }
            pack->gift = head->gift;
            pack->dest = head->dest;
            pack->id = head->id;
            pack->next = second_head;
            second_head = pack;
        }
        head = head->next;
    }
    sold->head = second_head;

    //removes unneeded items from the store inventory
    pack = NULL;
    head = store->head;
    if (total_pack > 1)
    {
        for (i = 0; head->next != NULL; i++)
            //a loop which checks the next item on the list, if the next item's store destination remove next item
        {
            if (exists_in_sold(head->next, sold->head))
                //if next item's destination is wrong
            {
                pack = head->next;              //saves next item's address
                head->next = head->next->next;  //save the address of the item after the next into current item's pointer
                free(pack);                     //free the wrong item
            }
            else
                head = head->next;
        }
    }
    else
        //if it's a single item list
        if (exists_in_sold(head, sold->head))
            free(head);
}

//checks if the package exists in the store inventory
int exists_in_sold(package_t *head, package_t* sold_list)
//Written by Anton Nicolaevsky
{
    int i;                              //a counter
    package_t *pack = sold_list;        //pointer
    if (!sold_list) return 0;           //if sold list is empty
    for (i = 0; pack != NULL; i++)
    {
        if (head->gift == pack->gift&&head->dest == pack->dest&&head->id == pack->id)
            //checks if the barcode is the same
        {
            return 1;                   //there's a match
        }
        pack = pack->next;
    }
    return 0;                           //no match
}

//prints let's play logo
void print_logo()
//written by Anton nicolaevsky
{
    int i, j;

    for (i = 0; i < 80; i++)
    {
        printf("-");
    }
    printf("\n");
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 80; j++)
        {
            printf("#");
        }
        printf("\n");
    }
    printf("\n\n\n");
    printf("\t##\t\t##########\t##########\t##########\n");
    printf("\t##\t\t##\t\t    ##\t\t##\n");
    printf("\t##\t\t##\t\t    ##\t\t##\n");
    printf("\t##\t\t##########\t    ##\t\t##########\n");
    printf("\t##\t\t##\t\t    ##\t\t\t##\n");
    printf("\t##\t\t##\t\t    ##\t\t\t##\n");
    printf("\t##########\t##########\t    ##\t\t##########\n");
    printf("\n\n");
    printf("\t##########\t##\t\t##########\t##\t##\n");
    printf("\t##\t##\t##\t\t##\t##\t##\t##\n");
    printf("\t##\t##\t##\t\t##\t##\t##\t##\n");
    printf("\t##########\t##\t\t##########\t##########\n");
    printf("\t##\t\t##\t\t##\t##\t    ##\n");
    printf("\t##\t\t##\t\t##\t##\t    ##\n");
    printf("\t##\t\t##########\t##\t##\t    ##\n");
    printf("\n\n\n");
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 80; j++)
        {
            printf("#");
        }
        printf("\n");
    }
    for (i = 0; i < 80; i++)
    {
        printf("-");
    }
    printf("\n");
}

