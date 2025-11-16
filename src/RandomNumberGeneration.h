// RandomNumberGeneration.h: interface for the RandomNumberGeneration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOMNUMBERGENERATION_H__DCADD841_8076_4FC8_9A63_5929F7670529__INCLUDED_)
#define AFX_RANDOMNUMBERGENERATION_H__DCADD841_8076_4FC8_9A63_5929F7670529__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PatnRandom {
public:
	PatnRandom(unsigned long long new_seed){
		seed = new_seed;
		next(); //discard first
	}	
	long double next(){			
		seed = ((unsigned long long) 16807*seed)%(unsigned long long)2147483647; // newseed = (seed * 7^5) % (2^32-1)	
		return seed / (long double)2147483648; // seed / 2^32
	}

public:
	unsigned long long seed;
};

#endif // !defined(AFX_RANDOMNUMBERGENERATION_H__DCADD841_8076_4FC8_9A63_5929F7670529__INCLUDED_)
