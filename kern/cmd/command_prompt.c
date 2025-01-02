// Simple command-line kernel prompt useful for
// controlling the kernel and exploring the system interactively.



#include <kern/cmd/command_prompt.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/kdebug.h>
#include <kern/cons/console.h>
#include <kern/tests/tst_handler.h>
#include <kern/cpu/cpu.h>
#include <kern/cpu/sched.h>
#include "commands.h"

// ********** This DosKey supported readline function is implemented by **********
// ********** Abdullah Najuib ( FCIS T.A.), 3rd year student, FCIS, 2012

//#define CMD_NUMBER sizeof(comds)/sizeof(comds[0])

#define WHITESPACE "\t\r\n "
#define HISTORY_MAX 19
int last_command_idx = -1;
char command_history[HISTORY_MAX+1][BUFLEN];
char empty[BUFLEN];

void clearandwritecommand(int* i, int commandidx, char* buf, int *last_index) {
	for (int j = 0; j < *i; j++) {
		cputchar('\b');
	}
	int len = strlen(command_history[commandidx]);
	memcpy(buf, empty, BUFLEN);
	for (*i = 0; *i < len; (*i)++) {
		cputchar(command_history[commandidx][*i]);
		buf[*i] = command_history[commandidx][*i];
	}
	*last_index = len;
}

void RoundAutoCompleteCommandWithTheSamePrefix(int old_buf_len, char* prefix_element,
		char* buf, int* i, int *last_index) {
	for (int j = 0; j < old_buf_len; j++) {
		cputchar('\b');
	}
	int len = strlen(prefix_element);
	memcpy(buf, empty, BUFLEN);
	for (*i = 0; *i < len; (*i)++) {
		cputchar(prefix_element[*i]);
		buf[*i] = prefix_element[*i];
	}
	*last_index = len;
}

char PrefixList[100][1024];
void clear_prefix_list()
{
	for (int i = 0; i < 100; ++i) {
		memset(PrefixList[i], 0, 1024);}
}

