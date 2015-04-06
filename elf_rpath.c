/*----------------------------------------------------------------------*
 * HolisticView/MetaKnowledge						*
 *									*
 *									*
 * Copyright Notice:                                                	*
 * Free use of this software is permitted under the guidelines and   	*
 * in accordance with the most current version of the Common Public 	*
 * License.                                                         	*
 * http://www.opensource.org/licenses/cpl.php                       	*
 *									*
 *									*
 *									*
 *----------+-----------------------------------------------------------*
 * PACKAGE  | N/A 							*
 *----------+-----------------------------------------------------------*
 * MODULE   | elf_rpath.c                                     		*
 *----------+-----------------------------------------------------------*
 *									*
 *									*
 *----------------------------------------------------------------------*
 * CREATED 								*
 *----------------------------------------------------------------------*
 * AGARCIA/12-2005							*
 *									*
 *									*
 *----------------------------------------------------------------------*
 * MODIFIED 	   							*
 *----------------------------------------------------------------------*
 *									*
 *									*
 *----------------------------------------------------------------------*
 * CHANGE LOG    							*
 *----------------------------------------------------------------------*
 *									*
 *									*
 *									*
 *									*
 *----------------------------------------------------------------------*
 * NOTES 	   							*
 *----------------------------------------------------------------------*
 * Changes the RUN PATH in the header section of an ELF object.		*
 * gcc elf_rpath.c -o elf_rpath -lelf									*
 *									*
 *									*
 *									*
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libelf.h>
#include <link.h>


/*----------------------------------------------------------------------*
 * DEFINES SECTION
 *		
 *----------------------------------------------------------------------*/
#define HELP      " elf_rpath command line options:\n\n\
elf_rpath -v <File>      => Shows the actual RUN PATH <=\n\
elf_rpath -o <File>      => Sets RPATH to $ORIGIN     <=\n\
elf_rpath <File> <RPATH> => Sets RPATH                <=\n"

#define INVALID   "\n\nInvalid command line arguments!\n\n"

#define ORIGIN		"$ORIGIN"
#define OPTIONS		"vof"



/*----------------------------------------------------------------------*
 * BEGIN	
 * TYPEDEF SECTION
 *----------------------------------------------------------------------*/

typedef struct {
 int 	m_iHandle;
 int	m_iForce;
 int	m_iView;
 int	m_iOrigin;
 char 	*m_szFile;
 char 	*m_szRPath;
} _OPTIONS;

typedef struct {
 Elf 		*m_objELF;
 Elf_Scn 	*m_objSCN;		
 Elf32_Dyn 	*m_objDyn;
} _ELFHND;

/*----------------------------------------------------------------------*
 * TYPEDEF SECTION
 * END
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 * BEGIN
 * GLOBALS SECTION	
 *----------------------------------------------------------------------*/

_OPTIONS g_objOptions;
_ELFHND g_objElfHnd;

extern char 	*optarg;
extern int 	optind;

/*----------------------------------------------------------------------*
 * GLOBALS SECTION	
 * END			
 *----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*
 * BEGIN		
 * INLINE FUNCTIONS	
 *----------------------------------------------------------------------*/

#define STRING(s) 	( s ? s : "(empty)" )
#define	SHOW_HELP()	( fprintf(stdout, HELP ) )
#define	SHOW_INVALID()	( fprintf(stdout, INVALID) )


#define SET_HANDLE(s)  	( g_objOptions.m_iHandle= s )
#define SET_FORCE(s)  	( g_objOptions.m_iForce= s )
#define SET_VIEW(s)  	( g_objOptions.m_iView= s)
#define SET_ORIGIN(s)  	( g_objOptions.m_iOrigin= s )
#define SET_FILE(s)  	( g_objOptions.m_szFile= strdup( s ) )
#define SET_RPATH(s)  	( g_objOptions.m_szRPath= strdup( s ) )

#define GET_HANDLE()  	( g_objOptions.m_iHandle )
#define GET_FORCE()  	( g_objOptions.m_iForce )
#define GET_VIEW()  	( g_objOptions.m_iView )
#define GET_ORIGIN()  	( g_objOptions.m_iOrigin )
#define GET_FILE()  	( g_objOptions.m_szFile )
#define GET_RPATH()  	( g_objOptions.m_szRPath )

/*----------------------------------------------------------------------*
 * INLINE FUNCTIONS	
 * END 			
 *----------------------------------------------------------------------*/

void INIT_OPTIONS( void );
void PARSE_OPTIONS( int iArgC, char *pszArgV[] );
void VALIDATE_OPTIONS( int iArgC, char *pszArgV[] );
void INIT_NAMES( int iArgC, char *pszArgV[] );

