# CMPUT379-Assign3

## Team:
Nikita Yevtukh 1431589

Oleg Krushenskyy 19415012515239543

## Work Break Down:
Nikita - Worked on the design document, did the report and the graphing with statistics of the tests from the TLB runs.

Oleg - Did the TLB simulator and worked on the design document.

## Tested On Lab Machines 

Use Makefile in order to compile.  
TO RUN:  
valgrind --tool=lackey --trace-mem=yes ./inputProgram 2>&1 | ./valtlb379 -i -f (flush period) -p (Policy) pagesize tablesize  
