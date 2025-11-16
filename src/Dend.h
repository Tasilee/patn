class CDend 
{
public:
	CDend();
	CDend(CFuseTable* pFuseTable, CDendOrder* pDendOrder,
		CDendOrder* pGroupDendOrder);

protected:
	CFuseTable* m_pFuseTable;
	CDendOrder* m_pDendOrder;
	CDendOrder* m_pGroupDendOrder;

protected:
	void Initialize();
	void Attach(CFuseTable* pFuseTable, CDendOrder* pDendOrder,
		CDendOrder* pGroupDendOrder);

public:
	BOOL SetDendOrder(CFuseTable* pFuseTable, CDendOrder* pDendOrder, 
		CDendTable* pDendTable, CArray<long> * Groups,
		CDendOrder* pGroupDendOrder, CDendTable* pGroupDendTable);
	BOOL SetDendTable(CFuseTable* pFuseTable, CDendOrder* pDendOrder, 
		CDendTable* pDendTable);
};
