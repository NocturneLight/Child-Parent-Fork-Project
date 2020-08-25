Files:
	main.cpp
		- This is the main file. It contains two objects: Memory, which houses the user's program, and CPU, which executes the program.
		- A pipe and fork is called for parsing command line arguments and handing the user's program to Memory. A second pipe and fork then is called to take that Memory and execute the user's program.
		- If you your third argument is not a valid number, an error will be thrown at you and you will have to start over.
		- If you input 0 for your third argument, the program will run but you will most likely see little to nothing happen on
		 screen.
		- When the program finishes executing, on the command line it will look like the program is expecting input from you. This is not the case. It's just a quirk in the programming caused by doing a fork and pipe inside of a child process. You can press enter and everything will look normal again, or you can run the program right then and there. The program has ended and you will be able to run it again regardless.
		HOW TO RUN:
		    To run the program, simply type in "main.cpp sample5.txt 500". If you do not have 3 arguments, an error will be thrown at you and you will have to start over.

	sample5.txt
		- My custom file for this project.
		- Due to time constraints, it is likely simpler than what you would've wanted.
		- The file prints out a name and then on the next line informs the user of how many
		 characters that name consists of. Note that this includes spaces and such things.
		- The file's output also changes depending on how large or small a time argument you
		 gave the program. The larger the time, the more of the sentence and name you see. The
		 less time, the less of the name and sentence you see.

	CompileAndExecute.sh
		- This is a shell file for compiling main.cpp and removing files with '~' in their name.
		- If for whatever reason you need to compile the program, just type "bash CompileAndExecute.sh".

	The other files
	    	- Ignore them. They're trash files I can't remove for one reason or another.
