/*
 *  param.c
 *  
 *
 *
 */

#include "param.h"


/*
 FUNCTION: ShowParameter
 + outputting the parameters as stdout
 OPTIONS:
 + *para - pointer to the PARAMETER value
 RETURN:
 + void
 */
void ShowParameter(PARAMETER *para, TABLE* table)
{
	//double pre = 0;
	//double rec = 0;
    char approach[FILE_LEN];
    strcpy(approach,"");

	double time = (double)(para->time_end - para->time_start)/CLOCKS_PER_SEC;
    double time_per = 0;
    if(para->time > 0){
		time_per = time / para->time;
    }
	
	fprintf(stdout,"inputFile: %s\n",para->in_filename);
    if(para->disp_output){
        fprintf(stdout,"outputFile: %s (ans/table)\n",para->out_filename);
    }
	fprintf(stdout,"options: <");
	
	if(para->lc){
		fprintf(stdout, "pc. ");
		strcat(approach, "pc-");
	}
	if(para->ss){
		fprintf(stdout, "rc. ");
		strcat(approach, "rc-");
	}
	if(!para->lc && !para->ss) {
		fprintf(stdout, "exact. ");
		strcat(approach, "exact-");
	}
    if(para->reduce){
        fprintf(stdout,"stream reduction. ");
    }
	if(para->comp){
		fprintf(stdout, "Delta-compression. ");
		strcat(approach, "comp.");
	}
	fprintf(stdout, ">\n");

	fprintf(stdout,"+ min support: %.4f\n", (double)para->support);

    if(para->ss){
        fprintf(stdout,"+ size constant: %d\n", para->table_size);
    }
    if(para->lc){
        fprintf(stdout,"+ error parameter: %.4f\n", para->error);
    }
    fprintf(stdout,"statics of dataset\n");
	fprintf(stdout,"+ num of transactions: %d\n",para->time);
    fprintf(stdout,"--- result --- \n");
    fprintf(stdout,"Delta: %d\n", table->delta);
    fprintf(stdout,"final error ratio (Delta / num of transactions): %.3f\n", (double)table->delta/para->time);

    //fprintf(stdout,"collision: %d\n", para->collision);
    fprintf(stdout,"total CPU time: %.3f (sec)    \n", (double)time);
	fprintf(stdout, "(per 1 trans.: %.3f msec)	\n", (double)(time_per * 1000));
    fprintf(stdout,"num of outputs: %d\n", para->num_outputs);
}
