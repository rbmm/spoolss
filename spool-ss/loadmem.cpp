#include "stdafx.h"

_NT_BEGIN

struct IMAGE_Ctx : public TEB_ACTIVE_FRAME
{
	inline static const char FrameName[] = "{0A0659E5-2962-480a-9A6F-01A02C5C043B}";

	PIMAGE_NT_HEADERS _M_pinth;
	PVOID _M_retAddr = 0, _M_pvImage, *_M_pBaseAddress = 0, _M_hmod = 0;
	PCUNICODE_STRING _M_lpFileName;
	NTSTATUS _M_status = STATUS_UNSUCCESSFUL;

	IMAGE_Ctx(PVOID pvImage, PIMAGE_NT_HEADERS pinth, PCUNICODE_STRING lpFileName) 
		: _M_pvImage(pvImage), _M_pinth(pinth), _M_lpFileName(lpFileName)
	{
		const static TEB_ACTIVE_FRAME_CONTEXT FrameContext = { 0, FrameName };
		Context = &FrameContext;
		Flags = 0;
		RtlPushFrame(this);
	}

	~IMAGE_Ctx()
	{
		RtlPopFrame(this);
	}

	static IMAGE_Ctx* get()
	{
		if (TEB_ACTIVE_FRAME * frame = RtlGetFrame())
		{
			do 
			{
				if (frame->Context->FrameName == FrameName)
				{
					return static_cast<IMAGE_Ctx*>(frame);
				}
			} while (frame = frame->Previous);
		}

		return 0;
	}
};

NTSTATUS OverwriteSection(_In_ PVOID BaseAddress, _In_ PVOID pvImage, _In_ PIMAGE_NT_HEADERS pinth)
{
	ULONG op, cb = pinth->OptionalHeader.SizeOfHeaders, VirtualSize, SizeOfRawData;
	PVOID pv = BaseAddress, VirtualAddress;
	SIZE_T ProtectSize = cb;

	NTSTATUS status;
	if (0 > (status = ZwProtectVirtualMemory(NtCurrentProcess(), &pv, &ProtectSize, PAGE_READWRITE, &op)))
	{
		return status;
	}

	memcpy(BaseAddress, pvImage, cb);

	ZwProtectVirtualMemory(NtCurrentProcess(), &pv, &ProtectSize, PAGE_READONLY, &op);

	if (ULONG NumberOfSections = pinth->FileHeader.NumberOfSections)
	{
		PIMAGE_SECTION_HEADER pish = IMAGE_FIRST_SECTION(pinth);

		do 
		{
			if (VirtualSize = pish->Misc.VirtualSize)
			{
				VirtualAddress = RtlOffsetToPointer(BaseAddress, pish->VirtualAddress);

				ULONG Characteristics = pish->Characteristics;

				if (0 > (status = ZwProtectVirtualMemory(NtCurrentProcess(), &(pv = VirtualAddress), 
					&(ProtectSize = VirtualSize), 
					Characteristics & IMAGE_SCN_MEM_EXECUTE ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE, &op)))
				{
					return status;
				}

				if (cb = min(VirtualSize, SizeOfRawData = pish->SizeOfRawData))
				{
					memcpy(VirtualAddress, RtlOffsetToPointer(pvImage, pish->PointerToRawData), cb);
				}

				if (SizeOfRawData < VirtualSize)
				{
					RtlZeroMemory(RtlOffsetToPointer(VirtualAddress, cb), VirtualSize - SizeOfRawData);
				}

				if (!(Characteristics & IMAGE_SCN_MEM_WRITE))
				{
					if (0 > (status = ZwProtectVirtualMemory(NtCurrentProcess(), &pv, &ProtectSize, 
						Characteristics & IMAGE_SCN_MEM_EXECUTE ? PAGE_EXECUTE_READ : PAGE_READONLY, &op)))
					{
						return status;
					}
				}
			}

		} while (pish++, --NumberOfSections);
	}

	return STATUS_SUCCESS;
}

//#define _PRINT_CPP_NAMES_
//#include "../inc/asmfunc.h"

NTSTATUS __fastcall retFromMapViewOfSection(NTSTATUS status)
{
	//CPP_FUNCTION;

	if (IMAGE_Ctx* ctx = IMAGE_Ctx::get())
	{
		*(void**)_AddressOfReturnAddress() = ctx->_M_retAddr;

		if (0 <= status)
		{
			PVOID BaseAddress = *ctx->_M_pBaseAddress;

			if (0 <= (status = OverwriteSection(BaseAddress, ctx->_M_pvImage, ctx->_M_pinth)))
			{
				if (BaseAddress != (PVOID)ctx->_M_pinth->OptionalHeader.ImageBase)
				{
					status = STATUS_IMAGE_NOT_AT_BASE;
				}
			}

			if (0 > status)
			{
				ZwUnmapViewOfSection(NtCurrentProcess(), BaseAddress);

				*ctx->_M_pBaseAddress = 0;
			}
			else
			{
				ctx->_M_hmod = BaseAddress;
			}
		}

		ctx->_M_status = status;
	}

	return status;
}

NTSTATUS aretFromMapViewOfSection();// ASM_FUNCTION;

