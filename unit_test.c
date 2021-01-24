#include  "acutest.h"			
#include "jsonStruct.h"         
#include "hashTable.h"         
#include "graph.h"
#include "math.h"
#include "logisticRegression.h"
#include "jobScheduler.h"
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;


void worker(void *arg)
{
	
    Arguments *args = arg;
	*(args->activeThreads) -= 1;
	

	//signal
	pthread_mutex_lock(&mu);
  	pthread_cond_signal(&cond);
  	pthread_mutex_unlock(&mu);
	
	

}

void test_create_list(void) {
	
	recordList *list = createRecordList(NULL);
	jsonFile* file_ptr = createJsonFile("test");
	recordNode* ptr = createRecordNode(file_ptr);

	TEST_ASSERT(list != NULL);
	TEST_ASSERT(list->counter == 0);	

	insertRecordList(list,ptr);
	TEST_ASSERT(!(strcmp(list->head->file->site,"test")));
	TEST_ASSERT(list->counter == 1);

	freeRecordList(list);
	
}

void test_create_jsonNode(void)
{
	jsonNode *ptr = createJsonNode("KEY","DESC");
	
	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(!(strcmp(ptr->desc,"DESC")) && !(strcmp(ptr->key,"KEY")));

	freeJsonNode(ptr);

}

void test_create_jsonFile(void)
{
	jsonFile* ptr = createJsonFile("test");
	

	//test node insertion
	jsonNode *ptr_node = createJsonNode("KEY","DESC");
	TEST_ASSERT(ptr != NULL);
	insertJsonFile(ptr, ptr_node );

	//test create and insert
	//jsonFile* ptr_node2 = createAndInsertJsonNode(ptr,"KEY2","DESC2");
	//jsonNode* test_ptr = ptr->tail;


	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(!(strcmp(ptr->site,"test")));
	TEST_ASSERT(!(strcmp(ptr->head->key,"KEY")) && !(strcmp(ptr->head->desc,"DESC")) && ptr->counter == 1 );
	//TEST_ASSERT(!(strcmp(test_ptr->key,"KEY2")) && !(strcmp(test_ptr->desc,"DESC2")));
	freeJsonFile(ptr);
	
	
}
	
	

void test_create_RecordNode(void)
{
	jsonFile* file_ptr = createJsonFile("test");
	recordNode* ptr = createRecordNode(file_ptr);

	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(!(strcmp(ptr->file->site,"test")));
	freeRecordNode(ptr);
	
}

void test_create_RecordList(void)
{
	jsonFile* file_ptr = createJsonFile("test");
	recordNode* node_ptr = createRecordNode(file_ptr);
	recordList* ptr = createRecordList();//create the list
	TEST_ASSERT(ptr != NULL);
	
	insertRecordList(ptr, node_ptr);
	//check if haid and tail have the right node
	TEST_ASSERT(!(strcmp(ptr->head->file->site,"test")));
	TEST_ASSERT(!(strcmp(ptr->tail->file->site,"test")));
	freeRecordList(ptr);
}


void test_create_hashTable_and_add(void)
{
	Bucket **ptr = initHashTables(1,500);
	TEST_ASSERT(ptr != NULL);

	jsonFile* file_ptr = createJsonFile("test1");//create json file to test the ht
	jsonNode *ptr_node = createJsonNode("KEY","DESC");//insert to file
	insertJsonFile(file_ptr, ptr_node );
	//lets just assume that the hash function returns 1
	ptr[0] = addToHashTable(ptr[0],"test",0,file_ptr);
	bucketNode * test = ptr[0]->array[0];
	TEST_ASSERT(!(strcmp(test->name,"test")));

	free(ptr[0]->array[0]->graph->head->name);
	free(ptr[0]->array[0]->graph->head);
	free(ptr[0]->array[0]->graph);
	freeBuckets(ptr,1);
	freeJsonFile(file_ptr);

}

void test_search(void)
{
	Bucket **ptr = initHashTables(3,500);

	jsonFile* file_ptr = createJsonFile("test");//create json file to test the ht
	jsonNode *ptr_node = createJsonNode("KEY","DESC");//insert to file
	insertJsonFile(file_ptr, ptr_node );
	//lets just assume that the hash function returns 1
	ptr[1] = addToHashTable(ptr[1],"test",1,file_ptr);

	Bucket* fnd = searchBuckets(ptr[1],"test");
	TEST_ASSERT(!(strcmp(fnd->array[0]->name,"test")));

	Clique* fnd_node = findEntry(ptr,1, "test");
	TEST_ASSERT(!(strcmp(fnd_node->name,"test")));
	//free graph
	free(ptr[1]->array[0]->graph->head->name);
	free(ptr[1]->array[0]->graph->head);
	free(ptr[1]->array[0]->graph);
	freeBuckets(ptr,3);
	freeJsonFile(file_ptr);
}

