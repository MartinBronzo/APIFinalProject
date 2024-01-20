# API Final Project - Text Editor Readme

## Project Overview

This command-line text editor was developed as part of a university project for the course "Algoritmi e Principi dell'Informatica" (Algorithms and Principles of Informatics) at Politecnico di Milano during the academic year 2019/2020. The primary objective of the project was to design a text editor using a data structure that respects specific memory and performance constraints. The programming language used for this project is C.

## Project Scope
- **Course**: Algoritmi e Principi dell'Informatica
- **University**: Politecnico di Milano
- **Academic Year**: 2019/2020

## Features

The text editor supports the following instructions:

- **(ind1, ind2)c**: Changes the text between lines ind1 and ind2 (inclusive). The input following the command must consist of a number of lines equal to ind2 - ind1 + 1. Ind1 must be either an address already present in the text or the first address after the last line in the text (or 1 if the text is still empty).

- **(ind1, ind2)d**: Deletes the lines between ind1 and ind2 (inclusive), moving up the lines following the one with address ind2 (if any). Deleting a line that does not exist in the text has no effect.

- **(ind1, ind2)p**: Prints the lines between ind1 and ind2, inclusive. If a line is not present in the text at the specified position, a line containing only the character '.' followed by a newline is printed.

- **(number)u**: Undoes a number of commands (c or d) specified within parentheses. A sequence of consecutive undo commands cancels a number of steps equal to the sum of the steps specified in each of them. If the number of commands to undo exceeds the number of executed commands, all steps are undone. The execution of a text modification command (c, d) after an undo command permanently cancels the effects of the undone commands. Note that the count includes commands that have no effect (e.g., deleting a block of lines that do not exist).

- **(number)r**: Undoes the effect of undo for a number of commands equal to the specified number from the current version (redo function). The number must be an integer strictly greater than zero. Therefore, a sequence of commands like 10u 5r is equivalent to the single command 5u. Similarly, the sequence 12u 3r 2u 7r is equivalent to the command 4u. If the number of commands to redo is greater than the currently undone commands, the maximum possible number of redo operations is performed.

- **q**: Terminates the execution of the editor.

## Code Structure

The C code is organized into functions and data structures to handle various editor operations. Key functions include `change`, `delete`, `print`, `undo`, `redo`, and others, each responsible for specific tasks. The code also includes a data structure (`progression_list_t`) to manage the progression of operations and support undo and redo functionalities.

## How to Use

1. Compile the code using a C compiler.
2. Run the compiled executable in the command line.
3. Enter editor commands based on the specified format and functionality.

## Important Note

Make sure to adhere to the specified command formats and guidelines to ensure the correct functioning of the text editor. Text samples are visible in the samples folder.

## Author
Martin Bronzo
