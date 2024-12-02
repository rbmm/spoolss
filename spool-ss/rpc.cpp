#include "stdafx.h"

_NT_BEGIN

#include "log.h"
#include "user_h.h"

extern volatile const UCHAR guz = 0;

ULONG GetRidFromSid(_In_ PSID Sid, _Out_ PULONG pCrc)
{
	*pCrc = RtlComputeCrc32(0, Sid, RtlLengthSid(Sid));
	return *RtlSubAuthoritySid(Sid, *RtlSubAuthorityCountSid(Sid) - 1);
}

NTSTATUS GetRid(_Out_ PULONG pRid, _Out_ PULONG pCrc)
{
	HANDLE hToken;
	NTSTATUS status;
	if (0 <= (status = NtOpenThreadToken(NtCurrentThread(), TOKEN_QUERY, TRUE, &hToken)))
	{
		union {
			PVOID buf = 0;
			PTOKEN_USER ptu;
		};

		PVOID stack = alloca(guz);
		ULONG cb = 0, rcb = sizeof(TOKEN_USER) + SECURITY_SID_SIZE(3);

		do 
		{
			if (cb < rcb)
			{
				cb = RtlPointerToOffset(buf = alloca(rcb - cb), stack);
			}

			if (0 <= (status = NtQueryInformationToken(hToken, TokenUser, buf, cb, &rcb)))
			{
				*pRid = GetRidFromSid(ptu->User.Sid, pCrc);

				WCHAR sz[SECURITY_MAX_SID_STRING_CHARACTERS];
				UNICODE_STRING us = { 0, sizeof(sz), sz };
				if (0 <= RtlConvertSidToUnicodeString(&us, ptu->User.Sid, FALSE))
				{
					DbgPrint("%x> SID:%wZ\r\n", GetCurrentProcessId(), &us);
				}
				break;
			}

		} while (STATUS_BUFFER_TOO_SMALL == status);

		NtClose(hToken);
	}

	return status;
}

NTSTATUS GetRid(_In_ handle_t IDL_handle, _Out_ PULONG pRid, _Out_ PULONG pCrc)
{
	HRESULT hr = RpcImpersonateClient(IDL_handle);

	if (RPC_S_OK == hr)
	{
		hr = GetRid(pRid, pCrc);

		if (RpcRevertToSelf() != RPC_S_OK) __debugbreak();
	}

	return hr;
}

#include "rf.h"

HRESULT RemoteOpenFile( 
					   /* [in] */ handle_t IDL_handle,
					   /* [out] */ ULONG64 *phFile,
					   /* [in] */ const wchar_t *pszName)
{
	*phFile = 0;

	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%ws)\r\n", __FUNCTION__, rid, pszName);

	PVOID FileId;
	NTSTATUS status = BCryptGenRandom(0, (PBYTE)&FileId, sizeof(FileId), BCRYPT_USE_SYSTEM_PREFERRED_RNG);

	if (0 <= status)
	{
		status = STATUS_NO_MORE_ENTRIES;

		if (RemoteFile* p = RemoteFile::alloc(FileId))
		{
			p->cbSize = 0;

			UNICODE_STRING ObjectName;
			OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, &ObjectName, OBJ_CASE_INSENSITIVE };

			if (0 <= (status = RtlDosPathNameToNtPathName_U_WithStatus(pszName, &ObjectName, 0, 0)))
			{
				IO_STATUS_BLOCK iosb;

				status = NtOpenFile(&p->hFile, FILE_GENERIC_READ, &oa, &iosb, FILE_SHARE_READ|FILE_SHARE_WRITE, 
					FILE_SYNCHRONOUS_IO_NONALERT|FILE_OPEN_FOR_BACKUP_INTENT);

				RtlFreeUnicodeString(&ObjectName);

				if (0 <= status)
				{
					*phFile = (ULONG64)(ULONG_PTR)p->FileId;
				}
			}

			if (0 > status)
			{
				p->close();
			}

			p->Release();
		}
	}

	return status;
}

HRESULT RemoteClose( 
					/* [in] */ handle_t IDL_handle,
					/* [in] */ ULONG64 hFile)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status;

		if (p->cbSize)
		{
			status = STATUS_OBJECT_TYPE_MISMATCH;
		}
		else
		{
			p->close();
			status = STATUS_SUCCESS;
		}

		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

HRESULT RemoteGetSize( 
					  /* [in] */ handle_t IDL_handle,
					  /* [in] */ ULONG64 hFile,
					  /* [out] */ ULONG64 *lpFileSize)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status = STATUS_OBJECT_TYPE_MISMATCH;
		if (!p->cbSize)
		{
			IO_STATUS_BLOCK iosb;
			FILE_STANDARD_INFORMATION fsi;
			status = NtQueryInformationFile(p->hFile, &iosb, &fsi, sizeof(fsi), FileStandardInformation);

			if (0 <= status)
			{
				*lpFileSize = fsi.EndOfFile.QuadPart;
			}
		}

		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

HRESULT RemoteRead( 
				   /* [in] */ handle_t IDL_handle,
				   /* [in] */ ULONG64 hFile,
				   /* [length_is][size_is][out] */ byte *buf,
				   /* [in] */ ULONG cb,
				   /* [in] */ ULONG64 ByteOffset,
				   /* [out] */ ULONG *pcb)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	*pcb = 0;

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status = STATUS_OBJECT_TYPE_MISMATCH;
		if (!p->cbSize)
		{
			IO_STATUS_BLOCK iosb;
			if (0 <= (status = NtReadFile(p->hFile, 0, 0, 0, &iosb, buf, cb, (PLARGE_INTEGER)&ByteOffset, 0)))
			{
				*pcb = (ULONG)iosb.Information;
			}
		}
		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

