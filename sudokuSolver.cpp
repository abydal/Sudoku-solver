#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;

bool populateBoard(signed char* board, int* mask, string input);
short checkHorizontal(signed char* board, char pos);
short checkVertical(signed char* board, char pos);
short check3x3Region(signed char* board, char pos);
signed char getNumberFromBitmask(short mask);
void setValue(int* mask, signed char* board, char value, char pos);
void removeValue(int* mask,signed char* board, char pos);
void printBoard(signed char* board);
void printBitmask(char size, int value);

signed char* board;
int* mask;

enum NUMBERS
{
	N1 = 1,
	N2 = 2,
	N3 = 4,
	N4 = 8,
	N5 = 16,
	N6 = 32,
	N7 = 64,
	N8 = 128,
	N9 = 256
};

const int LOCKED = 0x80000000;

enum ORIENTATION
{
	HORIZONTAL = 0,
	VERTICAL = 9,
	REGION3X3 = 18
};

int main(int argc, char* argv[])
{
	board = new signed char[81];
	mask = new int[81];
	
	string input;
	
	for(int i = 0; i < argc; i++)
	{
		printf("Parsing arg: %s\n",argv[i]);
		if(argv[i][1] == 'b')
		{
			input = argv[i+1];
			printf("Parsing arg: %s\n",argv[i+1]);
		}
	}
	
	if (!(populateBoard(board,mask, input)))
	{
		printf("Invalid input.\n");
		return 0;
	}
	
	printBoard(board);
	
	//analyze board for numbers that are safe to place down
	for(int i = 0; i < 81; i++)
	{
		//skip checks for numbers already on the board
		if(board[i] > 0)
			continue;
		
		//retrieve bitmasks for current position
		short numbersVert = (short)((mask[i] >> VERTICAL)&0x000001FF);
		short numbersHori = (short)((mask[i] >> HORIZONTAL)&0x000001FF);
		short numbers3x3R = (short)((mask[i] >> REGION3X3)&0x000001FF);
		
		short fullCheck = numbersVert|numbersHori|numbers3x3R;
		short fullCheckInv = ~fullCheck & 0x01FF;
		
		//check if current placement have only one number to choose
		if( (fullCheckInv == N1)||
			(fullCheckInv == N2)||
			(fullCheckInv == N3)||
			(fullCheckInv == N4)||
			(fullCheckInv == N5)||
			(fullCheckInv == N6)||
			(fullCheckInv == N7)||
			(fullCheckInv == N8)||
			(fullCheckInv == N9))
		{
			//place down found number and lock it
			signed char val = getNumberFromBitmask(fullCheckInv);
			setValue(mask,board,val ,i);
			mask[i] = mask[i]|LOCKED;
			
			//printout
			printf("Placing %d in pos %d\t | ",board[i],i);
			printBitmask(9,numbers3x3R);printf(" | ");
			printBitmask(9,numbersHori);printf(" | ");
			printBitmask(9,numbersVert);printf(" | ");
			printBitmask(9,fullCheck);
			printf("\n");
			
			//start analyzing from first square after a new number has been placed down
			i = -1;
		}
	}
	
	printBoard(board);

	char iteration = 1;
	for(int n = 0; n < 81; n = n+iteration)
	{
		//locked numbers can be skipped, just make sure to not skip outside of board
		if((mask[n]&LOCKED) == LOCKED)
		{
			if((iteration == -1) && (n == 0))
				iteration = 1;
			continue;
		}
		
		//if a 9 is found when reversing it can be removed and we can continue the reverse move
		if((board[n] == 9) && (iteration == -1))
		{
			removeValue(mask, board,n);
			continue;
		}
		
		//take the number in the square and iterate it once
		for(signed char number = board[n]+1; number <= 9; number++)
		{
			//if reversing, remove the current number from the square
			if(iteration == -1)
				removeValue(mask, board, n);
			
			//get the proper bitpattern for current number
			NUMBERS flag = (NUMBERS)pow(2,number-1);
			
			//check that current number can fit in square
			short numbersVert = (short)((mask[n] >> VERTICAL)&0x000001FF);
			short numbersHori = (short)((mask[n] >> HORIZONTAL)&0x000001FF);
			short numbers3x3R = (short)((mask[n] >> REGION3X3)&0x000001FF);
		
			short fullCheck = numbersVert|numbersHori|numbers3x3R;
			
			if((fullCheck&flag) != flag)
			{
				//the number fits, place it, update bitmasks and start iterate forward again
				setValue(mask, board, number, n);
				iteration = 1;
				break;
			}
			
			//no numbers fit and we need to continue to reverse
			if(number == 9)
			{
				//printBoard(board);
				removeValue(mask, board,n);
				iteration = -1;
			}
		}
	}
	printBoard(board);
}

