#pragma once

ULONG SecondsSince1980();

struct RemoteFile 
{
	PVOID FileId;
	union {
		HANDLE hFile;
		PVOID pvMemory;
	};
	LONG dwRefCount;
	LONG cbNeed;
	ULONG dwTime;
	ULONG cbSize;

	void Release();

	void close()
	{
		InterlockedBitTestAndReset(&dwRefCount, 31);
	}

	void UpdateLastAccess()
	{
		dwTime = SecondsSince1980() + 8;
	}

	static RemoteFile _S_a[];

	static RemoteFile* alloc(PVOID FileId);
	static RemoteFile* get(PVOID FileId);

	static void CheckTimeout();
};