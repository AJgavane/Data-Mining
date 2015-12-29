#include<stdio.h>
#include<stdlib.h>

#define db printf("Here\n");

/*Structure which encapsulates the "node" in a fp tree*/
typedef struct fp_node
{
  int id;
  int frequency;
  int children;
  struct fp_node **vertical_links;
  struct fp_node *horizontal_link;
  struct fp_node *parent_link;
}fp_node_t;

/*Structure which encapsulates the notion of an item*/
typedef struct 
{
  char *item_name;
  int item_count;
  
}item_t;

/*Structure which encapsulates the notion of transaction*/
typedef struct
{
  int *item_ids;
  int num_items;
}trans_t;


void applyMinSupport(item_t *itemset, trans_t *numeric_transactions, int minsup, int num_items, int num_trans);
fp_node_t **constructFPTree(item_t *itemset, trans_t *numeric_transactions, int num_trans, int num_items);
int checkChildren(fp_node_t *tmphead, int id);

void mineFrequentItemSetEndWith(fp_node_t *root, int item, int *prefix, int num_items, int level);
fp_node_t *createTreeEndWith(fp_node_t *root, int item);
void setNullPrefix(int *prefix, int level, int num_items);
void *wmalloc(int size);
void freeTree(fp_node_t *node);

FILE **fp;
int no_files=10;
void record(int *prefix, int frequency, int level);


int minsup;
item_t *itemset;
trans_t *numeric_transactions;
int main(int argc, char **argv)
{
  int num_items, num_trans;
  int ii, jj;
  int tempint;
  float min_sup_p = atof(argv[2]);
  FILE *processed_data = fopen(argv[1], "r");
  if(processed_data==NULL)
  {
    printf("Error opening file\n");
    exit(0);
  }

  fp_node_t *head;
  fp_node_t **head_pointers;
  fscanf(processed_data, "%d %d", &num_items, &num_trans);
  itemset = wmalloc(num_items*sizeof(item_t));
  for(ii=0; ii<num_items; ii++)
  {
    itemset[ii].item_name = wmalloc(50*sizeof(char));
    fscanf(processed_data,"%d%d", &tempint, &itemset[ii].item_count);
    fscanf(processed_data, "%s", itemset[ii].item_name);
  }
  numeric_transactions = wmalloc(num_trans*sizeof(trans_t));
  for(ii=0; ii<num_trans;ii++)
  {
    fscanf(processed_data,"%d", &numeric_transactions[ii].num_items);
    numeric_transactions[ii].item_ids = wmalloc(numeric_transactions[ii].num_items*sizeof(int));
    for(jj=0; jj<numeric_transactions[ii].num_items; jj++)
    {
      fscanf(processed_data,"%d", &numeric_transactions[ii].item_ids[jj]);
    }
  }
  minsup = (int)(min_sup_p*num_trans);
  printf("Mincount is %d\n", minsup);
  
  applyMinSupport(itemset, numeric_transactions, minsup, num_items, num_trans);
  
  
  FILE *test_thresh = fopen("testthresh.txt", "w");
  for(ii=0; ii<num_trans;ii++)
  {
    fprintf(test_thresh,"%d", numeric_transactions[ii].num_items);
    for(jj=0; jj<numeric_transactions[ii].num_items; jj++)
    {
      fprintf(test_thresh," %d", numeric_transactions[ii].item_ids[jj]);
    }
    fprintf(test_thresh, "\n");
  }
  fclose(test_thresh);
  fclose(processed_data);
  head_pointers = constructFPTree(itemset, numeric_transactions, num_trans, num_items);
  int *prefix = wmalloc(num_items*sizeof(int));
  setNullPrefix(prefix, -1, num_items);
  
  char *filename = malloc(100*sizeof(char));
  fp = wmalloc(no_files*sizeof(FILE *));

  //fp  = fopen("itemsets.txt", "w");
  for(ii=0; ii<no_files; ii++)
  {
    sprintf(filename, "%d_cardinality_itemsets.txt", ii+1);
    fp[ii] = fopen(filename, "w");
  }
  for(ii=0; ii<num_items; ii++)
  {
    mineFrequentItemSetEndWith(head_pointers[0], ii, prefix, num_items, 0);
    setNullPrefix(prefix, -1, num_items);
  }
  for(ii=0; ii<no_files; ii++)
  {
    fclose(fp[ii]);
  }
}

void mineFrequentItemSetEndWith(fp_node_t *root, int item, int *prefix, int num_items, int level)
{
  int i;
  fp_node_t *new_tree = createTreeEndWith(root, item);
//   for(i=0; i<level;i++)
//   {
//     printf("  ");
//   }
  //printf("Counted frequency of created tree for item %d is %d\n", item, new_tree->frequency);
  if(new_tree->frequency>minsup)
  {
    prefix[level] = item;
    record(prefix, new_tree->frequency, level+1);
    for(i=item-1; i>=0; i--)
    {
      mineFrequentItemSetEndWith(new_tree, i, prefix, num_items, level+1);
      setNullPrefix(prefix, level, num_items);
    }
  }
  freeTree(new_tree);
}

fp_node_t *createTreeEndWith(fp_node_t *root, int item)
{
  
  fp_node_t *head = wmalloc(sizeof(fp_node_t));
  head->vertical_links = wmalloc(root->children*sizeof(fp_node_t*));
  int i, children=0, count;
  head->frequency=0;
  head->id = root->id;
  for(i=0; i<root->children; i++)
  {
    count = countEndingWith(root->vertical_links[i], item);
    if(count>0)
    {
      head->vertical_links[children++] = createTreeEndWith(root->vertical_links[i], item);
    }
    head->frequency+=count;   
  }
  head->children =  children;
  head->vertical_links = realloc(head->vertical_links, children*sizeof(fp_node_t*));
  return head;
}