LONG NTAPI MyVexHandler(::PEXCEPTION_POINTERS ExceptionInfo)
{
	::PEXCEPTION_RECORD ExceptionRecord = ExceptionInfo->ExceptionRecord;
	::PCONTEXT ContextRecord = ExceptionInfo->ContextRecord;
	
	if (ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP && 
		ExceptionRecord->ExceptionAddress == (PVOID)ContextRecord->Dr3)
	{
		if (IMAGE_Ctx* ctx = IMAGE_Ctx::get())
		{
			UNICODE_STRING ObjectName;
			RtlInitUnicodeString(&ObjectName, (PCWSTR)reinterpret_cast<PNT_TIB>(NtCurrentTeb())->ArbitraryUserPointer);
			if (RtlEqualUnicodeString(&ObjectName, ctx->_M_lpFileName, FALSE))
			{
				ctx->_M_pBaseAddress =
#ifdef _WIN64
					(void**)ContextRecord->R8;

#define SP Rsp
#else
#define SP Esp
					((void***)ContextRecord->Esp)[3];
#endif

				*(PSIZE_T)((void**)ContextRecord->SP)[7] = ctx->_M_pinth->OptionalHeader.SizeOfImage;

				ctx->_M_retAddr = ((void**)ContextRecord->SP)[0];

				((void**)ContextRecord->SP)[0] = aretFromMapViewOfSection;
			}
		}

		ContextRecord->EFlags |= 0x10000;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

NTSTATUS LoadLibraryFromMem(_Out_ void** phmod, _In_ PVOID pvImage, _In_ PIMAGE_NT_HEADERS pinth, _In_ PUNICODE_STRING lpFileName)
{
	struct __declspec(uuid("1FC98BCA-1BA9-4397-93F9-349EAD41E057")) RtlpAddVectoredHandler;

	ULONG_PTR OldValue = 0;
	union {
		PVOID pvfn;
		NTSTATUS(NTAPI* RtlSetProtectedPolicy)(
			_In_ const GUID* PolicyGuid,
			_In_ ULONG_PTR PolicyValue,
			_Out_ PULONG_PTR OldPolicyValue
			);
	};

	if (pvfn = GetProcAddress(GetModuleHandleW(L"ntdll"), ("RtlSetProtectedPolicy")))
	{
		RtlSetProtectedPolicy(&__uuidof(RtlpAddVectoredHandler), 0, &OldValue);
	}

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (PVOID VectoredHandlerHandle = RtlAddVectoredExceptionHandler(TRUE, MyVexHandler))
	{
		CONTEXT ctx = {};
		ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		ctx.Dr3 = (ULONG_PTR)GetProcAddress(GetModuleHandleW(L"ntdll"),"ZwMapViewOfSection");
		ctx.Dr7 = 0x440;

		if (0 <= (status = ZwSetContextThread(NtCurrentThread(), &ctx)))
		{
			IMAGE_Ctx ictx(pvImage, pinth, lpFileName);

			status = LdrLoadDll(0, 0, lpFileName, phmod);

			ctx.Dr3 = 0;
			ctx.Dr7 = 0x400;
			ZwSetContextThread(NtCurrentThread(), &ctx);

			if (0 <= status && (0 > ictx._M_status || ictx._M_hmod != *phmod))
			{
				if (0 > ictx._M_status)
				{
					status = ictx._M_status;
				}
				else
				{
					status = STATUS_UNSUCCESSFUL;
				}

				LdrUnloadDll(*phmod);
			}
		}

		RtlRemoveVectoredExceptionHandler(VectoredHandlerHandle);
	}

	if (OldValue) RtlSetProtectedPolicy(&__uuidof(RtlpAddVectoredHandler), OldValue, &OldValue);

	return status;
}

NTSTATUS FindNoCfgDll(_In_ ULONG Machine, 
					  _In_ ULONG Magic, 
					  _In_ ULONG SizeOfImage, 
					  _Inout_opt_ PUNICODE_STRING FileName = 0,
					  _Out_opt_ PHANDLE SectionHandle = 0);

#define KI_USER_SHARED_DATA ((KUSER_SHARED_DATA * const)0x7ffe0000)
#define SharedUserData ((KUSER_SHARED_DATA * const)KI_USER_SHARED_DATA)

NTSTATUS LoadLibraryFromMem(_Out_ void** phmod, _In_ PVOID pvImage, _In_ ULONG size)
{
	PIMAGE_NT_HEADERS pinth;

	NTSTATUS status = RtlImageNtHeaderEx(0, pvImage, size, &pinth);

	if (0 <= status)
	{
		if (RtlImageNtHeader(&__ImageBase)->FileHeader.Machine != pinth->FileHeader.Machine)
		{
			return STATUS_IMAGE_MACHINE_TYPE_MISMATCH;
		}

		if (!(pinth->FileHeader.Characteristics & IMAGE_FILE_DLL))
		{
			return STATUS_INVALID_IMAGE_FORMAT;
		}

		WCHAR FileName[0x180];
		UNICODE_STRING ObjectName = { 0, sizeof(FileName), FileName };

		// RtlGetNtSystemRoot()
		if (0 <= (status = RtlAppendUnicodeToString(&ObjectName, SharedUserData->NtSystemRoot)) &&
			0 <= (status = RtlAppendUnicodeToString(&ObjectName, L"\\system32\\")) &&
			0 <= (status = FindNoCfgDll(pinth->FileHeader.Machine, 
			pinth->OptionalHeader.Magic, pinth->OptionalHeader.SizeOfImage, &ObjectName)))
		{
			status = LoadLibraryFromMem(phmod, pvImage, pinth, &ObjectName);
		}
	}

	return status;
}

_NT_END