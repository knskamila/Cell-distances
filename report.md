# Assignment 3: OpenMP

## Computing the distance of two points
What kind of operations that are faster and or can be executed in parallel pipes depends
on the architecture of your processor. However the general case is that integer operations have a higher throughput than floating point operations. Another thing to take into account is the size of your data type as generally smaller data types have a higher throughput than larger ones, notable exceptions to this are graphics processing units who have a throughput of orders of magnitudes higher for single precision (32 bit) than half precision (16 bit).

In our case we convert the data that we reed in to short int by multiplying the values with 1000 to not have any decimals and make a cast to short int. This means that the largest and smallest values we need to account for is 10000 and -10000 respectively. Thus we can not use any smaller data size then short int since short short int (8 bit) only goes between -128 and 128.
~~~
short int * unsorted_list = (short int*) malloc(sizeof(short int) * padded_output_size); //first calculated output
short int * sorted_list = (short int*) malloc(sizeof(short int) * padded_output_size); //output array
short int * count_list = (short int*) malloc(sizeof(short int) * padded_output_size); //counting array
~~~
Type conversion may incur a  overhead and it might not. It does depend on how the types are stored and used. If for example a short int stored in a 32 bit register, converting it to a int is free and thus you have no overhead. In our case we do convert from short int to float which does, in most cases, require some operations. However in comparison with the square root operation the overhead of converting from short int to float is neglectable.



## Parsing the file
Strings are encoded using the char data type as a building block for different characters. Stringing multiple chars after one another can thus produce a string. Looking at these building blocks that we said was of type char, that is 8 bits, we see that these are just binary numbers as anything other data type. So to get characters out on the screen we need to use a lookup table that assigns each character a numeric representation. Sins we have eight bits we can assign 256 characters with a unique numeric value. When we in C use the function printf() to print a variable to the consol we also need to specify how printf should interpret our variable, for integers we use %d or %i but for strings we use %s. What %s means is that we want printf to start printing what's at the pointer and continue to print characters until it comes to the stop character '\o'.

In the Ascii lookup table the digits are assigned number 48 - 56 and the signs + and - bot have number 43 and 45 respectively. Given the fixed format that we are given in the text file we could read line by line. Each line would be split up in the five characters read for each coordinate. The first character we would subtract 48 and then chest as a short in and multiplied with 10000 and added to our short int variable. The next chars would all go through the same process but be multiplied with 1000, 100, 10 and 1 respectively. lastly we need to flip the last bit in our short int variable if the first char was -.

~~~~~
cells_list[i_point][i] = (line[pos+1]-'0')*10000+(line[pos+2]-'0')*1000+(line[pos+4]-'0')*100+(line[pos+5]-'0')*10+(line[pos+6]-'0');
~~~~~

## Memory management
One obvious thing one should think about while managing memory is of course to use suitable data types as to not waste space. As we have mentioned before this usually also carries a higher throughput with it as well. In our case this is not quite enough, since our upper limit of cells is $2^{32}$. For the worst case coordinates will take up $3*2^{32}*2$ bytes which is almost 26 GB. Hence we need a more sophisticated way of dealing with large files containing lots of cells. One way is to split up the work of calculating distances in to blocks. Each block will work on a different piece of the file, in the first layer of the algorithm we split our threads in to teams using the distribute pragma each team will work on calculating the distances within each block, once each team is done the distance between each cell in the different block would be calculated. Once that is done the memory for each block is deallocated and the next piece of the file can be read in to the next two blocks. Once all of the blocks are calculated as well as the neighboring paired block distances all the distances between cells within different blocks must be calculated. Thus in the second level of the algorithm we load each pairing of blocks once more but merge the teams and only calculate the distances between cells in different blocks. Since there is never more then two blocks worth of data at the same time we can regulate the amount of data being used by the size of the data that is loaded in each block.

Sins we are working with a finite and corse resolution as well as a finite space that the cells can occupy we can just allocate an array that is the size of all posible distances posible. Maximum distance between two cells is $sqrt(20^2+20^2+20^2)$ which is approximatly 34.64. Thus there is 3464 posible distances for our level of precission. Now that a distance is calculated we only use that as our index, sinc it is a chast as a integer, and add 1 to the value in the array. There is one caviat though and that is that we will need to have one array for each thread so that two threads wont write to the same element at the same time. Then once all the distances are calculated we just element wise add the arrays for the different threads. An alternative would have been to have syncronios writing to one array, which would have been impractical. 

## Parallelization
It depends, if you are parallelizing in such a way that each thread is working on one digit in a number then with the algorithm that we used it might occur some odd writes. Since computers are working in binary multiplying digits by a factor ten does not mean that that part have uses bits that are separate form a digit multiplied by another factor of ten. One should rather parallelize by parsing different numbers rather then different digits within numbers.

Well no not really. One could think that with 3464 possible distances there would be no chanse of two threads are computing the same distance at the same time. This might be true for a smalle set of cells, however because we are not dealing with a randome distribution of points some distances may be more comon then others whisc may drive up the likelyhood of two threads at the same time. Thus it is much more samfe to have the threads not able to access the same element at the same time. 
