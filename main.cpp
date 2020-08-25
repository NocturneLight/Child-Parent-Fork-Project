#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <math.h>

// Create classes here.
class Memory
{
  public:

  // It will consist of 2000 integer entries.
  // 0-999 for the user program. 1000-1999 for system code.
  int memory[2000] = {0}; //Initializing each space to zero first to remove random large numbers from array.
  
  Memory(std::vector<std::string> set)
  {
    // Create variables exclusive to this function here.
    std::string tempString;
    int counter = 0;

    for (std::vector<std::string>::iterator i = set.begin(); i != set.end(); ++i)
    {
      // Store the line in a variable so we can modify it.
      tempString = *i;
      
      // If there's a comment, split the line on the start of the comments so we 
      // get a string with just the instruction.
      tempString = tempString.substr(0, tempString.find("//"));

      // Check if one of the instructions contains a period at the beginning.
      if (tempString.find('.') != std::string::npos)
      {
	// In the event of a period, replace it with a space.
	std::replace(tempString.begin(), tempString.end(), '.', ' ');

	// Then remove all spaces from the tempString variable.
	tempString.erase(remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());

	// We need to move to a specific index because there was
	// a period at the beginning of the line, so set the 
	// counter variable to tempString.
	counter = stoi(tempString);
      }
      else // Otherwise, carry on like normal.
      {
	// Remove any remaining whitespaces from the string.
	tempString.erase(remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());
	
	// If we ended up with a null line, move to the next
	// line. Otherwise, store the instruction in memory 
	// at index counter and the increment the counter
	// variable.
	if (tempString != "")
	{
	  memory[counter] = stoi(tempString);

	  counter++;
	}
      }
    }
  }

  // Our class destructor.
  ~Memory()
  {
  }

  // Returns the value at the address.
  int Read(int address)
  {
    return memory[address];
  }

  // Writes the data to the address.
  int Write(int address, int data)
  {
    memory[address] = data;
  }
};

class CPU
{
  public:
  // Define our registers here.
  int PC; // The Program Counter. Should be saved on the system stack.
  int SP; // The Stack Pointer. Should be saved on the system stack.
  int IR; // The Instruction Register.
  int AC; // The Accumulator Register.
  int X;
  int Y;
  int pTime; // A command line parameter. A timer interrupts the processor after every "X" instruction.
  bool kernel = false;
  Memory *program;

  // Upon initializing our object, we will instantiate our registers.
  CPU(int _PC, int _SP, int _IR, int _AC, int _X, int _Y, int _pTime, Memory *_program)
  {
    this->PC = _PC;
    this->SP = _SP;
    this->IR = _IR;
    this->AC = _AC;
    this->X = _X;
    this->Y = _Y;
    this->pTime = _pTime;
    this->program = _program;
  }

  // Our class destructor.
  ~CPU()
  {
  }


  // Create our supported CPU functions here.
  // Load the value into the AC.
  void LoadValue(int value)
  {
    //std::cout << "LoadValue Value: " << value << std::endl;
    this->AC = value;
  }

  // Load the value at the address into the AC.
  void LoadAddress(int address)
  {
    //std::cout << "LoadAddress Value: " << address << std::endl;
    try
    {
      if (address == 1000 || address == 1500)
	throw -1;
      else
        this->AC = program->Read(address);

    }
    catch (int exc)
    {
      std::cerr << "Error: User cannot access protected memory." << std::endl;
      exit(0);
    }
  }

  // Load the value from the address found in the given address into the AC.
  // (For example, if LoadInd 500, and 500 contains 100, then load from 100).
  void LoadInd(int address)
  {
    //std::cout << "LoadInd Value: " << address << std::endl;
    this->AC = program->Read(program->Read(address));
  }

  // Load the value at (address+X) into the AC.
  // (For example, if LoadIdxX 500, and X contains 10, then load from 510).
  void LoadIdxX(int address)
  {
    //std::cout << "LoadIdxX Value: " << address << std::endl;
    this->AC = program->Read(address + this->X);
  }

