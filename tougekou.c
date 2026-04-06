#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#define MAX 200
#define MAXTOKEN 40

#define COMMAND_RETRIEVE 1
#define COMMAND_ARRIVAL 2
#define COMMAND_DEPARTURE 3
#define COMMAND_EXIT 4

#define RETRIEVE_KEY_ID 1
#define RETRIEVE_KEY_NAME 2
#define RETRIEVE_KEY_SCHOOL 3

struct book{
	int id;
	char school[MAXTOKEN];
	char name[MAXTOKEN];
	int num;
	int borrowed;
};

struct idx_tbl{
	int id;
	int idx;
};

int create_tbl(FILE *infp,struct book *book,struct idx_tbl *idx_tbl);
void sort_book(struct idx_tbl *idx_tbl,int left,int right);
int retrieve_by_id(int id,struct idx_tbl *idx_tbl,int n);
int retrieve_by_name(char *name,struct book *book,int b,int n);
int retrieve_by_school(char *school,struct book *book,int b,int n);
void arrival(struct book *book,int i);
void departure(struct book *book,int i);
void proc_request(struct book *book, struct idx_tbl *idx_tbl,int n);

int main(int argc,char *argv[])
{
	int n;
	struct book Book[MAX];
	struct idx_tbl Idx_tbl[MAX];
	FILE *infp;
	
		if((infp=fopen("seitodata.txt","r"))==NULL){
			printf("file not found.\n");
			exit(1);
		}
	
	n=create_tbl(infp,Book,Idx_tbl);
	sort_book(Idx_tbl,0,n-1);
	
	proc_request(Book, Idx_tbl, n);
	
	fclose(infp);
	return 0;
}

int create_tbl(FILE *infp,struct book *Book, struct idx_tbl *Idx_tbl)
{
	int i=0;
	int id,num;
	char school[MAXTOKEN],name[MAXTOKEN];
	
	while(fscanf(infp,"%d %s %s %d",&id,name,school,&num)!=EOF){
		Book[i].id=id;
		strcpy(Book[i].name,name);
		strcpy(Book[i].school,school);
		Book[i].num=num;
		Idx_tbl[i].id=id;
		Idx_tbl[i].idx=i;
		Book[i].borrowed=0;
		++i;
	}
	return i;
}

void sort_book(struct idx_tbl *idx_tbl,int left,int right)
{
	int i,j,p,k;
	i=left;
	j=right;
	p=i+(j-i)/2;
	struct idx_tbl temp;
	
	if(left>=right)
		return;
		
	while(i<=j)
	{
		while(idx_tbl[i].id<idx_tbl[p].id)
		{
			i++;
		}
		while(idx_tbl[j].id>idx_tbl[p].id)
		{
			j--;
		}
	
		if(i<=j)
		{
			k=idx_tbl[i].id;
			idx_tbl[i].id=idx_tbl[j].id;
			idx_tbl[j].id=k;

			k=idx_tbl[i].idx;
			idx_tbl[i].idx=idx_tbl[j].idx;
			idx_tbl[j].idx=k;

			i++;
			j--;
		}
	}
	sort_book(idx_tbl,left,j);
	sort_book(idx_tbl,i,right);
}

int retrieve_by_id(int id,struct idx_tbl *idx_tbl,int n)
{
	int low,high,mid;
	high=n;
	low=0;
	
	while(low != high)
	{
		mid=(high+low)/2;
		
		if(id == idx_tbl[mid].id)
		{
			return idx_tbl[mid].idx;
		}
		if(id<idx_tbl[mid].id)
		{
			high=mid-1;
		}
		if(id>idx_tbl[mid].id)
		{
			low=mid+1;
		}
	}
	
	if(id == idx_tbl[high].id)
	{
		return idx_tbl[high].idx;
	}
	
	return -1;
}

