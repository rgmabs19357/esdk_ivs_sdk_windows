#ifndef _MEMDC_H_
#define _MEMDC_H_


class XCMemDC : public CDC 
{
private:	
	CBitmap		m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rect;
	BOOL		m_bMemDC;
public:
	
	XCMemDC();
	XCMemDC(CDC* pDC, const CRect* pRect = NULL, BOOL bBg = FALSE);
	
	~XCMemDC();
	void Initialize(CDC* pDC,const CRect* pRect = NULL, BOOL bBg = FALSE);
	XCMemDC* operator->() 
	{
		return this;
	}	
	operator XCMemDC*() 
	{
		return this;
	}
};

#endif


