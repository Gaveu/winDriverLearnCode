#include"drvHead.h"

//����ö��
//���ڽ���˫����ı������Ͽ죬�����ڲ���Ӳ������м���������
VOID PsEnum1()
{
	PEPROCESS pEproc = NULL;		//��ȡ���̵�ַ
	PEPROCESS pFirstProc = NULL;	//����EPROCESS����˫����ͷ�ڵ�
	PLIST_ENTRY pLe = NULL;
	PUCHAR pProcName = NULL;		//�������ַ���ָ��
	ULONG ulPid = 0;				//����ID

	//��ȡ��ǰ����EPROCESS
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
		if (ulPid > 300000)
		{
			break;
		}
		KdPrint(("PID:%d\tparentID:%d\t%s\n",
			ulPid,
			(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
			pProcName));
		ThEnum2(pEproc);
		pLe = (PLIST_ENTRY)((PCHAR)pEproc + 0xb8);
		pLe = pLe->Flink;
		pEproc = (PEPROCESS)((PCHAR)pLe - 0xb8);
	} while (pEproc != pFirstProc);
	KdPrint(("---------------------\n"));
}

//��ͨ�ã��޼��������⣬��Ϊ���Ա���
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
		if (NT_SUCCESS(status))
		{
			pProcName = PsGetProcessImageFileName(pEproc);
			KdPrint(("PID:%d\tparentID:%d\t%s\n",
				ulPid,
				(ULONG)PsGetProcessInheritedFromUniqueProcessId(pEproc),
				pProcName));
			ThEnum3(pEproc);
		}
	}
	KdPrint(("---------------------\n"));
}

//������ͣ
//��������pid����ͣ��Ӧ���̵�ִ��
NTSTATUS PsSuspend(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5�汾����Ӳ�������뷽ʽִ�к���
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

//���ָ̻�
//��������pid���ָ���Ӧ���̵�ִ��
NTSTATUS PsResume(HANDLE hPid)
{
	NTSTATUS status;
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId(hPid, &pEproc);

	if (NT_SUCCESS(status))
	{
		//xp->Nt.5�汾����Ӳ�������뷽ʽִ�к���
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

//������ֹ
//��������pid����ֹ��Ӧ���̵�ִ��
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
		KdPrint(("OpenProcess Failed!0x%x\n", status));
	}
	ZwClose(hProc);
	return status;
}