void ELF_OPEN( void );
void ELF_CLOSE( void );
void ELF_ITERATE_SCN( int (*objCallBack)(void) );
int ELF_GET_ENTRY( void );
int ELF_FIND_RPATH( void );
void MODIFY( char *szNewValue );


/*----------------------------------------------------------------------*
 * BEGIN		
 * MAIN			
 *----------------------------------------------------------------------*/

int main(int argc, char *argv[]) {

  	/*  
   	*  Init Section 
   	*/ 
 	INIT_OPTIONS();
 	PARSE_OPTIONS( argc, argv );
 	VALIDATE_OPTIONS( argc, argv );
 	INIT_NAMES( argc, argv );
  
	
	
	ELF_OPEN();
		    
	if( GET_VIEW() ) {
		ELF_ITERATE_SCN( ELF_FIND_RPATH );
		ELF_GET_ENTRY();
	}
    	
    	if( GET_ORIGIN() ) {
		ELF_ITERATE_SCN( ELF_FIND_RPATH );
		ELF_GET_ENTRY();
		MODIFY( ORIGIN );
		
	}
    	
    	if( GET_RPATH() ) {
		ELF_ITERATE_SCN( ELF_FIND_RPATH );
		ELF_GET_ENTRY();
		MODIFY( GET_RPATH() );
		
	}
    		
 	ELF_CLOSE();
	
}

/*----------------------------------------------------------------------*
 * MAIN			
 * END 			
 *----------------------------------------------------------------------*/



/*----------------------------------------------------------------------
 * Function: INIT_OPTIONS
 *
 *----------------------------------------------------------------------*/ 
void INIT_OPTIONS( void ) {
	SET_FORCE( 0 );
	SET_VIEW( 0 );
	SET_ORIGIN( 0 );
	g_objOptions.m_szRPath= NULL;
}

/*----------------------------------------------------------------------
 * Function: PARSE_OPTIONS
 *
 *----------------------------------------------------------------------*/ 
void PARSE_OPTIONS(int iArgC, char *pszArgV[]) {
 int m_iOption;

	while((m_iOption= getopt(iArgC, pszArgV, OPTIONS) ) != EOF) {
		switch(m_iOption) {
			case 'f': 
				SET_FORCE(1);
        			break;
			case 'v': 
				SET_VIEW(1);
        			break;
			case 'o': 
				SET_ORIGIN(1);
        			break;
			case '?': 
				SHOW_HELP();
				break;
		}
	}
}

/*----------------------------------------------------------------------
 * Function: VALIDATE_OPTIONS
 *
 *----------------------------------------------------------------------*/ 
void VALIDATE_OPTIONS( int iArgC, char *pszArgV[] ) {
 	if( ( !( GET_ORIGIN() & GET_VIEW() ) ) &&  iArgC == 3  ) return;
 	if( ( !( GET_ORIGIN() | GET_VIEW() ) ) &&  iArgC == 3  ) return;
 
 	SHOW_INVALID();
 	SHOW_HELP();
 	exit( 1 );
}


/*----------------------------------------------------------------------
 * Function: INIT_NAMES
 *
 *----------------------------------------------------------------------*/ 
void INIT_NAMES( int iArgC, char *pszArgV[] ) {
	if( GET_ORIGIN() | GET_VIEW()  ) 
		SET_FILE( pszArgV[ optind ] );		
	else {
		SET_FILE( pszArgV[ optind ] );		
		SET_RPATH( pszArgV[ optind + 1 ] );
	}
}


/*----------------------------------------------------------------------
 * Function: OPEN_FILE
 *
 *----------------------------------------------------------------------*/ 
int OPEN_FILE( ) {
 int m_iTemp= 0;
  	
 	if( ( m_iTemp= open( GET_FILE(), ( GET_VIEW() ? O_RDONLY : O_RDWR ) )) == -1) {
  		fprintf( stderr,"Cannot open file: %s\n", GET_FILE() );
  		exit(1); 
  	}
  	return( m_iTemp );
}

/*----------------------------------------------------------------------
 * Function: ELF_OPEN
 *
 *----------------------------------------------------------------------*/ 
void ELF_OPEN( void ) {
 Elf32_Ehdr *m_objSCN_HEADER;	
 
	SET_HANDLE( OPEN_FILE( ) );

 	elf_version( EV_CURRENT ) ;
	g_objElfHnd.m_objELF= elf_begin( GET_HANDLE() , ( GET_VIEW() ? ELF_C_READ : ELF_C_RDWR ), (Elf *)NULL);

 	if( elf_kind( g_objElfHnd.m_objELF ) != ELF_K_ELF ) {
  		fprintf(stderr,"Its not an ELF File\n");
  		exit(1);
	}
	
	if (( m_objSCN_HEADER = elf32_getehdr( g_objElfHnd.m_objELF )) == 0) {
  		fprintf(stderr,"elf32_getehdr failed.\n");
  		exit(1);
	}
}

