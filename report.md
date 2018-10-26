# Assignment 3: OpenMP

## Computing the distance of two points
What kind of operations that are faster and or can be exicuted in paralell pipes depends 
on the architecture of your processor. However the general case is that integer operations have a higher troughput then floting point operations. Another thing to take in to acouint is the size of your data type as generaly smaller data types have a higher troughput then larger ones, notable exeptions to this are graphics processing units who have a troughput of orders of magnitudes higer for single precission (32 bit) then half precission (16 bit).

In our case we convert the data that we reed in  to short int by multiplying the values with 1000 to not have any decimals and make a cast to short int. This means that the largest and smallest values we need to account for is 10000 and -10000 respectively. Thus we can not use any smaller data size then short int sins short short int (8 bit) only goes between -128 and 128. 

Type conversion may incure a  overhad and it might not. It does depend on how the types are stored and used. If for example a short in is tored in a 32 bit register, convirting it to a int is free and thus you have no overhead. In our case we do convert from short int to float which does have, in most cases, require some operations. However in comparison with the square root operation the ovehead of convirting from short int to float is negletable.

We are storing our distances as int's and when outputing we are just deviding by 100 to get the integer part and taking the modulus 100 to get the fractional part. However we are not compleatly sure if this is more eficient then to avoid the opreators done the integers or the float convirsion operation.

## Parsing the file
Strings are encoded using the char data type as a building bloch for differnet characters. Stringing multiple chars after one another can thus produce a strin. Looking at these building blocks that we said was of type char, that is 8 bits, we see that these are just binary numbers as anything other data type. So to get characters out on the screen we need to use a lookup table that asigns each charagter a numeric representation. Sins we have eight bits we can asign 256 characters with a uniqe numeric value. When we in C use the function printf() to print a variable to the consol we also need to spesify how printf should interpret our variable, for integers we use %d or %i but for strings we use %s. What %s meens is that we want printf to start printing whats at the pointer and continue to print characters untill it comes to the stop character '\o'.

In the Ascii lookup table the digits are asigned number 48 - 56 and the signs + and - bot have number 43 and 45 respectively. Given the fixed format that we are given in the textfile we could read line by line. Each line would be split up in the five characters read for each coordinate. The first character we would subtract 48 and then chast as a short in and multiplied with 10000 and aded to our short int variable. The next chars would all go through the same process but be multiplied with 1000, 100, 10 and 1 respectively. lastly we need to flip the last bit in our short int variable if the first char was -. 

## Memory management
