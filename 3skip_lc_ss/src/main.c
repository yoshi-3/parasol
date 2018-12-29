/*
 *  main.c
 *  
 *
 *
 */

#include"mineSubset.h"

void CheckLCM(LIST** map, TABLE* table, PARAMETER* para, char* file, int threshfold);
int split(char*** ary, const char* s, const char *delimiter);
void CompFreqItems(PARAMETER* para, int threshfold);
void SetEntries(int num, TABLE* table, LIST** map, PARAMETER*);
void MiningStream(TABLE *table, LIST** map, FILE* stream, PARAMETER *para);


/*
 * main function
 */
int main(int argc, char *argv[])
{
	
	TABLE *table;	//entry table
	
	LIST* map[HASH_SIZE]; //access map (hash map)
	
	FILE* stream;		//input streams
	PARAMETER* para;	//parameters
	int ch, opt_idx;				//to check the arguments
	
	time_t timer;
	struct tm *date;
	
	/*Outputting the start time of the process as stderr*/ 
	timer=time(NULL);
	date=localtime(&timer);
	fprintf(stderr,"%s",asctime(date));
	
	fprintf(stderr,"---------- Program START ----------\n");
	
	/*Initializing the values of parameters*/
	para=(PARAMETER*)malloc(sizeof(PARAMETER));
	para->table_size = TABLE_SIZE;  //maximal entry size to be registered
	para->support = 0.2;			//minimal support
	para->error = 0.1;				//maximal error ratio
	para->table_cap = 0;			//flag indicating if the entry table is full (0:OFF, 1:ON)
	para->table_max = 0;			//maximal size of entries that have been registered
	para->stream_len = -1;			//initial value of the stream length
	para->time = 1;					//initialize the current time step
	para->hash_size = HASH_SIZE;	//hash size
	para->collision = 0;			//num of collisions
	para->lcm_check = 0;			//flag if the recall and precision are computed
	para->match_freq_items = 0;		//the frequent outputs in table
	para->num_freq_items = 0;		//the number of frequent itemsets
	para->num_outputs = 0;			//the number of outputs
	para->max_stream_len = 0;		//maximal length of each stream
	para->total_stream_len = 0;		//total length of each stream
	para->skip_num = 0;				//total number of skip operations
	para->burst_num = 0;			//bursty transactions
	para->t_skip = 0;				//the flag for applying t-skip operation
	para->r_skip = 0;				//the flag for applying r-skip operation
	para->reduce = 0;				//the flag for reducing the transactions in advance
	strcpy(para->in_filename,DEF_INFILE);	//input file name
	strcpy(para->out_filename,DEF_OUTFILE);	//output file name
	
	/*Obtaining the arguments for the parameters by getopt function*/
	
	struct option long_opts[] = {
		{"tskip", 0, NULL, 0},
		{"rskip", 0, NULL, 1},
		{"skip", 0, NULL, 2},
		{0, 0, 0, 0}
	};
	
	while((ch = getopt_long(argc,argv,"s:e:i:o:l:k:hcr", long_opts, &opt_idx)) != -1){
		switch(ch){
			case 0:	 //applying the t-skip operation
				para->t_skip = 1;
				break;
			case 1:  //applying the r-skip operation
				para->r_skip = 1;
				break;
			case 2:  //applying both t and s skip operations
				para->t_skip = 1;
				para->r_skip = 1;
				break;
			case 's'://setting minimal support (delta)
				para->support=atof(optarg);
				break;
			case 'e'://setting maximal error ratio (epsilon)
				para->error=atof(optarg);
				break;
			case 'i'://setting input file name
				sprintf(para->in_filename,"%s",optarg);
				break;
			case 'o'://setting output file name
				sprintf(para->out_filename,"%s",optarg);
				break;
			case 'c'://checking the recall and precision with LCM file name
				para->lcm_check = 1;
				break;
			case 'r'://reducing the transactions by the single stream mining by SS method
				para->reduce = 1;
				break;
			case 'l'://setting the size of input streams
				para->stream_len=atof(optarg);
				break;
			case 'k'://setting the size of entry table
				para->table_size=atof(optarg);
				para->table_cap=1;
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
	table=MakeTable(para); //preparing the entry table
	
	InitMap(map, para->hash_size); //preparing the access map
	//stream=GetStream(para); //getting the input streams
	
	stream = fopen(para->in_filename, "r"); //opening the input stream file
	if(stream == NULL){
		fprintf(stderr,"Can not open %s!\n",para->in_filename);
		exit(0);
	}
	else {
		fprintf(stderr, "OpenStream\t OK!\n");
	}
	
	fprintf(stderr,"***********   Mining    ***********\n");
	MiningStream(table, map, stream, para); //mining the input streams
	
	if(para->lcm_check){
		fprintf(stderr,"***********   Testing   ***********\n");
		CompFreqItems(para, table->delta+1); //computing the freq. itemsets by lcm
		char* file = "lcm/data/output.dat";
		CheckLCM(map, table, para, file, table->delta+1);	
	}
	
	fprintf(stderr,"***********   Output    ***********\n");
	//OutputMap(map, table, para); //outputting the map
	//OutputTable(table, para);	//outputting the entry table
	
	OutputResult(map, table, para); //outputting the frequent itemsets 
	// Take care of using this function, since it must change the current 
	// status of the access map and entry table
	
	fprintf(stderr,"***********  Parameter  ***********\n");
	ShowParameter(para, table); //outputting each parameter
	//ShowStream(stream, para);
	
	fprintf(stderr,"*********** Free Memory ***********\n");
	FreeMap(map, para->hash_size); //opening the access map
	//FreeStream(stream, para->stream_len, 1); //opening the stream
	fclose(stream);
	FreeTable(table, para); //opening the entry table
	
	fprintf(stderr,"---------- Program  END  ----------\n");
	
	/*Outputting the end time of the process as stderr*/
	timer = time(NULL);
	date = localtime(&timer);
	fprintf(stderr,"%s",asctime(date));
	
	return 0;
}


void SetEntries(int num, TABLE* table, LIST** map, PARAMETER* para){
	
	int i, j;
	int* subsets[num];
	
	for(i = 0; i < num; i++){
		subsets[i] = (int*)malloc(sizeof(int) * 5);
	}
	
	// 値を入力
	for(i = 0; i < num; i++){
		for(j = 0; j < 5; j++){
			subsets[i][j] =  5 * i + j;
		}
	}	
	
	// 登録する
	for(i = 0; i < num; i++){			
		RegistSet(subsets[i], 5, map, table, GetHashKey(subsets[i], 5, para->hash_size));
	}	
}


void CompFreqItems(PARAMETER* para, int threshfold){
	
	// step 1. まずは問題を作成する
	pid_t pid2 = fork();
	if(pid2 == 0){
		//子プロセスにて実行
		// step 2. 頻度を計算
		//int freq = ceil(para->support * para->time);
		int freq = threshfold;
		
		//int freq = 100;
		fprintf(stderr, "min freq count: %d\n", freq);
		
		char freq_s[64];
		sprintf(freq_s,"%d",freq);
		
		char* argv2[10];
		// step 3. LCMで問題を解く
		printf("*** computing frequent itemsets by LCM ***\n");
		
		argv2[0] = (char* const)"./lcm/lcm"; //コマンド
		argv2[1] = (char* const)"FfV"; //引数
		argv2[2] = para->in_filename;
		argv2[3] = freq_s;
		argv2[4] = (char* const)"lcm/data/output.dat"; //出力ファイル名
		argv2[5] = (char* const)"1>";
		argv2[6] = (char* const)"lcm/data/log_s.txt"; //標準出力ログデータ
		argv2[7] = (char* const)"2>";
		argv2[8] = (char* const)"lcm/data/log_e.txt"; //標準エラー出力ログデータ
		argv2[9] = NULL;
		execvp(argv2[0], argv2); //実行 (closed itemsets)
		
	}
	else{
		int status;
		waitpid(pid2, &status, 0); // 計算を待つ			
	}
	
}



void CheckLCM(LIST** map, TABLE* table, PARAMETER* para, char* file, int threshfold){
	
	FILE *input;
	char buf[SUBSEQ_LEN];
	char delimiter[] = " ";
	ENTRY* e = NULL;
	
	int i, stream_num, len, hash_key;
	int hit = 0;
	int total = 0;
	
	char** ary = (char**)malloc(sizeof(char*));
	
	// step 1. file open
	if((input=fopen(file,"r")) == NULL){
		fprintf(stderr, "Can not open  %s \n", file);
		exit(0);
	}
	// step 2. parsing the input file
	fscanf(input, " (%d)\n", &stream_num);
	while(fgets(buf, SUBSEQ_LEN, input) != NULL){
		total++;
		int subset[SUBSEQ_LEN];
		len = split(&ary, buf, delimiter);
		PAIR* pairs = (PAIR*)malloc(sizeof(PAIR)*len);
		for(i = 0; i < len-1; i++){
			pairs[i].left = atoi(ary[i]);
		}
		q_sort(pairs, 0, 0, len-2); // quick sort
		for(i = 0; i < len-1; i++){
			subset[i] = pairs[i].left;
		}
		free(pairs); // free the memory
		//SubsetPrint(subset, len-1);
		// step 3. check if this frequent set is included in the table
		hash_key = GetHashKey(subset, len-1, para->hash_size);
		e = GetRegistSet(subset, len-1, map, table, hash_key);
		
		if(e != NULL){
			// this frequent set is included in the table
			// we check this entry is included in the outputs 
			if(e->e_count >= threshfold){
				hit++;
			}
		}
		//else {
		//	SubsetPrint(subset, len-1);
		//}
		
	}
	//fprintf(stderr, "hit: %d, table size: %d, freq size: %d\n", hit, table->last, total);
	para->match_freq_items = hit;
	para->num_freq_items = total;
}

int split(char*** ary, const char* s, const char* delimiter){
	
	char *tmp = strdup(s);
	int i, n;
	for(i = n = 0; (tmp = strtok(tmp, delimiter)) != NULL; i++){
		if(i >= n){
			n += ALLOCATE_SIZE;
			*ary = (char**)realloc(*ary, sizeof(char*) * n);
		}
		(*ary)[i] = strdup(tmp);
		tmp = NULL;
	}
	return i;
}