int retrieve_by_name(char *name,struct book *book,int b,int n)
{
	int i,p;
	for(i=b;i<=n;i++)
	{
		p= strcmp(name,book[i].name);
		if(p == 0)
		{
			return i;
		}
	}
	return -1;
}

int retrieve_by_school(char *school,struct book *book,int b,int n)
{
	int i,m,p;
	for(i=b;i<=n;i++)
	{
		m = strlen(school);
		p = strncmp(school,book[i].school,m);
		if(p == 0)
		{
			return i;
		}
	}
	return -1;
}

void arrival(struct book *book,int i)
{
	int num,borrowed;
	if(book[i].num>book[i].borrowed)
	{
		book[i].borrowed=book[i].borrowed+1;
		book[i].num=book[i].num-1;
		printf("Current Student Status：Entered\n");
	}
	else
	{
		printf("Already attended\n");
	}
}

void departure(struct book *book,int i)
{
	int num,borrowed;
	if(book[i].borrowed>0)
	{
		book[i].borrowed=book[i].borrowed-1;
		book[i].num=book[i].num+1;
		printf("Current Student Status：Departed\n");
	}
	else
	{
		printf("Not arrived yet\n");
	}
}

void proc_request(struct book *book, struct idx_tbl *idx_tbl, int n)
{
	int cmd,subcmd,id,i,b;
	char name[MAXTOKEN];
	char school[MAXTOKEN];
	while(1)
	{
		i=0;
		printf("Please enter a command: Search (1)/Check Out (2)/Return (3)/Exit (4):");
		scanf("%d",&cmd);
		switch(cmd)
		{
			case COMMAND_RETRIEVE:
			printf("\t What should the key be? ID (1)/Name (2)/School (3):");
			scanf("%d",&subcmd);
			switch(subcmd)
			{
				case RETRIEVE_KEY_ID:
				printf("\t\tPlease enter your ID:");
				scanf("%d",&id);
				i=retrieve_by_id(id,idx_tbl,n);
				if(i == -1)
				{
					printf("Nothing found\n");
				}
				else
				{
					printf("Id=%d Name=%s School=%s Num=%d\n",book[i].id,book[i].name,book[i].school,book[i].num);
				}
				break;
				
				case RETRIEVE_KEY_NAME:
				printf("\t\tPlease enter the student's name:");
				scanf("%s",name);
				b=0;
				for(;;)
				{
				i=retrieve_by_name(name,book,b,n);
				if(i==-1)break;
				printf("Id=%d Name=%s School=%s Num=%d\n",book[i].id,book[i].name,book[i].school,book[i].num);
				b=i+1;
				}
				if(b == 0)
				{
					printf("Nothing found\n");
				}
				break;
				
				case RETRIEVE_KEY_SCHOOL:
				printf("\t\tPlease enter a school name:");
				scanf("%s",school);
				b=0;
				for(;;)
				{
				i=retrieve_by_school(school,book,b,n);
				if(i==-1)break;
				printf("Id=%d Name=%s School=%s Num=%d\n",book[i].id,book[i].name,book[i].school,book[i].num);
				b=i+1;
				}
				if(b == 0)
				{
					printf("Nothing found\n");
				}
				break;
				default:
			}
			break;
			
			case COMMAND_ARRIVAL:
			printf("\tPlease enter student ID for attendance：");
			scanf("%d",&id);
			i=retrieve_by_id(id,idx_tbl,n);
			if(i == -1)
			{
				printf("Nothing found\n");
			}
			else
			{
				arrival(book,i);
			}
			break;
			
			case COMMAND_DEPARTURE:
			printf("\tPlease enter student ID for dismissal：");
			scanf("%d",&id);
			i=retrieve_by_id(id,idx_tbl,n);
			if(i == -1)
			{
				printf("Nothing found\n");
			}
			else
			{
				departure(book,i);
			}
			break;
			case COMMAND_EXIT:
			printf("Goodbye\n");
			return;
			default:
			printf("Invalid input\n");
		}
	}
}