bool populateBoard(signed char* board, int* mask, string input)
{
	//initalize arrays
	for(int n = 0; n < 81; n++)
	{
		mask[n] = 0;
		board[n] = 0;
	}
	
	//enter input values into board and mask arrays
	for(int i = 0; i < 81; i++)
	{
		printf("%c ",input[i]);
		
		//empty values are skipped
		if(input[i] == '_'|| input[i] == '.')
			continue;
		
		//convert string numbers to integer type		
		char digit = input.at(i);
		signed char converted = 0;
		converted = atoi(&digit);
		
		printf("%d\n",converted);
		
		if(converted == NULL)
			return false;
		else
		{
			setValue(mask, board, converted,i);
			mask[i] = mask[i]|LOCKED;
		}
	}
	return true;
}

void printBoard(signed char* board)
{
	printf("\n");
	for(int y = 0; y < 9; y++)
	{
		printf("- - - - - - - - - - - - - - - - - - -\n");
		for(int x = 0; x < 9; x++)
		{
			if(board[x+y*9] > 0)
				printf("| %d ",board[x+y*9]);
			else
				printf("|   ");
		}
		printf("|\n");
	}
	printf("- - - - - - - - - - - - - - - - - - -\n");
}

signed char getNumberFromBitmask(short mask)
{
	signed char count = 0; 
	while(mask != 0)
	{
		mask = mask >> 1;
		count++;
	}
	return count;
}

void printBitmask(char size, int value)
{
	int mask = 0;
	int count = 1;
	while(count <= size)
	{
		mask = (value >> (size-count))&(int)1;
		printf("%d",mask);
		count++;
	}
}

void setValue(int* mask, signed char* board, char value, char pos)
{
	int valueMask = (int)pow(2,value-1);
	board[pos] = value;
	
	char firstPos = pos - pos%9;
	char lastPos = firstPos + 9;
	for(int i = firstPos; i < lastPos; i++)
		mask[i] = mask[i]|(valueMask << HORIZONTAL);

	firstPos = pos%9;
	lastPos = firstPos + 72;
	for(int n = firstPos; n <= lastPos; n = n+9)
		mask[n] = mask[n]|(valueMask << VERTICAL);
	
	firstPos = pos - pos%3 - (((pos - pos%3)/9)%3)*9;
	lastPos = firstPos + 3*9;
	
	for(int t = firstPos; t < lastPos; t+=9)
	{
		mask[t] = mask[t]|(valueMask << REGION3X3);
		mask[t+1] = mask[t+1]|(valueMask << REGION3X3);
		mask[t+2] = mask[t+2]|(valueMask << REGION3X3);
	}
}

void removeValue(int* mask, signed char* board, char pos)
{
		
	signed char value = board[pos];
	board[pos] = 0;
	int valueMask = (int)pow(2,value-1);

	char firstPos = pos - pos%9;
	char lastPos = firstPos + 9;
	for(int i = firstPos; i < lastPos; i++)
		mask[i] = mask[i]^(valueMask << HORIZONTAL);

	firstPos = pos%9;
	lastPos = firstPos + 72;
	for(int n = firstPos; n <= lastPos; n = n+9)
		mask[n] = mask[n]^(valueMask << VERTICAL);
	
	firstPos = pos - pos%3 - (((pos - pos%3)/9)%3)*9;
	lastPos = firstPos + 3*9;
	
	for(int t = firstPos; t < lastPos; t+=9)
	{
		mask[t] = mask[t]^(valueMask << REGION3X3);
		mask[t+1] = mask[t+1]^(valueMask << REGION3X3);
		mask[t+2] = mask[t+2]^(valueMask << REGION3X3);
	}
}


//--------------------DEPRECATED------------------------\\

short checkHorizontal(signed char* board, char pos)
{
	short numbers = 0;
	char firstPos = pos - pos%9;
	char lastPos = firstPos + 9;
	for(int i = firstPos; i < lastPos; i++)
		if(board[i] > 0)
		{
			short number = (short)pow(2,board[i]-1);
			//printf("Number found horizontal: %d\n",number);
			numbers = numbers | number;
		}
	
	return numbers;
}

short checkVertical(signed char* board, char pos)
{
	short numbers = 0;
	char firstPos = pos%9;
	char lastPos = firstPos + 72;
	for(int i = firstPos; i <= lastPos; i = i+9)
		if(board[i] > 0)
		{	
			short number = (short)pow(2,board[i]-1);
			//printf("Number found vertical: %d\n",number);
			numbers = numbers | number;
		}
	
	return numbers;
}

short check3x3Region(signed char* board, char pos)
{
	short numbers = 0;
	int firstPos = pos - pos%3 - (((pos - pos%3)/9)%3)*9;
	int lastPos = firstPos + 3*9;
	
	for(int i = firstPos; i < lastPos; i+=9)
	{
		if(board[i] > 0)
		{	
			short number = (short)pow(2,board[i]-1);
			//printf("Number found 3x3: %d\n",number);
			numbers = numbers | number;
		}
		if(board[i+1] > 0)
		{	
			short number = (short)pow(2,board[i+1]-1);
			//printf("Number found 3x3: %d\n",number);
			numbers = numbers | number;
		}
		if(board[i+2] > 0)
		{
			short number = (short)pow(2,board[i+2]-1);
			//printf("Number found 3x3: %d\n",number);
			numbers = numbers | number;
		}
	}
	return numbers;
}