# Functions

function RequirementTitle
{
    Write-Host ""
    Write-Host "##########################################################"
    Write-Host "#               Requirement $args demonstration              #"
    Write-Host "##########################################################"
}

# Variables

$projectPath = $PSScriptRoot
$fileSystemPath = "${projectPath}\FileSystem"
$fileManagerPath = "${projectPath}\FileManager"
$textSearchPath = "${projectPath}\TextSearch"

# Title

Write-Host "##########################################################"
Write-Host "#                                                        #"
Write-Host "#   CSE 775 Distributed Object Project 1 Demonstration   #"
Write-Host "#                                                        #"
Write-Host "#         Author: Yang He, Date: Apr. 6th, 2019          #"
Write-Host "#                                                        #"
Write-Host "##########################################################"

# Build

Write-Host "`n ## Enter project directory then clean and rebuild the project `n"

$env:VCTargetsPath="C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\V140"
if (Get-Command msbuild -errorAction SilentlyContinue) {
  cd "${projectPath}"
  msbuild "${projectPath}\DOProject1.sln" /t:Clean
  #msbuild "${fileSystemPath}\FileSystem.sln" /t:Rebuild
  msbuild "${fileManagerPath}\FileManager.vcxproj" /t:Rebuild
  msbuild "${textSearchPath}\TextSearch.vcxproj" /t:Rebuild
  msbuild "${projectPath}\DOProject1.sln" /t:Rebuild
} else {
  cd "${projectPath}"
  $msbuild = $env:systemroot + "\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe";
  &$msbuild "${projectPath}\DOProject1.sln" /t:Clean
  #&$msbuild "${fileSystemPath}\FileSystem.sln" /t:Rebuild
  &$msbuild "${fileManagerPath}\FileManager.vcxproj" /t:Rebuild
  &$msbuild "${textSearchPath}\TextSearch.vcxproj" /t:Rebuild
  &$msbuild "${projectPath}\DOProject1.sln" /t:Rebuild
}

# Demonstration

# Requirement 1

RequirementTitle 1
Write-Host "# Demonstrate development environment. "
Write-Host "`nThis program is developed under C++ 11 standard. `n"

# Requirement 2

RequirementTitle 2
Write-Host "# Demonstrate basic functionalities. "
Write-Host "`nI named the executable file as 'ContentFinder'. `n"

Write-Host "# Case 1: Running the program with single file pattern. `n"
Write-Host "Run the program to acquire all the header files 
under TextSearch directory containing the search string 'IText' or 'IFile'. `n" 

Write-Host "## Now run the command: "
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p ./TextSearch /f *.h /r "IText|IFile"
Set-PSDebug -Trace 0

Write-Host "# Case 2: Running the program with multiple file patterns. `n"
Write-Host "Run the program to acquire all the '.h', '.c', and '.cpp' files 
under TextSearch directory containing the search string 'IText' or 'IFile'. `n" 

Write-Host "## Now run the command: "
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p ./TextSearch /f *.h *.c *.cpp /r "IText|IFile"
Set-PSDebug -Trace 0

Write-Host "# Requirement 2 demonstration finished.`n"

# Requirement 3

RequirementTitle 3
Write-Host "# Demonstrate multiple file types support. "
Write-Host "`nRun the program to search all the '*.c', '*.h', '*.cpp', '.cs', and '.txt' files 
and check if they contain strings matching pattern '^#include <[aiu0-9].*>'";

Write-Host "## Now run the command:"
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p . /f *.c *.h *.cpp *.cs *.txt /r "^#include <[aiu0-9].*>"
Set-PSDebug -Trace 0

Write-Host "# Requirement 3 demonstration finished.`n"

# Requirement 4

RequirementTitle 4
Write-Host "# Demonstrate text search component. "
Write-Host "`nTo demonstrate requirement 4, I want to run my program on the file '.\ContentFinder.cpp' 
and file '.\test\test_l2\test_l3\test_l3.cpp' to examine if they contain strings matching the pattern '\d+'.
If the file does contain this string, then the matched line will be printed on the screen.
If not, then the program will just print 'No result matched.' `n"

Write-Host "## Now run the program on the file '.\ContentFinder.cpp':"
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p . /f ContentFinder.cpp /r "\d+"
Set-PSDebug -Trace 0

Write-Host "## Then run the program on the file '.\test\test_l2\test_l3\test_l3.cpp':"
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p .\test\test_l2\test_l3 /f *.cpp /r "\d+"
Set-PSDebug -Trace 0
Write-Host

Write-Host "# Requirement 4 demonstration finished.`n"

# Requirement 5

RequirementTitle 5
Write-Host "# Demonstrate file manager component. "
Write-Host "`nTo demonstrate requirement 5, I will run my program to list all the
'*.h' files, '*.c' files and '*.cpp' files residing in the directory rooted at my project path."
Write-Host

Write-Host "## Now run the command:"
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p "${projectPath}" /f *.h *.c *.cpp /r "ZZZZZZZ"
Set-PSDebug -Trace 0

Write-Host "Results above show that my program can list all these files as we set.
The file names are sent by the fileManager to the textSearch.
And textSearch module can perform content search on these files.
Hence, the program meets requirement 5. `n"

Write-Host "# Requirement 5 demonstration finished.`n"

# Requirement 6
requirementtitle 6
Write-Host "# Final demonstration. "
Write-Host "`n## Now run the following command:"
Set-PSDebug -Trace 1
./Debug/ContentFinder.exe /p . /f *.h *.cpp *.cs *.txt /r "public[^\:]+"
Set-PSDebug -Trace 0

Write-Host "Results above show that the program is able to acquire files whose name matches file patterns. 
Then the program searches these files and list all the lines matching the content regular expression. 
Note that the content regular expression in this command means to locate all the string 'public' not followed by a colon.
After the text search module in the program has finished searching, it will return the result to the client 
and the client prints all the matched lines along with line numbers on the screen. `n"

# Termination

Write-Host "##########################################################"
Write-Host "#                 Demonstration Finished                 #"
Write-Host "##########################################################"

Set-PSDebug -Trace 0


