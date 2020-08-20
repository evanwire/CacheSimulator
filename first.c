#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct cache{
  int cacheSize;
  int setSize;
  int numSets;
  int blockSize;
  char* cachePolicy;
  int prefetchSize;

};




struct cacheBlock{
  int v;
  long int tag;
};


int checkInputs(char**);
int checkPowerTwo(int);
void fillCache(struct cache*, char**);
void writeToCache(long int, struct cacheBlock**, struct cache*);
int checkHit(long int, struct cacheBlock**, struct cache*, int prefetch);
void freeCache(struct cacheBlock**, struct cache*);
void cacheSimulation(struct cache*, char*);


int main(int argc, char** argv){


  int test = checkInputs(argv);



  if(test == 1){
    return 0;
  }

  struct cache* c = malloc(sizeof(struct cache));
  fillCache(c, argv);

  cacheSimulation(c, argv[6]);

  free(c);




}




int checkInputs(char** arr){
  
  
  char* cacheSize = arr[1];
  char* blockSize = arr[2];
  char* cachePolicy = arr[3];
  char* associativity = arr[4];
  char* prefetchSize = arr[5];
  char* traceFile = arr[6];



  //CACHE SIZE
  int cs;
  sscanf(cacheSize, "%d", &cs);
  int check_cs = checkPowerTwo(cs);
  if(check_cs == 1){
    return 1;
  }



  //BLOCK SIZE
  int bs;
  sscanf(blockSize, "%d", &bs);
  int check_bs = checkPowerTwo(bs);
  if(check_bs == 1){
    return 1;
  }


  
  //CACHE POLICY
  if(strcmp(cachePolicy, "lru") == 1 && strcmp(cachePolicy, "fifo") == 1){
    return 1;
  }



  //ASSOCIATIVITY
  if(strcmp(associativity, "direct") == 1 && strstr(associativity, "assoc") == NULL){
    return 1;
  }

  
  if(strstr(associativity, "assoc") != NULL && strcmp(associativity, "assoc") == 1){
    int sa;
    sscanf(associativity, "assoc:%d", &sa);

    sa = checkPowerTwo(sa);
    
    if(sa == 1){

      return 1;
    }
  }



  //PREFETCH SIZE
  int ps;
  sscanf(prefetchSize, "%d", &ps);
  if((float)ps > (cs / bs)){
    return 1;
  }



  //TRACE FILE
  FILE* file = fopen(traceFile, "r");
  if(file == NULL){
    fclose(file);
    return 1;
  }
  fclose(file);


  return 0;
  

}


int checkPowerTwo(int test){
  
  if(ceil(log2(test)) == floor(log2(test))){
    return 0;
  }
  return 1;
}

void fillCache(struct cache* c, char** arr){


  char* cacheSize = arr[1];
  char* blockSize = arr[2];
  char* cachePolicy = arr[3];
  char* assoc = arr[4];
  char* prefetchSize = arr[5];

  int cs;
  sscanf(cacheSize, "%d", &cs);
  c->cacheSize = cs;

  int bs;
  sscanf(blockSize, "%d", &bs);
  c->blockSize = bs;

  c->cachePolicy = cachePolicy;



  
  if(strcmp(assoc, "direct") == 0){
    c->setSize = 1;
  }else if(strcmp(assoc, "assoc") == 0){
    c->setSize = c->cacheSize / c->blockSize;
  }else{
    int sa;
    sscanf(assoc, "assoc:%d", &sa);
    c->setSize = sa;
  }

  c->numSets = cs / (c->setSize * bs);


  int ps;
  sscanf(prefetchSize, "%d", &ps);
  c->prefetchSize = atoi(arr[5]);



		  
  
}

