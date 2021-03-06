#include<ntifs.h>
#include<ntstrsafe.h>
#include<ntddk.h>

//微软有导出，但未文档化
PCHAR PsGetProcessImageFileName(PEPROCESS Process);	//获取指定进程的进程名
HANDLE PsGetProcessInheritedFromUniqueProcessId(PEPROCESS Process);	//获取指定进程的父进程ID
NTSTATUS PsSuspendProcess(PEPROCESS Process);	//暂停指定进程执行
NTSTATUS PsResumeProcess(PEPROCESS Process);	//恢复指定进程执行

//配置函数指针，使xp平台也能使用平台未导入的函数
typedef NTSTATUS(*pFuncXpPsSuspendProcess)(PEPROCESS Process);
typedef NTSTATUS(*pFuncXpPsResumeProcess)(PEPROCESS Process);

//函数首地址通过windbg执行“uf 函数符号名”，进行内存检索而得
pFuncXpPsSuspendProcess pXpPsSuspendProcess = (pFuncXpPsSuspendProcess)0x8411f717;	//uf nt!PsSuspendProcess
pFuncXpPsResumeProcess pXpPsResumeProcess = (pFuncXpPsResumeProcess)0x8411f7d4;		//uf nt!PsResumeProcess

//进程枚举
//基于进程双链表的遍历，较快，但由于采用硬编码而有兼容性问题
VOID PsEnum1()
{
	PEPROCESS pEproc = NULL;		//获取进程地址
	PEPROCESS pFirstProc = NULL;	//保存EPROCESS进程双链表头节点
	PLIST_ENTRY pLe = NULL;
	PUCHAR pProcName = NULL;		//进程名字符串指针
	ULONG ulPid = 0;				//进程ID

	//获取当前进程EPROCESS
	pEproc = PsGetCurrentProcess();
	if (!pEproc)
	{
		KdPrint(("GetCurrentProcess Failed!\n"));
		return;
	}
	pFirstProc = pEproc;
	//KdBreakPoint();
	KdPrint(("PsEnum1!\n"));
	do
	{
		pProcName = (PUCHAR)pEproc + 0x16c;
		ulPid = *(ULONG *)((PCHAR)pEproc + 0xb4);
		KdPrint(("PID:%d\tparentID:%d\t%s\n",
			ulPid,
			(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
			pProcName));
		pLe = (PLIST_ENTRY)((PCHAR)pEproc + 0xb8);
		pLe = pLe->Flink;
		pEproc = (PEPROCESS)((PCHAR)pLe - 0xb8);
	} while (pEproc != pFirstProc);
	KdPrint(("---------------------\n"));
}

//较通用，无兼容性问题，但为线性遍历
VOID PsEnum2()
{
	ULONG ulPid = 0;
	PEPROCESS pEproc = NULL;
	PUCHAR pProcName = NULL;
	NTSTATUS status;
	KdPrint(("PsEnum2!\n"));
	for (ulPid = 4; ulPid < 20000; ulPid += 4)
	{
		status = PsLookupProcessByProcessId((HANDLE)ulPid, &pEproc);
		if(NT_SUCCESS(status))
		{
			pProcName = PsGetProcessImageFileName(pEproc);
			KdPrint(("PID:%d\tparentID:%d\t%s\n", 
				ulPid, 
				(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
				pProcName));
		}
	}
	KdPrint(("---------------------\n"));
}

//进程暂停
//给定进程pid，暂停对应进程的执行
NTSTATUS PsSuspend(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5版本则以硬函数编码方式执行函数
		if (SharedUserData->NtMajorVersion == 5)
		{
			status = pXpPsSuspendProcess(pEproc);
		}
		else
		{
			status = PsSuspendProcess(pEproc);
		}
		if (NT_SUCCESS(status))
		{
			KdPrint(("pid:%d\t%s\t Suspend!\n", (ULONG)hPid, PsGetProcessImageFileName(pEproc)));
		}
	}
	return status;
}

//进程恢复
//给定进程pid，恢复对应进程的执行
NTSTATUS PsResume(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5版本则以硬函数编码方式执行函数
		if (SharedUserData->NtMajorVersion == 5)
		{
			status = pXpPsResumeProcess(pEproc);
		}
		else
		{
			status = PsResumeProcess(pEproc);
		}
		if (NT_SUCCESS(status))
		{
			KdPrint(("pid:%d\t%s\t Resume!\n", (ULONG)hPid, PsGetProcessImageFileName(pEproc)));
		}
	}

	return status;
}

