//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"

//
// TODO:Student Information
//
const char *studentName = "Chenyu Wu";
const char *studentID   = "A59011003";
const char *email       = "chw080@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

//
//TODO: Add your Cache data structures here

uint64_t ***icache;
uint64_t ***dcache;
uint64_t ***l2cache;
uint8_t  cache = 3;
//

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;
  
  //
  //TODO: Initialize Cache Simulator Data Structures
  icache = (uint64_t***)malloc(icacheSets*sizeof(uint64_t**));
  dcache = (uint64_t***)malloc(dcacheSets*sizeof(uint64_t**));
  l2cache = (uint64_t***)malloc(l2cacheSets*sizeof(uint64_t**));
  
  for(int i = 0; i < icacheSets; i++){
	icache[i] = (uint64_t**)malloc(icacheAssoc*sizeof(uint64_t*));
  }
  
  for(int i = 0; i < dcacheSets; i++){
	dcache[i] = (uint64_t**)malloc(dcacheAssoc*sizeof(uint64_t*));
  }
  
  for(int i = 0; i < l2cacheSets; i++){
	l2cache[i] = (uint64_t**)malloc(l2cacheAssoc*sizeof(uint64_t*));
  }
  
  for(int i = 0; i < icacheSets; i++){
	  for(int j = 0; j < icacheAssoc; j++){
		  icache[i][j] = (uint64_t*)malloc(cache * sizeof(uint64_t));
	  }
  }
  
  for(int i = 0; i < dcacheSets; i++){
	  for(int j = 0; j < dcacheAssoc; j++){
		  dcache[i][j] = (uint64_t*)malloc(cache * sizeof(uint64_t));
	  }
  }
  
  for(int i = 0; i < l2cacheSets; i++){
	  for(int j = 0; j < l2cacheAssoc; j++){
		  l2cache[i][j] = (uint64_t*)malloc(cache * sizeof(uint64_t));
	  }
  }
  
  for(int i = 0; i < icacheSets; i++){
	  for(int j = 0; j < icacheAssoc; j++){
			  icache[i][j][cache - 1] = j; //LRU bits
			  icache[i][j][cache - 2] = FALSE; //Vaild bits
			  icache[i][j][cache - 3] = FALSE; //tag bits
	  }
  }
  
  for(int i = 0; i < dcacheSets; i++){
	  for(int j = 0; j < dcacheAssoc; j++){
			  dcache[i][j][cache - 1] = j; //LRU bits
			  dcache[i][j][cache - 2] = FALSE; //Vaild bits
			  dcache[i][j][cache - 3] = FALSE; //tag bits
	  }
  }
  
  for(int i = 0; i < l2cacheSets; i++){
	  for(int j = 0; j < l2cacheAssoc; j++){
			  l2cache[i][j][cache - 1] = j; //LRU bits
			  l2cache[i][j][cache - 2] = FALSE; //Vaild bits
			  l2cache[i][j][cache - 3] = FALSE; //tag bits
	  }
  }
  
  //
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
	int flag;
	int flag1;
	int flag_hit = 0;
	int flag_vaild = 1;
  //
  //TODO: Implement I$
  uint32_t offsetBits = log10(blocksize)/log10(2); //Find the offsetBits of the icache
  uint32_t indexBits  = log10(icacheSets)/log10(2); //Find the indexBits of the icache
  uint32_t index = (addr >> offsetBits)&(icacheSets-1); //Index of the icache
  uint32_t tag = addr >> (offsetBits+indexBits);      // tag value
  
  
  if(icacheSets > 0){
	  icacheRefs++;
  for(int i = 0; i < icacheAssoc; i++){	  
	  if(icache[index][i][0] == tag && icache[index][i][1] != FALSE){
		  flag = i;
		  flag_hit = 1;
		  break;
	  }
  }
  
  if(flag_hit){
	  for(int i = 0; i  < icacheAssoc; i++){
		if(icache[index][flag][2] == i){
		   flag1 = i;
		  break; 
		}
	  }

	  for(int i=0; i < icacheAssoc; i++){
		if(icache[index][i][2] > flag1 && i != flag) icache[index][i][2] = icache[index][i][2] - 1;
	  }
	  icache[index][flag][2] = icacheAssoc - 1;
	  return icacheHitTime;
  }
  
  else{
	  icacheMisses++;
	  uint32_t memspeed_l2cache = l2cache_access(addr);
	  
	  for(int i = 0; i < icacheAssoc; i++){
		  if(icache[index][i][1] == 0){
			  flag = i;
			  icache[index][i][0] = tag;
			  icache[index][i][1] = 1;
			  flag_vaild = 0;
			  break;
		  }
	  }

	  if(flag_vaild == 0){
		for(int i = 0; i  < icacheAssoc; i++){
		if(icache[index][flag][2] == i){
		   flag1 = i;
		  break; 
		 }
	        }
	  	for(int i=0; i < icacheAssoc; i++){
			if(icache[index][i][2] > flag1 && i != flag) icache[index][i][2] = icache[index][i][2] - 1;
	  	}
	  	icache[index][flag][2] = icacheAssoc - 1;
	  }
	  
	  if(flag_vaild){
		  for(int i = 0; i < icacheAssoc; i++){
			  if(icache[index][i][2] == 0){
				  flag = i;
				  icache[index][i][0] = tag; //tag
				  break;
			  }
		  }
	  for(int i = 0; i < icacheAssoc; i++){
		if(icache[index][flag][2] == i){
		       	flag1 = i;
                        break;
		}
	  }
	  
	  for(int i = 0; i < icacheAssoc; i++){
		if(icache[index][i][2] > flag1 && i != flag) icache[index][i][2] = icache[index][i][2] - 1;
	  }

	  icache[index][flag][2] = icacheAssoc - 1;
	  }
	  icachePenalties = memspeed_l2cache + icachePenalties;
	  return (memspeed_l2cache + icacheHitTime);
  }
  }
  
  
  //
  //return memspeed;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
	int flag;
	int flag_hit = 0;
	int flag_vaild = 1;
	int flag1;
  //
  //TODO: Implement D$
  uint32_t offsetBits = log10(blocksize)/log10(2); //Find the offsetBits of the dcache
  uint32_t indexBits  = log10(dcacheSets)/log10(2); //Find the indexBits of the dcache
  uint32_t index = (addr >> offsetBits)&(dcacheSets-1); //Index of the icache
  uint32_t tag = (addr >> (offsetBits+indexBits));      // tag value
  
  
  if(dcacheSets > 0){
	  dcacheRefs++;
  for(int i = 0; i < dcacheAssoc; i++){
	  if(dcache[index][i][0] == tag && dcache[index][i][1] != FALSE){
		  flag = i;
		  flag_hit = 1;
		  break;
	  }
  }
  
  if(flag_hit){
	  for(int i = 0; i < dcacheAssoc; i++){
		if(dcache[index][flag][2] == i){
		       	flag1 = i;
			break;
		}
	  }
	  	  for(int i = 0; i < dcacheAssoc; i++){
		  if(i != flag && dcache[index][i][2] > flag1) dcache[index][i][2] = dcache[index][i][2] - 1;
	  }

          dcache[index][flag][2] = dcacheAssoc - 1;
	  return dcacheHitTime;
  }
  
  else{
	  dcacheMisses++;
	  uint32_t memspeed_l2cache = l2cache_access(addr);
	  
	  for(int i = 0; i < dcacheAssoc; i++){
		  if(dcache[index][i][1] != 1){
			  flag = i;
			  dcache[index][i][0] = tag;
			  dcache[index][i][1] = 1;
			  flag_vaild = 0;
			  break;
		  }
	  }

	  if(flag_vaild == 0){
		for(int i = 0; i < dcacheAssoc; i++){
			if(dcache[index][flag][2] == i){
				flag1 = i;
				break;
			}
		}

		for(int i = 0; i < dcacheAssoc; i++){
			if(dcache[index][i][2] > flag1 && i != flag) dcache[index][i][2] = dcache[index][i][2] - 1;
		}

		dcache[index][flag][2] = dcacheAssoc - 1;
	  }
	  
	  if(flag_vaild){
		  for(int i = 0; i < dcacheAssoc; i++){
			  if(dcache[index][i][2] == 0){
				  flag = i;
				  dcache[index][i][0] = tag;
				  break;
			  }
		  }
		  
		for(int i = 0; i < dcacheAssoc; i++){
			if(dcache[index][flag][2] == i){
				flag1 = i;
				break;
			}
		}

		for(int i = 0; i < dcacheAssoc; i++){
			if(dcache[index][i][2] > flag1 && i != flag) dcache[index][i][2] = dcache[index][i][2] - 1;
		}

		dcache[index][flag][2] = dcacheAssoc - 1;
	  }
	  dcachePenalties = memspeed_l2cache + dcachePenalties;
	  return (memspeed_l2cache + dcacheHitTime);
  }
  }
  
  //
  //return memspeed;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  //
  //TODO: Implement L2$
    int flag;
	int flag_hit = 0;
	int flag_vaild = 1;
	int flag1;
	uint32_t temp_tag;
    uint32_t offsetBits = log10(blocksize)/log10(2); //Find the offsetBits of the dcache
    uint32_t indexBits  = log10(l2cacheSets)/log10(2); //Find the indexBits of the dcache
    uint32_t index = (addr >> offsetBits)&(l2cacheSets-1); //Index of the icache
    uint32_t tag = (addr >> (offsetBits+indexBits));      // tag value
    
	
	if(l2cacheSets > 0){
		l2cacheRefs++;
  for(int i = 0; i < l2cacheAssoc; i++){
	  if(l2cache[index][i][0] == tag && l2cache[index][i][1] != FALSE){
		  flag = i;
		  flag_hit = 1;
		  break;
	  }
  }
  
  if(flag_hit){
	  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][flag][2] == i){
			  flag1 = i;
			  break;
		  }
	  }
	  
	  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][i][2] > flag1) l2cache[index][i][2] = l2cache[index][i][2] - 1;
	  }
	  
	  l2cache[index][flag][2] = l2cacheAssoc - 1;
	  return l2cacheHitTime;
  }
  
  else{
	  l2cacheMisses++;
	  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][i][1] != 1){
			  l2cache[index][i][0] = tag;
			  l2cache[index][i][1] = 1;
			  flag = i;
			  flag_vaild = 0;
			  break;
		  }
	  }
	  
	  if(flag_vaild == 0){
		  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][flag][2] == i){
			  flag1 = i;
			  break;
		  }
	  }
	  
	  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][i][2] > flag1) l2cache[index][i][2] = l2cache[index][i][2] - 1;
	  }
	  
	  l2cache[index][flag][2] = l2cacheAssoc - 1;
	  }
	  
	  else{
		  for(int i = 0; i < l2cacheAssoc; i++){
			  if(l2cache[index][i][2] == 0){
				  l2cache[index][i][0] = tag;
				  flag = i;
				  break;
			  }
		  }
		  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][flag][2] == i){
			  flag1 = i;
			  break;
		  }
	  }
	  
	  for(int i = 0; i < l2cacheAssoc; i++){
		  if(l2cache[index][i][2] > flag1) l2cache[index][i][2] = l2cache[index][i][2] - 1;
	  }
	  
	  l2cache[index][flag][2] = l2cacheAssoc - 1;
	  }
	  l2cachePenalties = memspeed + l2cachePenalties;
	  return (memspeed+l2cacheHitTime);
  }
	}
  //realize the inclusive of the L2 Cache  
  //
  //return memspeed;
}
