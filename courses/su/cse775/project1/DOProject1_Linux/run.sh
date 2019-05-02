#!/bin/bash

# Functions

title() {
    echo "##########################################################"
    echo "#                                                        #"
    echo "#   CSE 775 Distributed Object Project 1 Demonstration   #"
    echo "#                                                        #"
    echo "#         Author: Yang He, Date: Apr. 6th, 2019          #"
    echo "#                                                        #"
    echo "##########################################################"
}

requirementtitle() {
    echo
    echo "##########################################################"
    echo "#               Requirement ${1} demonstration              #"
    echo "##########################################################"
}

# Variables

executable=$(dirname $0)/content-search
test_parameter1=''

# Shell starts

title

echo
echo "# Enter project directory, clean and rebuild the project:"
echo
cd $(dirname $0) && make clean && make

if [ ! -f "${executable}" ]
then
    echo
    echo "Error: "
	  echo "Build failed." 
	  echo "${executable} not found."
    echo "Exit with code 1."
	  exit 1
fi

# Requirement 1

requirementtitle 1
echo "# Demonstrate development environment."
echo
echo "Complier is using the options \"CXXFLAGS := -Wall -std=c++11 -g\" to build the project." \
"That means the project is implemented in C++11." \
"And the build system will use the default C++ complier on the machine to build the project."
echo

# Requirement 2

requirementtitle 2
echo "# Demonstrate basic functionalities."
echo
echo "I named the executable program as \"content-search\". "
echo

echo "# Case 1: Running the program with single file pattern."
echo
echo "Run the program to search if there is any file under \"include\" directory" \
     "containing the search string \"IText\" or \"IFile\"."
echo

echo "## Now run the command: "
set -x
./content-search -p "./include" -f "*.*" -r "IText|IFile"
{ set +x; } 2>/dev/null
echo

echo "## Double check the result with grep command: "
set -x
grep -E "IText|IFile" ./include/*
{ set +x; } 2>/dev/null
echo

echo "## Here we can get same result with running grep and my content search program" \
     "when we search text files, which means my program can get correct results."

echo
echo "# Case 2: Running the program with multiple file patterns."
echo
echo "Run the program to search text meeting pattern \"\\s+ITextSearch|\\s+IFileManager\"" \
     "in the files which are located in the current path and have name ended with \".cc\" or \".h\""
echo

echo "## Now run the command:"
set -x
./content-search -p "." -f "*.cc" -f "*.h" -r "\s+ITextSearch|\s+IFileManager"
{ set +x; } 2>/dev/null
echo

echo "## Double check the result with find and grep command: "
set -x
find . -name "*.cc" -o -name "*.h" | xargs grep -E "\s+ITextSearch|\s+IFileManager"
{ set +x; } 2>/dev/null
echo

echo "## Here we can get same result with running grep and my content search program" \
     "when we search text files, which means my program can get correct results."
echo

# Requirement 3

requirementtitle 3
echo "# Demonstrate multiple file types support."
echo
echo "Run the program to search all the \"*.cc\", \"*.h\", \".cs\", and \".txt\" files" \
     "and check if they contain strings matching pattern \"^#include <[v0-9].*>\"";

echo "## Now run the command:"
set -x
./content-search -p "." -f "*.cc" -f "*.h" -f "*.cs" -f "*.txt" -r "^#include <[v0-9].*>"
{ set +x; } 2>/dev/null
echo

echo "## Double check the result with grep command: "
set -x
grep -ER "^#include <[v0-9].*>" .
{ set +x; } 2>/dev/null
echo

echo "## Here we can get same result with running grep and my content search program" \
     "when we search text files, which means my program can get correct results."
echo

# Requirement 4

requirementtitle 4
echo "# Demonstrate text search component. "
echo
echo "To demonstrate requirement 4, I want to run my program on the file" \
     "\"./content-search.cc\" and file \"./test/test_l2/test_l3/test_l3.cc\"" \
     "to examine if they contain strings matching the pattern \"\d+\"." \
     "If the file does contain this string, then the matched line" \
     "will be printed on the screen." \
     "If not, then the program will just print \"No result matched.\""
echo

echo "## Now run the program on the file \"./content-search.cc\":"
set -x
./content-search -p "." -f "content-search.cc" -r "\d+"
{ set +x; } 2>/dev/null
echo

echo "## Then run the program on the file \"./test/test_l2/test_l3/test_l3.cc\""
set -x
./content-search -p "./test/test_l2/test_l3" -f "*.cc" -r "\d+"
{ set +x; } 2>/dev/null
echo

# Requirement 5

requirementtitle 5
echo "# Demonstrate file manager component. "
echo
echo "To demonstrate requirement 5, I will run my program to list all the" \
     "\"*.cc\" files and \"*.h\" files residing in the directory rooted at" \
     "my project path."
echo

echo "## Now run the command:"
project_path=$(dirname $(realpath $0))
set -x
./content-search -p ${project_path} -f "*.cc" -f "*.h" -r "ZZZZZZZ"
{ set +x; } 2>/dev/null
echo

echo "Results above show that my program can list all these files as we set." \
     "The file names are sent by the fileManager to the textSearch." \
     "And textSearch module can perform content search on these files." \
     "Hence, the program meets requirement 5."
echo

# Requirement 6

requirementtitle 6
echo "# Final demonstration. "
echo
echo "## Now run the following command:"
set -x
./content-search -p "." -f "*.cc" -f "*.h" -f "*.cs" -f "*.txt" -r "public[^\:]+"
{ set +x; } 2>/dev/null
echo

echo "Results above show that the program is able to acquire files whose name matches" \
     "file patterns. Then the program searches these files and list all the lines" \
     "matching the content regular expression. Note that the content regular expression" \
     "in this command means to locate all the string \"public\" not followed by a colon." \
     "After the text search module in the program has finished searching, it will return" \
     "the result to the client and the client prints all the matched lines along with" \
     "line numbers on the screen."
echo

# Termination

echo "##########################################################"
echo "#                 Demonstration Finished                 #"
echo "##########################################################"
