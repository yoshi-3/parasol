//helpMineSubset.c

#include "headerSubset.h"

/* Funtion for displaying the manual*/
void PrintHelp()
{
	
	printf("##### MineSubset HELP #####\n");
	printf("This is the online mining tool with the baseline algorithm\n");
	printf("Options are as follows:\n\n");
	printf("-s float\n");
	printf("     Setting the minimal support (float value)\n");
	printf("     In default, %1.2f\n",DEF_SUP);
	printf("\n");
	printf("-e float\n");
	printf("     Setting the maximal error ratio (float value)\n");
	printf("     In default, %1.2f\n",DEF_ERR);
	printf("     Note that support>error>= 0\n");
	printf("\n");
	printf("-i char\n");
	printf("     Recieving the input streams as char values\n");
	printf("     In default, %s\n",DEF_INFILE);
	printf("\n");
	printf("-o char\n");
	printf("     Outputting the result as char values\n");
	printf("     In default %s\n",DEF_OUTFILE);
	printf("\n");
	printf("-l int(>0)\n");
	printf("     Setting the number of streams (int value)\n");
	printf("     In default %d\n",DEF_STRLEN);
	printf("\n");
	printf("-k int(>=0)\n ");
	printf("     Setting the maximal size of entry table (int value)\n");
	printf("     There is no restriction if the value should be zero\n");
	printf("\n");
	printf("-c \n");
	printf("     Computing the precision and recall of outputs\n");
	printf("     with respect to the frequent sets by LCM");
	printf("\n");	
	printf("-r \n");
	printf("     Reducing the transaction in advance\n");
	printf("     with the single stream mining\n");
	printf("\n");
	printf("-h\n");
	printf("     Displaying the help menue\n");	
	printf("\n");
	printf("--skip \n");
	printf("     Applying both the t-skip and r-skip operations\n");
	printf("\n");
	printf("--tskip \n");
	printf("     Applying the t-skip operation\n");
	printf("\n");	
	printf("--rskip \n");
	printf("     Applying the r-skip operation\n");
	printf("\n");	

}