void cacheSimulation(struct cache* c, char* tracefile){

  struct cacheBlock** cache = malloc(c->numSets * sizeof(struct cacheBlock*));
  for(int i = 0; i < c->numSets; i++){
    cache[i] = malloc(c->setSize * sizeof(struct cacheBlock));
  }

  struct cacheBlock** prefetch_cache = malloc(c->numSets * sizeof(struct cacheBlock*));
  for(int i = 0; i < c->numSets; i++){
    prefetch_cache[i] = malloc(c->setSize * sizeof(struct cacheBlock));
  }

  for(int i = 0; i < c->numSets; i++){
    for(int j = 0; j < c->setSize; j++){
      cache[i][j].v = 0;
      prefetch_cache[i][j].v = 0;
    }
  }



  FILE* file;
  char line[50];
  char mode;
  long int address;
  file = fopen(tracefile, "r");
  
  int reads = 0;
  int writes = 0;
  int hits = 0;
  int misses = 0;

  int preads = 0;
  int pwrites = 0;
  int phits = 0;
  int pmisses = 0;
  
    
  while(fgets(line, 50, file) != NULL){



    sscanf(line, "%c %zx", &mode, &address);
    if(mode == '#'){
      break;
    }
    int hit = checkHit(address, cache, c, 0);

    int prefetch_hit = checkHit(address, prefetch_cache, c, 0);

    if(hit == 1){
      hits++;
      if(mode == 'W'){
	writes++;
      }
    }

    if(prefetch_hit == 1){
      phits++;
      if(mode == 'W'){
	pwrites++;
      }
    }


    
    
    if(prefetch_hit == 0){
      pmisses++;
      writeToCache(address, prefetch_cache, c);

    
      if(mode == 'W'){
	preads++;
	pwrites++;
      }else{
	preads++;
      }

      long prefetchAddress = address;
      for(int i = 0; i < c->prefetchSize; i++){
	prefetchAddress += c->blockSize;
	int prefetchHit = checkHit(prefetchAddress, prefetch_cache, c, 1);
	if(prefetchHit == 0){
	  preads++;
	  writeToCache(prefetchAddress, prefetch_cache, c);
	
	}
      }
    }
      

    if(hit == 0){
      
      misses++;
      writeToCache(address, cache, c);

    
      if(mode == 'W'){
	reads++;
	writes++;
      }else{
	reads++;
      }
    }
    

    

  }

  printf("no-prefetch\n");
  printf("Memory reads: %d\n", reads);
  printf("Memory writes: %d\n", writes);
  printf("Cache hits: %d\n", hits);
  printf("Cache misses: %d\n", misses);
  printf("with-prefetch\n");
  printf("Memory reads: %d\n", preads);
  printf("Memory writes: %d\n", pwrites);
  printf("Cache hits: %d\n", phits);
  printf("Cache misses: %d\n", pmisses);
  
    
  freeCache(cache, c);
  freeCache(prefetch_cache, c);
}

void writeToCache(long int address, struct cacheBlock** cache, struct cache* c){

  char* method = c->cachePolicy;



  int blockBits = log2(c->blockSize);
  int setBits = log2(c->numSets);

  int mask =(1 << setBits) - 1;
       
  int index = (address>>blockBits)&mask;
  long tag = (address>>blockBits)>>setBits;


  if(strcmp(method, "fifo") == 0){
    for(int i = 0; i < c->setSize; i++){
      
      if(cache[index][i].v == 0){
	cache[index][i].tag = tag;
	cache[index][i].v = 1;
	return;
      }
    }
  }

  if(strcmp(method, "lru") == 0){
    for(int i = 0; i < c->setSize; i++){
      
      if(cache[index][i].v == 0){
	int tempTag = cache[index][0].tag;
	int temp2 = cache[index][0].tag;
	for(int j = 0; j < i; j++){
	  temp2 = cache[index][j+1].tag;
	  cache[index][j+1].tag = tempTag;
	  tempTag = temp2;
	}
	cache[index][0].tag = tag;
	cache[index][i].v = 1;
	return;
      }
    }
  }


  if(strcmp(method, "fifo") == 0){

    for(int i = 1; i < c->setSize; i++){
      cache[index][i-1].tag = cache[index][i].tag;
    }
    cache[index][c->setSize - 1].tag = tag;
  }else{
    int tempTag = cache[index][0].tag;
    int temp2 = cache[index][0].tag;
    for(int i = 0; i < c->setSize - 1; i++){
      temp2 = cache[index][i+1].tag;
      cache[index][i + 1].tag = tempTag;
      tempTag = temp2;
    }
    cache[index][0].tag = tag;
  }
    
}
    
int checkHit(long int address, struct cacheBlock** cache, struct cache* c, int prefetch){


  int blockBits = log2(c->blockSize);
  int setBits = log2(c->numSets);

  int mask = (1<<setBits) - 1;
       
  int index = (address>>blockBits)&mask;
  long tag = (address>>blockBits)>>setBits;

  int hit = 0;
  int hitIndex = 0;
  for(int i = 0; i < c->setSize; i++){

    if(cache[index][i].tag == tag){
      hit =  1;
      hitIndex = i;
      break;
    }
  }

  if(hit == 1 && strcmp(c->cachePolicy, "lru") == 0 && prefetch == 0){
    int storeTag = cache[index][hitIndex].tag;
    int tempTag = cache[index][0].tag;
    int temp2 = cache[index][0].tag;
    for(int i = 0; i < hitIndex; i++){
      temp2 = cache[index][i+1].tag;
      cache[index][i+1].tag = tempTag;
      tempTag  = temp2;
    }
    cache[index][0].tag = storeTag;
  }
    
  return hit;
    
 }


void freeCache(struct cacheBlock** cache, struct cache* c){

  for(int i = 0; i < c->numSets; i++){
    free(cache[i]);
  }

  free(cache);

}
