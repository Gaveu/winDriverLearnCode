#include<stdio.h>
#include<string.h>
#include<windows.h>

void func1()
{
	__try
	{
		__try
		{
			printf("try 2 hit\n");
			__try
			{
				char *p = NULL;
				//1���˴������쳣
				strcpy(p, "HelloWorld");
				printf("try 3 hit\n");
			}
			__finally
			{	//3��1�������쳣������ִ�б�try��Ӧ��finally��
				printf("finally 3 hit\n");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)//2���쳣�����������1�����յ��쳣
		{	//4��3��finally��ִ����Ϻ�ִ�д˴���__except�飬��Ĭ���쳣�������
			printf("except 2 hit\n");
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)//5������ִ����4���Ĵ����
										//�Ѵ������쳣���ʴ˴��Ĺ�����������ܵ��쳣
	{	
		printf("except 1 hit\n");
	}
}

void func2()
 {
	 __try
	 {
		 __try
		 {
			 char *p = NULL;
			 p[0] = '\0';
		 }
		 __except (EXCEPTION_CONTINUE_SEARCH)
		 {	
			 printf("except 2 hit\n");
		 }
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 printf("except 1 hit\n");
	 }
 }

LONG func4(int *i)
{
	printf("i = %d\n", *i);
	if (*i < 3)
	{
		*i = *i + 1;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
}

void func3()
{
	char *p = NULL;
	int i = 0;
	__try
	{
		*p = 'A';
	}
	__except (func4(&i))
	{
		printf("Finally,i = %d\n", i);
		printf("except 1 hit\n");
	}
}


int main()
{
	func3();
	return 0;
}

