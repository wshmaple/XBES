#include "stdafx.h"
#include "CXBuffer.h"


CXBuffer::CXBuffer()
{
	m_nSize = 0;
	m_pPtr = m_pBase = NULL;
	InitializeCriticalSection(&m_cs);
}


CXBuffer::~CXBuffer()
{
	if (m_pBase)
		VirtualFree(m_pBase, 0, MEM_RELEASE);
	DeleteCriticalSection(&m_cs);
}

UINT CXBuffer::ReAllocateBuffer(UINT nRequestedSize)
{
	if (nRequestedSize < GetMemSize())
		return 0;
	UINT nNewSize = (UINT)ceil(nRequestedSize / 1024.0) * 1024;
	PBYTE pNewBuffer = (PBYTE)VirtualAlloc(NULL, nNewSize, MEM_COMMIT, PAGE_READWRITE);
	if (pNewBuffer == NULL)
		return -1;
	UINT nBufferLen = GetBufferLen();
	CopyMemory(pNewBuffer, m_pBase, nBufferLen);
	if (m_pBase)
		VirtualFree(m_pBase, 0, MEM_RELEASE);
	m_pBase = pNewBuffer;
	m_pPtr = m_pBase + nBufferLen;
	m_nSize = nNewSize;
	return m_nSize;
}

UINT CXBuffer::DeAllocateBuffer(UINT nRequestedSize)
{
	if (nRequestedSize < GetBufferLen())
		return 0;
	UINT nNewSize = (UINT)ceil(nRequestedSize / 1024.0) * 1024;
	if (nNewSize < GetMemSize())
		return 0;
	PBYTE pNewBuffer = (PBYTE)VirtualAlloc(NULL, nNewSize, MEM_COMMIT, PAGE_READWRITE);
	UINT nBufferLen = GetBufferLen();
	CopyMemory(pNewBuffer, m_pBase, nBufferLen);
	VirtualFree(m_pBase, 0, MEM_RELEASE);
	m_pBase = pNewBuffer;
	m_pPtr = m_pBase + nBufferLen;
	m_nSize = nNewSize;
	return m_nSize;
}

UINT CXBuffer::GetMemSize()
{
	return m_nSize;
}

void CXBuffer::ClearBuffer()
{
	EnterCriticalSection(&m_cs);
	m_pPtr = m_pBase;
	DeAllocateBuffer(1024);
	LeaveCriticalSection(&m_cs);
}

UINT CXBuffer::Delete(UINT nSize)
{
	if (nSize > GetMemSize())
		return 0;
	if (nSize > GetBufferLen())
		nSize = GetBufferLen();
	if (nSize)
	{
		MoveMemory(m_pBase, m_pBase + nSize, GetMemSize() - nSize);
		m_pPtr -= nSize;
	}
	DeAllocateBuffer(GetBufferLen());
	return nSize;
}

UINT CXBuffer::Read(PBYTE pData, UINT nSize)
{
	EnterCriticalSection(&m_cs);
	if (nSize > GetMemSize())
	{
		LeaveCriticalSection(&m_cs);
		return 0;
	}
	if (nSize > GetBufferLen())
		nSize = GetBufferLen();
	if (nSize)
	{
		CopyMemory(pData, m_pBase, nSize);
		MoveMemory(m_pBase, m_pBase + nSize, GetMemSize() - nSize);
		m_pPtr -= nSize;
	}
	DeAllocateBuffer(GetBufferLen());
	LeaveCriticalSection(&m_cs);
	return nSize;
}

BOOL CXBuffer::Write(PBYTE pData, UINT nSize)
{
	EnterCriticalSection(&m_cs);
	if (ReAllocateBuffer(nSize + GetBufferLen()) == -1)
	{
		LeaveCriticalSection(&m_cs);
		return false;
	}
	CopyMemory(m_pPtr, pData, nSize);
	m_pPtr += nSize;
	LeaveCriticalSection(&m_cs);
	return nSize;
}

UINT CXBuffer::GetBufferLen()
{
	if (m_pBase == NULL)
		return 0;
	int nSize =
		m_pPtr - m_pBase;
	return nSize;
}

int CXBuffer::Scan(PBYTE pScan, UINT nPos)
{
	if (nPos > GetBufferLen())
		return -1;
	PBYTE pStr = (PBYTE)strstr((char*)(m_pBase + nPos), (char*)pScan);
	int nOffset = 0;
	if (pStr)
		nOffset = (pStr - m_pBase) + strlen((char*)pScan);
	return nOffset;
}

BOOL CXBuffer::Insert(PBYTE pData, UINT nSize)
{
	EnterCriticalSection(&m_cs);
	if (ReAllocateBuffer(nSize + GetBufferLen()) == -1)
	{
		LeaveCriticalSection(&m_cs);
		return false;
	}
	MoveMemory(m_pBase + nSize, m_pBase, GetMemSize() - nSize);
	CopyMemory(m_pBase, pData, nSize);
	m_pPtr += nSize;
	LeaveCriticalSection(&m_cs);
	return nSize;
}

void CXBuffer::Copy(CXBuffer& buffer)
{
	int nReSize = buffer.GetMemSize();
	int nSize = buffer.GetBufferLen();
	ClearBuffer();
	if (ReAllocateBuffer(nReSize) == -1)
		return;
	m_pPtr = m_pBase + nSize;
	CopyMemory(m_pBase, buffer.GetBuffer(), buffer.GetBufferLen());
}

PBYTE CXBuffer::GetBuffer(UINT nPos /*= 0*/)
{
	return m_pBase + nPos;

}