void command_prompt_readline(const char *prompt, char* buf) {
	int i, c, echoing, lastIndex;
	if (prompt != NULL)
		cprintf("%s", prompt);

	int commandidx = last_command_idx + 1;
	int prefix_list_idx = lastIndex = i = 0;
	int prefix_list_size, last_c;
	echoing = iscons(0);
	bool is_run_cmd = 0;
	bool is_tst_cmd = 0;

	while (1) {
		c = getchar();
		if (i > lastIndex)
			lastIndex = i;
		if (c < 0) {

			if (c != -E_EOF)
				cprintf("read error: %e\n", c);
			return;
		} else if (c == 226) { // Up arrow
			if (commandidx)
				commandidx--;
			clearandwritecommand(&i, commandidx, buf, &lastIndex);
		} else if (c == 227) { // Down arrow
			if (commandidx < last_command_idx)
				commandidx++;
			if (last_command_idx >= 0)
				clearandwritecommand(&i, commandidx, buf, &lastIndex);
		} else if (c == 9) { // Tab button
			if (last_c != 9) {
				clear_prefix_list(PrefixList, 100);
				if (strlen(buf) == 0 || last_c == 255)
					continue;
				char *arguments[MAX_ARGUMENTS];
				int number_of_arguments = prefix_list_size = 0;
				char temp_buf[1024];
				strcpy(temp_buf, buf);
				int bufLength = strlen(buf);
				if (buf[bufLength - 1] == ' ')
					continue;
				strsplit(temp_buf, WHITESPACE, arguments, &number_of_arguments);
				int it_str = 0;
				if (number_of_arguments > 1) {
					if((strcmp(arguments[0], "run") != 0) && (strcmp(arguments[0], "load") != 0)
							&& (strcmp(arguments[0], "tst") != 0)) // to autocomplete only in case that the command take arguments and defined arguments (run & load & tst) only
						continue;
					if ((strcmp(arguments[0], "tst") == 0))
					{
						is_tst_cmd = 1;
					}
					else
					{
						is_run_cmd = 1;
					}
					char temp[1024] = "";
					int TotalLen = bufLength - strlen(arguments[number_of_arguments - 1]);
					for (int var = 0; var < TotalLen; ++var) {
						temp[it_str++] = buf[var];
					}
					strcpy(buf, temp);   //buf contains all arguments except the last one
					strcpy(temp_buf, arguments[number_of_arguments - 1]);   //temp_buf contains the last argument
				}
				int it_prefix_list = 0;
				if(number_of_arguments == 1)
				{
					for (int var = 0; var < NUM_OF_COMMANDS; ++var) {
						int x = strncmp(temp_buf, commands[var].name, strlen(temp_buf));
						if (x == 0) {
							it_str = -1;
							char string[1024] = "";
							for (int var3 = 0; var3 < strlen(commands[var].name); ++var3) {
								string[++it_str] = commands[var].name[var3];
							}
							memset(PrefixList[it_prefix_list], 0, 1024);
							strncpy(PrefixList[it_prefix_list], string, it_str + 1);
							it_prefix_list++;
						}
					}
				}
				else
				{
					if(is_run_cmd)
					{
						for (int var = 0; var < NUM_USER_PROGS; ++var) {
							int x = strncmp(temp_buf, ptr_UserPrograms[var].name, strlen(temp_buf));
							if (x == 0) {
								it_str = -1;
								char string[1024] = "";
								if (number_of_arguments > 1) {
									for (int var2 = 0; var2 < strlen(buf); ++var2) {
										string[++it_str] = buf[var2];
									}
								}
								for (int var3 = 0; var3 < strlen(ptr_UserPrograms[var].name) ; ++var3) {
									string[++it_str] = ptr_UserPrograms[var].name[var3];
								}
								memset(PrefixList[it_prefix_list], 0, 1024);
								strncpy(PrefixList[it_prefix_list], string, it_str + 1);
								it_prefix_list++;
							}
						}
					}
					else if(is_tst_cmd)
					{
						for (int var = 0; var < NUM_OF_TESTS; ++var) {
							int x = strncmp(temp_buf, tests[var].name, strlen(temp_buf));
							if (x == 0) {
								it_str = -1;
								char string[1024] = "";
								if (number_of_arguments > 1) {
									for (int var2 = 0; var2 < strlen(buf); ++var2) {
										string[++it_str] = buf[var2];
									}
								}
								for (int var3 = 0; var3 < strlen(tests[var].name) ; ++var3) {
									string[++it_str] = tests[var].name[var3];
								}
								memset(PrefixList[it_prefix_list], 0, 1024);
								strncpy(PrefixList[it_prefix_list], string, it_str + 1);
								it_prefix_list++;
							}
						}
					}
				}
				prefix_list_size = it_prefix_list;
				if (it_prefix_list) {
					prefix_list_idx = it_str = 0;
					for (int var2 = 0; var2 < strlen(PrefixList[0]); ++var2) {
						buf[it_str++] = PrefixList[0][var2];}
					for (int var = 0; var < bufLength; ++var) {
						cputchar('\b');}
					for (int j = 0; j < strlen(buf); ++j) {
						cputchar(buf[j]);}
					i = lastIndex = strlen(buf);
				}
			}
			else {
				if (prefix_list_size > 0) {	int prev = prefix_list_idx;
				prefix_list_idx = (prefix_list_idx + 1) % prefix_list_size;
				RoundAutoCompleteCommandWithTheSamePrefix(strlen(PrefixList[prev]), PrefixList[prefix_list_idx], buf, &i, &lastIndex);
				}
			}
		}

		else if (c == 228) { // left arrow
			if (i > 0) {
				i--;
				cputchar(c);
			}
		} else if (c == 229) { // right arrow
			if (i < lastIndex) {
				i++;
				cputchar(c);
			}
		}
		else if (c == 0xE9 && i > 0) {		 // KEY_DEL
			for (int var = i; var <= lastIndex; ++var) {
				buf[var] = buf[var + 1];
			}
			lastIndex--;
		}
		else if (c >= ' ' && i < BUFLEN - 1 && c != 229 && c != 228) {
			if (echoing)
				cputchar(c);
			buf[i++] = c;
			lastIndex++;
		} else if (c == '\b' && i > 0) {

			if (echoing)
				cputchar(c);
			for (int var = i; var <= i; ++var) {
				buf[var - 1] = buf[var];
			}
			i--;
		} else if (c == '\n' || c == '\r') {

			if (echoing)
				cputchar(c);

			buf[lastIndex] = 0;
			if (last_command_idx == HISTORY_MAX) {
				for (int idx = 0; idx < HISTORY_MAX; idx++) {
					memcpy(command_history[idx], command_history[idx + 1],
							BUFLEN);
				}
				memcpy(command_history[HISTORY_MAX], buf, BUFLEN);
			} else if (strcmp(command_history[last_command_idx], buf) != 0) {
				memcpy(command_history[++last_command_idx], buf, BUFLEN);
			}
			return;

		}
		last_c = c;
	}
}
// ******************************************************************
// ******************************************************************

