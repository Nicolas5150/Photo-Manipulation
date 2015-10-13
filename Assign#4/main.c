//  Created by Nicolas Emery on 11/14/14.
//  COP 3223 - 0002
//  Assignment 4 - (Assign4.c)
//  Copyright (c) 2014 Nicolas Emery. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <math.h>

#define  BUFFER_SIZE  70
#define  TRUE          1
#define  FALSE         0


int**  img;

int** temppicx, temppicy;
int**  img,sobelout;

FILE*  fo1;
int    numRows;
int    numCols;
int    maxVal;

FILE* fo2;
int** imgTwo;
int   OtherMaxVal;
int   OtherNumRows;
int   OtherNumCols;



void addtopixels(int** imgtemp, int value);
void  writeoutpic(char* fileName, int** imgtemp);
int** readpic(char* fileName);
void  readHeader(FILE* imgFin);
int   isComment(char* line);
void  readImgID(char* line);
void  readImgSize(char* line);
void  readMaxVal(char* line);
int** setImage();
void  readBinaryData(FILE* imgFin, int** imgtemp);
void sobelfunc(int** pic, int** edges, int** tempx, int** tempy);

void subfiletofile(int** imgtemp, int **imgtwotemp);

int main()
{
   
    char menu, fileName[BUFFER_SIZE], outName[BUFFER_SIZE] = {'O','U','T',1,'.','p','g','m','\0'};
    int i,j,rows,cols, value, invalidInput=0;
    char ci;
    
        while(menu!='4')
    {
        menu='0';
        system("cls");
        if(invalidInput==1)
            printf("Invalid input!\n");
        invalidInput=0;
        printf("This program processes images.\n");
        printf("1) Change the brightness to the input picture\n2) Subtract Two Pictures\n3) Highlight Edges\n4) Quit the program\n");
        printf("Enter desired choice:");
        scanf("\n%c", &menu);
        
        // OPTION #1 BRIGHTNESS OPTION
        if(menu=='1')
        {
            printf("Enter the image input filename: ");
            scanf("%s", fileName);
            img = readpic(fileName);
            printf("Successfully read image input filename '%s'\n", fileName);
            
            printf("Enter the desired value to add to the brightness: ", fileName);
            scanf("%d", &value);
            
            addtopixels(img,value);
            outName[3]='1';
            writeoutpic(outName,img);
            
            free(img);
            img = NULL;
        }
        // OPTION #2 SUBTRACTION OPTION
        if(menu=='2')
        {
          
            printf("Enter the first image input filename: ");
            scanf("%s", fileName);
            imgTwo = readpic(fileName);
            OtherNumRows=numRows;
            OtherNumCols=numCols;
            OtherMaxVal=maxVal;
            fo2=fo1;
            printf("Successfully read the first image input filename '%s'\n", fileName);
            
            printf("Enter the second input image filename: ", fileName);
            scanf("%s", fileName);
            img = readpic(fileName);
            printf("Successfully read the second image input filename '%s'\n", fileName);
            
            subfiletofile(img,imgTwo);
            outName[3]='2';
            writeoutpic(outName,img);
            
            free(img);
            img = NULL;
            free(imgTwo);
            imgTwo = NULL;
        }
        // OPTION #3 SOBEL OPTION
        if(menu=='3')
        {
            
            printf("Enter the image input filename for output: ");
            scanf("%s", fileName);
            
            img = readpic(fileName);
            printf("Successfully read image input filename '%s'\n", fileName);
            
            sobelout= setImage();
            temppicx= setImage();
            temppicy= setImage();
            sobelfunc(img,sobelout,temppicx,temppicy);
            
            outName[3]='3';
            writeoutpic(outName,sobelout);
            
            free(img);
            img = NULL;
        }

    }
    
    invalidInput=0;
    printf("Exiting Program\n");
    system("PAUSE");
    return(EXIT_SUCCESS);
}

void addtopixels(int** imgtemp, int value)
{
    int i,j;
    
    for (i=0;i<numRows;i++)
    { for (j=0;j<numCols;j++)
    {
        imgtemp[i][j] += value;
    }
    }
}

void subfiletofile(int** imgtemp, int **imgtwotemp)
{
    
    int i,j, OtherRows=numRows, OtherCols=numCols;
    
    if(numRows>OtherNumRows)
        OtherRows=OtherNumRows;
    if(numCols>OtherNumCols)
        OtherCols=OtherNumCols;
    
    for (i=0;i<OtherRows;i++)
    {
        for (j=0;j<OtherCols;j++)
        {
            imgtemp[i][j] =abs(imgtemp[i][j]-imgtwotemp[i][j]);
        }
    }
}

