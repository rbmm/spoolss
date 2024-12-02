#include "stdafx.h"

_NT_BEGIN

#include "MiniWnd.h"
#include "resource.h"

inline ULONG BOOL_TO_ERROR(BOOL f)
{
	return f ? NOERROR : GetLastError();
}

HRESULT GetLastErrorEx(ULONG dwError = GetLastError());
int CustomMessageBox(HWND hWnd, PCWSTR lpText, PCWSTR lpszCaption, UINT uType);
int ShowErrorBox(HWND hwnd, HRESULT dwError, PCWSTR lpCaption, UINT uType);

RPC_STATUS rt(_In_ PCWSTR NetworkAddr,
			  _In_ HANDLE hFile, 
			  _In_ PCWSTR pszFileName,
			  _In_opt_ HWND hwnd,
			  _In_opt_ PSEC_WINNT_AUTH_IDENTITY_W AuthIdentity = 0,
			  _In_opt_ BOOL bAnotherDomain = FALSE);

HRESULT OnBrowse(_In_ HWND hwndDlg, 
				 _In_ UINT cFileTypes, 
				 _In_ const COMDLG_FILTERSPEC *rgFilterSpec, 
				 _Out_ PWSTR* ppszFilePath, 
				 _In_ UINT iFileType = 0,
				 _In_ const CLSID* pclsid = &__uuidof(FileOpenDialog),
				 _In_ PCWSTR pszDefaultExtension = 0)
{
	IFileDialog *pFileOpen;

	HRESULT hr = CoCreateInstance(*pclsid, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		pFileOpen->SetOptions(FOS_NOVALIDATE|FOS_NOTESTFILECREATE|
			FOS_NODEREFERENCELINKS|FOS_DONTADDTORECENT|FOS_FORCESHOWHIDDEN);

		if (pszDefaultExtension)
		{
			pFileOpen->SetDefaultExtension(pszDefaultExtension);
		}

		if (0 <= (hr = pFileOpen->SetFileTypes(cFileTypes, rgFilterSpec)) && 
			0 <= (hr = pFileOpen->SetFileTypeIndex(1 + iFileType)) && 
			0 <= (hr = pFileOpen->Show(hwndDlg)))
		{
			IShellItem *pItem;
			hr = pFileOpen->GetResult(&pItem);

			if (SUCCEEDED(hr))
			{
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, ppszFilePath);
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}

	return hr;
}

void OnBrowse(HWND hwndDlg, 
			  UINT nIDDlgItem, 
			  UINT cFileTypes, 
			  const COMDLG_FILTERSPEC *rgFilterSpec, 
			  _In_ UINT iFileType = 0,
			  _In_ const CLSID* pclsid = &__uuidof(FileOpenDialog),
			  _In_ PCWSTR pszDefaultExtension = 0)
{
	PWSTR pszFilePath;
	HRESULT hr = OnBrowse(hwndDlg, cFileTypes, rgFilterSpec, &pszFilePath, iFileType, pclsid, pszDefaultExtension);

	if (SUCCEEDED(hr))
	{
		SetDlgItemTextW(hwndDlg, nIDDlgItem, pszFilePath);
		CoTaskMemFree(pszFilePath);
	}
}

NTSTATUS IsImageOk(PCWSTR pszPath, USHORT ImageCharacteristics)
{
	SECTION_IMAGE_INFORMATION sii;
	UNICODE_STRING ObjectName;
	NTSTATUS status;
	if (0 <= (status = RtlDosPathNameToNtPathName_U_WithStatus(pszPath, &ObjectName, 0, 0)))
	{
		IO_STATUS_BLOCK iosb;
		OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, &ObjectName, OBJ_CASE_INSENSITIVE };
		status = NtOpenFile(&oa.RootDirectory, FILE_EXECUTE|SYNCHRONIZE, &oa, &iosb, 
			FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE);
		RtlFreeUnicodeString(&ObjectName);

		if (0 <= status)
		{
			HANDLE hSection;
			status = NtCreateSection(&hSection, SECTION_QUERY, 0, 0, PAGE_EXECUTE, SEC_IMAGE, oa.RootDirectory);
			NtClose(oa.RootDirectory);
			if (0 <= status)
			{
				status = ZwQuerySection(hSection, SectionImageInformation, &sii, sizeof(sii), 0);
				
				NtClose(hSection);

				if (0 <= status)
				{
					if (ImageCharacteristics != (sii.ImageCharacteristics & IMAGE_FILE_DLL))
					{
						return STATUS_INVALID_IMAGE_FORMAT;
					}

					switch (sii.Machine)
					{
					case IMAGE_FILE_MACHINE_I386:
						if (ImageCharacteristics)
						{
							return STATUS_INVALID_IMAGE_FORMAT;
						}
					case IMAGE_FILE_MACHINE_AMD64:
						break;
					default:
						return STATUS_INVALID_IMAGE_FORMAT;
					}
				}
			}
		}
	}

	return status;
}

