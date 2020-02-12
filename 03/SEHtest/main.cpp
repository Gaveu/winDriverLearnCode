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
				//1、此处产生异常
				strcpy(p, "HelloWorld");
				printf("try 3 hit\n");
			}
			__finally
			{	//3、1处产生异常后优先执行本try对应的finally块
				printf("finally 3 hit\n");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)//2、异常程序过滤器从1处接收到异常
		{	//4、3处finally块执行完毕后执行此处的__except块，并默认异常处理完毕
			printf("except 2 hit\n");
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)//5、由于执行完4处的代码后，
										//已处理完异常，故此处的过滤器不会接受到异常
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

