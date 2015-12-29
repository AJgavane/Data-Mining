#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

typedef struct 
{
  char *item_name;
  int item_count;
  
}item_t;

typedef struct
{
  int *item_ids;
  int num_items;
}trans_t;
int countLines(FILE *basket_file);
char* readContents(FILE *basket_file);
char **tokenize(char *astr, char a_delim, int *num);
int tokenizeItems(char **transactions,item_t **itemset, trans_t **numeric_transactions, int count);
void sortItems(item_t *itemset, trans_t transaction);
void exchange(int *a, int *b);
void removeSpaces(char *item_name);
int main(int argc, char **argv)
{
  
  FILE *basket_file = fopen(argv[1], "r");
  int no_lines, num_items;
  int ii, jj;
  char **transactions;
  char *file_contents = readContents(basket_file);
  transactions = tokenize(file_contents, '\n', &no_lines);
  no_lines--;
  trans_t *numeric_transactions;
  item_t * itemset;
  num_items = tokenizeItems(transactions, &itemset, &numeric_transactions, no_lines);
  //printf("The items are \n");
  /*for(ii=0; ii<no_lines; ii++)
  {
    for(jj=0; jj<numeric_transactions[ii].num_items; jj++)
    {
      printf("%s ", itemset[numeric_transactions[ii].item_ids[jj]].item_name);
    }
    printf("\n");
  }*/
  for(ii=0; ii<no_lines; ii++)
  {
    //printf("Transaction no : %d\n", ii+1);
    sortItems(itemset, numeric_transactions[ii]);
  }
  
  for(ii=0; ii<num_items; ii++)
  {
    removeSpaces(itemset[ii].item_name);
  }
  FILE *processed_data = fopen("market_output.txt", "w");
  fprintf(processed_data, "%d %d\n", num_items, no_lines);
  for(ii=0; ii<num_items; ii++)
  {
    fprintf(processed_data,"%d %d %s\n", ii, itemset[ii].item_count, itemset[ii].item_name);
  }
  for(ii=0; ii<no_lines;ii++)
  {
    fprintf(processed_data,"%d %d", numeric_transactions[ii].num_items, numeric_transactions[ii].item_ids[0]);
    for(jj=1; jj<numeric_transactions[ii].num_items; jj++)
    {
      fprintf(processed_data," %d", numeric_transactions[ii].item_ids[jj]);
    }
    fprintf(processed_data, "\n");
  }
  fclose(basket_file);
  fclose(processed_data);
  
  
}
void sortItems(item_t *itemset, trans_t transaction)
{
  int n=transaction.num_items;
  if(n==1)
    return;
  int i, done;
  do
  {
    done=1;
    for(i=0; i<n-1; i++)
    {
      if(itemset[transaction.item_ids[i]].item_count<itemset[transaction.item_ids[i+1]].item_count)
      {
	//printf("Items to be exchanged : %s, %s\n", itemset[transaction.item_ids[i]].item_name, itemset[transaction.item_ids[i+1]].item_name);
	exchange(transaction.item_ids + i, transaction.item_ids + i+ 1);
	done=0;
      }	
    }
	
  }while(done==0);
}
void exchange(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int tokenizeItems(char **transactions,item_t **add_itemset, trans_t **add_numeric_transactions, int count)
{
  int ii,jj, no_items;
  *add_numeric_transactions = malloc(count*sizeof(trans_t));
  trans_t *numeric_transactions = *add_numeric_transactions;
  char **tmp = tokenize(transactions[0], ',', &no_items);
  no_items--;
  numeric_transactions[0].item_ids = malloc(no_items*sizeof(int));
  numeric_transactions[0].num_items=no_items;
  int item_set_size=0, tmp_pos;
  *add_itemset = malloc(no_items*sizeof(item_t));
  item_t *itemset = *add_itemset;
  for(jj=0; jj<no_items; jj++)
  {
    itemset[jj].item_name = strdup(tmp[jj]);
    itemset[jj].item_count=1;
    numeric_transactions[0].item_ids[jj]=item_set_size;
    item_set_size++;
  }
  for(ii=1; ii<count; ii++)
  {
    tmp = tokenize(transactions[ii], ',', &no_items);
    no_items--;
    numeric_transactions[ii].item_ids = malloc(no_items*sizeof(int));
    numeric_transactions[ii].num_items=no_items;
    for(jj=0; jj<no_items; jj++)
    {
      tmp_pos = checkItem(itemset, tmp[jj], item_set_size);
      if(tmp_pos<0)
      {
	*add_itemset = realloc(*add_itemset, (item_set_size+1)*sizeof(item_t));
	itemset = *add_itemset;	
	itemset[item_set_size].item_name = strdup(tmp[jj]);
	itemset[item_set_size].item_count = 1;
	numeric_transactions[ii].item_ids[jj]=item_set_size;
	item_set_size++;
      }
      else
      {
	//printf("%d here %d\n", tmp_pos, item_set_size);
	itemset[tmp_pos].item_count++;
	numeric_transactions[ii].item_ids[jj]=tmp_pos;
      }
    }
  }
  return item_set_size;
}

int checkItem(item_t *itemset, char *item, int item_set_size)
{
  int i;
  for(i=0; i<item_set_size; i++)
  {
    if(strcmp(itemset[i].item_name, item)==0)
      return i;
  }
  return -1;
  
}

char **tokenize(char *a_str, char a_delim, int *num)
{
  char *tmp=a_str;
  char **result=0;
  char delim[2];
  char* last_comma = 0;
  size_t count=0;
  delim[0] = a_delim;  
  delim[1] = 0;
  while (*tmp)
  {
      if (a_delim == *tmp)
      {
          count++;
          last_comma = tmp;
      }
      tmp++;
  }
  count += last_comma < (a_str + strlen(a_str) - 1);
  count++;
  //printf("%s\nNo of tokens %d\n\n", a_str, count);
  result = malloc((count)*(sizeof(char*)));
  if (result)
  {
    size_t idx  = 0;
    char* token = strtok(a_str, delim);

     while (token)
     {
        assert(idx < count);
        *(result + idx++) = strdup(token);
        token = strtok(0, delim);
     }
     assert(idx == count -1);
     *(result + idx) = 0;
  }
  *num = count;
  return result;
  
}
int countLines(FILE *basket_file)
{
  FILE *tmp = basket_file;
  char chr = getc(tmp);
  int count_lines=0;
  while(chr!=EOF)
  {
    if(chr=='\n')
      count_lines++;
    chr = getc(tmp);
  }
  return count_lines;
}

char* readContents(FILE *basket_file)
{
  char *file_contents;
  FILE *input_file = basket_file;
  long input_file_size;
  fseek(input_file, 0, SEEK_END);
  input_file_size = ftell(input_file);
  rewind(input_file);
  file_contents = malloc(input_file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), input_file_size, input_file);
  return file_contents;  
}

void removeSpaces(char *item_name)
{
  char *tmp = item_name;
  while(*tmp!='\0')
  {
    if(*tmp==' ')
      *tmp='_';
    tmp++;
  }
}