NTSTATUS OpenFileForWrite(_Out_ PHANDLE FileHandle, HWND hwnd, PCWSTR pszPath)
{
	UNICODE_STRING ObjectName;
	NTSTATUS status;
	if (0 <= (status = RtlDosPathNameToNtPathName_U_WithStatus(pszPath, &ObjectName, 0, 0)))
	{
		IO_STATUS_BLOCK iosb;
		OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, &ObjectName, OBJ_CASE_INSENSITIVE };
		
		ULONG CreateDisposition = FILE_CREATE;

__loop:
		if (STATUS_OBJECT_NAME_COLLISION == (status = NtCreateFile(FileHandle, FILE_APPEND_DATA|SYNCHRONIZE, &oa, &iosb, 
			0, 0, 0, CreateDisposition, FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE, 0, 0)))
		{
			if (FILE_CREATE == CreateDisposition &&
				IDYES == ShowErrorBox(hwnd, STATUS_OBJECT_NAME_COLLISION, L"overwrite ?", MB_YESNO|MB_DEFBUTTON2|MB_ICONWARNING))
			{
				CreateDisposition = FILE_OVERWRITE_IF;
				goto __loop;
			}
		}
		
		RtlFreeUnicodeString(&ObjectName);
	}

	return status;
}

const static UINT _s_iid[] = { ICON_BIG, ICON_SMALL };

class PipeDlg : public YDlg
{
	PWSTR _M_buf = 0;
	SEC_WINNT_AUTH_IDENTITY_W AuthIdentity = { };
	LONG _M_flags = 0;
	enum  { iExe, iDll, iFile } _M_iMetod = iExe;
	enum { f_domain, f_cred };