int countEndingWith(fp_node_t *root, int item)
{
  if(root->id==item)
    return root->frequency;
  int i, count=0;
  for(i=0; i<root->children; i++)
  {
    count+=countEndingWith(root->vertical_links[i], item);
  }
  return count;
}


void setNullPrefix(int *prefix, int level, int num_items)
{
  int ii;
  for(ii=num_items; ii>level; ii--)
  {
    prefix[ii] = -1;
  }
}

void *wmalloc(int size)
{
  void *pointer;
  if((pointer= malloc(size))==NULL)
  {
    printf("Could not allocate memory\n");
    exit(0);
  }
  return pointer;
}

void applyMinSupport(item_t *itemset, trans_t *numeric_transactions, int minsup, int num_items, int num_trans)
{
  int ii, jj, kk;
  for(kk=0; kk<num_items; kk++)
  {
    if(itemset[kk].item_count<minsup)
    {
      //printf("Item no: %d, %s with count %d is infrequent\n", kk, itemset[kk].item_name,  itemset[kk].item_count);
      for(ii=0; ii<num_trans; ii++)
      {
	for(jj=0; jj<numeric_transactions[ii].num_items;jj++)
	{
	  if(numeric_transactions[ii].item_ids[jj]==kk)
	  {
	    numeric_transactions[ii].item_ids = realloc(numeric_transactions[ii].item_ids, jj*sizeof(int));
	    numeric_transactions[ii].num_items = jj;
	    break;
	  }
	}
      }
    }
  }
}

fp_node_t **constructFPTree(item_t *itemset, trans_t *numeric_transactions, int num_trans, int num_items)
{
  int ii, jj, k;
  fp_node_t **head_pointers = wmalloc((num_items+1)*sizeof(fp_node_t*));
  fp_node_t *head = wmalloc(sizeof(fp_node_t));
  head->id = -1;
  head->frequency = 0;
  head->children=0;
  head->vertical_links = wmalloc(sizeof(fp_node_t*));
  head->vertical_links[0] = NULL;
  head->parent_link = NULL;
  head->horizontal_link = NULL;
  fp_node_t *iter, *tmp, *horizont_traversal;
  for(ii=1; ii<=num_items; ii++)
  {
    head_pointers[ii]=wmalloc(sizeof(fp_node_t));
    head_pointers[ii]->id=ii-1;
    head_pointers[ii]->frequency=0;
    head_pointers[ii]->children=0;
    head_pointers[ii]->vertical_links = wmalloc(sizeof(fp_node_t*));
    head_pointers[ii]->vertical_links[0] = NULL;
    head_pointers[ii]->horizontal_link = NULL;
    head_pointers[ii]->parent_link = NULL;
  }
  
  for(ii=0; ii<num_trans-1; ii++)
  {
    iter = head;
    //printf("%d\n", numeric_transactions[9834].num_items);
    for(jj=0; jj<numeric_transactions[ii].num_items; jj++)
    {
      k=checkChildren(iter, numeric_transactions[ii].item_ids[jj]);
      if(k<0)
      {
	tmp = wmalloc(sizeof(fp_node_t));
	tmp->id = numeric_transactions[ii].item_ids[jj];
	tmp->frequency=1;
	tmp->children=0;
	tmp->vertical_links=wmalloc(sizeof(fp_node_t*));
	tmp->vertical_links[0]=NULL;
	tmp->horizontal_link=NULL;
	tmp->parent_link = iter;
	if(head_pointers[tmp->id+1]->horizontal_link==NULL)
	{
	  head_pointers[tmp->id+1]->horizontal_link=tmp;
	  head_pointers[tmp->id+1]->frequency++;
	}
	else
	{
	  horizont_traversal = head_pointers[tmp->id+1];
	  head_pointers[tmp->id+1]->frequency++;
	  while(horizont_traversal->horizontal_link!=NULL)
	    horizont_traversal = horizont_traversal->horizontal_link;
	  horizont_traversal->horizontal_link = tmp;
	}
	iter->vertical_links = realloc(iter->vertical_links, (iter->children+1)*sizeof(fp_node_t*));
	iter->vertical_links[iter->children] = tmp;
	iter->children++;
	iter=tmp;
	//printf("Added: %s\n", itemset[iter->id].item_name);
      }
      else
      {
	iter = iter->vertical_links[k];
	iter->frequency++;
	head_pointers[iter->id+1]->frequency++;
      }
	
    }
  }
  head_pointers[0] = head;
  return head_pointers;
}

int checkChildren(fp_node_t *tmphead, int id)
{
  int num_child = tmphead->children;
  //db;
  int i;
  for(i=0; i<num_child; i++)
    if(tmphead->vertical_links[i]->id==id)
      return i;
    
  return -1;
  
}

void freeTree(fp_node_t *node)
{
  int i;
  if(node->children==0)
    return;
  for(i=0; i<node->children; i++)
  {
    freeTree(node->vertical_links[i]);
    free(node->vertical_links[i]);
  }
  free(node->vertical_links);
}
void record(int *prefix, int frequency, int level)
{
  int i=0;
  
  fprintf(fp[level-1], "%d ", level);
  while(prefix[i]!=-1)
  {
    fprintf(fp[level-1], "%d ", prefix[i]);
    i++;
  }
  fprintf(fp[level-1], "%d\n", frequency);
}
