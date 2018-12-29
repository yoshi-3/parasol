/*
 *  main.c
 *  
 */


#include "parasol.h"

/*
 * main function
 */
int main(int argc, char *argv[])
{
	TABLE* table;		   //entry table
    LIST** map;            //access map (hash map)
    PARAMETER* para;	   //parameters
	TREE* tree;		       //tree
	
    I_TABLE* i_table = NULL; //entry table for item mining
    MAP** i_map = NULL;
    I_PARAMETER* i_para;     //parameters for item mining
	FILE* stream;		     //input streams
		
	int ch, opt_idx;	    //to check the arguments
    int table_max;
	time_t timer;
	struct tm *date;
	
	/* Outputting the start time of the process as stderr */
	timer=time(NULL);
	date=localtime(&timer);
	fprintf(stderr,"%s",asctime(date));
	
	fprintf(stderr,"---------- Program START ----------\n");
	
	/* Initializing the values of parameters */
	para=(PARAMETER*)malloc(sizeof(PARAMETER));
	para->table_size = TABLE_SIZE;  //maximal entry size to be registered
    para->support = 0.0;			//minimal support threshold
	para->error = 0.0;				//error parameter
	para->time = 0;                 //the stream length
	para->reduce = 0;				//flag for stream reduction (0:OFF, 1:ON)
	para->comp = 0;				    //flag for delta-compression (0:OFF, 1:ON)
    para->disp_output = 0;           //flag for writing the output files
    para->lc = 0;					//PC approximation
	para->ss = 0;					//RC approximation
	strcpy(para->in_filename,DEF_INFILE);	//input file name
	strcpy(para->out_filename,DEF_OUTFILE);	//output file name
	
	//Initializing the values of parameters (for item mining)
	i_para=(I_PARAMETER*)malloc(sizeof(I_PARAMETER));
	i_para->table_cap = 0;
	i_para->table_size = TABLE_SIZE;
	i_para->table_max = 0;
	i_para->hash_size = HASH_SIZE;
	i_para->item_cnt = 0;
	
	//Obtaining the arguments for the parameters by getopt function
	struct option long_opts[] = {
		{"reduce", 0, NULL, 0},
		{"comp", 0, NULL, 1},
        {"len", 0, NULL, 2},
		{0, 0, 0, 0}
	};
	
    while((ch = getopt_long(argc, argv, "s:e:k:i:o:h", long_opts, &opt_idx)) != -1){
		switch(ch){
			case 0://performing the stream reduction
				para->reduce = 1;
				break;
			case 1://performing the Delta-compression
				para->comp = 1;
				break;
            case 2://setting the maximal transaction length
                i_para->T_max = atof(optarg);
                break;
			case 's'://setting minimal support
				para->support=atof(optarg);
				break;
			case 'e'://setting an error parameter (epsilon)
				para->error=atof(optarg);
                para->lc = 1;
                break;
            case 'k'://setting the size of entry table
                para->table_size=atof(optarg);
                para->ss= 1;
                break;
            case 'i'://setting input file name
                sprintf(para->in_filename,"%s",optarg);
                break;
            case 'o'://setting result output file name
                para->disp_output = 1;
                sprintf(para->out_filename,"%s",optarg);
                break;
			case 'h'://displaying the help menue
				PrintHelp();
				exit(0);
				break;
			default://in default, displaying the help menue
				PrintHelp();
				exit(0);
				break;
		}
	}
		
	fprintf(stderr,"*********** Preparation ***********\n");
	table_max = 2*para->table_size + 1; //maximal size of entries that have been registered
	
    if(para->reduce){
        i_map = (MAP**)malloc(sizeof(MAP*) * HASH_SIZE); //access map (hash map) for item mining
    }
    
    table = MakeTable(table_max); //preparing the entry table
	tree = SetTree(table_max); //preparing the entry tree
	map = InitMap(); //preparing the access map
	
	stream = fopen(para->in_filename, "r"); //opening the input stream file
	if(stream == NULL){
		fprintf(stderr,"Can not open %s!\n",para->in_filename);
		exit(0);
	}
	else {
		fprintf(stderr, "OpenStream\t OK!\n");
	}

	//for stream reduction
	if(para->reduce){
		fprintf(stderr, "i-table size: %d \n", i_para->table_size);
        if(para->error == 0){
            fprintf(stderr, "Please specify the error patameter to use the stream reduction");
            exit(0);
        }
		i_para->table_size = ceil((float)SUBSEQ_LEN / para->error);
		i_table = I_MakeTable(i_para); //preparing the table for item mining
		I_InitMap(i_map); //pareparing the map for item mining
	}
	
	fprintf(stderr,"***********   Mining    ***********\n");
	
	//mining the input streams
	MiningStream(tree, table, map, stream, para, i_table, i_map, i_para);

    if(para->disp_output){
        fprintf(stderr,"***********   Output    ***********\n");
        OutputTable(table, para);	//outputting the entry table
        OutputResult(map, table, tree, para); //outputting the frequent itemsets
    }
    fprintf(stderr,"***********  Parameter  ***********\n");
	ShowParameter(para, table); //outputting each parameter
	
	fprintf(stderr,"*********** Free Memory ***********\n");
	FreeMap(map); //opening the access map
    free(map);
	FreeTable(table, para); //opening the entry table
	
	if(para->reduce){
		I_FreeMap(i_map, i_para->hash_size);
		I_FreeTable(i_table, i_para);
        free(i_map);
	}
	fprintf(stderr,"---------- Program  END  ----------\n");
	
	//Outputting the end time of the process as stderr
	timer = time(NULL);
	date = localtime(&timer);
	fprintf(stderr,"%s",asctime(date));
	
	return 0;
}