	void OnOk(HWND hwnd)
	{
		if (_bittest(&_M_flags, f_cred))
		{
			if (!AuthIdentity.Flags)
			{
				CustomMessageBox(hwnd, L"no creds", 0, MB_ICONWARNING);
				return;
			}
		}

		WCHAR Server[0x100];
		ULONG len = GetDlgItemTextW(hwnd, IDC_EDIT1, Server, _countof(Server));

		if (_countof(Server) - 3 < len - 1)
		{
			CustomMessageBox(hwnd, L"no server", 0, MB_ICONWARNING);
			return ;
		}

		if (!(len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDIT2))))
		{
			CustomMessageBox(hwnd, L"no file", 0, MB_ICONWARNING);
			return ;
		}

		HRESULT hr = S_OK;

		if (PWSTR pszFile = new WCHAR[++len])
		{
			HANDLE hFile = 0;
			HWND hwnd2 = 0;

			GetDlgItemTextW(hwnd, IDC_EDIT2, pszFile, len);
			switch (_M_iMetod)
			{
			case iExe:
				hr = IsImageOk(pszFile, 0);
				break;
			case iDll:
				hr = IsImageOk(pszFile, IMAGE_FILE_DLL);
				hwnd2 = hwnd;
				break;
			case iFile:
				if (!(len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDIT3))))
				{
					CustomMessageBox(hwnd, L"no remote file", 0, MB_ICONWARNING);
					return ;
				}
				hr = OpenFileForWrite(&hFile, hwnd, pszFile);
				delete [] pszFile;
				if (pszFile = new WCHAR[++len])
				{
					GetDlgItemTextW(hwnd, IDC_EDIT3, pszFile, len);
				}
				else
				{
					hr = STATUS_NO_MEMORY;
				}
				
				break;
			}

			if (S_OK == hr)
			{
				hr = rt(Server, hFile, pszFile, hwnd2, 
					_bittest(&_M_flags, f_cred) ? &AuthIdentity : 0, _bittest(&_M_flags, f_domain));
			}

			ShowErrorBox(hwnd, hr, L"op", MB_ICONINFORMATION);

			if (pszFile)
			{
				delete [] pszFile;
			}

			if (hFile)
			{
				NtClose(hFile);
			}
		}
		else
		{
			hr = STATUS_NO_MEMORY;
		}
	}

	HRESULT InitCreds(HWND hwnd)
	{
		ULONG ap = 0;
		PVOID pv = 0;
		ULONG cb = 0;
		CREDUI_INFOW UiInfo { sizeof(UiInfo), hwnd, L"Message", L"Caption" };
		HRESULT hr = CredUIPromptForWindowsCredentialsW(&UiInfo, 0, &ap, 0, 0, &pv, &cb, 0, CREDUIWIN_GENERIC);
		if (NOERROR == hr)
		{
			if (_M_buf)
			{
				delete [] _M_buf;
				_M_buf = 0;
			}

			RtlZeroMemory(&AuthIdentity, sizeof(AuthIdentity));

			PWSTR psz = 0;

			while (ERROR_INSUFFICIENT_BUFFER == (hr = BOOL_TO_ERROR(
				CredUnPackAuthenticationBufferW(CRED_PACK_PROTECTED_CREDENTIALS, pv, cb, 
				(PWSTR)AuthIdentity.User, &AuthIdentity.UserLength, 
				(PWSTR)AuthIdentity.Domain, &AuthIdentity.DomainLength, 
				(PWSTR)AuthIdentity.Password, &AuthIdentity.PasswordLength))))
			{
				if (psz)
				{
					break;
				}

				if (!(psz = new WCHAR[AuthIdentity.UserLength + AuthIdentity.DomainLength + AuthIdentity.PasswordLength]))
				{
					hr = E_OUTOFMEMORY;
					break;
				}

				PUSHORT pu = (PUSHORT)psz;
				if (AuthIdentity.DomainLength) 
				{
					AuthIdentity.Domain = pu, pu += AuthIdentity.DomainLength;
				}
				AuthIdentity.User = pu, pu += AuthIdentity.UserLength;
				AuthIdentity.Password = pu;
			}

			if (NOERROR == hr)
			{
				_M_buf = psz;

				AuthIdentity.PasswordLength--;
				AuthIdentity.UserLength--;

				if (AuthIdentity.DomainLength) 
				{
					AuthIdentity.DomainLength--;
				}
				else
				{
					if (psz = wcschr((PWSTR)AuthIdentity.User, '\\'))
					{
						AuthIdentity.Domain = AuthIdentity.User;
						AuthIdentity.DomainLength = (ULONG)(psz - (PWSTR)AuthIdentity.Domain);
						*psz++ = 0;
						AuthIdentity.User = (PUSHORT)psz;
						AuthIdentity.UserLength -= AuthIdentity.DomainLength + 1;
					}
				}

				AuthIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

				SetDlgItemTextW(hwnd, IDC_EDIT4, (PCWSTR)AuthIdentity.Domain);
				SetDlgItemTextW(hwnd, IDC_EDIT5, (PCWSTR)AuthIdentity.User);
			}
			else if (psz)
			{
				delete [] psz;
			}

			LocalFree(pv);
		}

		return hr;
	}

	virtual BOOL OnInitDialog(HWND hwnd)
	{
		SendDlgItemMessageW(hwnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);

		WCHAR sz[0x100];

		static const UINT id[] = { IDC_EDIT5, IDC_EDIT4 };
		static PCWSTR name[] = { L"USERNAME", L"USERDOMAIN" };

		ULONG i = _countof(id);
		do 
		{
			if (GetEnvironmentVariableW(name[--i], sz, _countof(sz)))
			{
				SetDlgItemTextW(hwnd, id[i], sz);
			}
		} while (i);

		SetDlgItemTextW(hwnd, IDC_EDIT1, L"localhost");

		i = _countof(_s_iid) - 1;
		INT cx[] = { GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CXSMICON) }, 
			cy[] = { GetSystemMetrics(SM_CYICON), GetSystemMetrics(SM_CYSMICON) };

		do 
		{
			HICON hi;
			if (0 <= LoadIconWithScaleDown((HINSTANCE)&__ImageBase, MAKEINTRESOURCEW(1), cx[i], cy[i], &hi))
			{
				SendMessageW(hwnd, WM_SETICON, _s_iid[i], (LPARAM)hi);
			}
		} while (i--);


		return TRUE;
	}

	void OnCheck(HWND hwnd, int i)
	{
		if (SendMessageW(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED)
		{
			_bittestandset(&_M_flags, i);
		}
		else
		{
			_bittestandreset(&_M_flags, i);
		}
	}

	void OnRadio(HWND hwnd, BOOL b)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT3), b);
	}

	void SelectLocalFile(HWND hwnd)
	{
		static const COMDLG_FILTERSPEC rgSpec1[] =
		{ 
			{ L"EXE files", L"*.exe" },
			{ L"All files", L"*" },
		};

		static const COMDLG_FILTERSPEC rgSpec2[] =
		{ 
			{ L"DLL files", L"*.dll" },
			{ L"All files", L"*" },
		};

		static const COMDLG_FILTERSPEC rgSpec3[] =
		{ 
			{ L"All files", L"*" },
		};

		UINT cFileTypes;
		const COMDLG_FILTERSPEC *rgFilterSpec;
		const CLSID* pclsid = &__uuidof(FileOpenDialog);

		switch (_M_iMetod)
		{
		case iExe:
			rgFilterSpec = rgSpec1;
			cFileTypes = _countof(rgSpec1);
			break;
		case iDll:
			rgFilterSpec = rgSpec2;
			cFileTypes = _countof(rgSpec2);
			break;
		case iFile:
			pclsid = &__uuidof(FileSaveDialog);
			rgFilterSpec = rgSpec3;
			cFileTypes = _countof(rgSpec3);
			break;

		default:
			return;
		}

		OnBrowse(hwnd, IDC_EDIT2, cFileTypes, rgFilterSpec, 0, pclsid);
	}

	INT_PTR DlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
	{
		switch (umsg)
		{
		case WM_NCDESTROY:
			OnDestroy();
			break;

		case WM_DESTROY:
			if (_M_buf)
			{
				delete [] _M_buf;
			}
			umsg = _countof(_s_iid);
			do 
			{
				if (lParam = SendMessageW(hwnd, WM_GETICON, _s_iid[--umsg], 0))
				{
					DestroyIcon((HICON)lParam);
				}
			} while (umsg);

			break;

		case WM_COMMAND:
			switch (wParam)
			{
			case IDCANCEL:
				EndDialog(hwnd, 0);
				break;

			case IDOK:
				OnOk(hwnd);
				break;

			case IDC_BUTTON1:
				InitCreds(hwnd);
				break;
			
			case IDC_BUTTON2:
				SelectLocalFile(hwnd);
				break;

			case IDC_CHECK1:
				OnCheck((HWND)lParam, f_domain);
				if (_bittest(&_M_flags, f_domain))
				{
					_bittestandset(&_M_flags, f_cred);
					SendDlgItemMessageW(hwnd, IDC_CHECK2, BM_SETCHECK, BST_CHECKED, 0);
				}
				EnableWindow(GetDlgItem(hwnd, IDC_CHECK2), !_bittest(&_M_flags, f_domain));
				break;

			case IDC_CHECK2:
				OnCheck((HWND)lParam, f_cred);
				break;

			case IDC_RADIO1:
				_M_iMetod = iExe;
				OnRadio(hwnd, FALSE);
				break;

			case IDC_RADIO2:
				_M_iMetod = iDll;
				OnRadio(hwnd, FALSE);
				break;

			case IDC_RADIO3:
				_M_iMetod = iFile;
				OnRadio(hwnd, TRUE);
				break;
			}
			break;
		}

		return 0;
	}

public:
};

void WINAPI ep(void*)
{
	if (0 <= CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))
	{
		PipeDlg dlg;
		dlg.DoModal((HINSTANCE)&__ImageBase, MAKEINTRESOURCEW(IDD_DIALOG2), 0);
		CoUninitialize();
	}
	ExitProcess(0);
}

_NT_END