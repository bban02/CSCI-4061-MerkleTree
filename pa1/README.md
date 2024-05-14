Group 57  
Brady Bannick, Alex Ashworth  
banni041, ashwo043  

CSElabs computer: kh1262-13
added 2 lines to merkle that convert the arguments, so that we could test utils.c  


planned contributions:  
Alex: implement 3.2.1 (leaf processes)

Brady: handle non leaf processes, start the recursion

implementation plan:  

We will start off in child_process and create an if else statement to determine if the node
is a leaf node. If the id of the node is greater than or equal to N - 1 then it is a leaf node
else it is a parent node. If it is a leaf node we will convert the id into a valid block file number by
subtracting id - N and adding 1 or by just subtracting by N - 1. Then we will hash the block
file with that number. We will then use snprintf to write the hash to the block files corresponding
hash file. We will then make sure to exit the program. 

Now for the parent nodes. If it is a parent node it will create 2 child nodes by using fork. 
These will not be a chain they will be a tree. Then we calculated the 2 child ids. Next using exec we 
have both child process run child_process. We then use wait(NULL) in a while loop to have the parent wait for both of its children. 
Next we use open the parent hash file. To do this we create the name of the file using snprintf and then open. Next we open both
child process's hash files and read their data into a char array. Finally we create a result char array to use in the compute_dual_hash
function. Using this and the 2 children's char arrays we get result. Then we write result to the parents hash file. Lastly we close
all files.