void test_word_list_and_node(void)
{
	wordList* list_ptr;
	list_ptr = createWordList();

	TEST_ASSERT(list_ptr != NULL);
	TEST_ASSERT(list_ptr->counter == 0);	

	wordNode* node_ptr;
	char* word = "test";
	node_ptr = createWordNode(word);
	TEST_ASSERT(!strcmp(node_ptr->word,"test" ));

	insertWordNode(list_ptr,node_ptr);
	TEST_ASSERT(!strcmp(list_ptr->head->word,"test" ));
	TEST_ASSERT(list_ptr->counter == 1);	

	freeWordList(list_ptr);

}

void test_word_ht(void)
{
	word_ht* ht;
	ht = create_word_ht(500);

	TEST_ASSERT(ht != NULL);
	TEST_ASSERT(ht->id_counter == 0);	

	insert_pair_hash_table(ht,"test");
	
	//Search the ht for the word.The id must be zero because its the first word
	int id = searchWordHashTable(ht,"test");
	TEST_ASSERT(id== 0);

	//free the ht
	freeWordHT(ht);

}

// void test_logistic_regression(void)
// {
// 	logistic_reg *ptr;
// 	ptr = create_logistic_reg(100);
// 	TEST_ASSERT(ptr != NULL);
// 	TEST_ASSERT(ptr->lineSize == 100);
// 	TEST_ASSERT(ptr->weights != NULL);

// 	double** array;
// 	double* ar_ptr;

// 	array = malloc(2* sizeof(double*));

// 	for(int i = 0; i< 2; i++)
// 	{
// 		array[i] = malloc(100*sizeof(i));
// 	}

// 	for(int i = 0; i< 2; i++)
// 	{
// 		ar_ptr = array[i];
// 		//give some values to array
// 		for(int i = 0; i< 100; i++)
// 		{
// 			ar_ptr[i] = 1.0/i;
// 		}
// 	}
	
// 	fit(ptr,array,0,100,2);

// 	TEST_ASSERT(ptr->x != NULL);
// 	TEST_ASSERT(ptr->y == 0);

// 	ptr = logisticRegretionAlgorithm(ptr,10);

// 	//test if there is any non zero weight
// 	int flag = 0;
// 	for(int i = 0; i< ptr->lineSize; i++)
// 	{
// 		if(ptr->weights[i]!=0)
// 		{
// 			flag = 1;
// 		}
// 	}
// 	TEST_ASSERT(flag);

	
	
// 	free(array[0]);
// 	free(array[1]);
// 	free(array);

// 	freeLogisticRegressor(ptr);

// }

void test_array_functions()
{
	double firstArray[5];
	double secondArray[5];
	int size = 5;

	// fill array with numbers
	for(int j=0;j<5;j++){
		firstArray[j] = 1.1;
		secondArray[j] = 2.2;
	}

	double *newArray;

	newArray = arrayConcat(firstArray,secondArray,size);

	int flag = 1;
	for(int i=0;i<size;i++){
		
		
		if(newArray[i] != sqrt(pow((firstArray[i] - secondArray[i]), 2) +  pow((firstArray[i] - secondArray[i]), 2) ))
		{
			flag = 0;
		}
		
	}
	TEST_ASSERT(flag);

	free(newArray);
}

void test_JobScheduler()
{
	// sparceMatrix * matrix = createSparceMatrix();
    // for(int i = 0; i < 10; i++)
    // {
    //     insertMatrixNode(matrix,i,(i*0.1));
    // }

    jobScheduler *jb;
    Job *job;
    Arguments *args;

	int num = 10;

	args = malloc(sizeof(Arguments));
	args->activeThreads = &num;
	args->start = 0;
	args->finish = 0;
	args->J = NULL;
	args->k = 0;
	args->classfier = NULL;
	

    jb = initialise_jobScheduler(1);

    for (int i = 0; i<1; i++) {
        
        job = create_job(worker,args);
  
        queueInsert(jb,job);
    }
    
	//wait worker to run
	pthread_mutex_lock(&mu);
	pthread_cond_wait(&cond,&mu);
	pthread_mutex_unlock(&mu);

	jb->stop = 1;
	jb->alive_thread_count = 0;
	
 
	
	free(args);
	free(jb->tids);
	free(jb->q);
	free(jb);

	

	TEST_ASSERT(num == 9);

}

TEST_LIST = {
    { "list_create", test_create_list },
	{ "JsonNode_create", test_create_jsonNode },
	{"JsonFile_create", test_create_jsonFile},
	{"RecordNode_create", test_create_RecordNode},
	{"RecordList_create", test_create_RecordList},
	{"hashTable_create", test_create_hashTable_and_add},
	{"test_search", test_search},
	{"test_word_list_and_node", test_word_list_and_node},
	{"test_word_hash_table", test_word_ht},
	//{"test_logistic_regression", test_logistic_regression},
    {"test_array_functions",test_array_functions},
	{"test_JobScheduler",test_JobScheduler},
    { NULL, NULL } 
};
