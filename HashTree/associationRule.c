#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int k = 3;
int SIZE = 122;

typedef struct frequent_item_set{
	int k;
	int *frequentItem;
	int supportCount;
}frequent_item_set_t;

typedef struct subset
{
	int size_lhs;
	int *elements_lhs;
	int *elements_rhs;
	int supportCount_lhs;
	int Validbit;
}subset_t;

subset_t *subset_data;

frequent_item_set_t *frequentItem_data;

void display();
void gen_Rule(int i, int supportCount);

int main(int argc, char const *argv[])
{
	FILE *fin,*fp;
	fin = fopen("frequentItemSetData.txt","r");
	//getInfo(&SIZE,&k, fp);
	printf("%d %d\n",SIZE,k);
	frequentItem_data = (frequent_item_set_t*) malloc (sizeof(frequent_item_set_t) * SIZE * ( sizeof(int) * k));
	// initialize frequentItem_data
	int i = 0;
	int k_temp,j, data,s;
	int i_start_index, i_end_index;

	while( fscanf(fin, "%d", &k_temp) != EOF){
		frequentItem_data[i].k = k_temp;
		frequentItem_data[i].frequentItem = (int *) malloc (sizeof(int) * k_temp);
		for(j = 0 ; j < k_temp; j++){
			fscanf(fin,"%d",&data);
			frequentItem_data[i].frequentItem[j] = data;
		}
		fscanf(fin, "%d", &s);
		frequentItem_data[i].supportCount = s;
		if( k_temp == 3)
			i_start_index = i;
		i++;
	}
	i_end_index = i;

	int subset_size = pow(2,k);
	subset_size -= 2;
	subset_size *= 2;
	subset_data = (subset_t *) malloc (sizeof(subset_t) * (sizeof(int) * k ) * subset_size);

	printf("%d %d\n",i_start_index,i_end_index);

	for (i = i_start_index ; i < i_end_index; i++){
		gen_Rule(i, frequentItem_data[i].supportCount);
	}

	
	// testing if everything is stored correctly in the structure
	//display();

	fclose(fin);
	return 0;
}

void gen_Rule(int frequent_item_set_num, int supportCount){
	int j;
	for(j = 1 ; j < k ; j++){
		// for each subset of rule_i of size j
		//gen_subset(j,frequent_item_set_num );
	}
	return;
}


void display(){
	int i,j;
	for ( i = 0; i < SIZE; i++)
	{
		printf("\nk = %d\tItemSet: ",frequentItem_data[i].k);
		for(j = 0 ; j < frequentItem_data[i].k ; j++){
			printf("%d ", frequentItem_data[i].frequentItem[j] );
		}
		printf("\tsupport: %d\n",frequentItem_data[i].supportCount);
	}
}
