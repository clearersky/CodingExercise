# Top K Frequent Words
### Problem Description
The question asks to write a program to read a text file and count the top K frequent words while reading. 
1. The program reads from the console (a) the name of the file (for instance example.txt), and (b) an integer K.
1. It then reads that file. While the program reads the file (not after the file has been read), it needs to also count the occurrence of each word. Note that for instance cat and cats are separate words. Also, the special characters **, . ! ? ;** are all separate words. Therefore the sentence **“I have two cats.”** has the 5 words **{“I”, “have”, “two”, “cats”, “.”}**.
1. At the end, the program needs to output the exact top K frequent words, where K is the parameter read in Step 1.
