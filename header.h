//header.h

typedef struct listnode * listptr;

struct listnode {

	char* clientIP;
	char* clientPort;
	listptr next;

};


//List funcions
void insertList(listptr*, char*, char*);
void destroyList(listptr*);
int deleteClient(listptr*, char*, char*);
int isInList(listptr,char*, char*);
void print(listptr);


//Other 
int numOfDigits(int);
void perror_exit(char* message);
int numOfDigits(int num);
