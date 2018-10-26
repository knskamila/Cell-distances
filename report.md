# Assignment 3: OpenMP

## Computing the distance of two points
What kind of operations that are faster and or can be exicuted in paralell pipes depends 
on the architecture of your processor. However the general case is that integer operations have a higher troughput then floting point operations. Another thing to take in to acouint is the size of your data type as generaly smaller data types have a higher troughput then larger ones, notable exeptions to this are graphics processing units who have a troughput of orders of magnitudes higer for single precission (32 bit) then half precission (16 bit).

In our case we convert the data that we reed in  to short int by multiplying the values with 1000 to not have any decimals and make a cast to short int. This means that the largest and smallest values we need to account for is 10000 and -10000 respectively. Thus we can not use any smaller data size then short int sins short short int (8 bit) only goes between -128 and 128. 

Type conversion may incure a  overhad and it might not. It does depend on how the types are stored and used. If for example a short in is tored in a 32 bit register, convirting it to a int is free and thus you have no overhead. In our case we do convert from short int to float which does have, in most cases, require some operations. However in comparison with the square root operation the ovehead of convirting from short int to float is negletable.

We are storing our distances as int's and when outputing we are just deviding by 100 to get the integer part and taking the modulus 100 to get the fractional part

## 