  // Load the value at (address+Y) into the AC.
  void LoadIdxY(int address)
  {
    //std::cout << "LoadIdxY Value: " << address << std::endl;
    this->AC = program->Read(address + this->Y);
  }

  // Load from (Sp + X) into the AC (if SP is 990, and X is 1, load from 991).
  void LoadSpX()
  {
    //std::cout << "LoadSpX Values before: " << this->SP << "and" << this->X << std::endl;
    this->AC = program->Read(this->SP + this->X + 1);
    //std::cout << "LoadSpX value now stored in AC." << this->AC << std::endl;
  }

  // Store the value in the AC into the address.
  void StoreAddress(int address)
  {
    //std::cout << "StoreAddress Values: " << this->AC << "is being stored in memory location  " << address << std::endl;
    program->Write(address, this->AC);
  }

  // Gets a random integer from 1 to 100 into the AC.
  void Get()
  {
    srand(time(NULL) % (rand() % 100) + 1);
    this->AC = (rand() % 100) + 1;
    //std::cout << "Get Value: The random number stored in AC is " << this->AC << "."  << std::endl;
  }
  
  // If port = 1, writes AC as an int to the screen.
  // If port = 2, writes AC as a char to the screen.
  void PutPort(int port)
  {
    //std::cout << "PutPort Values: " << this->AC << "is going to port " << port << std::endl;
    if (port == 1)
    {
      std::cout << (int)this->AC;
    }
    else if (port == 2)
    {
      std::cout << (char)this->AC;
    }
  }

  // Add the value in X to the AC.
  void AddX()
  {
    //std::cout << "AddX Values: We are getting the sum of AC and X, specifically, " << this->AC << this->X << std::endl;
    this->AC += this->X;
  }

  // Add the value in Y to the AC.
  void AddY()
  {
    //std::cout << "AddY Values: We are getting the sum of AC and Y, specifically, " << this->AC << this->Y << std::endl;
    this->AC += this->Y;
  }

  // Subtract the value in X from the AC.
  void SubX()
  {
    //std::cout << "SubX Values: We are getting the difference between AC and X, specifically, " << this->AC << this->X << std::endl;
    this->AC -= this->X; 
  }

  // Subtract the value in Y from the AC.
  void SubY()
  {
    //std::cout << "SubY Values: We are getting the difference between AC and Y, specifically, " << this->AC << this->Y << std::endl;
    this->AC -= this->Y;
  }

  // Copy the value in the AC to X.
  void CopyToX()
  {
    //std::cout << "CopyToX Values: We are copying AC to X, specifically, " << this->AC << " to " << this->X << std::endl;
    this->X = this->AC;
  }

  // Copy the value in X to the AC.
  void CopyFromX()
  {
    //std::cout << "CopyFromX Values: We are copying X to AC, specifically, " << this->X << " to " << this->AC << std::endl;
    this->AC = this->X;
  }

  // Copy the value in the AC to Y.
  void CopyToY()
  {
    //std::cout << "CopyToY Values: We are copying AC to Y, specifically, " << this->AC << " to " << this->Y << std::endl;
    this->Y = this->AC;
  }

  // Copy the value in Y to the AC.
  void CopyFromY()
  {
    //std::cout << "CopyFromY Values: We are copying Y to AC, specifically, " << this->Y << " to " << this->AC << std::endl;
    this->AC = this->Y;
  }

  // Copy the value in AC to the SP.
  void CopyToSp()
  {
    //std::cout << "CopyToSp Values: We are copying AC to SP, specifically, " << this->AC << " to " << this->SP << std::endl;
    this->SP = this->AC;
  }

  // Copy the value in SP to the AC.
  void CopyFromSp()
  {
    //std::cout << "CopyFromSp Values: We are copying SP to AC, specifically, " << this->SP << " to " << this->AC << std::endl;
    this->AC = this->SP;
  }

