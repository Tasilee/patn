// DataTransform.h : header file
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

class CPatnDoc;

class CDataTransform 
{
public:
	CDataTransform(CPatnDoc* pDoc);

protected:
	CPatnDoc* m_pDoc;

// Columns
public:	
	void RangeColumns(CArray<long>* paItems);
	void LogeColumns(CArray<long>* paItems, double nConstant);	
	void Log10Columns(CArray<long>* paItems, double nConstant);
	void PowerColumns(CArray<long>* paItems, double nExponent);
	void NonZeroConstantColumns(CArray<long>* paItems, double nNonZeroConstant);	
	void SqrtSosCols(CArray<long>* paItems);
	void SumCols(CArray<long>* paItems);
	
// Rows
public:	
	void RangeRows(CArray<long>* paItems);
	void LogeRows(CArray<long>* paItems, double nConstant);	
	void Log10Rows(CArray<long>* paItems, double nConstant);	
	void PowerRows(CArray<long>* paItems, double nExponent);
	void NonZeroConstantRows(CArray<long>* paItems, double nNonZeroConstant);	
	void SqrtSosRows(CArray<long>* paItems);
	void SumRows(CArray<long>* paItems);

};

#endif // TRANSFORM_H


