#pragma once

template<typename A, int iSize>
class CStore
{
public:


	void AddToStore(A* pObject)
	{
		if (m_nCount >= m_nSize)
			return;

		m_pObjects[m_nCount] = pObject;
		m_nCount++;
	}

	void RemoveFromStore(A* pObject)
	{
		for (int i = 0; i < m_nSize; ++i)
		{
			if (m_pObjects[i] == pObject)
			{
				m_pObjects[i] = nullptr;
				--m_nCount;
			}
		}
	}

	void RemoveFromStore(int iIdx)
	{
		if (m_pObjects[iIdx])
		{
			m_pObjects[iIdx] = nullptr;
			--m_nCount;
		}
	}

	A* GetObject(int iIdx)
	{
		if (iIdx >= m_nSize)
			return nullptr;

		return m_pObjects[iIdx];
	}

	int GetCount()
	{
		return m_nCount;
	}
private:

	A * m_pObjects[iSize];
	int m_pObjectsIndexes[iSize];
	int m_nSize = iSize;
	int m_nCount = 0;
};
