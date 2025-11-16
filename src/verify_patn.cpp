// verify_patn.cpp: implementation of the verify_patn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "verify_patn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <string.h>

char * GenerateSequenceFromKey(char * input,int size)
{
	char * output, *inv_input;
	int pos_i,pos_inv_i,pos_o;

	if(strlen(input) == 0)
		return NULL;

	inv_input = _strrev(_strdup(input));

	output = new char[size+1];
	for(pos_o=0;pos_o<size;pos_o++){
		output[pos_o] = 0;
	}

	pos_i = pos_inv_i = pos_o = 0;
	if(size < 1.0*strlen(input)){
		while(pos_i < 1.0*strlen(input)){
			int value1 = input[pos_i%strlen(input)] * 256;
			int value2 = (abs(inv_input[(pos_inv_i + (pos_inv_i/strlen(input)))%strlen(inv_input)])+1);;			
			int value3 = value1 / value2;
			output[pos_o%size] = (output[pos_o%size]+value3)%256;
			pos_i++;pos_inv_i++;pos_o++;
		}
	}else{
		while(pos_o < size){
			int value1 = input[pos_i%strlen(input)] * 256;
			int value2 = (abs(inv_input[(pos_inv_i + (pos_inv_i/strlen(input)))%strlen(inv_input)])+1);;			
			int value3 = value1 / value2;
			output[pos_o%size] = (output[pos_o%size]+value3)%256;
			pos_i++;pos_inv_i++;pos_o++;
		}
	}

	for(pos_o=0;pos_o<size;pos_o++){
		unsigned int o = (unsigned int)(output[pos_o]+256);
		o = o%36;
		if(o < 10)
			o += '0';
		else
			o += 'A'-10;
		output[pos_o] = o;
	}
	
	output[size] = '\0';

	delete inv_input;
	return output;
}

