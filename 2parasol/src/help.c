//help.c

#include "entryTable.h"

/* Funtion for displaying the manual*/

void PrintHelp()
{
	
	printf("##### HELP #####\n");
	printf("This is PARASOL implementation (ver. 1.0) \n");
	printf("Options are as follows:\n\n");
	printf("-s float\n");
	printf("     Setting the minimal support (float value)\n");
	printf("     In default, %1.2f\n",DEF_SUP);
	printf("\n");
	printf("-i char\n");
	printf("     Recieving the input streams as char values\n");
	printf("     In default, %s\n",DEF_INFILE);
	printf("\n");
	printf("-o char\n");
	printf("     Outputting the result as char values\n");
	printf("     In default %s\n",DEF_OUTFILE);
	printf("\n");
    printf("-e float\n");
    printf("     Setting the maximal error ratio (float value)\n");
    printf("     In default, %1.2f\n",DEF_ERR);
    printf("     Note that support>=error>= 0\n");
    printf("\n");
	printf("-k int(>=0)\n ");
	printf("     Setting the maximal size of entry table (int value)\n");
	printf("\n");
	printf("--comp \n");
	printf("     Exploiting the Delta-compression\n");
    printf("--reduce \n");
	printf("     Exploiting the Stream reduction\n");
    printf("\n");
    printf("--len int(>0)\n");
    printf("     Setting the maximal transaction length (int value)\n");
	printf("\n");
	printf("-h\n");
	printf("     Displaying the help menue\n");	
	printf("\n");
}