  // Jump to the address.
  void Jump(int address)
  {
    //std::cout << "Jump Value: We are jumping to address " << address << std::endl;
    this->PC = address;
  }

  // Jump to the address only if the value in the AC is zero.
  void JumpIfEqual(int address)
  {
    //std::cout << "JumpIfEqual Value: We are jumping to address " << address << " if AC is 0. AC is " << this->AC << std::endl;
    if (this->AC == 0)
    {
      this->PC = address;
    }
    else
    {
      this->PC++;
    }
  }

  // Jump to the address only if the value in the AC is not zero.
  void JumpIfNotEqual(int address)
  {
    //std::cout << "JumpIfNotEqual Value: We are jumping to address " << address << " if AC is not 0. AC is " << this->AC << std::endl;
    if (this->AC != 0)
    {
      this->PC = address;
    }
    else
    {
      this->PC++;
    }
  }

  // Push return address onto stack, jump to the address.
  void CallAddress(int address)
  {
    //std::cout << "CallAddress Values: Address we're jumping to is " << address << ". Return Address is " << this->PC << ". SP is currently " << this->SP << " and will be decremented." << std::endl;
    try
    {
      if (!kernel)
      {
	program->memory[this->SP] = this->PC;
    
	this->PC = address;
      
	this->SP--;
      
	if (this->SP - 200 < 0)
	{
	  throw -1;
	}
      }
      else if (kernel)
      {
	program->memory[this->SP] = this->PC;

        this->PC = address;

        this->SP--;

        if (this->SP < 0)
	{
	  throw -1;
	}
      }
    }
    catch (int exc)
    {
      std::cerr << "Error: You have run out of memory." << std::endl;
      std::exit(0);
    }
  }

  // Pop return address from the stack, jump to the address.
  void Return()
  {
    //std::cout << "Return Values: Address we're jumping back to is " << program->memory[this->SP + 1] << ". SP is currently " << this->SP << " and will be incremented." << std::endl;
    try
    {
      if (!kernel)
      {
	this->SP++;

	if (this->SP > 999)
	{
	  throw -2;
	}
      
	this->PC = program->memory[this->SP];
      }
      else if (kernel)
      {
	this->SP++;

        if (this->SP > 1999)
	{
	  throw -3;
	}

        this->PC = program->memory[this->SP];
      }
    }
    catch (int exc)
    {
      if (exc == -2)
      {
	std::cerr << "Error: User is not allowed to access system memory." << std::endl;
	std::exit(0);
      }
      else if (exc == -3)
      {
	std::cerr << "Error: You have run out of memory." << std::endl;
	std::exit(0);
      }
    }
  }

  // Increment the value in X.
  void IncrementX()
  {
    //std::cout << "IncrementX Value: X is " << this->X << " and will be incremented." << std::endl;
    this->X++;
  }

  // Decrement the value in X.
  void DecrementX()
  {
    //std::cout << "DecrementX Value: X is " << this->X << " and will be decremented." << std::endl;
    this->X--;
  }

  // Push AC onto stack.
  void Push()
  {
    //std::cout << "Push Values: AC is being pushed to stack. Specifically, " << this->AC << " is going to " << this->SP << " and then SP will be decremented." << std::endl;
    try
    {
      if (!kernel)
      {
	program->memory[this->SP] = this->AC;

	this->SP--;

	if (this->SP < 0)
	{
	  throw -1;
	}
      }
      else if (kernel)
      {
	program->memory[this->SP] = this->AC;

	this->SP--;

	if (this->SP < 0)
	{
	  throw -1;
	}
      }
    }
    catch (int exc)
    {
      std::cerr << "Error: You have run out of memory." << std::endl;
      std::exit(0);
    }
  }

