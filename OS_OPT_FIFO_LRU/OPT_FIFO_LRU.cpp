#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#define EMPTY -1
int total_ins;	//总指令个数
int total_vp;	//用户内存页面数
int *p_ins;		//指令数组
int diseffect;
typedef struct pf_struct{
	int ins;
	struct pf_struct *next;
}Pframe;
Pframe *pages, *p_head, *p_tail;
void readIns();
void init(int total_vp);
int FIFO(int total_vp);
int OPT(int total_vp);
int LRU(int total_vp);
/*
	从文件中读入指令
	*/
void readIns()
{
	FILE *fp;
	fp = fopen("ins.txt", "r");
	if (!fp)
	{
		printf("can't open file!\n");
		exit(0);
	}
	fscanf(fp, "%d", &total_ins);
	p_ins = new int[total_ins];
	for (int i = 0; i < total_ins; i++)
	{
		fscanf(fp, "%d", &p_ins[i]);
	}
	fclose(fp);
}
/*
	初始化
	*/
void init(int total_vp)
{
	diseffect = 0;
	pages = new Pframe[total_vp];
	for (int i = 0; i < total_vp - 1; i++)
	{
		pages[i].ins = EMPTY;
		pages[i].next = &pages[i + 1];
	}
	p_head = pages;
	p_tail = &pages[total_vp - 1];
	p_tail->ins = EMPTY;
	p_tail->next = NULL;
}

int FIFO(int total_vp)
{
	init(total_vp);
	for (int i = 0; i < total_ins; i++)
	{
		int j = 0;
		for (; j < total_vp; j++)
		{
			if (pages[j].ins == p_ins[i])	break;
		}
		if (j == total_vp)			//缺页
		{
			diseffect++;

			for (j = 0; j < total_vp; j++)
			{
				if (pages[j].ins == EMPTY)	break;
			}
			//无空闲空间，需要进行FIFO置换
			if (j == total_vp)
			{
				Pframe *p = p_head->next;
				p_head->next = NULL;
				p_head->ins = p_ins[i];
				p_tail->next = p_head;
				p_tail = p_head;
				pages = p_head = p;
			}
			//有空闲空间
			else
			{
				pages[j].ins = p_ins[i];
			}
		}
	}
	printf("FIFO %d\n", diseffect);
	return 0;
}

int OPT(int total_vp)
{
	init(total_vp);
	for (int i = 0; i < total_ins; i++)
	{
		int j = 0;
		for (; j < total_vp; j++)
		{
			if (pages[j].ins == p_ins[i])	break;
		}
		if (j == total_vp)			//缺页
		{
			diseffect++;

			for (j = 0; j < total_vp; j++)
			{
				if (pages[j].ins == EMPTY)	break;
			}
			//无空闲空间，需要进行OPT置换
			if (j == total_vp)
			{
				int *p_content = new int[total_vp];
				int *dist = new int[total_vp];
				Pframe *p = p_head;
				int index = 0;
				//准备工作，将页内内容存入p_content数组中,并且初始化距离
				while (p->next != NULL)
				{
					p_content[index] = p->ins;
					dist[index] = total_ins;
					p++;
					index++;
				}
				p_content[index] = p->ins;
				dist[index] = total_ins;
				for (j = i + 1; j < total_ins; j++)
				{
					for (int z = 0; z < total_vp; z++)
					{
						if (p_content[z] == p_ins[j] && dist[z] == total_ins)
						{
							dist[z] = j;
						}
					}
				}
				//找寻最远距离的那个数字的下标
				int max = -1;
				for (j = 0; j < total_vp; j++)
				{
					if (dist[j]>max)
					{
						max = dist[j];
					}
				}
				for (j = 0; j < total_vp; j++)
				{
					if (dist[j] == max)		break;
				}
				index = 0;
				p = p_head;
				while (p->next != NULL)
				{
					if (p->ins == p_content[j])
						break;
					index++;
					p++;
				}
				p->ins = p_ins[i];
			}
			//有空闲空间
			else
			{
				pages[j].ins = p_ins[i];
			}
		}
	}
	printf("OPT %d\n", diseffect);
	return 0;
}

int LRU(int total_vp)
{
	init(total_vp);
	int *count = new int[total_vp];
	memset(count, 0, sizeof(int)*total_vp);
	for (int i = 0; i < total_ins; i++)
	{
		int j = 0;
		for (; j < total_vp; j++)
		{
			//LRU,命中了,对其计数器清零，重新开始计算周期数,其他数周期加一
			if (pages[j].ins == p_ins[i])
			{
				for (int z = 0; z < total_vp; z++)
				{
					if (z == j)
					{
						count[z] = 0;
					}
					else
					{
						if (pages[z].ins != EMPTY)
						{
							count[z]++;
						}
					}
				}
				break;
			}
		}
		if (j == total_vp)			//缺页
		{
			diseffect++;

			for (j = 0; j < total_vp; j++)
			{
				if (pages[j].ins == EMPTY)	break;
			}
			//无空闲空间，需要进行LRU置换
			if (j == total_vp)
			{
				/*
					LRU置换算法
					*/
				int max = count[0];
				int index=0;
				for (int z = 0; z < total_vp; z++)
				{
					if (count[z]>max)
					{
						max = count[z];
						index = z;
					}
				}
				pages[index].ins = p_ins[i];
				for (int z = 0; z < total_vp; z++)
				{
					if (z == index)
					{
						count[z] = 0;
					}
					else
					{
						count[z]++;
					}
				}
			}
			//有空闲空间
			else
			{
				//周期数置0,并给其他页周期数加1
				pages[j].ins = p_ins[i];
				for (int z = 0; z < total_vp; z++)
				{
					if (z == j)
					{
						count[z] = 0;
					}
					else
					{
						if (pages[z].ins != EMPTY)
						{
							count[z]++;
						}
					}
				}
			}
		}
	}
	printf("LRU %d\n", diseffect);
	return 0;
}



int main(int argc, char* argv[])
{
	total_vp = atoi(argv[1]);
	readIns();
	OPT(total_vp);
	FIFO(total_vp);
	LRU(total_vp);
	return 0;
}