//进程终止
//给定进程pid，终止对应进程的执行
NTSTATUS PsTerminate(HANDLE hPid)
{
	HANDLE hProc = NULL;
	CLIENT_ID clientId = { 0 };
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	clientId.UniqueProcess = hPid;
	InitializeObjectAttributes(
		&oa,
		NULL,
		OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	status = ZwOpenProcess(
		&hProc,
		PROCESS_ALL_ACCESS,
		&oa,
		&clientId
	);
	if (NT_SUCCESS(status))
	{
		status = ZwTerminateProcess(hProc, 0);
		if (NT_SUCCESS(status))
		{
			KdPrint(("PID:%d\tTerminate Success!\n", hPid));
		}
		else
		{
			KdPrint(("TerminateProcess Failed!0x%x\n", status));
		}
	}
	else
	{
		KdPrint(("OpenProcess Failed!0x%x\n",status));
	}
	ZwClose(hProc);
	return status;
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//PsResume((HANDLE)2424);	//虚拟机内运行calc.exe进程对应pid为2424，执行成功时该进程继续执行
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	//PsEnum1();
	//PsEnum2();

	//PsSuspend((HANDLE)2424);	//虚拟机内运行calc.exe进程对应pid为2424，执行成功时该进程暂停执行

	PsTerminate((HANDLE)2548);	//虚拟机内运行calc.exe进程对应pid为2548，执行成功时该进程结束执行

	return STATUS_SUCCESS;
}

/*
Win7SP1.7601.17514.101119-1850.X86
nt!_EPROCESS
+ 0x000 Pcb              : _KPROCESS
+ 0x098 ProcessLock : _EX_PUSH_LOCK
+ 0x0a0 CreateTime : _LARGE_INTEGER
+ 0x0a8 ExitTime : _LARGE_INTEGER
+ 0x0b0 RundownProtect : _EX_RUNDOWN_REF
+ 0x0b4 UniqueProcessId : Ptr32 Void
+ 0x0b8 ActiveProcessLinks : _LIST_ENTRY
+ 0x0c0 ProcessQuotaUsage : [2] Uint4B
+ 0x0c8 ProcessQuotaPeak : [2] Uint4B
+ 0x0d0 CommitCharge : Uint4B
+ 0x0d4 QuotaBlock : Ptr32 _EPROCESS_QUOTA_BLOCK
+ 0x0d8 CpuQuotaBlock : Ptr32 _PS_CPU_QUOTA_BLOCK
+ 0x0dc PeakVirtualSize : Uint4B
+ 0x0e0 VirtualSize : Uint4B
+ 0x0e4 SessionProcessLinks : _LIST_ENTRY
+ 0x0ec DebugPort : Ptr32 Void
+ 0x0f0 ExceptionPortData : Ptr32 Void
+ 0x0f0 ExceptionPortValue : Uint4B
+ 0x0f0 ExceptionPortState : Pos 0, 3 Bits
+ 0x0f4 ObjectTable : Ptr32 _HANDLE_TABLE
+ 0x0f8 Token : _EX_FAST_REF
+ 0x0fc WorkingSetPage : Uint4B
+ 0x100 AddressCreationLock : _EX_PUSH_LOCK
+ 0x104 RotateInProgress : Ptr32 _ETHREAD
+ 0x108 ForkInProgress : Ptr32 _ETHREAD
+ 0x10c HardwareTrigger : Uint4B
+ 0x110 PhysicalVadRoot : Ptr32 _MM_AVL_TABLE
+ 0x114 CloneRoot : Ptr32 Void
+ 0x118 NumberOfPrivatePages : Uint4B
+ 0x11c NumberOfLockedPages : Uint4B
+ 0x120 Win32Process : Ptr32 Void
+ 0x124 Job : Ptr32 _EJOB
+ 0x128 SectionObject : Ptr32 Void
+ 0x12c SectionBaseAddress : Ptr32 Void
+ 0x130 Cookie : Uint4B
+ 0x134 Spare8 : Uint4B
+ 0x138 WorkingSetWatch : Ptr32 _PAGEFAULT_HISTORY
+ 0x13c Win32WindowStation : Ptr32 Void
+ 0x140 InheritedFromUniqueProcessId : Ptr32 Void
+ 0x144 LdtInformation : Ptr32 Void
+ 0x148 VdmObjects : Ptr32 Void
+ 0x14c ConsoleHostProcess : Uint4B
+ 0x150 DeviceMap : Ptr32 Void
+ 0x154 EtwDataSource : Ptr32 Void
+ 0x158 FreeTebHint : Ptr32 Void
+ 0x160 PageDirectoryPte : _HARDWARE_PTE
+ 0x160 Filler : Uint8B
+ 0x168 Session : Ptr32 Void
+ 0x16c ImageFileName : [15] UChar
+ 0x17b PriorityClass : UChar
+ 0x17c JobLinks : _LIST_ENTRY
+ 0x184 LockedPagesList : Ptr32 Void
+ 0x188 ThreadListHead : _LIST_ENTRY
+ 0x190 SecurityPort : Ptr32 Void
+ 0x194 PaeTop : Ptr32 Void
+ 0x198 ActiveThreads : Uint4B
+ 0x19c ImagePathHash : Uint4B
+ 0x1a0 DefaultHardErrorProcessing : Uint4B
+ 0x1a4 LastThreadExitStatus : Int4B
+ 0x1a8 Peb : Ptr32 _PEB
+ 0x1ac PrefetchTrace : _EX_FAST_REF
+ 0x1b0 ReadOperationCount : _LARGE_INTEGER
+ 0x1b8 WriteOperationCount : _LARGE_INTEGER
+ 0x1c0 OtherOperationCount : _LARGE_INTEGER
+ 0x1c8 ReadTransferCount : _LARGE_INTEGER
+ 0x1d0 WriteTransferCount : _LARGE_INTEGER
+ 0x1d8 OtherTransferCount : _LARGE_INTEGER
+ 0x1e0 CommitChargeLimit : Uint4B
+ 0x1e4 CommitChargePeak : Uint4B
+ 0x1e8 AweInfo : Ptr32 Void
+ 0x1ec SeAuditProcessCreationInfo : _SE_AUDIT_PROCESS_CREATION_INFO
+ 0x1f0 Vm : _MMSUPPORT
+ 0x25c MmProcessLinks : _LIST_ENTRY
+ 0x264 HighestUserAddress : Ptr32 Void
+ 0x268 ModifiedPageCount : Uint4B
+ 0x26c Flags2 : Uint4B
+ 0x26c JobNotReallyActive : Pos 0, 1 Bit
+ 0x26c AccountingFolded : Pos 1, 1 Bit
+ 0x26c NewProcessReported : Pos 2, 1 Bit
+ 0x26c ExitProcessReported : Pos 3, 1 Bit
+ 0x26c ReportCommitChanges : Pos 4, 1 Bit
+ 0x26c LastReportMemory : Pos 5, 1 Bit
+ 0x26c ReportPhysicalPageChanges : Pos 6, 1 Bit
+ 0x26c HandleTableRundown : Pos 7, 1 Bit
+ 0x26c NeedsHandleRundown : Pos 8, 1 Bit
+ 0x26c RefTraceEnabled : Pos 9, 1 Bit
+ 0x26c NumaAware : Pos 10, 1 Bit
+ 0x26c ProtectedProcess : Pos 11, 1 Bit
+ 0x26c DefaultPagePriority : Pos 12, 3 Bits
+ 0x26c PrimaryTokenFrozen : Pos 15, 1 Bit
+ 0x26c ProcessVerifierTarget : Pos 16, 1 Bit
+ 0x26c StackRandomizationDisabled : Pos 17, 1 Bit
+ 0x26c AffinityPermanent : Pos 18, 1 Bit
+ 0x26c AffinityUpdateEnable : Pos 19, 1 Bit
+ 0x26c PropagateNode : Pos 20, 1 Bit
+ 0x26c ExplicitAffinity : Pos 21, 1 Bit
+ 0x270 Flags : Uint4B
+ 0x270 CreateReported : Pos 0, 1 Bit
+ 0x270 NoDebugInherit : Pos 1, 1 Bit
+ 0x270 ProcessExiting : Pos 2, 1 Bit
+ 0x270 ProcessDelete : Pos 3, 1 Bit
+ 0x270 Wow64SplitPages : Pos 4, 1 Bit
+ 0x270 VmDeleted : Pos 5, 1 Bit
+ 0x270 OutswapEnabled : Pos 6, 1 Bit
+ 0x270 Outswapped : Pos 7, 1 Bit
+ 0x270 ForkFailed : Pos 8, 1 Bit
+ 0x270 Wow64VaSpace4Gb : Pos 9, 1 Bit
+ 0x270 AddressSpaceInitialized : Pos 10, 2 Bits
+ 0x270 SetTimerResolution : Pos 12, 1 Bit
+ 0x270 BreakOnTermination : Pos 13, 1 Bit
+ 0x270 DeprioritizeViews : Pos 14, 1 Bit
+ 0x270 WriteWatch : Pos 15, 1 Bit
+ 0x270 ProcessInSession : Pos 16, 1 Bit
+ 0x270 OverrideAddressSpace : Pos 17, 1 Bit
+ 0x270 HasAddressSpace : Pos 18, 1 Bit
+ 0x270 LaunchPrefetched : Pos 19, 1 Bit
+ 0x270 InjectInpageErrors : Pos 20, 1 Bit
+ 0x270 VmTopDown : Pos 21, 1 Bit
+ 0x270 ImageNotifyDone : Pos 22, 1 Bit
+ 0x270 PdeUpdateNeeded : Pos 23, 1 Bit
+ 0x270 VdmAllowed : Pos 24, 1 Bit
+ 0x270 CrossSessionCreate : Pos 25, 1 Bit
+ 0x270 ProcessInserted : Pos 26, 1 Bit
+ 0x270 DefaultIoPriority : Pos 27, 3 Bits
+ 0x270 ProcessSelfDelete : Pos 30, 1 Bit
+ 0x270 SetTimerResolutionLink : Pos 31, 1 Bit
+ 0x274 ExitStatus : Int4B
+ 0x278 VadRoot : _MM_AVL_TABLE
+ 0x298 AlpcContext : _ALPC_PROCESS_CONTEXT
+ 0x2a8 TimerResolutionLink : _LIST_ENTRY
+ 0x2b0 RequestedTimerResolution : Uint4B
+ 0x2b4 ActiveThreadsHighWatermark : Uint4B
+ 0x2b8 SmallestTimerResolution : Uint4B
+ 0x2bc TimerResolutionStackRecord : Ptr32 _PO_DIAG_STACK_RECORD
*/

/*
Win7SP1.7601.17514.101119-1850.X86
nt!_KPROCESS
+0x000 Header           : _DISPATCHER_HEADER
+ 0x010 ProfileListHead : _LIST_ENTRY
+ 0x018 DirectoryTableBase : Uint4B
+ 0x01c LdtDescriptor : _KGDTENTRY
+ 0x024 Int21Descriptor : _KIDTENTRY
+ 0x02c ThreadListHead : _LIST_ENTRY
+ 0x034 ProcessLock : Uint4B
+ 0x038 Affinity : _KAFFINITY_EX
+ 0x044 ReadyListHead : _LIST_ENTRY
+ 0x04c SwapListEntry : _SINGLE_LIST_ENTRY
+ 0x050 ActiveProcessors : _KAFFINITY_EX
+ 0x05c AutoAlignment : Pos 0, 1 Bit
+ 0x05c DisableBoost : Pos 1, 1 Bit
+ 0x05c DisableQuantum : Pos 2, 1 Bit
+ 0x05c ActiveGroupsMask : Pos 3, 1 Bit
+ 0x05c ReservedFlags : Pos 4, 28 Bits
+ 0x05c ProcessFlags : Int4B
+ 0x060 BasePriority : Char
+ 0x061 QuantumReset : Char
+ 0x062 Visited : UChar
+ 0x063 Unused3 : UChar
+ 0x064 ThreadSeed : [1] Uint4B
+ 0x068 IdealNode : [1] Uint2B
+ 0x06a IdealGlobalNode : Uint2B
+ 0x06c Flags : _KEXECUTE_OPTIONS
+ 0x06d Unused1 : UChar
+ 0x06e IopmOffset : Uint2B
+ 0x070 Unused4 : Uint4B
+ 0x074 StackCount : _KSTACK_COUNT
+ 0x078 ProcessListEntry : _LIST_ENTRY
+ 0x080 CycleTime : Uint8B
+ 0x088 KernelTime : Uint4B
+ 0x08c UserTime : Uint4B
+ 0x090 VdmTrapcHandler : Ptr32 Void
*/

/*
	case WINDOWS_VERSION_XP:
		offset_eprocess_flink = 0x88;
		break;
	case WINDOWS_VERSION_7_7600_UP:
	case WINDOWS_VERSION_7_7000:
		offset_eprocess_flink = 0xb8;
		break;
	case WINDOWS_VERSION_VISTA_2008:
		offset_eprocess_flink = 0x0a0;
		break;
	case WINDOWS_VERSION_2K3_SP1_SP2:
		offset_eprocess_flink = 0x98;
		break;
	case WINDOWS_VERSION_2K3:
		offset_eprocess_flink = 0x088;
		break;

*/