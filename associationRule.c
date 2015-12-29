#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define min_confidence 30.0

int k = 3;
int SIZE = 107;

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
	double Confidence;
	int Validbit;
}subset_t;

subset_t *subset_data;
int subset_counter = 0;

frequent_item_set_t *frequentItem_data;

void display();
void gen_Rule(int i, int supportCount);
void gen_subset(int size_lhs, int frequent_item_set_num);
void printSubset();
void getSupport();
int match(int *a, int *b, int size);
void calculateConfidence(int supportCount);

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
	i_end_index = i-1;

	int subset_size = pow(2,k);
	subset_size -= 2;
	subset_size *= 2;
	subset_data = (subset_t *) malloc (sizeof(subset_t) * (sizeof(int) * k ) * subset_size);

	printf("%d %d\n",i_start_index,i_end_index);
	i = 0;
	for (i = i_start_index ; i <= i_end_index; i++){
		printf("Processing frequent data : %d\n", i);
		gen_Rule(i, frequentItem_data[i].supportCount);
	}

	printf("Output is : \n");
	FILE *fout;
	fout = fopen("rule.txt","w");
	for( i = 0 ; i < subset_counter; i++){
		if( subset_data[i].Confidence > min_confidence){
			// printf("lhs: ");
			for(j = 0 ; j < subset_data[i].size_lhs; j++)
				fprintf(fout,"%d ",subset_data[i].elements_lhs[j]);
			fprintf(fout," -> ");
			for(j = 0 ;  j < k - subset_data[i].size_lhs; j++)
				fprintf(fout,"%d ",subset_data[i].elements_rhs[j]);
			fprintf(fout,"\t Confidence: %lf\n", subset_data[i].Confidence);
		}
	}
	fclose(fout);
	
	// testing if everything is stored correctly in the structure
	//display();

	fclose(fin);
	return 0;
}

void gen_Rule(int frequent_item_set_num, int supportCount){
	int j;
		// for each subset of rule_i of size j
		gen_subset(j,frequent_item_set_num );
	 // printSubset();
	getSupport();
	printSubset();
	calculateConfidence(supportCount);
	
	
	return;
}

void gen_subset(int size_lhs, int frequent_item_set_num){
	/*
		for(no_of_subset=1;no_of_subset<=no_of_element;no_of_subset++)
		{
			for(start=0;start<=no_of_element-no_of_subset;start++)
				{
				if(no_of_subset==1)
					printf(“%d\n”,a[start]);
				else
				{
					index=start+no_of_subset-1;
					for(j=index;j<no_of_element;j++)
					{
						for(i=start;i<index;i++)
							printf(“%d”,a[i]);
						printf(“%d\n”,a[j]);
					}
				}
			}
		}


	*/
	int i, j, p, temp;
	int num_of_subset, start, index;
	for( num_of_subset = 1; num_of_subset < k; num_of_subset++){

		for(start = 0; start <= k - num_of_subset ; start++){

			
			// printf("count : %d: ",subset_counter);

			if(num_of_subset == 1){
				subset_data[subset_counter].size_lhs = num_of_subset;
				subset_data[subset_counter].elements_lhs = (int *) malloc (sizeof(int) * num_of_subset);
				subset_data[subset_counter].elements_rhs = (int *) malloc (sizeof(int) * (k - num_of_subset) );
				subset_data[subset_counter].elements_lhs[0] = frequentItem_data[frequent_item_set_num].frequentItem[start];
				// printf("%d\n",subset_data[subset_counter].elements_lhs[0]);
				j = 0;
				for( i = 0 ; i < k ; i++){
					if(subset_data[subset_counter].elements_lhs[0] != frequentItem_data[frequent_item_set_num].frequentItem[i]){
						subset_data[subset_counter].elements_rhs[j] = frequentItem_data[frequent_item_set_num].frequentItem[i];
						j++;
					}
				}
				subset_counter += 1;
			} else {
				
				index = start + num_of_subset - 1;
				for( j = index; j < k; j++){
					subset_data[subset_counter].size_lhs = num_of_subset;
					subset_data[subset_counter].elements_lhs = (int *) malloc (sizeof(int) * num_of_subset);
					subset_data[subset_counter].elements_rhs = (int *) malloc (sizeof(int) * (k - num_of_subset) );
					// printf("index: %d\n",index);
					p = 0;
					for(i = start; i < index ; i++){
						// printf("%d ",frequentItem_data[frequent_item_set_num].frequentItem[i]);
						int temp = frequentItem_data[frequent_item_set_num].frequentItem[i];
						subset_data[subset_counter].elements_lhs[p] = temp;
						// printf("%d p:%d.",subset_data[subset_counter].elements_lhs[p],p);
						p++;
					}
					subset_data[subset_counter].elements_lhs[p]  =  frequentItem_data[frequent_item_set_num].frequentItem[j];
					// printf("%d\n",subset_data[subset_counter].elements_lhs[p]);
					p = 0;
					for(i = 0 ; i < k; i++){
						if( isPresent(subset_data[subset_counter].elements_lhs, frequentItem_data[frequent_item_set_num].frequentItem[i], k - num_of_subset + 1 ) == 0 ){
							subset_data[subset_counter].elements_rhs[p] = frequentItem_data[frequent_item_set_num].frequentItem[i];
							p++;
						}
					}
					subset_counter += 1;
				}
				
				//printf("w y no increase %d\n",subset_counter);
			}
				
		}
	}
	printf("%d\n",subset_counter);
}

void printSubset(){
	int i,j;
	for( i = 0 ; i < subset_counter; i++){
		printf("size: %d  lhs: ", subset_data[i].size_lhs);
		for(j = 0 ; j < subset_data[i].size_lhs; j++)
			printf("%d ",subset_data[i].elements_lhs[j]);
		printf("  rhs: ");
		for(j = 0 ;  j < k - subset_data[i].size_lhs; j++)
			printf("%d ",subset_data[i].elements_rhs[j]);
		printf("  Sup_lhs : %d  Confidence: %lf  Validbit: %d\n", subset_data[i].supportCount_lhs,subset_data[i].Confidence, subset_data[i].Validbit);
	}
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

int isPresent(int *arr, int key, int size){
	int i;
	int result = 0;
	for(i = 0 ; i < size; i++){
		if(key == arr[i])
			result = 1;
	}
	return result;
}	

void getSupport(){
	int i, j;
	int subset_sz, subset_rhs;
	for(i = 0 ; i < subset_counter ; i++){
		subset_sz = subset_data[i].size_lhs;
		printf("subset_sz: %d\n",subset_sz);
		for( j = 0 ; j < SIZE; j++){
			if( frequentItem_data[j].k < subset_sz )
				continue;
			if( frequentItem_data[j].k > subset_sz)
				break;
			if(  match(frequentItem_data[j].frequentItem , subset_data[i].elements_lhs, subset_sz) ){
				subset_data[i].supportCount_lhs = frequentItem_data[j].supportCount;
				printf("%d\n",frequentItem_data[j].supportCount );
			}
		} 
	}
}

int match(int *a, int *b, int size){
	int i;
	int result = 1;
	for(i = 0 ; i < size; i++){
		if(a[i] != b[i]){
			result = 0;
			return result;
		}
	}
	return result;
}

void calculateConfidence(int supportCount){
	int i;
	int temp;
	for(i = 0 ; i <= subset_counter; i++){
		temp = subset_data[i].supportCount_lhs;
		if(temp != 0)
			subset_data[i].Confidence = (double) supportCount / (double) temp * 100;
		else 
			subset_data[i].Confidence = 0;
	}
}