#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "analysis.h"

#include ".\recipehandler.h"


CRecipeHandler::CRecipeHandler(CAnalysisRecipe2 * r,EvaluationRecipe * re)
{
	recipe = r;
	recipe_e = re;
}

CRecipeHandler::~CRecipeHandler(void)
{
}

bool CRecipeHandler::Import(CString filename)
{
	CStdioFile io;
	if ( !io.Open(filename,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	long i;
	CString row;
	CString words[10];
	long word_pos = 0;
	long type = 0;
	while(io.ReadString(row))
	{//process line by line		
		for(i = 0; i < row.GetLength(); i++)
		{
            if(type == 0) //not in a command
			{
				if(row.Mid(i,1) == "-") 
				{
					//start of a command
					type = 1;
					word_pos = 0;
					words[word_pos] = "";
				}
			}
			else if(type == 1) //next characters to a space are a command
			{	//end of word?
				if(row.Mid(i,1) == " ")
				{
					//if the command is invalid, reset and continue
					if(words[0] != "ara" && words[0] != "arc" && words[0] != "aro" && 
						words[0] != "aca" && words[0] != "acc" && 
						words[0] != "era" && words[0] != "eca" && words[0] != "eb" && 
						words[0] != "ep" && words[0] != "em")
					{
						word_pos = 0;
						words[word_pos] = "";
						type = 0;
					}
					else
					{
						word_pos = word_pos + 1;
						words[word_pos] = "";
						type = 2; 
					}
				}
				else
				{
					words[word_pos] = words[word_pos] + row.Mid(i,1);
				}
			}
			else if(type == 2) //got the command title, now append the variable contents as string
			{
				if(row.Mid(i,1) == "(")
				{
					type = 3; 
				}
				else if(row.Mid(i,1) == " ")
				{
					type = 0; //end of the command
					//transfer to the recipe!
					transferWordsToRecipe(words);
				}
			}
			else if(type == 3) //in a variable
			{
				if(row.Mid(i,1) == ")")
				{
					type = 2;
					word_pos = word_pos + 1;
					words[word_pos] = "";
				}
				else
				{
					words[word_pos] = words[word_pos] + row.Mid(i,1);
				}
			}
		}		
	}
	//did the last word transfer?
	transferWordsToRecipe(words);

	io.Close();
	return TRUE;
}

void CRecipeHandler::transferWordsToRecipe(CString words[10])
{	
	//determine the type
	if(words[0].GetLength() == 0)
	{
	}
	else
	{
		if(words[0] == "ara") //row association
		{
			recipe->rows.nAssocMeasure = atoi(words[1]);
			recipe->rows.nMinkConst = atof(words[2]);
		}
		else if(words[0] = "aca") //column association
		{
			recipe->cols.nAssocMeasure = atoi(words[1]);
			recipe->cols.nMinkConst = atof(words[2]);
		}
		else if(words[0] == "arc") //row classification
		{	
			recipe->rows.nFuseTechnique = atoi(words[1]);
			recipe->rows.nClassStrategy = atoi(words[2]);
			recipe->rows.nBeta = atof(words[3]);			
			recipe->rows.nAllocAssocMeasure = atoi(words[4]);
			recipe->rows.nNoOfGroups = atoi(words[5]);
		}
		else if(words[0] == "acc") //column classification
		{
			recipe->cols.nFuseTechnique = atoi(words[1]);
			recipe->cols.nClassStrategy = atoi(words[2]);
			recipe->cols.nBeta = atof(words[3]);			
			recipe->cols.nAllocAssocMeasure = atoi(words[4]);
			recipe->cols.nNoOfGroups = atoi(words[5]);
		}
		else if(words[0] == "aro") //row ordination
		{
			recipe->rows.nOrd3d = atoi(words[1]);
			recipe->rows.nOrdination = atoi(words[2]);
			recipe->rows.nCut = atof(words[3]);
			recipe->rows.nOrdRandomStarts = atoi(words[4]);
			recipe->rows.nOrdIterations = atoi(words[5]);
		}
		else if(words[0] == "era") //evaluation row anosim
		{			
			recipe_e->anosim_rows = atoi(words[1]);
//			recipe_e->anosim_rows_groups = atoi(words[2]);
			recipe_e->anosim_rows_iterations = atoi(words[3]);
			recipe_e->anosim_rows_pairs = atoi(words[4]);
			recipe_e->anosim_rows_seed = atoi(words[5]);
		}
		else if(words[0] == "eca") //evaluation column anosim
		{
			recipe_e->anosim_cols = atoi(words[1]);
//			recipe_e->anosim_cols_groups = atoi(words[2]);
			recipe_e->anosim_cols_iterations = atoi(words[3]);
			recipe_e->anosim_cols_pairs = atoi(words[4]);
			recipe_e->anosim_cols_seed = atoi(words[5]);
		}
		else if(words[0] == "eb") //evaluation box&whisker
		{
			recipe_e->kw = atoi(words[1]);
			recipe_e->kw_patn_generated_groups =atoi(words[1]);
		}
		else if(words[0] == "ep") //evaluation pcc
		{
			recipe_e->pcc = atoi(words[1]);
//			recipe_e->pcc_indexes = atoi(words[2]);
		}
		else if(words[0] == "em") //evaluation mcao
		{
			recipe_e->mcao = atoi(words[1]);
			recipe_e->mcao_iterations = atoi(words[1]);
			recipe_e->mcao_seed = atoi(words[1]);
		}
		
	}
}

CString CRecipeHandler::transferRecipeToWords()
{	
	//determine the type
	CString word;
	long count = 0;
	if(recipe != NULL)
	{
		if(recipe->rows.nAssocMeasure != 0)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format("%s -ara (%i)(%f)", word,recipe->rows.nAssocMeasure ,
				recipe->rows.nMinkConst);
		}
		if(recipe->cols.nAssocMeasure != 0)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format("%s -aca (%i)(%g)", word,recipe->cols.nAssocMeasure ,
				recipe->cols.nMinkConst);
		}
		if(recipe->rows.nFuseTechnique != 0 || recipe->rows.nClassStrategy != 0)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format("%s -arc (%i)(%i)(%g)(%i)(%i)",word,recipe->rows.nFuseTechnique,
				recipe->rows.nClassStrategy  ,
				recipe->rows.nBeta ,	
				recipe->rows.nAllocAssocMeasure ,
				recipe->rows.nNoOfGroups  );
		}
		if(recipe->cols.nFuseTechnique != 0 || recipe->cols.nClassStrategy != 0)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format("%s -acc (%i)(%i)(%g)(%i)(%i)",word, recipe->cols.nFuseTechnique,
				recipe->cols.nClassStrategy  ,
				recipe->cols.nBeta  ,
				recipe->cols.nAllocAssocMeasure ,
				recipe->cols.nNoOfGroups);
		}
		if(recipe->rows.nOrdination)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format(word + " -aro (%i)(%i)(%g)(%i)(%i)",recipe->rows.nOrd3d ,
				recipe->rows.nOrdination ,
				recipe->rows.nCut ,
				recipe->rows.nOrdRandomStarts,
				recipe->rows.nOrdIterations);
		}
	}
	if(recipe_e != NULL)
	{
		if(recipe_e->anosim_rows)
		{
			if(count > 0) word = word + "\r\n";
			count ++;

			word.Format("%s -era (%i)(%i)(%i)(%i)",word,recipe_e->anosim_rows,
				recipe_e->anosim_rows_iterations,recipe_e->anosim_rows_pairs,
				recipe_e->anosim_rows_seed);
		}
		if(recipe_e->anosim_cols)
		{
				if(count > 0) word = word + "\r\n";
			count ++;

		word.Format("%s -eca (%i)(%i)(%i)(%i)",word,recipe_e->anosim_cols,
				recipe_e->anosim_cols_iterations,recipe_e->anosim_cols_pairs,
				recipe_e->anosim_cols_seed);
		}
		if(recipe_e->kw)
		{
				if(count > 0) word = word + "\r\n";
			count ++;

		word.Format("%s -eb (%i)(%i)",word,recipe_e->kw,
				recipe_e->kw_patn_generated_groups);
		}
		if(recipe_e->pcc)
		{
				if(count > 0) word = word + "\r\n";
			count ++;

		word.Format("%s -ep (%i)",word,recipe_e->pcc);
		}
		if(recipe_e->mcao)
		{
				if(count > 0) word = word + "\r\n";
			count ++;

		word.Format("%s -em (%i)(%i)(%i)",word,recipe_e->mcao,
				recipe_e->mcao_iterations,recipe_e->mcao_seed);
		}
		
	}
	return word;
}