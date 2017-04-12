# Top K Frequent Words
### Problem Description
The question asks to write a program to read a text file and count the top K frequent words while reading. 
1. The program reads from the console (a) the name of the file (for instance example.txt), and (b) an integer K.
1. It then reads that file. While the program reads the file (not after the file has been read), it needs to also count the occurrence of each word. Note that for instance cat and cats are separate words. Also, the special characters **, . ! ? ;** are all separate words. Therefore the sentence **“I have two cats.”** has the 5 words **{“I”, “have”, “two”, “cats”, “.”}**.
1. At the end, the program needs to output the exact top K frequent words, where K is the parameter read in Step 1.

### Data Structures and Algorithm
1. Use a trie to store the words and their counts: when the program reads a word, if the word exists in the trie, update its count; otherwise, add the new word to the trie and set the count to 1
1. Use a min-heap to store the top-K frequent words: after the count of a word in trie has been updated, if the word is already in the heap, update its location in heap; otherwise, if the heap is not full, add the word to the heap and adjust its location in the heap; otherwise, if its count is larger than the heap root (the word with the smallest word count in the heap), replace the root by the current word and adjust its location.

### Note
The source codes of this problem is based on the implementation at http://www.geeksforgeeks.org/find-the-k-most-frequent-words-from-a-file/
