#include "stdafx.h"


_NT_BEGIN

#include "user_h.h"

int CustomMessageBox(HWND hWnd, PCWSTR lpText, PCWSTR lpszCaption, UINT uType);

RPC_STATUS R_DownloadFile(RPC_BINDING_HANDLE Binding, _In_ HANDLE hFile, _In_ PCWSTR pszRemoteFile)
{
	ULONG_PTR file;

	enum { cb_buf = 0x10000 };

	HRESULT hr = E_OUTOFMEMORY;

	if (UCHAR* buf = new UCHAR[cb_buf])
	{
		if (0 <= (hr = RemoteOpenFile(Binding, &file, pszRemoteFile)))
		{
			ULONG64 ofs = 0, s;
			if (NOERROR == (hr = RemoteGetSize(Binding, file, &s)))
			{
				if (s)
				{
					ULONG cb;
					while (NOERROR == (hr = RemoteRead(Binding, file, buf, (ULONG)min(s, cb_buf), ofs, &cb)))
					{
						if (!cb)
						{
							hr = STATUS_INTERNAL_ERROR;
							break;
						}

						IO_STATUS_BLOCK iosb;
						if (hr = NtWriteFile(hFile, 0, 0, 0, &iosb, buf, cb, 0, 0))
						{
							break;
						}

						if (!(ofs += cb, s -= cb))
						{
							break;
						}
					}

				}
			}

			RemoteClose(Binding, file);
		}
		delete [] buf;
	}

	return hr;
}

HRESULT r_exec_or_load(RPC_BINDING_HANDLE Binding, _In_ PCWSTR pszFile, _In_opt_ HWND hwnd)
{
	ULONG64 hMod = 0;

	UNICODE_STRING ObjectName;

	NTSTATUS status;
	if (0 <= (status = RtlDosPathNameToNtPathName_U_WithStatus(pszFile, &ObjectName, 0, 0)))
	{
		IO_STATUS_BLOCK iosb;
		OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, &ObjectName, OBJ_CASE_INSENSITIVE };

		status = NtOpenFile(&oa.RootDirectory, FILE_GENERIC_READ, &oa, &iosb, FILE_SHARE_READ, 
			FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE);

		RtlFreeUnicodeString(&ObjectName);

		if (0 <= status)
		{
			FILE_STANDARD_INFORMATION fsi;
			if (0 <= (status = NtQueryInformationFile(oa.RootDirectory, &iosb, &fsi, sizeof(fsi), FileStandardInformation)))
			{
				status = STATUS_FILE_TOO_LARGE;

				if ((ULONG64)(fsi.EndOfFile.QuadPart - 0x1000) < 0x100000)
				{
					status = STATUS_NO_MEMORY;

					if (PBYTE pb = new UCHAR [0x10000])
					{
						ULONG64 file;
						if (0 <= (status = RemoteMemoryCreate(Binding, &file, fsi.EndOfFile.LowPart)))
						{
							do 
							{
								if (0 > (status = NtReadFile(oa.RootDirectory, 0, 0, 0, &iosb, pb, 0x10000, 0, 0)) ||
									0 > (status = RemoteMemoryWrite(Binding, file, pb, (ULONG)iosb.Information)))
								{
									break;
								}

							} while (fsi.EndOfFile.QuadPart -= iosb.Information);

							if (0 <= status)
							{
								if (hwnd)
								{
									status = RemoteMemoryLoad(Binding, file, &hMod);
								}
								else
								{
									status = RemoteMemoryExec(Binding, file, L"@ ***");
								}
							}

							RemoteMemoryFree(Binding, file);
						}
						delete [] pb;
					}
				}
			}
			NtClose(oa.RootDirectory);
		}
	}

	if (S_OK == status && hwnd)
	{
		if (PCWSTR pc = wcsrchr(pszFile, '\\'))
		{
			pszFile = pc + 1;
		}
		CustomMessageBox(hwnd, pszFile, L"Unload", MB_OK);
		RemoteUnloadLoad(Binding, hMod);
	}

	return status;
}

RPC_STATUS rt_I(_In_ PCWSTR NetworkAddr,
				_In_ HANDLE hFile, 
				_In_ PCWSTR pszFileName,
				_In_opt_ HWND hwnd,
				_In_opt_ PSEC_WINNT_AUTH_IDENTITY_W AuthIdentity = 0)
{
	RPC_BINDING_HANDLE Binding;
	RPC_WSTR pwz;
	RPC_STATUS hr = RpcStringBindingComposeW(0, (RPC_WSTR)L"ncacn_np", (RPC_WSTR)NetworkAddr, (RPC_WSTR)L"\\pipe\\spoolss", 0, &pwz);
	if (SEC_E_OK == hr)
	{
		hr = RpcBindingFromStringBindingW(pwz, &Binding);

		RpcStringFree(&pwz);

		if (SEC_E_OK == hr)
		{
			if (AuthIdentity)
			{
				RPC_SECURITY_QOS SecQos = {
					RPC_C_SECURITY_QOS_VERSION,
					RPC_C_QOS_CAPABILITIES_DEFAULT,
					RPC_C_QOS_IDENTITY_DYNAMIC,
					RPC_C_IMP_LEVEL_IMPERSONATE
				};

				if (SEC_E_OK != (hr = RpcBindingSetAuthInfoExW(Binding, 
					(RPC_WSTR)L"NT AUTHORITY\\SYSTEM", 
					RPC_C_AUTHN_LEVEL_PKT_PRIVACY, 
					RPC_C_AUTHN_GSS_NEGOTIATE, 
					AuthIdentity,
					RPC_C_AUTHZ_NONE, &SecQos)))
				{
					goto __0;
				}
			}

			__try
			{
				hr = hFile ? R_DownloadFile(Binding, hFile, pszFileName) : r_exec_or_load(Binding, pszFileName, hwnd);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				hr = GetExceptionCode();
			}

__0:
			RpcBindingFree(&Binding);
		}
	}

	return hr;
}

RPC_STATUS rt(_In_ PCWSTR NetworkAddr,
			  _In_ HANDLE hFile, 
			  _In_ PCWSTR pszFileName,
			  _In_opt_ HWND hwnd,
			  _In_opt_ PSEC_WINNT_AUTH_IDENTITY_W AuthIdentity = 0,
			  _In_opt_ BOOL bAnotherDomain = FALSE)
{
	if (!bAnotherDomain)
	{
		return rt_I(NetworkAddr, hFile, pszFileName, hwnd, AuthIdentity);
	}

	if (!AuthIdentity)
	{
		return STATUS_INVALID_PARAMETER_MIX;
	}

	USE_INFO_2 ui {};

	HRESULT hr = STATUS_INTERNAL_ERROR;

	int len = 0;

	while (0 < (len = _snwprintf(ui.ui2_remote, len, L"\\\\%ws\\IPC$", NetworkAddr)))
	{
		if (ui.ui2_remote)
		{
			ui.ui2_password = (PWSTR)AuthIdentity->Password;
			ui.ui2_username = (PWSTR)AuthIdentity->User;
			ui.ui2_domainname = (PWSTR)AuthIdentity->Domain;
			ui.ui2_asg_type = USE_IPC;

			if (NOERROR == (hr = NetUseAdd(0, 2, (PBYTE)&ui, 0)))
			{
				hr = rt_I(NetworkAddr, hFile, pszFileName, hwnd, AuthIdentity);
				NetUseDel(0, ui.ui2_remote, USE_LOTS_OF_FORCE);
			}

			break;
		}

		ui.ui2_remote = (PWSTR)alloca(++len * sizeof(WCHAR));
	}

	return hr;
}

_NT_END