extern bool autograde ;
void run_command_prompt()
{
	if (autograde)
	{
		char cmd1_2[BUFLEN] = "tst bsd_nice 0";
		char cmd2_2[BUFLEN] = "tst bsd_nice 1";
		char cmd3_2[BUFLEN] = "tst bsd_nice 2";
		//execute_command(cmd3_2);
		autograde = 0;
	}
	/*2024*/
	LIST_INIT(&foundCommands);
	//========================

	char command_line[BUFLEN];

	while (1==1)
	{
		//readline("FOS> ", command_line);

		// ********** This DosKey supported readline function is a combined implementation from **********
		// ********** 		Mohamed Raafat & Mohamed Yousry, 3rd year students, FCIS, 2017		**********
		// ********** 				Combined, edited and modified by TA\Ghada Hamed				**********
		memset(command_line, 0, sizeof(command_line));
		command_prompt_readline("FOS> ", command_line);

		//parse and execute the command
		if (command_line != NULL)
			if (execute_command(command_line) < 0)
				break;
	}
}

/* get into the command prompt - This function does not return.
 * The only way to get into the prompt is via this function to ensure correct re-initializations
 * The following variables are used to clear the entire content of the KERNEL STACK before getting into the prompt
 * They're placed globally (instead of locally) to avoid clearing them while they're in use [el7 :)]
 */
int m;
char *p ;
void get_into_prompt()
{
	while (1)
	{
		//disable interrupt if it's already enabled
		if (read_eflags() & FL_IF)
			cli();

		//Switch to the kernel virtual memory
		switchkvm();

		//Reset current CPU
		struct cpu *c = mycpu();
		c->ncli = 0;
		c->intena = 0;
		c->scheduler = NULL;
		c->scheduler_status = SCH_STOPPED ;
		c->proc = NULL;

		//Read current ESP
		uint32 cur_esp = read_esp();
		//cprintf("*** KERNEL SP: BEFORE RESIT = %x - ", cur_esp);

//		//Make sure it's in the correct stack (i.e. KERN STACK below KERN_BASE)
//		assert(cur_esp < SCHD_KERN_STACK_TOP && cur_esp >= SCHD_KERN_STACK_TOP - KERNEL_STACK_SIZE);

		//Reset ESP to the beginning of the SCHED KERNEL STACK of this CPU before getting into the cmd prmpt
		uint32 cpuStackTop = (uint32)c->stack + KERNEL_STACK_SIZE;
		uint32 cpuStackBottom = (uint32)c->stack + PAGE_SIZE/*GUARD Page*/;
		write_esp(cpuStackTop);

		//cprintf("AFTER RESIT = %x ***\n", read_esp());

		//Clear the stack content to avoid any garbage data on it when getting back into prompt
		if (cur_esp < cpuStackTop && cur_esp >= cpuStackBottom)
		{
			//memset((char*)cur_esp, 0, SCHD_KERN_STACK_TOP - cur_esp);
			p = (char*)cur_esp;
			m = cpuStackTop - cur_esp;
			while (--m >= 0)
				*p++ = 0;
		}
		else	//clear the ENTIRE SCHED KERN STACK
		{
			//memset((char*)schd_kern_stack_bottom, 0, SCHD_KERN_STACK_TOP - schd_kern_stack_bottom);
			p = (char*)cpuStackBottom;
			m = cpuStackTop - cpuStackBottom;
			while (--m >= 0)
				*p++ = 0;
		}

		//Reset EBP to ZERO so that when calling the run_command_prompt() it pushes ZERO into the stack
		write_ebp(0);

		//Get into the prompt (should NOT return)
		run_command_prompt(NULL);
	}

}


/***** Kernel command prompt command interpreter *****/

//define the white-space symbols
#define WHITESPACE "\t\r\n "

