#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_FILE_NAME_LEN	256
#define MAX_WORD_LEN		40
#define MAX_NUM_SYMBOLS		31 //26 letters+special characters

struct TrieNodeStruct
{
	bool	isEnd;
	int 	wordFrequency;
	int 	indexInHeap;
	struct  TrieNodeStruct*	nextSymbol[MAX_NUM_SYMBOLS];
};

typedef struct TrieNodeStruct TrieNode;

typedef struct
{
	TrieNode* root;
	int 	wordFrequency;
	char* 	word;
}MinHeapNode;

typedef struct
{
	int minHeapSize;
	int count;
	MinHeapNode* nodeArray;
}MinHeap;

TrieNode* wordRootNode;	//store the root of the global word tree

//create a new trie node
TrieNode* newTrieNode()
{
    TrieNode* trieNode = (TrieNode*) malloc(sizeof(TrieNode));
	
	//initialize
    trieNode->isEnd = 0;
    trieNode->wordFrequency = 0;
    trieNode->indexInHeap = -1;

    for (int i = 0; i < MAX_NUM_SYMBOLS; i++)
    {
    	trieNode->nextSymbol[i] = NULL;
    }

    return trieNode;
}

//create a new min heap
MinHeap* createMinHeap(int minHeapSize)
{
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
 
    minHeap->minHeapSize = minHeapSize;
    minHeap->count  = 0;
    minHeap->nodeArray = (MinHeapNode*) malloc(sizeof(MinHeapNode)*minHeap->minHeapSize);
 
    return minHeap;
}

//swap the two min-heap nodes, and update the indexInHeap in trie
void swapMinHeapNodes(MinHeap* minHeap, int indexA, int indexB)
{
    MinHeapNode* nodeA = &minHeap->nodeArray[indexA];
    MinHeapNode* nodeB = &minHeap->nodeArray[indexB];
    MinHeapNode  tempNode = minHeap->nodeArray[indexA];

    *nodeA = *nodeB;
    *nodeB = tempNode;

    //update indexInHeap in trie
    nodeA->root->indexInHeap = indexA;
    nodeB->root->indexInHeap = indexB;
}

//when the value of a node decreases, or newly added to the heap, readjust the order by moving it up
//(if needed)
void upReorderMinHeap(MinHeap* minHeap, int indexInHeap)
{
    int parentIndex = (indexInHeap - 1)/2;

    if (minHeap->nodeArray[indexInHeap].wordFrequency < 
        minHeap->nodeArray[parentIndex].wordFrequency)
    {
        //swap the nodes
        swapMinHeapNodes(minHeap, indexInHeap, parentIndex);

        upReorderMinHeap(minHeap, parentIndex);
    }
}

//when the value of node increases, readjust its order by moving it from top to down (if needed)
void downReorderMinHeap(MinHeap* minHeap, int indexInHeap)
{
    int leftChild = 2*indexInHeap + 1;
    int rightChild = 2*indexInHeap + 2;
    int smallest = indexInHeap;

    //find the smallest node among the parent and its two children
    if ((leftChild < minHeap->count) && 
        (minHeap->nodeArray[leftChild].wordFrequency < minHeap->nodeArray[smallest].wordFrequency))
    {
        smallest = leftChild;
    }

    if ((rightChild < minHeap->count) && 
        (minHeap->nodeArray[rightChild].wordFrequency < minHeap->nodeArray[smallest].wordFrequency))
    {
        smallest = rightChild;
    }
 
    if (smallest != indexInHeap)
    {
        //parent larger than one of its children, need to swap with the smallest
        swapMinHeapNodes(minHeap, smallest, indexInHeap);
        
        //go down and adjust the order for the next level in min-heap
        downReorderMinHeap(minHeap, smallest);
    }
}

