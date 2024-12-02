#include "stdafx.h"

_NT_BEGIN

#define _makeachar(x) #@x
#define makeachar(x) _makeachar(x)
#define _makewchar(x) L## #@x
#define makewchar(x) _makewchar(x)
#define echo(x) x
#define label(x) echo(x)##__LINE__
#define showmacro(x) __pragma(message(__FILE__ _CRT_STRINGIZE((__LINE__): \nmacro\t)#x" expand to\n" _CRT_STRINGIZE(x)))

#define BEGIN_PRIVILEGES(name, n) static const union { TOKEN_PRIVILEGES name;\
struct { ULONG PrivilegeCount; LUID_AND_ATTRIBUTES Privileges[n];} label(_) = { n, {

#define LAA(se) {{se}, SE_PRIVILEGE_ENABLED }
#define LAA_D(se) {{se} }

#define END_PRIVILEGES }};};

extern const SECURITY_QUALITY_OF_SERVICE sqos = {
	sizeof(sqos), SecurityImpersonation, SECURITY_DYNAMIC_TRACKING, FALSE
};

extern const OBJECT_ATTRIBUTES oa_sqos = { sizeof(oa_sqos), 0, 0, 0, 0, const_cast<SECURITY_QUALITY_OF_SERVICE*>(&sqos) };

BEGIN_PRIVILEGES(tp_ait, 5)
LAA(SE_ASSIGNPRIMARYTOKEN_PRIVILEGE),
LAA(SE_INCREASE_QUOTA_PRIVILEGE),
LAA(SE_TCB_PRIVILEGE),
LAA(SE_DEBUG_PRIVILEGE),
LAA(SE_IMPERSONATE_PRIVILEGE),
END_PRIVILEGES

BEGIN_PRIVILEGES(tp_CreateTcb, 2)
LAA(SE_CREATE_TOKEN_PRIVILEGE),
LAA(SE_TCB_PRIVILEGE),
END_PRIVILEGES

HRESULT GetLastErrorEx(ULONG dwError = GetLastError());

NTSTATUS AdjustPrivileges()
{
	NTSTATUS status;
	HANDLE hToken;

	if (0 <= (status = NtOpenProcessToken(NtCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)))
	{
		status = NtAdjustPrivilegesToken(hToken, FALSE, const_cast<PTOKEN_PRIVILEGES>(&tp_ait), 0, 0, 0);
		NtClose(hToken);
	}

	return status;
}

NTSTATUS GetToken(_In_ PVOID buf, _In_ const TOKEN_PRIVILEGES* RequiredSet, _Out_ PHANDLE phToken)
{
	NTSTATUS status;

	union {
		PVOID pv;
		PBYTE pb;
		PSYSTEM_PROCESS_INFORMATION pspi;
	};

	pv = buf;
	ULONG NextEntryOffset = 0;

	do
	{
		pb += NextEntryOffset;

		HANDLE hProcess, hToken, hNewToken;

		CLIENT_ID ClientId = { pspi->UniqueProcessId };

		if (ClientId.UniqueProcess)
		{
			if (0 <= NtOpenProcess(&hProcess, PROCESS_QUERY_LIMITED_INFORMATION,
				const_cast<POBJECT_ATTRIBUTES>(&oa_sqos), &ClientId))
			{
				status = NtOpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken);

				NtClose(hProcess);

				if (0 <= status)
				{
					status = NtDuplicateToken(hToken, TOKEN_ADJUST_PRIVILEGES | TOKEN_IMPERSONATE | TOKEN_QUERY,
						const_cast<POBJECT_ATTRIBUTES>(&oa_sqos), FALSE, TokenImpersonation, &hNewToken);

					NtClose(hToken);

					if (0 <= status)
					{
						status = NtAdjustPrivilegesToken(hNewToken, FALSE, const_cast<PTOKEN_PRIVILEGES>(RequiredSet), 0, 0, 0);

						if (STATUS_SUCCESS == status)
						{
							*phToken = hNewToken;
							return STATUS_SUCCESS;
						}

						NtClose(hNewToken);
					}
				}
			}
		}

	} while (NextEntryOffset = pspi->NextEntryOffset);

