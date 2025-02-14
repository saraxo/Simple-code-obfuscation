# Simple-code-obfuscation

This repository contains a code obfuscation tool developed for the "Analysis and Synthesis of Algorithms" course. The main goal is to randomly shuffle lines of code that meet certain conditions while ensuring that the program's output remains unchanged.

The tool analyzes the given code to identify the dependencies between various operations. By applying topological sorting, it rearranges the operations in a way that respects these dependencies, allowing for a randomized order without altering the final output.

I used topological sorting to maintain the correct order of operations based on their dependencies, ensuring the output stays consistent even after shuffling. The input file is very straightforward and consists of only two types of operations: assignment and addition. In input file you can see for loops also. The professor specified that nested for loops would never occur, so the implementation was built with this assumption in mind. However, the code can be extended to detect and handle nested loops if needed. A constraint was set to ensure that variables declared inside a loop do not appear outside of it in the output, and variables that are not part of the loop do not enter it. In other words, each loop independently handles the shuffling of its operations.


