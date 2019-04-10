// adapted from: https://gist.github.com/muhammedeminoglu/c69cabee84fd2d1fb3dcceb0d09344b7

#include <stdio.h>
#include <stdlib.h>

int myArray[1000000 - 1];

void swapf(int x, int y)
{
    int temp;
    temp = myArray[x];
    myArray[x] = myArray[y];
    myArray[y] = temp;

}

void bubbleSort(int x[], int number_of_elements)
{
    int i, j;

    for(i = 1; i < number_of_elements; i++)
    {
        for( j = 0; j < number_of_elements - 1; j++)
        {
            if(myArray[j] > myArray[j+1])
                swapf(j, j+1);
        }
    }
}

void printSorted(int number_of_elements)
{
    int i;
    for( i = 0; i < number_of_elements - 1; i++)
    {
        printf("%d\n", myArray[i]);
    }
}

void init(int number_of_elements)
{
    int i;
    for( i = 0; i < number_of_elements - 1; i++)
    {
        myArray[i] = rand()%number_of_elements;
    }
}

int main( int argc, char *argv[])
{


    int number_of_elements;
	if(argc <= 1 || !argc){
      exit(0);
   }
	sscanf (argv[1],"%d",&number_of_elements);
	if(number_of_elements<10 || number_of_elements>10000000){
		exit(0);
	}

    init(number_of_elements);
    bubbleSort(myArray, number_of_elements);
    printSorted(number_of_elements);
}