  // Pop from stack into AC.
  void Pop()
  {
    //std::cout << "Pop Values: Something is being popped from stack to AC. Specifically, " << program->memory[this->SP + 1] << " is going into back into AC. The current SP is " << this->SP << " but still needs to be decremented." << std::endl;
    try
    {
      if (!kernel)
      {
	this->SP++;

	if (this->SP > 999)
	{
	  throw -2;
	}

	this->AC = program->memory[this->SP];
      }
      else if (kernel)
      {
	this->SP++;

        if (this->SP > 1999)
	{
	  throw -3;
	}

        this->AC = program->memory[this->SP];
      }
    }
    catch (int exc)
    {
      if (exc == -2)
      {
	std::cerr << "Error: User is not allowed to access system memory." << std::endl;
      }
      else if (exc == -3)
      {
	std::cerr << "Error: You have run out of memory." << std::endl;
      }

      std::exit(0);
    }
  }

  // Perform system call.
  void Int()
  {
    
    //std::cout << "Int Values: " << this->SP << " and " << this->PC << " are getting written to memory spaces 1999 and 1998 respectively and then become 1997 and 1500 respectively." << std::endl;
    this->kernel = true;
    program->Write(1999, this->SP);
    program->Write(1998, this->PC);
    this->SP = 1997;
    this->PC = 1500;
  }

  // Return from system call.
  void IRet()
  {
    //std::cout << "IRet values: SP and PC are currently " << this->SP << " and " << this->PC <<  " respectively. But will change back to " << program->Read(1999) << " and " << program->Read(1998) << "." << std::endl;
    this->kernel = false;
    this->SP = program->Read(1999);
    this->PC = program->Read(1998);
  }

  // End execution.
  void End()
  {
    //std::cout << "Exiting the program now that we're done." << std::endl;
    std::exit(0);
  }


