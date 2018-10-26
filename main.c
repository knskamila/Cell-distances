#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>
#include <math.h>

#define LINE_SIZE 26 //strict file format: assuming fixed length for each line and value
#define POINT_SIZE 8

#define MAX_OUTPUT 3464


void print_cells(short int ** arr, short int line_count)
{
    for ( size_t ix = 0; ix < line_count; ++ix )
            printf("%02d.%03d %02d.%03d %02d.%03d\n", arr[ix][0]/1000, abs(arr[ix][0]%1000), arr[ix][1]/1000, abs(arr[ix][1]%1000), arr[ix][2]/1000, abs(arr[ix][2]%1000));
}


void print_output(short int * arr_counts)
{
    for ( size_t ix = 0; ix < MAX_OUTPUT; ++ix )
    {
        if(arr_counts[ix] != 0)
        {
            float out = (float)ix/100;
            printf("%5.2f %d\n", out, arr_counts[ix]);
        }
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
            float x1, y1, z1;
            short int * outputs_local = (short int*) malloc(sizeof(short int) * MAX_OUTPUT);
    	    for ( size_t ix = 0; ix < MAX_OUTPUT; ++ix )
            {
                outputs_local[ix] = 0;
            }
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
                int index = (short) (100 * sqrt((z2 - z1) * (z2 - z1) + (y2 - y1) * (y2 - y1) +
                                                (x2 - x1) * (x2 - x1)));
                outputs_local[index]++;
            }

        #pragma omp critical
        {
            for ( size_t ix = 0; ix < MAX_OUTPUT; ++ix )
                outputs[ix] += outputs_local[ix];
        }
	    free(outputs_local);
    }
}


void read_cells(FILE* pFile, short int ** cells_list, int line_count)
{
    char line[LINE_SIZE];
    int i_point;

    for (i_point = 0; i_point < line_count; i_point++)
    {
        fgets(line, LINE_SIZE, pFile);
        int p = 0;
        int i = 0;
        int pos = 0;

        for (;i<3; ++i, ++p)
        {
            pos = i*POINT_SIZE;
            cells_list[i_point][i] = (line[pos+1]-'0')*10000+(line[pos+2]-'0')*1000+(line[pos+4]-'0')*100+(line[pos+5]-'0')*10+(line[pos+6]-'0');
            if(line[pos] == '-') cells_list[i_point][i] *= -1;
        }
    }
    fclose(pFile);
    //print_cells(cells_list, i_point);
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
    char line[LINE_SIZE];
    while(fgets(line, LINE_SIZE, pFile))
    {
        line_count++;
    }
    rewind(pFile);
    //printf("line count: %d\n", line_count);


    short int ** cells_list = (short int**) malloc(sizeof(short int*) * line_count); //array of points read
    for ( size_t ix = 0; ix < line_count; ++ix )
    {
        cells_list[ix] = (short int*) malloc(sizeof(short int) * 3);
    }

    //short int * output_list = (short int*) malloc(sizeof(short int) * MAX_OUTPUT); //output array
    short int * count_list = (short int*) malloc(sizeof(short int) * MAX_OUTPUT); //counting array
    for ( size_t ix = 0; ix < MAX_OUTPUT; ++ix )
    {
        //output_list[ix] = ix;
        count_list[ix] = 0;
    }

    read_cells(pFile, cells_list, line_count);
    process_cells(cells_list, count_list, line_count);
    //count_instances(unsorted_list, sorted_list, count_list, output_size);
    print_output(count_list);


    free(cells_list);
    return 0;
}
