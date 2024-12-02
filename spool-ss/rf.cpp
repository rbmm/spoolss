#include "stdafx.h"

_NT_BEGIN
#include "log.h"
#include "rf.h"

ULONG SecondsSince1980()
{
	LARGE_INTEGER time;
	GetSystemTimeAsFileTime((PFILETIME)&time);
	ULONG sec;
	RtlTimeToSecondsSince1980(&time, &sec);
	return sec;
}

RemoteFile RemoteFile::_S_a[16];

void RemoteFile::Release()
{
	if (!InterlockedDecrement(&dwRefCount))
	{
		if (hFile)
		{
			if (cbSize)
			{
				LocalFree(pvMemory);
			}
			else
			{
				NtClose(hFile);
			}
			hFile = 0;
		}

		dwTime = 0, cbSize = 0, FileId = 0;
	}
}

RemoteFile* RemoteFile::alloc(PVOID FileId)
{
	RemoteFile *p = _S_a;
	ULONG n = _countof(_S_a);
	do 
	{
		if (0 == InterlockedCompareExchangePointer(&p->FileId, FileId, 0))
		{
			if (p->hFile || p->dwRefCount)
			{
				__debugbreak();
			}
			p->UpdateLastAccess();
			p->dwRefCount = 0x80000001;
			return p;
		}
	} while (p++, --n);

	return 0;
}

[[nodiscard]] BOOL Acquire(PLONG pValue)
{
	LONG Value, NewValue;

	if (0 > (Value = *pValue))
	{
		do 
		{
			NewValue = InterlockedCompareExchangeNoFence(pValue, Value + 1, Value);

			if (NewValue == Value) return TRUE;

		} while (0 > (Value = NewValue));
	}

	return FALSE;
}

RemoteFile* RemoteFile::get(PVOID FileId)
{
	RemoteFile *p = _S_a;
	ULONG n = _countof(_S_a);
	do 
	{
		if (p->FileId == FileId)
		{
			if (Acquire(&p->dwRefCount))
			{
				if (p->FileId == FileId)
				{
					p->UpdateLastAccess();
					return p;
				}

				p->Release();
			}
		}

	} while (p++, --n);

	return 0;
}

void RemoteFile::CheckTimeout()
{
	ULONG dwTime = SecondsSince1980();
	RemoteFile *p = _S_a;
	ULONG n = _countof(_S_a);
	do 
	{
		if (p->dwTime && p->dwTime < dwTime)
		{
			if (Acquire(&p->dwRefCount))
			{
				if (p->dwTime < dwTime)
				{
					DbgPrint("%p: timeout !!\r\n", p->FileId);
					p->close();
				}

				p->Release();
			}
		}

	} while (p++, --n);
}

_NT_END
