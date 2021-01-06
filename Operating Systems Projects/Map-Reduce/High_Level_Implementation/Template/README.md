## Header
```
test machine: csel-vole-36 
date: October 4, 2020
name: Ryan Mower, Zach Siew, Isaac Lee
x500: mower023, leex8934, siew0053
```


## Purpose of our program
The purpose of our program is to track the different words along with how many times they appear in a text file. A common approach to this problem would be
to track the words and update their account accordingly. However, this becomes quite inefficent with large text files. To solve this issue, our program's
algorithm utilizes parallel programming by splitting up the text file into chunks and sending out the data to mutiple 'mappers'. These mappers track the words
along with their count for that given data. The mappers output is then combined into master outputs by new processes called reducers. These master outputs show 
all the different words along with their frequency. The main purpose to count up words in a parallel way is to improve effeciency. The traditional way
of lineary searching a file and incrementing word counts can become quite ineffecient when dealing with large files. Splitting up this work to multiple processes 
increases effiency, making this parallel algorithim much faster. 

## How to compile program
We did not modify the file structure of the provided template.
```
make clean
make
make t1
```

To run the program.
```
./mapreduce #mappers #reducers path/to/input_file
```

## What our program does
Our program calculates how many times certain words show up within a text file. It does this with a parallel algorithm that divides the text file into multiple
chunks of data. These chunks are then sent to seperate process called mappers. These mappers do all the work. They identify the different words along with 
how many times they show up. Mappers then output their data into files. These output files are assigned to new processes called reducers. These reducers 
read through the files and total up the word counts and later display them in a master file that contains that particular words word-count. 


## Any Assumptions
1. File paths won't be longer than 50 characters.
2. The number of mappers or reducers wont be larger than 999,999,999 (string for ID is allocated for 10 bytes)
3. Words wont be longer than 99 characters.

## x500s
```
mower023
leex8934
siew0053
```

## Contributions by each member
We all worked on most parts of the project togeher so we could all learn the different aspects of the project. 
```
Ryan:
	README.md
	mapreduce.c
	figure out how to test code
	mapper.c
	reducer.c
	comments
	error checking
Zach:
	mapreduce.c
	reducer.c
	comments
	error checking
	learned the final data structure and how to interact with it
Isaac:
	mapreduce.c
	mapper.c
	comments
	error handling
	learned intermediate data structure and how to interact with it