void writeoutpic(char* fileName, int** imgtemp)
{
    int i,j;
    char ci;
    FILE* fo1;
    
    if((fo1 = fopen(fileName, "wb")) == NULL)
    {
        printf("Unable to open out image file '%s'\n", fileName);
        exit(EXIT_FAILURE);
    }
    
    fprintf(fo1,"P5\n");
    fprintf(fo1,"%d %d\n", numRows, numCols);
    fprintf(fo1,"255\n");
    
    for (i=0;i<numRows;i++)
    { for (j=0;j<numCols;j++)
    {
        ci   =  (char) (imgtemp[i][j]);
        fprintf(fo1,"%c", ci);
    }
    }
}


int** readpic(char* fileName)
{
    FILE* imgFin;
    int** imgtemp;
    
    if((imgFin = fopen(fileName, "rb")) == NULL)
    {
        printf("Unable to open image file '%s'\n", fileName);
        exit(EXIT_FAILURE);
    }
    
    readHeader(imgFin);
    
    
    imgtemp  = setImage();
    
    readBinaryData(imgFin, imgtemp);
    
    fclose(imgFin);
    
    return  imgtemp;
   
    
}

void readHeader(FILE* imgFin)
{
    int  haveReadImgID   = FALSE;
    int  haveReadImgSize = FALSE;
    int  haveReadMaxVal  = FALSE;
    char line[BUFFER_SIZE];
    
    while(!(haveReadImgID && haveReadImgSize && haveReadMaxVal))
    {
        fgets(line, BUFFER_SIZE, imgFin);
        
        if((strlen(line) == 0) || (strlen(line) == 1))
            continue;
        
        if(isComment(line))
            continue;
        
        if(!(haveReadImgID))
        {
            readImgID(line);
            haveReadImgID = TRUE;
        }
        else if(!(haveReadImgSize))
        {
            readImgSize(line);
            haveReadImgSize = TRUE;
        }
        else if(!(haveReadMaxVal))
        {
            readMaxVal(line);
            haveReadMaxVal = TRUE;
        }
    }
    
}

int isComment(char *line)
{
    if(line[0] == '#')
        return(TRUE);
    
    return(FALSE);
}

void readImgID(char* line)
{
    if(strcmp(line, "P5\n") != 0)
    {
        printf("Invalid image ID\n");
        exit(EXIT_FAILURE);
    }
}

void readImgSize(char* line)
{
    unsigned i;
    
    for(i = 0; i < strlen(line); ++i)
    {
        if(!((isdigit((int) line[i])) || (isspace((int) line[i]))))
        {
            printf("Invalid image size\n");
            exit(EXIT_FAILURE);
        }
    }
    
    sscanf(line, "%d %d", &numRows, &numCols);
}

void readMaxVal(char* line)
{
    unsigned i;
    
    for(i = 0; i < strlen(line); ++i)
    {
        if(!((isdigit((int) line[i])) || (isspace((int) line[i]))))
        {
            printf("Invalid image max value\n");
            exit(EXIT_FAILURE);
        }
    }
    
    maxVal = atoi(line);
}

int** setImage()
{
    int** imgtemp;
    unsigned i;
    
    imgtemp = (int**) calloc(numRows, sizeof(int*));
    
    for(i = 0; i < numRows; ++i)
    {
        imgtemp[i] = (int*) calloc(numCols, sizeof(int));
    }
    return imgtemp;
}

void readBinaryData(FILE* imgFin, int** imgtemp)
{
    unsigned  i;
    unsigned  j;
    for(i = 0; i < numRows; ++i)
    {
        for(j = 0; j < numCols; ++j)
        {
            imgtemp[i][j] =
            fgetc(imgFin);
        }
    }
}

void sobelfunc(int** pic, int** edges, int** tempx, int** tempy)
{
    
    int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
    int maxival;
    
    
    
    
    int i,j,p,q,mr,sum1,sum2;
    double threshold;
    
    
    mr = 1;
    
    
    for (i=mr;i<numRows-mr;i++)
    { for (j=mr;j<numCols-mr;j++)
    {
        sum1 = 0;
        sum2 = 0;
        for (p=-mr;p<=mr;p++)
        {
            for (q=-mr;q<=mr;q++)
            {
                sum1 += pic[i+p][j+q] * maskx[p+mr][q+mr];
                sum2 += pic[i+p][j+q] * masky[p+mr][q+mr];
            }
        }
        tempx[i][j] = sum1;
        tempy[i][j] = sum2;
    }
    }
    
    maxival = 0;
    for (i=mr;i<numRows-mr;i++)
    { for (j=mr;j<numCols-mr;j++)
    {
        edges[i][j]=(int) (sqrt((double)((tempx[i][j]*tempx[i][j]) +
                                         (tempy[i][j]*tempy[i][j]))));
        if (edges[i][j] > maxival)
            maxival = edges[i][j];
        
    }
    }
    
    
    
    for (i=0;i<numRows;i++)
    { for (j=0;j<numCols;j++)
    {
        edges[i][j] = ((edges[i][j]*1.0) / maxival) * 255;
        
    }
    }
}
