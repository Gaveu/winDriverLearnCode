#define Delay_One_MicroSecond (-10)
#define Delay_One_MilliSecond (Delay_One_MicroSecond * 1000)
void MySleep(LONG msec)
{
	LARGE_INTEGER li;
	li.QuadPart = Delay_One_MilliSecond;
	li.QuadPart *= msec;
	KeDelayExecutionThread(KernelMode, 0, &li);
}