HRESULT RemoteMemoryCreate( 
						   /* [in] */ handle_t IDL_handle,
						   /* [out] */ ULONG64 *phFile,
						   /* [in] */ ULONG size)
{
	*phFile = 0;

	if (0x100000 < (ULONG)(size - 0x1000))
	{
		return STATUS_FILE_TOO_LARGE;
	}

	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%x)\r\n", __FUNCTION__, rid, size);

	PVOID FileId;
	NTSTATUS status = BCryptGenRandom(0, (PBYTE)&FileId, sizeof(FileId), BCRYPT_USE_SYSTEM_PREFERRED_RNG);

	if (0 <= status)
	{
		status = STATUS_NO_MORE_ENTRIES;

		if (RemoteFile* p = RemoteFile::alloc(FileId))
		{
			p->cbSize = size, p->cbNeed = size;

			if (p->pvMemory = LocalAlloc(LMEM_FIXED, size))
			{
				*phFile = (ULONG64)(ULONG_PTR)p->FileId;
				status = STATUS_SUCCESS;
			}
			else
			{
				p->close();
				status = STATUS_NO_MEMORY;
			}

			p->Release();
		}
	}

	return status;

}

HRESULT RemoteMemoryWrite( 
						  /* [in] */ handle_t IDL_handle,
						  /* [in] */ ULONG64 hFile,
						  /* [size_is][in] */ byte *buf,
						  /* [in] */ ULONG cb)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status = STATUS_OBJECT_TYPE_MISMATCH;
		if (p->cbSize)
		{
			ULONG cbNeed = p->cbNeed;
			if (cb <= cbNeed)
			{
				if (cbNeed - InterlockedCompareExchange(&p->cbNeed, cbNeed - cb, cbNeed))
				{
					status = STATUS_REQUEST_OUT_OF_SEQUENCE;
				}
				else
				{
					memcpy(RtlOffsetToPointer(p->pvMemory, p->cbSize - cbNeed), buf, cb);
					status = STATUS_SUCCESS;
				}
			}
			else
			{
				status = STATUS_BUFFER_OVERFLOW;
			}

		}
		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

NTSTATUS ExecAsUser(HANDLE hToken, PVOID BaseOfImage, ULONG size, PCWSTR lpCmdLine);
NTSTATUS GetTokenForUser(_In_ HANDLE hToken, _Out_ HANDLE* phToken);

NTSTATUS Exec(handle_t IDL_handle, PVOID BaseOfImage, ULONG size, PCWSTR lpCmdLine)
{
	HRESULT hr = RpcImpersonateClient(IDL_handle);

	if (RPC_S_OK == hr)
	{
		HANDLE hToken, hNewToken;

		hr = NtOpenThreadToken(NtCurrentThread(), TOKEN_QUERY|TOKEN_QUERY_SOURCE, TRUE, &hToken);

		if (RpcRevertToSelf() != RPC_S_OK) __debugbreak();

		if (0 <= hr)
		{
			hr = GetTokenForUser(hToken, &hNewToken);

			NtClose(hToken);

			if (0 > hr)
			{
				hr = ExecAsUser(0, BaseOfImage, size, lpCmdLine);
			}
			else
			{
				hr = ExecAsUser(hNewToken, BaseOfImage, size, lpCmdLine);

				NtClose(hNewToken);
			}
		}
	}

	return hr;
}

HRESULT RemoteMemoryExec( 
						 /* [in] */ handle_t IDL_handle,
						 /* [in] */ ULONG64 hFile,
						 /* [string][in] */ const wchar_t *pszCmdLine)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status = STATUS_OBJECT_TYPE_MISMATCH;
		if (p->cbSize)
		{
			status = p->cbNeed ? STATUS_INVALID_DEVICE_STATE : Exec(IDL_handle, p->pvMemory, p->cbSize, pszCmdLine);
		}
		p->Release();

		DbgPrint("%hs = %x\r\n", __FUNCTION__, status);

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

NTSTATUS LoadLibraryFromMem(_Out_ void** phmod, _In_ PVOID pvImage, _In_ ULONG size);

ULONG64 _G_rnd;

HRESULT RemoteMemoryLoad( 
						 /* [in] */ handle_t IDL_handle,
						 /* [in] */ ULONG64 hFile,
						 /* [out] */ ULONG64 *phMod)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status = STATUS_OBJECT_TYPE_MISMATCH;
		if (p->cbSize)
		{
			void* hmod = 0;
			if (0 <= (status = p->cbNeed ? STATUS_INVALID_DEVICE_STATE : LoadLibraryFromMem(&hmod, p->pvMemory, p->cbSize)))
			{
				*phMod = _G_rnd ^ (ULONG64)hmod ^ crc;
			}

			DbgPrint("%hs = %x [%p]\r\n", __FUNCTION__, status, hmod);
		}
		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

HRESULT RemoteUnloadLoad( 
						 /* [in] */ handle_t IDL_handle,
						 /* [in] */ ULONG64 hMod)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hMod);

	return LdrUnloadDll((HMODULE)(_G_rnd ^ hMod ^ crc));
}

HRESULT RemoteMemoryFree( 
						 /* [in] */ handle_t IDL_handle,
						 /* [in] */ ULONG64 hFile)
{
	ULONG rid, crc;
	GetRid(IDL_handle, &rid, &crc);
	DbgPrint("%hs[%x](%p)\r\n", __FUNCTION__, rid, hFile);

	if (RemoteFile* p = RemoteFile::get((PVOID)(ULONG_PTR)hFile))
	{
		NTSTATUS status;

		if (p->cbSize)
		{
			p->close();
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_OBJECT_TYPE_MISMATCH;
		}

		p->Release();

		return status;
	}

	return STATUS_INVALID_HANDLE;
}

_NT_END