  // Function to run the user's instructions.
  int RunProgram()
  {
    // Create variables exclusive to this class function here.
    bool instruction = false;
    int originalTime = this->pTime;

    while ((this->PC <= 999 && !kernel) || (this->PC <= 1999 && kernel))
    {
      // Skip to the next iteration if the instruction before the current was one of the ones in the if statement.
      // Because then you're not an instruction.
      if ((program->Read(this->PC - 1) == 1 || program->Read(this->PC - 1) == 2 || program->Read(this->PC - 1) == 3 || program->Read(this->PC - 1) == 4 || program->Read(this->PC - 1) == 5 ||program->Read(this->PC - 1) == 7 ||  program->Read(this->PC - 1) == 9 || program->Read(this->PC - 1) == 20 || program->Read(this->PC - 1) == 21 ||program->Read(this->PC - 1) == 22 || program->Read(this->PC - 1) == 23) && instruction)
      {
	instruction = false;
	this->PC++;
	continue;
      }

      // Get an instruction from memory and store it in the IR register.
      this->IR = program->Read(this->PC);
	
      //std::cout << "Address: " << PC << " | Instruction: " << IR << std::endl; 
      //std::cout << "Loop: " << this->PC; 

      // Execute whichever function corresponds to the number stored in the IR register.
      if (IR == 1)
      {
        instruction = true;
        //std::cout << "Load Value called." << std::endl;
        LoadValue(program->Read(PC + 1));
      }
      else if (IR == 2)
      {
        instruction = true;
	//std::cout << "LoadAddress called." << std::endl;
        LoadAddress(program->Read(PC + 1));
      }
      else if (IR == 3)
      {
        instruction = true;
        //std::cout << "LoadInd called." << std::endl;
        LoadInd(program->Read(PC + 1));
      }
      else if (IR == 4)
      {
        instruction = true;
        //std::cout << "LoadIdxX called." << std::endl;
        LoadIdxX(program->Read(PC + 1));
      }
      else if (IR == 5)
      {
        instruction = true;
        //std::cout << "LoadIdxY called." << std::endl;
        LoadIdxY(program->Read(PC + 1));
      }
      else if (IR == 6)
      {
        //std::cout << "LoadSpX called." << std::endl;
        LoadSpX();
      }
      else if (IR == 7)
      {
        instruction = true;
        //std::cout << "StoreAddress called." << std::endl;
        StoreAddress(program->Read(PC + 1));
      }
      else if (IR == 8)
      {
        //std::cout << "Get called." << std::endl;
        Get();
      }
      else if (IR == 9)
      {
        instruction = true;
        //std::cout << "Put Port called." << std::endl;
        PutPort(program->Read(this->PC + 1));
      }
      else if (IR == 10)
      {
        //std::cout << "AddX called." << std::endl;
        AddX();
      }
      else if (IR == 11)
      {
        //std::cout << "AddY called." << std::endl;
        AddY();
      }
      else if (IR == 12)
      {
        //std::cout << "SubX called." << std::endl;
        SubX();
      }
      else if (IR == 13)
      {
        //std::cout << "SubY called." << std::endl;
        SubY();
      }
      else if (IR == 14)
      {
        //std::cout << "CopyToX called." << std::endl;
	CopyToX();
      }
      else if (IR == 15)
      {
        //std::cout << "CopyFromX called." << std::endl;
        CopyFromX();
      }
      else if (IR == 16)
      {
        //std::cout << "CopyToY called." << std::endl;
        CopyToY();
      }
      else if (IR == 17)
      {
        //std::cout << "CopyFromY called." << std::endl;
        CopyFromY();
      }
      else if (IR == 18)
      {
        //std::cout << "CopyToSp called." << std::endl;
        CopyToSp();
      }
      else if (IR == 19)
      {
        //std::cout << "CopyFromSp called." << std::endl;
        CopyFromSp();
      }
      else if (IR == 20)
      {
        instruction = true;
        //std::cout << "Jump called." << std::endl; 
        Jump(program->Read(PC + 1));
        continue;
      }
      else if (IR == 21)
      {
	instruction = true;
        //std::cout << "JumpIfEqual called." << std::endl;
	JumpIfEqual(program->Read(PC + 1));
        continue;
      }
      else if (IR == 22)
      {
        instruction = true;
        //std::cout << "JumpIfNotEqual called." << std::endl;
        JumpIfNotEqual(program->Read(PC + 1));
        continue;
      }
      else if (IR == 23)
      {
        instruction = true;
        //std::cout << "CallAddress called." << std::endl;
        CallAddress(program->Read(this->PC + 1));
        continue;
      }
      else if (IR == 24)
      {
        //std::cout << "Return called." << std::endl;
        Return();
      }
      else if (IR == 25)
      {
        //std::cout << "IncrementX called." << std::endl; 
        IncrementX();
      }
      else if (IR == 26)
      {
        //std::cout << "DecrementX called." << std::endl;
        DecrementX();
      }
      else if (IR == 27)
      {
        //std::cout << "Push called." << std::endl;
        Push();
      }
      else if (IR == 28)
      {
        //std::cout << "Pop called." << std::endl;
        Pop();
      }
      else if (IR == 29 && !kernel)
      {
        //std::cout << "Int called." << std::endl;
        Int();
        continue;
      }
      else if (IR == 30 && kernel)
      {
        //std::cout << "IRet called." << std::endl;
        IRet();
      }
      else if (IR == 50)
      { 
        //std::cout << "End called." << std::endl;
        End();
      }
	
      // Enter kernel mode if the time argument reaches 0 and
      // execute the instruction stored on address 1000 as well
      // as store SP and PC on the system stack.
      if (pTime == 0 && !kernel)
      {
        kernel = true;
        program->Write(1999, this->SP);
        program->Write(1998, this->PC);
        this->SP = 1997;
        this->PC = 1000;
        this->pTime = originalTime;
        continue;
      }
	
      // If we've reached the end, increment the PC register.
      this->PC++;

      // Decrement the time argument if we're not in kernel mode.
      if (!kernel)
      {
	this->pTime--;
      }
    }
  }
};

// Function prototypes go here.
std::vector<std::string> getFileContents(int _argc, char **_argv);

