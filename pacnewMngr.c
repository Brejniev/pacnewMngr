#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Max Size :
// Determine the maximum length of the buffer and strings used 
// in this program.
#define TMAX_BUF 256

// Environment :
// Global variable used to store the exectution environment of the program.
static char **env;

// Search directory :
// directory in which pacnewMngr will search for .pacnew files.
#define searchDirectory "/etc"

// Temporary File Directory :
// directory in which the temporary file will be created.
#define tmpFileDirectory ""

// Temporary File :
// name of the temporary file.
#define tmpFileName "tmp"

// find :
// the binary and arguments for the 'find' command.
#define progFind "/usr/bin/find"
static char *argsFind[] = { progFind, searchDirectory, "-name", "*.pacnew", NULL };

// rm :
// the binary and arguments for the 'rm' command.
#define progRm "/usr/bin/rm"
static char *argsRm[] = { progRm, NULL, NULL };

// diff :
// the binary and arguments for the 'diff' command.
#define progDiff "/usr/bin/diff"
static char *argsDiff[] = { progDiff, NULL, NULL, NULL };

// mv :
// the binary and arguments for the 'mv' command.
#define progMv "/usr/bin/mv"
static char *argsMv[] = { progDiff, NULL, NULL, NULL };

// vim :
// the binary and arguments for the 'vim' editor.
#define progVim "/usr/bin/vim"
static char *argsVim[] = { progVim, NULL, NULL };

// ----------------------------------------------

/**
 * Execution Function :
 * This function execute the command that correspond to the binary file 
 * indicated by the 'prog' string with arguments given in the 'args'
 * string array. The output of the command is redirected in the file
 * designated by the file descriptor 'fd'. If 'fd' is less or equal to 0
 * then the output is not redirected.
 */
void execute( const char *prog, char **args, int fd ) {
	pid_t p;
	p = fork();
	if( p<0 ) { exit(EXIT_FAILURE); }
	if( p==0 ) {
		if( fd>0 ) {
			dup2( fd, 1 );
		}
		execve( prog, args, env );
		exit(EXIT_FAILURE);
	}
	wait(NULL);
}

/**
 * Answer Tester :
 * Test the answer that the user entered, return TRUE if the answer is
 * equal to 'Y' or 'y', else return FALSE.
 */
int testAns( const char ans ) {
	return ( ans=='Y' || ans=='y' /*|| ans=='\n'*/ );
}

/**
 * Buffer Cleaner :
 * Empty the buffer.
 */
void emptyBuffer() {
	char c;
	while( (c=getchar())!='\n' && c!=EOF ) {};
}

/**
 * Answer Reader :
 * Read an answer from the standard input, empty the buffer and 
 * finally return the answer.
 */
char readChar() {
	char c = getchar();
	emptyBuffer();
	return c;
}

/**
 * Process Function :
 * This function is in charge of the processing part of the program, it 
 * contain the code that is used to process a single .pacnew file given 
 * by 'pacnewFile'.
 * This function is thus applied on each found .pacnew file.
 * The parameter n indicate the length of the string 'pacnewfile'.
 */
void process_pacnewFile( char *pacnewFile, int n );

// ----------------------------------------------
// ---[ main ]---

int main( int argc, char *argv[], char *envp[] ) {
	
	env = envp;
	FILE *tmp;
	char *pacnewFile = (char*) malloc( TMAX_BUF*sizeof(char) );
	int n = 0;
	
	// Initialize
	fprintf( stderr, "Creation du fichier temporaire ... \n" );
	tmp = fopen( tmpFileName, "w" );
	if( tmp==NULL ) { exit(EXIT_FAILURE); }
	
	// Search
	fprintf( stderr, "Recherche des fichiers '.pacnew' ... \n" );
	execute( progFind, argsFind, fileno(tmp) );
	fclose(tmp);
	fprintf( stderr, "Recherche terminee.\n" );
	
	// Process
	fprintf( stderr, "Demarrage du traitement des fichiers trouves.\n" );
	tmp = fopen( tmpFileName, "r" );
	while( fgets(pacnewFile,TMAX_BUF,tmp)!=NULL ) {
		n = strlen(pacnewFile);
		pacnewFile[n-1] = '\0';
		process_pacnewFile( pacnewFile, n );
	}
	
	// Finalize
	fprintf( stderr, "Fin du traitement.\n" );
	fprintf( stderr, "Fermeture du fichier temporaire ... \n" );
	fclose(tmp);
	fprintf( stderr, "Suppression du fichier temporaire ... \n" );
	argsRm[1] = tmpFileName;
	execute( progRm, argsRm, -1 );
	fprintf( stderr, "Fin du programme.\n" );
	
	return 0;
}

// ----------------------------------------------
// ---[ process_pacnewFile ]---

void process_pacnewFile( char *pacnewFile, int n ) {
	
	char ans;
	char *currentFile = (char*) malloc( TMAX_BUF*sizeof(char) );
	char *pacoldFile = (char*) malloc( TMAX_BUF*sizeof(char) );
	strncpy( currentFile, pacnewFile, n-7 );
	currentFile[n-8] = '\0';
	
	printf( "fichier : %s\n\n", pacnewFile );
	
	// Show the difference between the current file and the 
	// .pacnew file using the command 'diff'.
	argsDiff[1] = pacnewFile;
	argsDiff[2] = currentFile;
	execute( progDiff, argsDiff, -1 );
	
	// Ask the user if he/she wants to replace the current file 
	// by the corresponding .pacnew file.
	printf( "\nreplace ? [y/N] " );
	ans = readChar();
	if( !testAns(ans) ) {
	// If not : Ask the user if he/she wants to remove the .pacnew file.
		printf( "\nremove ? [y/N] " );
		ans = readChar();
		if( testAns(ans) ) {
		// If yes : remove the .pacnew file.
			argsRm[1] = pacnewFile;
			execute( progRm, argsRm, -1 );
		}
		goto End;
	}
	
	// If yes : proceed with the replacement
	strcpy( pacoldFile, currentFile );
	strcat( pacoldFile, ".pacold" );
	if( !access( pacoldFile, F_OK ) ) {
		argsRm[1] = pacoldFile;
		execute( progRm, argsRm, -1 );
	}
	argsMv[1] = currentFile;
	argsMv[2] = pacoldFile;
	execute( progMv, argsMv, -1 );
	argsMv[1] = pacnewFile;
	argsMv[2] = currentFile;
	execute( progMv, argsMv, -1 );
	printf( "replacement done.\n" );
	
	// Ask the user if he/she wants to edit the new current file
	// (which is the former .pacnew file)
	printf( "\nedit new file ? [y/N] " );
	ans = readChar();
	if( !testAns(ans) ) {
	// If not : this is the end of this process.
		goto End;
	}
	
	// If yes : open the new current file with vim.
	argsVim[1] = currentFile;
	execute( progVim, argsVim, -1 );
	
	End: // (of this process)
	printf( "\n\n\t~--------------------~\n\n" );
	return;
}

// ----------------------------------------------