	return STATUS_UNSUCCESSFUL;
}

NTSTATUS GetToken(_In_ const TOKEN_PRIVILEGES* RequiredSet, _Out_ PHANDLE phToken)
{
	NTSTATUS status;

	ULONG cb = 0x40000;

	do
	{
		status = STATUS_INSUFFICIENT_RESOURCES;

		if (PBYTE buf = new BYTE[cb += PAGE_SIZE])
		{
			if (0 <= (status = NtQuerySystemInformation(SystemProcessInformation, buf, cb, &cb)))
			{
				status = GetToken(buf, RequiredSet, phToken);

				if (status == STATUS_INFO_LENGTH_MISMATCH)
				{
					status = STATUS_UNSUCCESSFUL;
				}
			}

			delete[] buf;
		}

	} while (status == STATUS_INFO_LENGTH_MISMATCH);

	return status;
}

NTSTATUS RtlRevertToSelf()
{
	HANDLE hToken = 0;
	return NtSetInformationThread(NtCurrentThread(), ThreadImpersonationToken, &hToken, sizeof(hToken));
}

static HANDLE _G_hLsaToken;

NTSTATUS GetLsaToken()
{
	return GetToken(&tp_CreateTcb, &_G_hLsaToken);
}

void CloseLsaToken()
{
	if (_G_hLsaToken) NtClose(_G_hLsaToken);
}

extern const volatile UCHAR guz;

NTSTATUS ExpandToken(_Out_ PHANDLE phToken, _In_ HANDLE hToken, _In_ ULONG SessionId, _In_ PSID LogonSid)
{
	NTSTATUS status;
	PVOID stack = alloca(guz);
	PVOID buf = 0;

	ULONG cb = 0, rcb;

	struct {
		PTOKEN_GROUPS Groups;
		PTOKEN_USER User;
		PTOKEN_PRIVILEGES Privileges;
		PTOKEN_OWNER Owner;
		PTOKEN_PRIMARY_GROUP PrimaryGroup;
		PTOKEN_DEFAULT_DACL DefaultDacl;
		PTOKEN_SOURCE TokenSource;
	} s;

	void** ppv = (void**)&s.TokenSource;

	static const ULONG rcbV[] = {
		sizeof(TOKEN_GROUPS) + 0x80,
		sizeof(TOKEN_USER) + 0x40,
		sizeof(TOKEN_PRIVILEGES) + 0x80,
		sizeof(TOKEN_OWNER) + 0x40,
		sizeof(TOKEN_PRIMARY_GROUP) + 0x40,
		sizeof(TOKEN_DEFAULT_DACL) + 0x40,
		sizeof(TOKEN_SOURCE),
	};

	static TOKEN_INFORMATION_CLASS TokenInformationClassV[] = {
		TokenGroups,
		TokenUser,
		TokenPrivileges,
		TokenOwner,
		TokenPrimaryGroup,
		TokenDefaultDacl,
		TokenSource,
	};

	ULONG n = _countof(TokenInformationClassV);

	do
	{
		TOKEN_INFORMATION_CLASS TokenInformationClas = TokenInformationClassV[--n];

		rcb = rcbV[n], cb = 0;

		do
		{
			if (cb < rcb)
			{
				cb = RtlPointerToOffset(buf = alloca(rcb - cb), stack);
			}

			status = NtQueryInformationToken(hToken, TokenInformationClas, buf, cb, &rcb);

		} while (status == STATUS_BUFFER_TOO_SMALL);

		if (0 > status)
		{
			return status;
		}

		*(ppv--) = buf, stack = buf;

	} while (n);

	// reserve stack space for extend groups
	alloca(2 * sizeof(SID_AND_ATTRIBUTES));
	PSID_AND_ATTRIBUTES Groups = s.Groups->Groups - 2;
	ULONG GroupCount = s.Groups->GroupCount + 2;
	s.Groups = CONTAINING_RECORD(Groups, TOKEN_GROUPS, Groups);
	s.Groups->GroupCount = GroupCount;

	SID InteractiveSid = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_INTERACTIVE_RID } };
	Groups->Sid = LogonSid;
	Groups++->Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_LOGON_ID;
	Groups->Sid = const_cast<SID*>(&InteractiveSid);
	Groups++->Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY;

	const static LUID AuthenticationId = SYSTEM_LUID;
	const static LARGE_INTEGER ExpirationTime = { MAXDWORD, MAXLONG };
	
	if (0 <= (status = NtCreateToken(&hToken, TOKEN_ALL_ACCESS, 0, TokenPrimary,
		const_cast<PLUID>(&AuthenticationId), const_cast<PLARGE_INTEGER>(&ExpirationTime),
		s.User, s.Groups, s.Privileges,
		s.Owner, s.PrimaryGroup, s.DefaultDacl, s.TokenSource)))
	{
		if (0 > (status = NtSetInformationToken(hToken, TokenSessionId, &SessionId, sizeof(SessionId))))
		{
			NtClose(hToken);
		}
		else
		{
			*phToken = hToken;
		}
	}

	return status;
}

