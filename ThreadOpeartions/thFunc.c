#include"drvHead.h"

//Ïß³ÌÃ¶¾Ù
VOID ThEnum1()
{
	PETHREAD pEth = NULL;
	PEPROCESS pEproc = NULL;
	NTSTATUS status;
	ULONG ulThreadId = 0;
	ULONG ulPid = 0;
	PCHAR pProcName = NULL;

	KdPrint(("ThEnum1\n"));
	for (ulThreadId = 0; ulThreadId < 80000; ulThreadId += 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)ulThreadId, &pEth);
		if (NT_SUCCESS(status))
		{
			pEproc = (PEPROCESS)(*(PEPROCESS *)((PCHAR)pEth + 0x150));
			ulPid = (ULONG)PsGetProcessId(pEproc);
			pProcName = PsGetProcessImageFileName(pEproc);
			KdPrint(("Tid:%d\tPid:%d\t%s\n", ulThreadId, ulPid, pProcName));
		}
		ObDereferenceObject(pEth);
	}
}

VOID ThEnum2(PEPROCESS process)
{
	PETHREAD pEth;
	PETHREAD pEthFirst;
	PEPROCESS pEproc = process;
	PLIST_ENTRY pLe = NULL;
	NTSTATUS status;
	ULONG ulThreadId;

	KdPrint(("ThEnum2\n"));
	//KdBreakPoint();
	pLe = (PLIST_ENTRY)((PCHAR)pEproc + 0x2c);
	pLe = pLe->Flink;
	pEth = (PETHREAD)((PCHAR)pLe - 0x1e0);
	pEthFirst = pEth;
	do
	{
		ulThreadId = *(PULONG)((PCHAR)pEth + 0x230);
		if (ulThreadId)
		{
			KdPrint(("\tTid:%d\n", ulThreadId));
		}
		pLe = (PLIST_ENTRY)((PCHAR)pEth + 0x1e0);
		pLe = pLe->Flink;
		pEth = (PETHREAD)((PCHAR)pLe - 0x1e0);
	} while (pEth != pEthFirst);
}

VOID ThEnum3(PEPROCESS process)
{
	PETHREAD pThd = NULL;
	NTSTATUS status;
	ULONG tid;
	KdPrint(("ThEnum2\n"));
	for (tid = 0; tid < 20000; tid += 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)tid, &pThd);
		if (NT_SUCCESS(status))
		{
			if (IoThreadToProcess(pThd) == process)
			{
				KdPrint(("\tTid:%d\n", tid));
			}
		}
		ObDereferenceObject(pThd);
	}
}