main(int argc, char **argv)
{
  // Create variables here.
  Memory* mem;
  CPU* cpu;
  pid_t pipeID;
  pid_t pipeID2;
  int pipeFD[2];
  int pipeFD2[2];

  // First, we check to make sure the pipe was successfully made.
  if (pipe(pipeFD))
  {
    std::cerr << "Failed to create the Memory pipe." << std::endl;
    return -1;
  }

  // Next, we execute a fork and store it in pipeID.
  pipeID = fork();

  // Then, we check pipeID's number. If it's negative, the fork failed.
  if (pipeID < 0)
  {
    std::cerr << "The fork has failed." << std::endl;
    return -1;
  }
  // If pipeID is positive, that's our parent process.
  else if (pipeID > 0)
  {
    // We can close the read and write end of the pipe. We won't
    // need the parent.
    close(pipeFD[1]);
    close(pipeFD[0]);
  }
  // If pipeID is exactly 0, that's our child process.
  else if (pipeID == 0)
  {
    // First, we close the read and write end of the pipe since we won't need it in
    // the child process.
    close(pipeFD[0]);
    close(pipeFD[1]);

    // Next, we check our arguments. If argc is 1 or less, then
    // we don't have enough arguments. Inform the user and then exit.
    if (argc <= 1)
    {
      std::cout << "Too few arguments provided." << std::endl;
      return 0;
    }
    // If argc is 4 or more, then we have too many arguments. Inform
    // the user and then exit the program. 
    else if (argc >= 4)
    {
      std::cout << "Too many arguments provided." << std::endl;
      return 0;
    }
    // If argc is exactly 3, then we have the right amount of arguments.
    // Parse the 3rd argument and make sure it's a valid number. If it's
    // invalid, inform the user and exit the program.
    else if (argc == 3)
    {
      std::string tempString = argv[2];
    
      if (!std::all_of(tempString.begin(), tempString.end(), ::isdigit))
      {
	std::cout << "An invalid number was provided. Check that you only used numbers and no letters." << std::endl;
	return 0;
      }
    }

    // After everything is valid and working, parse the user's program and give it to memory.
    mem = new Memory(getFileContents(argc, argv));

    // Now we create another parent and child process.
    // Again, we check to make sure the pipe was successfully made.
    if (pipe(pipeFD2))
    {
      std::cerr << "Failed to create the CPU pipe." << std::endl;
      return -1;
    }

    // Next, we execute a fork and store it in pipeID2.
    pipeID2 = fork();

    // Then, we check pipeID2's number. If it's negative, the fork failed.
    if (pipeID2 < 0)
    {
      std::cerr << "The fork has failed." << std::endl;
      return -1;
    }
    // If it's positive, it's the parent.
    else if (pipeID2 > 0)
    {
      // We can close these read and write end of these pipes. We won't need the parent.
      close(pipeFD2[1]);
      close(pipeFD2[0]);
    }
    // If it's 0, it's the child.
    else if (pipeID2 == 0)
    {
      // We can close these read and write end of these pipes. We won't need to read or write to the parent.
      close(pipeFD[1]);
      close(pipeFD[0]);

      // Now we need to initialize the CPU and hand it memory.
      cpu = new CPU(0, 999, 0, 0, 0, 0, std::stoi(argv[2]), mem);

      // Give a newline to offset UNIX command line quirks caused by pipes and forks.
      std::cout << std::endl;

      // Then we run the user's program.
      cpu->RunProgram();
    }
  }
}


// Actual functions go here.
std::vector<std::string> getFileContents(int _argc, char **_argv)
{
  // Variables needed to access file.
  std::string buffer;
  std::ifstream file(_argv[1]);
  std::vector<std::string> fileContent;
    
  // Read each line of the file and store
  // it in the fileContent vector.
  while (getline(file, buffer))
  {
    fileContent.push_back(buffer);
  }

  // Close the file now that we're finished here.
  file.close();

  // Return the vector so that we can store it in a variable.
  return fileContent;
}
