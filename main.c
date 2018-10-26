#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>
#include <math.h>

#define LINE_SIZE 25 //strict file format: assuming fixed length for each line and value
#define POINT_SIZE 8

#define UP 1
#define DOWN 0


int closest_p2(int n)
{
    int k=2;
    while (k<n)
    {
        k = k << 1;
    }
    return k;
}

void compare_switch(int i, int j, int dir, short int * data)
{
    if (dir == (data[i] > data[j]))
    {
        int t;
        t = data[i];
        data[i] = data[j];
        data[j] = t;
    }
}

void bitonic_merge(int low, int c, int dir, short int * data)
{
    if (c > 1)
    {
        int k = c / 2;
        for (int i = low;i < low+k ;i++)
            compare_switch(i, i+k, dir, data);
        bitonic_merge(low, k, dir, data);
        bitonic_merge(low+k, k, dir, data);
    }
}
void bitonic_sort(int low, int c, int dir, short int * data)
{
    if (c > 1)
    {
        int k = c / 2;
        bitonic_sort(low, k, UP, data);
        bitonic_sort(low + k, k, DOWN, data);
        bitonic_merge(low, c, dir, data);
    }
}

//--------------------------------------------------------------------------
int factorial(int n)
{
    if(n == 1) return 1;
    return n*factorial(n-1);
}

int num_combinations(int line_count)
{
    if(line_count == 1) return 0;
    if(line_count == 2) return 1;
    return factorial(line_count)/(2*factorial(line_count-2));
}
//---------------------------------------------------------------------------

void print_cells(short int ** arr, short int line_count)
{
    for ( size_t ix = 0; ix < line_count; ++ix )
            printf("%02d.%03d %02d.%03d %02d.%03d\n", arr[ix][0]/1000, abs(arr[ix][0]%1000), arr[ix][1]/1000, abs(arr[ix][1]%1000), arr[ix][2]/1000, abs(arr[ix][2]%1000));
}

void count_instances(short int * arr_outputs, short int * arr_pruned, short int * arr_counts, int max_point)
{
    int ix = 0;
    int cx = 0;
    while (ix < max_point)
    {
        arr_pruned[cx] = arr_outputs[ix];

        while(ix < max_point && arr_outputs[ix] == arr_pruned[cx])
        {
            arr_counts[cx]++;
            ix++;
        }
        cx++;
    }
}

void print_output(short int * arr_outputs, short int * arr_counts, int num_lines, int test)
{
    for ( size_t ix = 0; ix < num_lines; ++ix )
    {
        if(arr_counts[ix] == 0 - test) break;
        printf("%02d.%02d %d\n", arr_outputs[ix]/100, abs(arr_outputs[ix]%100), arr_counts[ix]);
    }
}

void process_cells(short int ** inputs, short int * outputs, int max_point)
{
    int pos = 0;
    //int ID = omp_get_thread_num();
    //int t = omp_get_num_threads();
    size_t current_point = 0;
    #pragma omp parallel for private(current_point)
        for (current_point = 0; current_point < max_point - 1; current_point++) {
            int starting_point = num_combinations(max_point) - num_combinations(max_point - current_point);
            float x1, y1, z1;
            x1 = inputs[current_point][0] / 1000.0;
            y1 = inputs[current_point][1] / 1000.0;
            z1 = inputs[current_point][2] / 1000.0;

            for (size_t i = 1; i < max_point - current_point; ++i) {
                //compare ix with current_point and store in outputs[current_point+ix-1]
                size_t ix = current_point + i;
                float x2, y2, z2;
                x2 = inputs[ix][0] / 1000.0;
                y2 = inputs[ix][1] / 1000.0;
                z2 = inputs[ix][2] / 1000.0;
                outputs[starting_point + i - 1] = (int) (100 * sqrt((z2 - z1) * (z2 - z1) + (y2 - y1) * (y2 - y1) +
                                                          (x2 - x1) * (x2 - x1)));
            }
        }
}


void read_cells(FILE* pFile, short int ** cells_list)
{
    char line[LINE_SIZE];
    int i_point = 0;
    while(fread(line, sizeof(char), LINE_SIZE*sizeof(char), pFile) != 0)
    {
        int p = 0;
        int i = 0;
        int pos = 0;

        for (;i<3; ++i, ++p)
        {
            pos = i*POINT_SIZE;
            cells_list[i_point][i] = (line[pos+1]-'0')*10000+(line[pos+2]-'0')*1000+(line[pos+4]-'0')*100+(line[pos+5]-'0')*10+(line[pos+6]-'0');
            if(line[pos] == '-') cells_list[i_point][i] *= -1;
        }
        i_point++;
    }

    print_cells(cells_list, i_point);
}

int main(int argc, char *argv[])
{
    int param_t;

    if(argc != 2)
    {
        printf("The number of arguments is invalid. Expected: -t#\n");
        exit(1);
    }

    param_t = atoi(&argv[1][2]);
    omp_set_num_threads(param_t);

    char filename[] = "cells.txt";
    FILE * pFile;
    pFile = fopen(filename, "r");

    //determine number of lines
    int line_count = 0;
    char line[LINE_SIZE+1];
    while(fread(line, sizeof(char), LINE_SIZE*sizeof(char), pFile) != 0)
    {
        line_count++;
    }
    int output_size = num_combinations(line_count);
    int padded_output_size = closest_p2(output_size); //POWER OF 2
    printf("line count: %d\n", line_count);
    printf("output size: %d\n", output_size);
    printf("padded: %d\n", padded_output_size);

    short int ** cells_list = (short int**) malloc(sizeof(short int*) * line_count); //array of points read
    for ( size_t ix = 0; ix < line_count; ++ix )
    {
        cells_list[ix] = (short int*) malloc(sizeof(short int) * 3);
    }

    short int * unsorted_list = (short int*) malloc(sizeof(short int) * padded_output_size); //first calculated output
    short int * sorted_list = (short int*) malloc(sizeof(short int) * padded_output_size); //output array
    short int * count_list = (short int*) malloc(sizeof(short int) * padded_output_size); //counting array
    for ( size_t ix = 0; ix < padded_output_size; ++ix )
    {
        unsorted_list[ix] = 32000;
        sorted_list[ix] = 32000;
        count_list[ix] = 0;
    }

    rewind(pFile);
    read_cells(pFile, cells_list);
    process_cells(cells_list, unsorted_list, line_count);
    print_output(unsorted_list, count_list, padded_output_size, 1);
    printf(" \n\n");
    bitonic_sort(0, padded_output_size, 1, unsorted_list);
    print_output(unsorted_list, count_list, output_size, 1);
    printf(" \n\n");
    count_instances(unsorted_list, sorted_list, count_list, output_size);
    print_output(sorted_list, count_list, output_size, 0);


    free(cells_list);
    return 0;
}