//insert a node in min heap
void insertInMinHeap(MinHeap* minHeap, TrieNode* root, char* word)
{
    //if the word already in heap
    if (root->indexInHeap != -1)
    {
    	minHeap->nodeArray[root->indexInHeap].wordFrequency++;
		
		//reordering the heap
        downReorderMinHeap(minHeap, root->indexInHeap);
    }
 
    //word is not present and heap is not full
    else if (minHeap->count < minHeap->minHeapSize)
    {
        int count = minHeap->count;

        minHeap->nodeArray[count].wordFrequency = root->wordFrequency;
        minHeap->nodeArray[count].word = (char*) malloc(sizeof(char)*(strlen(word)+1));
        strcpy(minHeap->nodeArray[count].word, word);
 
        minHeap->nodeArray[count].root = root;
        root->indexInHeap = minHeap->count;
        minHeap->count++;
		
		//reordering the heap
        upReorderMinHeap(minHeap, minHeap->count-1);
    }
 
    //word is not present and heap is full, and the frequency is larger than
    //the least frequent word, replace the least frequent word
    else if (root->wordFrequency > minHeap->nodeArray[0].wordFrequency)
    { 
        minHeap->nodeArray[0].root->indexInHeap = -1;
        minHeap->nodeArray[0].root = root;
        minHeap->nodeArray[0].root->indexInHeap = 0;
        minHeap->nodeArray[0].wordFrequency = root->wordFrequency;
 
        free(minHeap->nodeArray[0].word);
        minHeap->nodeArray[0].word = (char*) malloc(sizeof(char)*(strlen(word)+1));
        strcpy(minHeap->nodeArray[0].word, word);
		
		//reordering
        downReorderMinHeap(minHeap, 0);
    }
}

//add a word to the trie with root at rootNode, and update its position in min-heap (if needed)
//rootNode: rootNode of trie; minHeap: min-heap to store top K words;
//word: string to be inserted; originalWord: copy of the original word to store in min-heap
void insertWord(TrieNode*& rootNode, MinHeap* minHeap, char* word, char* originalWord)
{
    if (rootNode == NULL)
    {
        rootNode = newTrieNode();
    }
 
    if (!(*word == '\0' || *word == ',' || *word == '.' || *word == '!' || *word == '?' || *word == ';'))
    {
    	//the head letter is not '\0' or a special character, so it is not end of word
        //keep inserting to the next level
        insertWord(rootNode->nextSymbol[tolower(*word)-97], minHeap, word+1, originalWord);
    }
    else
    {
        //we met a '\0' or a special character, so this is already the end of the last word
        if (rootNode->isEnd)
        {
            rootNode->wordFrequency++;
        }
        else
        {
            rootNode->isEnd = 1;
            rootNode->wordFrequency = 1;
        }

        //for a special character, we also need to add it to trie to count its frequency
        switch(*word)
        {
            case ',':
                insertWord(wordRootNode->nextSymbol[26], minHeap, word+1, word);
                break;
            case '.':
                insertWord(wordRootNode->nextSymbol[27], minHeap, word+1, word);
                break;
            case '!':
                insertWord(wordRootNode->nextSymbol[28], minHeap, word+1, word);
                break;
            case '?':
                insertWord(wordRootNode->nextSymbol[29], minHeap, word+1, word);
                break;
            case ';':
                insertWord(wordRootNode->nextSymbol[30], minHeap, word+1, word);
                break;
            default:                
                break;
        }

        if (*word != '\0')
        {
            //if we met a special character, we eliminate the special character in originalWord,
            //so that the stored word does not have the special character
            *word = '\0'; 
        }
        
        //printf("%s: %d\n", originalWord, rootNode->wordFrequency);

        //adjust the word's position in min-heap	
        insertInMinHeap(minHeap, rootNode, originalWord);
    }
}

//add a word to the trie and the min heap
void addToTrieAndHeap(TrieNode*& root, MinHeap* minHeap, char* word)
{
	insertWord(root, minHeap, word, word);
}

//print out the words on min heap
void printMinHeap(MinHeap* minHeap)
{
    for(int i = 0; i < minHeap->count; i++)
    {
        printf( "%s : %d\n", minHeap->nodeArray[i].word, minHeap->nodeArray[i].wordFrequency);
    }
}

//read the file, count the frequency of each word, and then output the top-k words
void printTopKWords(FILE* inputFilePtr, int valueK)
{
    MinHeap* 	minHeap = createMinHeap(valueK);
    wordRootNode = newTrieNode();	//initialize the global tree node   
    char 	  	inputWord[MAX_WORD_LEN] = {0};

    while(fscanf(inputFilePtr, "%s", inputWord) != EOF)
    {
        addToTrieAndHeap(wordRootNode, minHeap, inputWord);
    }

    printMinHeap(minHeap);
}

int main()
{
	char fileName[MAX_FILE_NAME_LEN];
	int  valueK = 0;

	printf("Input the name of the file:\n");
	//fgets(fileName, sizeof(fileName), stdin);
    scanf("%s", fileName);
	printf("Input the value of K:\n");
	scanf("%d", &valueK);

	FILE* 	inputFilePtr = fopen(fileName, "r");
    //FILE*   inputFilePtr = fopen("test.txt", "r");
	if(inputFilePtr == NULL)
	{
		printf("No file!\n");
	}
	else
	{
        printTopKWords(inputFilePtr, valueK);
	}

	return 0;
}