/*----------------------------------------------------------------------
 * Function: ELF_CLOSE
 *
 *----------------------------------------------------------------------*/ 
void ELF_CLOSE( void ) {
	if( ! GET_VIEW() )
		if ( elf_update( g_objElfHnd.m_objELF , ELF_C_WRITE) == -1 ) 
	  		fprintf(stdout,"elf_update failed.\n"); 
	
	elf_end( g_objElfHnd.m_objELF );
}


/*----------------------------------------------------------------------
 * Function: ELF_ITERATE_SCN
 *
 *----------------------------------------------------------------------*/ 
void ELF_ITERATE_SCN( int (*objCallBack)(void) ) {
 Elf32_Shdr 	*m_objSCN_SHDR;
 Elf_Data 	*m_objData;
 

	g_objElfHnd.m_objSCN= NULL;
	while(( g_objElfHnd.m_objSCN= elf_nextscn( g_objElfHnd.m_objELF, g_objElfHnd.m_objSCN )) != NULL) {
		m_objSCN_SHDR = elf32_getshdr( g_objElfHnd.m_objSCN );
  		if ( m_objSCN_SHDR->sh_type == SHT_DYNAMIC) {
    			m_objData= NULL;
    			while (( m_objData= elf_getdata( g_objElfHnd.m_objSCN, m_objData ) ) != NULL) {
      				g_objElfHnd.m_objDyn= (Elf32_Dyn *) m_objData->d_buf;
      				while ( g_objElfHnd.m_objDyn->d_tag != DT_NULL ) {
					if(!(*objCallBack)()) return;
					g_objElfHnd.m_objDyn++;
				}
			}
		}

	}

}

/*----------------------------------------------------------------------
 * Function: ELF_GET_ENTRY
 *
 *----------------------------------------------------------------------*/ 
int ELF_GET_ENTRY( void ) {
 char *m_szBuffer= NULL;
 size_t 	m_objIndx;
 Elf32_Shdr 	*m_objSCN_SHDR;
 
 m_objSCN_SHDR= elf32_getshdr( g_objElfHnd.m_objSCN );
 m_objIndx= m_objSCN_SHDR->sh_link;
 m_szBuffer= elf_strptr( g_objElfHnd.m_objELF, m_objIndx, g_objElfHnd.m_objDyn->d_un.d_ptr);
 
 //printf("VALUE: %s\n", STRING( m_szBuffer ) );
 return(0);
}

/*----------------------------------------------------------------------
 * Function: ELF_FIND_RPATH
 *
 *----------------------------------------------------------------------*/ 
int ELF_FIND_RPATH(void) {
	if( g_objElfHnd.m_objDyn->d_tag == DT_RPATH  ) return 0;
	return(1);
}

/*----------------------------------------------------------------------
 * Function: MODIFY
 *
 *----------------------------------------------------------------------*/ 
void MODIFY( char *szNewValue ) {
 Elf_Scn  	*m_objSCN;	
 Elf_Data 	*m_objData;
 Elf32_Shdr 	*m_objSCN_SHDR;
 size_t 	m_objIndx;
 unsigned char  *m_szBuffer= NULL;
 int 		m_iBufferSize; 
 int 		m_iSize;
 int 		m_iNewSize;

 
 	m_objSCN_SHDR= elf32_getshdr( g_objElfHnd.m_objSCN );
 	m_objIndx= m_objSCN_SHDR->sh_link;
 	
	m_objSCN= elf_getscn( g_objElfHnd.m_objELF , m_objIndx ); 
        m_objData= NULL; 
        
        while( ( m_objData= elf_getdata( m_objSCN, m_objData )) != NULL ) { 
              	m_iBufferSize= m_objData->d_size; 
            	m_szBuffer= m_objData->d_buf; 
            	
            	fprintf(stdout, "-> %d\n", m_iBufferSize );

            	elf_flagdata( m_objData,ELF_C_SET,ELF_F_DIRTY ); 

            	memmove(&m_szBuffer[g_objElfHnd.m_objDyn->d_un.d_ptr], szNewValue, strlen( szNewValue ) ); 
            	m_szBuffer[g_objElfHnd.m_objDyn->d_un.d_ptr+ strlen( szNewValue ) ] = 0; 
        } 

}
