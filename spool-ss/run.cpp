#include "stdafx.h"

_NT_BEGIN

#include "log.h"
#include "SvcBase.h"
#include "user_h.h"
#include "rf.h"

NTSTATUS GetRid(_Out_ PULONG pRid);

NTSTATUS AdjustPrivileges();
NTSTATUS GetLsaToken();
void CloseLsaToken();

#define FILE_SHARE_VALID_FLAGS 0x00000007

BOOL IsNameOccupied()
{
	UNICODE_STRING ObjectName;
	OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, &ObjectName, OBJ_CASE_INSENSITIVE };
	RtlInitUnicodeString(&ObjectName, L"\\Device\\NamedPipe\\spoolss");
	HANDLE hFile;
	IO_STATUS_BLOCK iosb;
	switch (NtOpenFile(&hFile, SYNCHRONIZE, &oa, &iosb, FILE_SHARE_VALID_FLAGS, 0))
	{
	case STATUS_OBJECT_NAME_NOT_FOUND:
		return FALSE;
	case STATUS_SUCCESS:
		NtClose(hFile);
		break;
	}

	return TRUE;
}

HRESULT StopSpool()
{
	HRESULT hr;
	SERVICE_STATUS ss{};

	if (SC_HANDLE hSCManager = HR(hr, OpenSCManagerW(NULL, NULL, 0 )))
	{
		SC_HANDLE hService = HR(hr, OpenServiceW(hSCManager, L"Spooler", SERVICE_STOP));

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			HR(hr, ControlService(hService, SERVICE_CONTROL_STOP, &ss));
			CloseServiceHandle(hService);
		}
	}

	DbgPrint("%hs = %x [%x]\r\n", __FUNCTION__, hr, ss.dwCurrentState);

	if (NOERROR == hr && SERVICE_STOPPED != ss.dwCurrentState)
	{
		Sleep(1000);
	}

	return hr;
}

RPC_STATUS CALLBACK IfCallback(RPC_IF_HANDLE , void* /*Context*/)
{
	//DbgPrint("IfCallback(%p)\r\n", Context);

	//RPC_STATUS status = RpcImpersonateClient(Context);

	//if (RPC_S_OK == status)
	//{
	//	ULONG rid;

	//	status = GetRid(&rid);
	//	DbgPrint("IfCallback[%x-%u]\r\n", rid, rid);

	//	if (RpcRevertToSelf() != RPC_S_OK) __debugbreak();
	//}

	//return status;

	return S_OK;
}

HRESULT InitRpcServer()
{
	if (IsNameOccupied())
	{
		StopSpool();
	}

	static const SECURITY_DESCRIPTOR _s_sd = { 
		SECURITY_DESCRIPTOR_REVISION, 0, SE_DACL_PRESENT|SE_DACL_PROTECTED
	};

	HRESULT hr;

	if (RPC_S_OK == (hr = RpcServerRegisterAuthInfoW((RPC_WSTR)L"NT AUTHORITY\\SYSTEM", RPC_C_AUTHN_WINNT, NULL, NULL)))
	{
		if (RPC_S_OK == (hr = RpcServerRegisterIf3(remote_file_v1_0_s_ifspec,
			NULL, NULL, RPC_IF_SEC_NO_CACHE|RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, RPC_C_LISTEN_MAX_CALLS_DEFAULT, 
			0x21000, IfCallback, const_cast<SECURITY_DESCRIPTOR*>(&_s_sd))))
		{
			if (RPC_S_OK == (hr = RpcServerUseProtseqEpW(
				(RPC_WSTR)L"ncacn_np",
				RPC_C_PROTSEQ_MAX_REQS_DEFAULT, 
				(RPC_WSTR)L"\\pipe\\spoolss", 
				0)))
			{
				hr = RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, TRUE);
			}
		}
	}

	DbgPrint("%hs=%x\r\n", __FUNCTION__, hr);

	return hr;
}

class CService : public CSvcBase
{
	virtual HRESULT Run();

	virtual DWORD Handler(
		DWORD    dwControl,
		DWORD    dwEventType,
		PVOID   lpEventData
		);

public:
	CService() 
	{
		DbgPrint("%hs<%p>\r\n", __FUNCTION__, this);
	}

	~CService()
	{
		DbgPrint("%hs<%p>\r\n", __FUNCTION__, this);
	}
};

VOID NTAPI OnTimer(
				   _Inout_     PTP_CALLBACK_INSTANCE /*Instance*/,
				   _Inout_opt_ PVOID                 /*Context*/,
				   _Inout_     PTP_TIMER             /*Timer*/
				   )
{
	RemoteFile::CheckTimeout();
}

extern ULONG64 _G_rnd;

HRESULT CService::Run()
{
	HRESULT hr = SetState(0);

	if (NOERROR == hr)
	{
		if (RPC_S_OK == (hr = InitRpcServer()))
		{
			if (hr = SetState(SERVICE_RUNNING, SERVICE_ACCEPT_STOP))
			{
				RpcMgmtStopServerListening(0);
			}
			else
			{
				hr = AdjustPrivileges();

				DbgPrint("AdjustPrivileges=%x\r\n", hr);

				hr = GetLsaToken();

				DbgPrint("GetLsaToken=%x\r\n", hr);

				BCryptGenRandom(0, (PBYTE)&_G_rnd, sizeof(_G_rnd), BCRYPT_USE_SYSTEM_PREFERRED_RNG);

				PTP_TIMER Timer;
				if (0 <= TpAllocTimer(&Timer, OnTimer, 0, 0))
				{
					LARGE_INTEGER du = { (ULONG)-80000000, -1 };
					TpSetTimer(Timer, &du, 8000, 0);

					DbgPrint("RpcMgmtWaitServerListen ...\r\n");
					RpcMgmtWaitServerListen();
					DbgPrint("... RpcMgmtWaitServerListen\r\n");

					TpWaitForTimer(Timer, TRUE);
					TpReleaseTimer(Timer);
				}

				CloseLsaToken();
			}
		}
	}

	return hr;
}

DWORD CService::Handler( DWORD dwControl, DWORD , PVOID  )
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		return RpcMgmtStopServerListening(0);
	}
	return ERROR_SERVICE_CANNOT_ACCEPT_CTRL;
}

void NTAPI ServiceMain(DWORD argc, PWSTR argv[])
{
	LOG(Init());

	if (argc)
	{
		CService o;
		o.ServiceMain(argv[0]);
	}

	LOG(Destroy());
}

_NT_END