//Function to parse any command and execute it
//(simply by calling its corresponding function)
int execute_command(char *command_string)
{
	// Split the command string into whitespace-separated arguments
	int number_of_arguments;
	//allocate array of char * of size MAX_ARGUMENTS = 16 found in string.h
	char *arguments[MAX_ARGUMENTS];


	strsplit(command_string, WHITESPACE, arguments, &number_of_arguments) ;
	if (number_of_arguments == 0)
		return 0;

	int ret = process_command(number_of_arguments, arguments);

	//cprintf("cmd %s, num of args %d, return %d\n", arguments[0], number_of_arguments, ret);

	if (ret == CMD_INVALID)
	{
		cprintf("Unknown command '%s'\n", arguments[0]);
	}
	else if (ret == CMD_INV_NUM_ARGS)
	{
		int numOfFoundCmds = LIST_SIZE(&foundCommands);
		if (numOfFoundCmds != 1)
		{
			panic("command is found but the list is either empty or contains more than one command!");
		}
		struct Command * cmd = LIST_FIRST(&foundCommands);
		cprintf("%s: invalid number of args.\nDescription: %s\n", cmd->name, cmd->description);
	}
	else if (ret == CMD_MATCHED)
	{
		int i = 1;
		int numOfFoundCmds = LIST_SIZE(&foundCommands);
		if (numOfFoundCmds == 0)
		{
			panic("command is matched but the list is empty!");
		}
		struct Command * cmd = NULL;
		LIST_FOREACH(cmd, &foundCommands)
		{
			cprintf("[%d] %s\n", i++, cmd->name);
		}
		cprintf("Please select the required command [1] to [%d] and press enter? or press any other key to cancel: ", numOfFoundCmds);
		char Chose = getchar();
		cputchar(Chose);
		int selection = 0;
		while (Chose >= '0' && Chose <= '9')
		{
			selection = selection*10 + (Chose - '0') ;
			if (selection < 1 || selection > numOfFoundCmds)
				break;

			Chose = getchar();
			cputchar(Chose);
		}
		cputchar('\n');
		if (selection >= 1 && selection <= numOfFoundCmds)
		{
			int c = 1;
			LIST_FOREACH(cmd, &foundCommands)
			{
				if (c++ == selection)
				{
					if (cmd->num_of_args == 0)
					{
						cprintf("FOS> %s\n", cmd->name);
						return cmd->function_to_execute(number_of_arguments, arguments);
					}
					else
					{
						cprintf("%s: %s\n", cmd->name, cmd->description);
						return 0;
					}
				}
			}
		}
	}
	else
	{
		return commands[ret].function_to_execute(number_of_arguments, arguments);
	}
	return 0;
}


int process_command(int number_of_arguments, char** arguments)
{
	//TODO: [PROJECT'24.MS1 - #01] [1] PLAY WITH CODE! - process_command
int command_found = 0;
int command_number=-1;
int argumentsnum=number_of_arguments-1;
LIST_INIT(&foundCommands);
for (int i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(arguments[0], commands[i].name) == 0)
		{
			command_found = 1;
			command_number = i;
			break;
		}
	}
if(command_found==1)
{
	if (commands[command_number].num_of_args==-1)
  {
	if(argumentsnum>=1)
	{
	return command_number;
	}
	else
	{
		struct Command * cmd = &commands[command_number];
		LIST_INSERT_TAIL(&foundCommands, cmd);
		return CMD_INV_NUM_ARGS;
	}

  }
else if (commands[command_number].num_of_args>-1)
  {
	int actual_argsnum=commands[command_number].num_of_args;
	 if(argumentsnum==actual_argsnum)
	 {
		 return command_number;
	 }
	 else if (argumentsnum!=actual_argsnum)
	 {
		 struct Command * cmd = &commands[command_number];
		 		LIST_INSERT_TAIL(&foundCommands, cmd);
		 		return CMD_INV_NUM_ARGS;
	 }

  }

}
else if(command_found==0)
{
for (int i =0;i<NUM_OF_COMMANDS;i++)
{
int k=0;
int j=0;
int counter=0;
	while(commands[i].name[k]!='\0' && arguments[0][j]!='\0')
	{

		 if(commands[i].name[k]==arguments[0][j])
		 {
			 j++;
			 counter++;
		 }

		 k++;

	}
	if (counter == strlen(arguments[0])) {
	            struct Command *cmd = &commands[i];
	            LIST_INSERT_HEAD(&foundCommands, cmd);
	        }

}
int numOfFoundCmds = LIST_SIZE(&foundCommands);
		if (numOfFoundCmds == 0)
		{
			return CMD_INVALID;
		}
		else
			return CMD_MATCHED;


}

return 0;
}