EXTERN_C 
DECLSPEC_IMPORT 
BOOL 
WINAPI 
WTSQueryUserToken(ULONG SessionId, PHANDLE phToken);

NTSTATUS GetLogonSid(_In_ ULONG SessionId, _Out_ PSID LogonSid)
{
	HANDLE hToken;

	if (WTSQueryUserToken(SessionId, &hToken))
	{
		NTSTATUS status;

		union {
			PVOID buf;
			PTOKEN_GROUPS ptg;
		};

		PVOID stack = alloca(guz);
		ULONG cb = 0, rcb = 0x100;

		do
		{
			if (cb < rcb)
			{
				cb = RtlPointerToOffset(buf = alloca(rcb - cb), stack);
			}
			status = NtQueryInformationToken(hToken, TokenGroups, buf, cb, &rcb);

		} while (STATUS_BUFFER_TOO_SMALL == status);
		NtClose(hToken);

		if (0 > status)
		{
			return status;
		}

		if (ULONG GroupCount = ptg->GroupCount)
		{
			PSID_AND_ATTRIBUTES Groups = ptg->Groups;
			do
			{
				if (Groups->Attributes & SE_GROUP_LOGON_ID)
				{
					PSID Sid = Groups->Sid;
					if (SECURITY_LOGON_IDS_RID_COUNT == *RtlSubAuthorityCountSid(Sid) &&
						SECURITY_LOGON_IDS_RID == *RtlSubAuthoritySid(Sid, 0))
					{
						return RtlCopySid(SECURITY_SID_SIZE(SECURITY_LOGON_IDS_RID_COUNT), LogonSid, Sid);
					}
				}
			} while (Groups++, --GroupCount);
		}

		return STATUS_NO_SUCH_GROUP;
	}

	return GetLastErrorEx();
}

NTSTATUS GetTokenForUser(_In_ HANDLE hToken, _Out_ HANDLE* phToken)
{
	LONG SessionId = WTSGetActiveConsoleSessionId();

	if (0 > SessionId)
	{
		return STATUS_NO_SUCH_LOGON_SESSION;
	}

	UCHAR LogonSid[SECURITY_SID_SIZE(SECURITY_LOGON_IDS_RID_COUNT)];
	NTSTATUS status = GetLogonSid(SessionId, LogonSid);

	if (0 <= status)
	{
		if (0 <= (status = NtSetInformationThread(NtCurrentThread(),
			ThreadImpersonationToken, &_G_hLsaToken, sizeof(_G_hLsaToken))))
		{
			status = ExpandToken(phToken, hToken, SessionId, LogonSid);
			RtlRevertToSelf();
		}
	}

	return status;
}

_NT_END