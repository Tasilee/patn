#pragma once


#include "evaluationrecipe.h"
class CRecipeHandler
{
public:
	CRecipeHandler(CAnalysisRecipe2 * r,EvaluationRecipe * re);
	~CRecipeHandler(void);

	bool Import(CString filename);
	void transferWordsToRecipe(CString words[10]);
	CString transferRecipeToWords();

	CAnalysisRecipe2 * recipe;
	EvaluationRecipe * recipe_e;
};
