#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <varargs.h>

#include <tcl.h>
/*
 * TCL & Opnet both define a STRINGIFY
 * Keep GCC happy by turning off Tcl's
 * Before going into Opnet stuff
 */
#undef STRINGIFY
#include <opnet.h>
#include <ema.h>
#include <opnet_emadefs.h>
#include <opnet_constants.h>


#ifdef WIN32
#include <io.h>
/*#define TCL_API __attribute__((dllexport))*/
#define TCL_API __declspec(dllexport)
#else
#include <unistd.h>
#define TCL_API
#endif

#ifdef WIN32
#define dup _dup
#define fileno _fileno
#define fdopen _fdopen
#define close _close
#endif

#define TKEMA_COMPOUND (0x10000000)
#define MAX_EMA_ATTR_GET 100
#define MAX_EMA_ATTR_SET  MAX_EMA_ATTR_GET

__attribute__((__unused__))
static char SccsId[] = "$Id: tkema.c,v 1.53 2004/03/02 19:34:13 ted Exp $ TN";

/*
 * Function prototypes
 */
int tcl_ema_map(char *str);

TCL_API int Tkema_Init(Tcl_Interp *interp);
static int hash_init(void);
static int hash_lookup(char *);

static int tkEma_Init(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[]);
	
static int tkEma_Model_Attr_Add(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Attr_Get(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Attr_Nth(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Attr_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Attr_Set(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Base_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Code_Gen(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Copy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Create(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Destroy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Read(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Write(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Attr_Add(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Attr_Get(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Attr_Nth(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Attr_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Attr_Set(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Copy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Create(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Destroy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Move(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Nth(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Prom_Attr_Set(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Create(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Append(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Copy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Destroy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Line_Count(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Line_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Line_Set(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Vos_Textlist_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

/*
 * These functions are not direct counterparts to Mil3 Ema routines
 */
static int tkEma_Object_Store(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Object_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Num_Objects(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Const(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Attr_Make(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_errchk(Tcl_Obj *resultPtr);

static int tkEma_Object_Attr_Info(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_Model_Attr_Info(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

/*
 * These functions come from the prg_list package which doesn't seem to be
 * officially documented, but are useful in some EMA work
 */
static int tkEma_prg_list_create(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[]);

static int tkEma_prg_list_strings_append(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

/* 
 * misc..
 */
static FILE *diddle_stdout(void);
static FILE *undiddle_stdout(FILE *tmpfp);

/*
 * Init everything
 */
TCL_API int Tkema_Init(Tcl_Interp *interp){

#if TCL_USE_STUBS
	if(Tcl_InitStubs(interp, "8.3", 0) == NULL) {
		return TCL_ERROR;
	}
#endif

	if(hash_init() < 0)
		return TCL_ERROR;

	Tcl_CreateObjCommand(interp, "Ema_Init",
		tkEma_Init, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Add",
		tkEma_Model_Attr_Add, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Get",
		tkEma_Model_Attr_Get, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Nth",
		tkEma_Model_Attr_Nth, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Print",
		tkEma_Model_Attr_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Set",
		tkEma_Model_Attr_Set, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Base_Print",
		tkEma_Model_Base_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Code_Gen",
		tkEma_Model_Code_Gen, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Copy",
		tkEma_Model_Copy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Create",
		tkEma_Model_Create, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Destroy",
		tkEma_Model_Destroy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Print",
		tkEma_Model_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);
	
	Tcl_CreateObjCommand(interp, "Ema_Model_Read",
		tkEma_Model_Read, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Write",
		tkEma_Model_Write, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Add",
		tkEma_Object_Attr_Add, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Get",
		tkEma_Object_Attr_Get, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Nth",
		tkEma_Object_Attr_Nth, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Print",
		tkEma_Object_Attr_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Set",
		tkEma_Object_Attr_Set, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Copy",
		tkEma_Object_Copy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Create",
		tkEma_Object_Create, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Destroy",
		tkEma_Object_Destroy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Move",
		tkEma_Object_Move, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Nth",
		tkEma_Object_Nth, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Print",
		tkEma_Object_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Prom_Attr_Set",
		tkEma_Object_Prom_Attr_Set, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Create",
		tkEma_Vos_Textlist_Create, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Append",
		tkEma_Vos_Textlist_Append, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Copy",
		tkEma_Vos_Textlist_Copy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Destroy",
		tkEma_Vos_Textlist_Destroy, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Line_Count",
		tkEma_Vos_Textlist_Line_Count, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Line_Get",
		tkEma_Vos_Textlist_Line_Get, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Line_Set",
		tkEma_Vos_Textlist_Line_Set, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Vos_Textlist_Print",
		tkEma_Vos_Textlist_Print, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	/*
	 * Functions not directly based on Mil3 Ema calls
	 */
	Tcl_CreateObjCommand(interp, "Ema_Model_Num_Objects",
		tkEma_Model_Num_Objects, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Store",
		tkEma_Object_Store, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Get",
		tkEma_Object_Get, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Const",
		tkEma_Const, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Attr_Make",
		tkEma_Attr_Make, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "prg_list_create",
		tkEma_prg_list_create, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "prg_list_strings_append",
		tkEma_prg_list_strings_append, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Object_Attr_Info",
		tkEma_Object_Attr_Info, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "Ema_Model_Attr_Info",
		tkEma_Model_Attr_Info, (ClientData)NULL,
		(Tcl_CmdDeleteProc *) NULL);


	Tcl_PkgProvide(interp,"Tkema", "2.0");

	return 0;
}


/* for mapping names to EMA objects */
static Tcl_HashTable *namehash = NULL;

/*
 * This table is expanded as necessary.  It contains lots of Opnet/EMA
 * symbolic constants and macros that are helpful in TkEMA programs.
 * 
 * The ones marked TKEMA_COMPOUND (by setting the high bit) are actually
 * macros that expand into two parts and must be handled specially.
 */
static Tcl_HashTable *hashpt = NULL;
static struct hashmap{
	char *key;
	ClientData value;
} maptab[] = {
	{"EMAC_MODE_NONE", 		(ClientData) EMAC_MODE_NONE},
	{"EMAC_MODE_ERR_HALT", 		(ClientData) EMAC_MODE_ERR_HALT},
	{"EMAC_MODE_ERR_PRINT", 	(ClientData) EMAC_MODE_ERR_PRINT},
	{"EMAC_MODE_DIAG", 		(ClientData) EMAC_MODE_DIAG},
	{"EMAC_MODE_REL_23", 		(ClientData) EMAC_MODE_REL_23},
	{"EMAC_MODE_REL_24", 		(ClientData) EMAC_MODE_REL_24},
	{"EMAC_MODE_REL_25", 		(ClientData) EMAC_MODE_REL_25},
	{"EMAC_MODE_REL_30", 		(ClientData) EMAC_MODE_REL_30},
	{"EMAC_DISABLED", 		(ClientData) EMAC_DISABLED},
	{"EMAC_ENABLED", 		(ClientData) EMAC_ENABLED},
	{"EMAC_EOL",			(ClientData) EMAC_EOL},
	{"COMP_ARRAY_CONTENTS",		(ClientData) (TKEMA_COMPOUND | COMP_TYPE_ARRAY_CONTENTS)},
	{"COMP_DVEC_CONTENTS",		(ClientData) (TKEMA_COMPOUND | COMP_TYPE_DVEC_CONTENTS)},
	{"COMP_ARRAY_SIZE",		(ClientData) COMP_TYPE_ARRAY_SIZE},
	{"COMP_DVEC_SIZE",		(ClientData) COMP_TYPE_DVEC_SIZE},
	{"COMP_INTENDED",		(ClientData) COMP_TYPE_INTENDED},
	{"COMP_CONTENTS",		(ClientData) COMP_TYPE_CONTENTS},
	{"COMP_CONTENTS_SYMBOL",	(ClientData) COMP_TYPE_CONTENTS_SYMBOL},
	{"COMP_CONTENTS_TYPE",		(ClientData) COMP_TYPE_CONTENTS_TYPE},  
	{"COMP_PROMOTE",		(ClientData) COMP_TYPE_PROMOTE},
	{"COMP_PROPS",			(ClientData) COMP_TYPE_PROPS},
	{"COMP_SET",			(ClientData) COMP_TYPE_SET},
	{"COMP_TOGGLE",			(ClientData) COMP_TYPE_TOGGLE},
	{"EMAC_NO_SYMBOL",		(ClientData) EMAC_NO_SYMBOL},
	{"EMAC_INTEGER",		(ClientData) EMAC_INTEGER},
	{"EMAC_DOUBLE",			(ClientData) EMAC_DOUBLE},
	{"EMAC_STRING",			(ClientData) EMAC_STRING},
	{"EMAC_TOGGLE",			(ClientData) EMAC_TOGGLE},
	{"EMAC_TOGGLE_DOUBLE",		(ClientData) EMAC_TOGGLE_DOUBLE},
	{"EMAC_DATAFILE",		(ClientData) EMAC_DATAFILE},
	{"EMAC_COMPOUND",		(ClientData) EMAC_COMPOUND},
	{"ATTR_EMA_NUM_OBJECTS",	(ClientData) ATTR_EMA_NUM_OBJECTS},
	{"EMAC_INTEGER",		(ClientData) EMAC_INTEGER},
	{"EMAC_DOUBLE",			(ClientData) EMAC_DOUBLE},
	{"EMAC_STRING",			(ClientData) EMAC_STRING},
	{"EMAC_DATAFILE",		(ClientData) EMAC_DATAFILE},
	{"MOD_NETWORK",			(ClientData) MOD_NETWORK},
	{"MOD_NODE",			(ClientData) MOD_NODE},
	{"MOD_PROCESS",			(ClientData) MOD_PROCESS},
	{"MOD_LINK",			(ClientData) MOD_LINK},
	{"MOD_PDF",			(ClientData) MOD_PDF},
	{"MOD_MODULATION",		(ClientData) MOD_MODULATION},
	{"MOD_ANT_PATTERN",		(ClientData) MOD_ANT_PATTERN},
	{"MOD_PK_FORMAT",		(ClientData) MOD_PK_FORMAT},
	{"MOD_ICI_FORMAT",		(ClientData) MOD_ICI_FORMAT},
	{"MOD_ORBIT",			(ClientData) MOD_ORBIT},
	{"MOD_PROBES",			(ClientData) MOD_PROBES},
	{"MOD_SIM_SEQ",			(ClientData) MOD_SIM_SEQ},
	{"MOD_OUTVECTORS",		(ClientData) MOD_OUTVECTORS},
	{"MOD_OUTSCALARS",		(ClientData) MOD_OUTSCALARS},
	{"MOD_CARTO",			(ClientData) MOD_CARTO},
	{"MOD_PUB_AD",			(ClientData) MOD_PUB_AD},
	{"MOD_DER_NODE",		(ClientData) MOD_DER_NODE},
	{"MOD_DER_LINK",		(ClientData) MOD_DER_LINK},
	{"OBJ_ATTR_SYMMAP",		(ClientData) OBJ_ATTR_SYMMAP},
	{"OBJ_ATTR_PROPS",		(ClientData) OBJ_ATTR_PROPS},
	{"OBJ_ATTR_DEF",		(ClientData) OBJ_ATTR_DEF},
	{"OBJ_ATTR_COMP_VALUE",		(ClientData) OBJ_ATTR_COMP_VALUE},
	{"OBJ_ATTR_COMP_OBJ",		(ClientData) OBJ_ATTR_COMP_OBJ},
	{"OBJ_NT_ANNOT_TEXT",		(ClientData) OBJ_NT_ANNOT_TEXT},
	{"OBJ_NT_SUBNET",		(ClientData) OBJ_NT_SUBNET},
	{"OBJ_NT_SUBNET_FIX",		(ClientData) OBJ_NT_SUBNET_FIX},
	{"OBJ_NT_SUBNET_MOB",		(ClientData) OBJ_NT_SUBNET_MOB},
	{"OBJ_NT_NODE_FIXED",		(ClientData) OBJ_NT_NODE_FIXED},
	{"OBJ_NT_NODE_MOBILE",		(ClientData) OBJ_NT_NODE_MOBILE},
	{"OBJ_NT_NODE_SAT",		(ClientData) OBJ_NT_NODE_SAT},
	{"OBJ_NT_PTSIMP_LINK",		(ClientData) OBJ_NT_PTSIMP_LINK},
	{"OBJ_NT_PTDUP_LINK",		(ClientData) OBJ_NT_PTDUP_LINK},
	{"OBJ_NT_BU_LINK",		(ClientData) OBJ_NT_BU_LINK},
	{"OBJ_NT_TAP",			(ClientData) OBJ_NT_TAP},
	{"OBJ_NT_POS",			(ClientData) OBJ_NT_POS},
	{"OBJ_NT_INTF_DESC",		(ClientData) OBJ_NT_INTF_DESC},
	{"OBJ_NT_ATTR_ASSGN",		(ClientData) OBJ_NT_ATTR_ASSGN},
	{"OBJ_NT_SUBNET_VIEW",		(ClientData) OBJ_NT_SUBNET_VIEW},
	{"OBJ_NT_PT_LINK",		(ClientData) OBJ_NT_PT_LINK},
	{"OBJ_ND_PROCESSOR",		(ClientData) OBJ_ND_PROCESSOR},
	{"OBJ_ND_QUEUE",		(ClientData) OBJ_ND_QUEUE},
	{"OBJ_ND_IGEN",			(ClientData) OBJ_ND_IGEN},
	{"OBJ_ND_CGEN",			(ClientData) OBJ_ND_CGEN},
	{"OBJ_ND_PT_TX",		(ClientData) OBJ_ND_PT_TX},
	{"OBJ_ND_PT_RX",		(ClientData) OBJ_ND_PT_RX},
	{"OBJ_ND_BU_TX",		(ClientData) OBJ_ND_BU_TX},
	{"OBJ_ND_BU_RX",		(ClientData) OBJ_ND_BU_RX},
	{"OBJ_ND_RA_TX",		(ClientData) OBJ_ND_RA_TX},
	{"OBJ_ND_RA_RX",		(ClientData) OBJ_ND_RA_RX},
	{"OBJ_ND_ANTENNA",		(ClientData) OBJ_ND_ANTENNA},
	{"OBJ_ND_SUBQ",			(ClientData) OBJ_ND_SUBQ},
	{"OBJ_ND_PT_RXCH",		(ClientData) OBJ_ND_PT_RXCH},
	{"OBJ_ND_PT_TXCH",		(ClientData) OBJ_ND_PT_TXCH},
	{"OBJ_ND_BU_RXCH",		(ClientData) OBJ_ND_BU_RXCH},
	{"OBJ_ND_BU_TXCH",		(ClientData) OBJ_ND_BU_TXCH},
	{"OBJ_ND_RA_RXCH",		(ClientData) OBJ_ND_RA_RXCH},
	{"OBJ_ND_RA_TXCH",		(ClientData) OBJ_ND_RA_TXCH},
	{"OBJ_ND_POS",			(ClientData) OBJ_ND_POS},
	{"OBJ_ND_STRM",			(ClientData) OBJ_ND_STRM},
	{"OBJ_ND_STATWIRE",		(ClientData) OBJ_ND_STATWIRE},
	{"OBJ_ND_ASSOC",		(ClientData) OBJ_ND_ASSOC},
	{"OBJ_ND_INTF_DESC",		(ClientData) OBJ_ND_INTF_DESC},
	{"OBJ_ND_ATTR_ASSGN",		(ClientData) OBJ_ND_ATTR_ASSGN},
	{"OBJ_ND_STAT_INTF",		(ClientData) OBJ_ND_STAT_INTF},
	{"OBJ_PB_POS",			(ClientData) OBJ_PB_POS},
	{"OBJ_PB_CPROBE",		(ClientData) OBJ_PB_CPROBE},
	{"OBJ_PB_SPROBE",		(ClientData) OBJ_PB_SPROBE},
	{"OBJ_PB_AAPROBE",		(ClientData) OBJ_PB_AAPROBE},
	{"OBJ_PB_CAPROBE",		(ClientData) OBJ_PB_CAPROBE},
	{"OBJ_PB_SAPROBE",		(ClientData) OBJ_PB_SAPROBE},
	{"OBJ_PB_GSPROBE",		(ClientData) OBJ_PB_GSPROBE},
	{"OBJ_PB_STANPROBE",		(ClientData) OBJ_PB_STANPROBE},
	{"OBJ_PB_LKPROBE",		(ClientData) OBJ_PB_LKPROBE},
	{"OBJ_PR_STATE",		(ClientData) OBJ_PR_STATE},
	{"OBJ_PR_TRANS",		(ClientData) OBJ_PR_TRANS},
	{"OBJ_PR_POS",			(ClientData) OBJ_PR_POS},
	{"OBJ_PR_INTF_DESC",		(ClientData) OBJ_PR_INTF_DESC},
	{"OBJ_PR_ATTR_ASSGN",		(ClientData) OBJ_PR_ATTR_ASSGN},
	{"OBJ_PR_STAT_DESC",		(ClientData) OBJ_PR_STAT_DESC},
	{"OBJ_PR_CET",			(ClientData) OBJ_PR_CET},
	{"OBJ_LNK_INTF_DESC",		(ClientData) OBJ_LNK_INTF_DESC},
	{"OBJ_LNK_ATTR_ASSGN",		(ClientData) OBJ_LNK_ATTR_ASSGN},
	{"OBJ_OV_VECTOR",		(ClientData) OBJ_OV_VECTOR},
	{"OBJ_OV_ELEMENT",		(ClientData) OBJ_OV_ELEMENT},
	{"OPC_OV_TYPE_VECTOR_DOUBLE",	(ClientData) OPC_OV_TYPE_VECTOR_DOUBLE},
	{"OPC_OV_TYPE_VECTOR_ENUM",	(ClientData) OPC_OV_TYPE_VECTOR_ENUM},
	{"OPC_OV_TYPE_TOP_ELEMENT",	(ClientData) OPC_OV_TYPE_TOP_ELEMENT},
	{"OPC_OV_TYPE_OBJECT",		(ClientData) OPC_OV_TYPE_OBJECT},
	{"OPC_OV_TYPE_STAT_GROUP",	(ClientData) OPC_OV_TYPE_STAT_GROUP},
	{"OPC_VECTOR_TYPE_DOUBLE",	(ClientData) OPC_VECTOR_TYPE_DOUBLE},
	{"OPC_VECTOR_TYPE_ENUM",	(ClientData) OPC_VECTOR_TYPE_ENUM},
	{"OPC_OV_DRAW_STYLE_SAMPHOLD",(ClientData) OPC_OV_DRAW_STYLE_SAMPHOLD},
	{"OPC_OV_DRAW_STYLE_BAR",	(ClientData) OPC_OV_DRAW_STYLE_BAR},
	{"OPC_OV_DRAW_STYLE_DISCRETE",(ClientData) OPC_OV_DRAW_STYLE_DISCRETE},
	{"OPC_OV_DRAW_STYLE_LINEAR",	(ClientData) OPC_OV_DRAW_STYLE_LINEAR},
	{"OPC_OV_DRAW_STYLE_SQR_WAVE",(ClientData) OPC_OV_DRAW_STYLE_SQR_WAVE},
	{"OPC_OV_DRAW_STYLE_NONE",	(ClientData) OPC_OV_DRAW_STYLE_NONE},
	{"OBJ_PB_CPROBE",		(ClientData) OBJ_PB_CPROBE},
	{"OBJ_PB_SPROBE",		(ClientData) OBJ_PB_SPROBE},
	{"OBJ_PB_AAPROBE",		(ClientData) OBJ_PB_AAPROBE},
	{"OBJ_PB_CAPROBE",		(ClientData) OBJ_PB_CAPROBE},
	{"OBJ_PB_POS",			(ClientData) OBJ_PB_POS},
	{"OBJ_PB_SAPROBE",		(ClientData) OBJ_PB_SAPROBE},
	{"OBJ_PB_GSPROBE",		(ClientData) OBJ_PB_GSPROBE},
	{"OBJ_PB_STANPROBE",		(ClientData) OBJ_PB_STANPROBE},
	{"OBJ_PB_LKPROBE",		(ClientData) OBJ_PB_LKPROBE},
	{"OBJ_PB_PROBE",		(ClientData) OBJ_PB_PROBE},
	{"OBJ_PB_APROBE",		(ClientData) OBJ_PB_APROBE},
	{"OBJ_PDF_IMPULSE",		(ClientData) OBJ_PDF_IMPULSE},
	{"OBJ_PF_FIELD",		(ClientData) OBJ_PF_FIELD},
	{"OBJ_SEQ_RUN",			(ClientData) OBJ_SEQ_RUN},
	{"OBJ_SEQ_ARG",			(ClientData) OBJ_SEQ_ARG},
	{"OBJ_SEQ_POS",			(ClientData) OBJ_SEQ_POS},
	{"OBJ_PAT_PLANE",		(ClientData) OBJ_PAT_PLANE},
	{"OBJ_ICF_ATTR_DESC",		(ClientData) OBJ_ICF_ATTR_DESC},
	{"OBJ_OS_RUN",			(ClientData) OBJ_OS_RUN},
	{"OBJ_OS_STAT",			(ClientData) OBJ_OS_STAT},
	{"OBJ_CDS_LINE",		(ClientData) OBJ_CDS_LINE},
	{"OBJ_CDS_OBJECT",		(ClientData) OBJ_CDS_OBJECT},
	{"OBJ_NDDER_INTF_DESC",		(ClientData) OBJ_NDDER_INTF_DESC},
	{"OBJ_NDDER_ATTR_ASSGN",	(ClientData) OBJ_NDDER_ATTR_ASSGN},
	{"OBJ_LNKDER_INTF_DESC",	(ClientData) OBJ_LNKDER_INTF_DESC},
	{"OBJ_LNKDER_ATTR_ASSGN",	(ClientData) OBJ_LNKDER_ATTR_ASSGN},
	{"EMAC_NULL_OBJ_ID",		(ClientData) EMAC_NULL_OBJ_ID},
{"OPC_AA_IGNORE",		(ClientData) OPC_AA_IGNORE},
{"OPC_AA_SET",			(ClientData) OPC_AA_SET},
{"OPC_AA_SUPPRESS",		(ClientData) OPC_AA_SUPPRESS},
{"OPC_ANNOT_DRAW_STYLE_LINE",	(ClientData) OPC_ANNOT_DRAW_STYLE_LINE},
{"OPC_ANNOT_DRAW_STYLE_SPLINE",	(ClientData) OPC_ANNOT_DRAW_STYLE_SPLINE},
{"OPC_ANNOT_LINE_STYLE_DASHED",	(ClientData) OPC_ANNOT_LINE_STYLE_DASHED},
{"OPC_ANNOT_LINE_STYLE_SOLID",	(ClientData) OPC_ANNOT_LINE_STYLE_SOLID},
{"OPC_ANNOT_SHAPE_ELLIPSE",	(ClientData) OPC_ANNOT_SHAPE_ELLIPSE},
{"OPC_ANNOT_SHAPE_RECT",	(ClientData) OPC_ANNOT_SHAPE_RECT},
{"OPC_CDS_LINE_TYPE_COAST",	(ClientData) OPC_CDS_LINE_TYPE_COAST},
{"OPC_CDS_LINE_TYPE_IBOUND",	(ClientData) OPC_CDS_LINE_TYPE_IBOUND},
{"OPC_CDS_LINE_TYPE_NBOUND",	(ClientData) OPC_CDS_LINE_TYPE_NBOUND},
{"OPC_CDS_LINE_TYPE_RIVER",	(ClientData) OPC_CDS_LINE_TYPE_RIVER},
{"OPC_ESYS_DIR_IN",		(ClientData) OPC_ESYS_DIR_IN},
{"OPC_ESYS_DIR_INOUT",		(ClientData) OPC_ESYS_DIR_INOUT},
{"OPC_ESYS_DIR_NC",		(ClientData) OPC_ESYS_DIR_NC},
{"OPC_ESYS_DIR_OUT",		(ClientData) OPC_ESYS_DIR_OUT},
{"OPC_ESYS_SIDE_BOTTOM",	(ClientData) OPC_ESYS_SIDE_BOTTOM},
{"OPC_ESYS_SIDE_LEFT",		(ClientData) OPC_ESYS_SIDE_LEFT},
{"OPC_ESYS_SIDE_RIGHT",		(ClientData) OPC_ESYS_SIDE_RIGHT},
{"OPC_ESYS_SIDE_TOP",		(ClientData) OPC_ESYS_SIDE_TOP},
{"OPC_ESYS_TYPE_BIT",		(ClientData) OPC_ESYS_TYPE_BIT},
{"OPC_ESYS_TYPE_BOOLEAN",	(ClientData) OPC_ESYS_TYPE_BOOLEAN},
{"OPC_ESYS_TYPE_CHARACTER",	(ClientData) OPC_ESYS_TYPE_CHARACTER},
{"OPC_ESYS_TYPE_DOUBLE",	(ClientData) OPC_ESYS_TYPE_DOUBLE},
{"OPC_ESYS_TYPE_INTEGER",	(ClientData) OPC_ESYS_TYPE_INTEGER},
{"OPC_ESYS_TYPE_POINTER",	(ClientData) OPC_ESYS_TYPE_POINTER},
{"OPC_ESYS_TYPE_STDLOGIC",	(ClientData) OPC_ESYS_TYPE_STDLOGIC},
{"OPC_ESYS_TYPE_STRING",	(ClientData) OPC_ESYS_TYPE_STRING},
{"OPC_ESYS_TYPE_UNDEFINED",	(ClientData) OPC_ESYS_TYPE_UNDEFINED},
{"OPC_GFX_COLOR_BEIGE",		(ClientData) OPC_GFX_COLOR_BEIGE},
{"OPC_GFX_COLOR_BLACK",		(ClientData) OPC_GFX_COLOR_BLACK},
{"OPC_GFX_COLOR_BLUE",		(ClientData) OPC_GFX_COLOR_BLUE},
{"OPC_GFX_COLOR_BRICK",		(ClientData) OPC_GFX_COLOR_BRICK},
{"OPC_GFX_COLOR_BROWN",		(ClientData) OPC_GFX_COLOR_BROWN},
{"OPC_GFX_COLOR_CORN",		(ClientData) OPC_GFX_COLOR_CORN},
{"OPC_GFX_COLOR_CYAN",		(ClientData) OPC_GFX_COLOR_CYAN},
{"OPC_GFX_COLOR_FUCHSIA",	(ClientData) OPC_GFX_COLOR_FUCHSIA},
{"OPC_GFX_COLOR_GREEN",		(ClientData) OPC_GFX_COLOR_GREEN},
{"OPC_GFX_COLOR_GREY",		(ClientData) OPC_GFX_COLOR_GREY},
{"OPC_GFX_COLOR_MAGENTA",	(ClientData) OPC_GFX_COLOR_MAGENTA},
{"OPC_GFX_COLOR_MAROON",	(ClientData) OPC_GFX_COLOR_MAROON},
{"OPC_GFX_COLOR_OLIVE",		(ClientData) OPC_GFX_COLOR_OLIVE},
{"OPC_GFX_COLOR_ORANGE",	(ClientData) OPC_GFX_COLOR_ORANGE},
{"OPC_GFX_COLOR_ORCHID",	(ClientData) OPC_GFX_COLOR_ORCHID},
{"OPC_GFX_COLOR_PEACH",		(ClientData) OPC_GFX_COLOR_PEACH},
{"OPC_GFX_COLOR_PINK",		(ClientData) OPC_GFX_COLOR_PINK},
{"OPC_GFX_COLOR_PLUM",		(ClientData) OPC_GFX_COLOR_PLUM},
{"OPC_GFX_COLOR_PURPLE",	(ClientData) OPC_GFX_COLOR_PURPLE},
{"OPC_GFX_COLOR_RED",		(ClientData) OPC_GFX_COLOR_RED},
{"OPC_GFX_COLOR_RGB000",	(ClientData) OPC_GFX_COLOR_RGB000},
{"OPC_GFX_COLOR_RGB001",	(ClientData) OPC_GFX_COLOR_RGB001},
{"OPC_GFX_COLOR_RGB002",	(ClientData) OPC_GFX_COLOR_RGB002},
{"OPC_GFX_COLOR_RGB003",	(ClientData) OPC_GFX_COLOR_RGB003},
{"OPC_GFX_COLOR_RGB010",	(ClientData) OPC_GFX_COLOR_RGB010},
{"OPC_GFX_COLOR_RGB011",	(ClientData) OPC_GFX_COLOR_RGB011},
{"OPC_GFX_COLOR_RGB012",	(ClientData) OPC_GFX_COLOR_RGB012},
{"OPC_GFX_COLOR_RGB013",	(ClientData) OPC_GFX_COLOR_RGB013},
{"OPC_GFX_COLOR_RGB020",	(ClientData) OPC_GFX_COLOR_RGB020},
{"OPC_GFX_COLOR_RGB021",	(ClientData) OPC_GFX_COLOR_RGB021},
{"OPC_GFX_COLOR_RGB022",	(ClientData) OPC_GFX_COLOR_RGB022},
{"OPC_GFX_COLOR_RGB023",	(ClientData) OPC_GFX_COLOR_RGB023},
{"OPC_GFX_COLOR_RGB030",	(ClientData) OPC_GFX_COLOR_RGB030},
{"OPC_GFX_COLOR_RGB031",	(ClientData) OPC_GFX_COLOR_RGB031},
{"OPC_GFX_COLOR_RGB032",	(ClientData) OPC_GFX_COLOR_RGB032},
{"OPC_GFX_COLOR_RGB033",	(ClientData) OPC_GFX_COLOR_RGB033},
{"OPC_GFX_COLOR_RGB100",	(ClientData) OPC_GFX_COLOR_RGB100},
{"OPC_GFX_COLOR_RGB101",	(ClientData) OPC_GFX_COLOR_RGB101},
{"OPC_GFX_COLOR_RGB102",	(ClientData) OPC_GFX_COLOR_RGB102},
{"OPC_GFX_COLOR_RGB103",	(ClientData) OPC_GFX_COLOR_RGB103},
{"OPC_GFX_COLOR_RGB110",	(ClientData) OPC_GFX_COLOR_RGB110},
{"OPC_GFX_COLOR_RGB111",	(ClientData) OPC_GFX_COLOR_RGB111},
{"OPC_GFX_COLOR_RGB112",	(ClientData) OPC_GFX_COLOR_RGB112},
{"OPC_GFX_COLOR_RGB113",	(ClientData) OPC_GFX_COLOR_RGB113},
{"OPC_GFX_COLOR_RGB120",	(ClientData) OPC_GFX_COLOR_RGB120},
{"OPC_GFX_COLOR_RGB121",	(ClientData) OPC_GFX_COLOR_RGB121},
{"OPC_GFX_COLOR_RGB122",	(ClientData) OPC_GFX_COLOR_RGB122},
{"OPC_GFX_COLOR_RGB123",	(ClientData) OPC_GFX_COLOR_RGB123},
{"OPC_GFX_COLOR_RGB130",	(ClientData) OPC_GFX_COLOR_RGB130},
{"OPC_GFX_COLOR_RGB131",	(ClientData) OPC_GFX_COLOR_RGB131},
{"OPC_GFX_COLOR_RGB132",	(ClientData) OPC_GFX_COLOR_RGB132},
{"OPC_GFX_COLOR_RGB133",	(ClientData) OPC_GFX_COLOR_RGB133},
{"OPC_GFX_COLOR_RGB200",	(ClientData) OPC_GFX_COLOR_RGB200},
{"OPC_GFX_COLOR_RGB201",	(ClientData) OPC_GFX_COLOR_RGB201},
{"OPC_GFX_COLOR_RGB202",	(ClientData) OPC_GFX_COLOR_RGB202},
{"OPC_GFX_COLOR_RGB203",	(ClientData) OPC_GFX_COLOR_RGB203},
{"OPC_GFX_COLOR_RGB210",	(ClientData) OPC_GFX_COLOR_RGB210},
{"OPC_GFX_COLOR_RGB211",	(ClientData) OPC_GFX_COLOR_RGB211},
{"OPC_GFX_COLOR_RGB212",	(ClientData) OPC_GFX_COLOR_RGB212},
{"OPC_GFX_COLOR_RGB213",	(ClientData) OPC_GFX_COLOR_RGB213},
{"OPC_GFX_COLOR_RGB220",	(ClientData) OPC_GFX_COLOR_RGB220},
{"OPC_GFX_COLOR_RGB221",	(ClientData) OPC_GFX_COLOR_RGB221},
{"OPC_GFX_COLOR_RGB222",	(ClientData) OPC_GFX_COLOR_RGB222},
{"OPC_GFX_COLOR_RGB223",	(ClientData) OPC_GFX_COLOR_RGB223},
{"OPC_GFX_COLOR_RGB230",	(ClientData) OPC_GFX_COLOR_RGB230},
{"OPC_GFX_COLOR_RGB231",	(ClientData) OPC_GFX_COLOR_RGB231},
{"OPC_GFX_COLOR_RGB232",	(ClientData) OPC_GFX_COLOR_RGB232},
{"OPC_GFX_COLOR_RGB233",	(ClientData) OPC_GFX_COLOR_RGB233},
{"OPC_GFX_COLOR_RGB300",	(ClientData) OPC_GFX_COLOR_RGB300},
{"OPC_GFX_COLOR_RGB301",	(ClientData) OPC_GFX_COLOR_RGB301},
{"OPC_GFX_COLOR_RGB302",	(ClientData) OPC_GFX_COLOR_RGB302},
{"OPC_GFX_COLOR_RGB303",	(ClientData) OPC_GFX_COLOR_RGB303},
{"OPC_GFX_COLOR_RGB310",	(ClientData) OPC_GFX_COLOR_RGB310},
{"OPC_GFX_COLOR_RGB311",	(ClientData) OPC_GFX_COLOR_RGB311},
{"OPC_GFX_COLOR_RGB312",	(ClientData) OPC_GFX_COLOR_RGB312},
{"OPC_GFX_COLOR_RGB313",	(ClientData) OPC_GFX_COLOR_RGB313},
{"OPC_GFX_COLOR_RGB320",	(ClientData) OPC_GFX_COLOR_RGB320},
{"OPC_GFX_COLOR_RGB321",	(ClientData) OPC_GFX_COLOR_RGB321},
{"OPC_GFX_COLOR_RGB322",	(ClientData) OPC_GFX_COLOR_RGB322},
{"OPC_GFX_COLOR_RGB323",	(ClientData) OPC_GFX_COLOR_RGB323},
{"OPC_GFX_COLOR_RGB330",	(ClientData) OPC_GFX_COLOR_RGB330},
{"OPC_GFX_COLOR_RGB331",	(ClientData) OPC_GFX_COLOR_RGB331},
{"OPC_GFX_COLOR_RGB332",	(ClientData) OPC_GFX_COLOR_RGB332},
{"OPC_GFX_COLOR_RGB333",	(ClientData) OPC_GFX_COLOR_RGB333},
{"OPC_GFX_COLOR_SIENNA",	(ClientData) OPC_GFX_COLOR_SIENNA},
{"OPC_GFX_COLOR_SLATE",		(ClientData) OPC_GFX_COLOR_SLATE},
{"OPC_GFX_COLOR_STEEL",		(ClientData) OPC_GFX_COLOR_STEEL},
{"OPC_GFX_COLOR_TAN",		(ClientData) OPC_GFX_COLOR_TAN},
{"OPC_GFX_COLOR_TURQ",		(ClientData) OPC_GFX_COLOR_TURQ},
{"OPC_GFX_COLOR_VIOLET",	(ClientData) OPC_GFX_COLOR_VIOLET},
{"OPC_GFX_COLOR_WHITE",		(ClientData) OPC_GFX_COLOR_WHITE},
{"OPC_GFX_COLOR_YELLOW",	(ClientData) OPC_GFX_COLOR_YELLOW},
{"OPC_GRID_STYLE_DASHED",	(ClientData) OPC_GRID_STYLE_DASHED},
{"OPC_GRID_STYLE_NONE",		(ClientData) OPC_GRID_STYLE_NONE},
{"OPC_GRID_STYLE_SOLID",	(ClientData) OPC_GRID_STYLE_SOLID},
{"OPC_GRID_UNIT_DEG",		(ClientData) OPC_GRID_UNIT_DEG},
{"OPC_GRID_UNIT_FOOT",		(ClientData) OPC_GRID_UNIT_FOOT},
{"OPC_GRID_UNIT_KILOMETER",	(ClientData) OPC_GRID_UNIT_KILOMETER},
{"OPC_GRID_UNIT_METER",		(ClientData) OPC_GRID_UNIT_METER},
{"OPC_GRID_UNIT_MILE",		(ClientData) OPC_GRID_UNIT_MILE},
{"OPC_GRID_UNIT_NONE",		(ClientData) OPC_GRID_UNIT_NONE},
{"OPC_ICA_TYPE_DOUBLE",		(ClientData) OPC_ICA_TYPE_DOUBLE},
{"OPC_ICA_TYPE_INTEGER",	(ClientData) OPC_ICA_TYPE_INTEGER},
{"OPC_ICA_TYPE_STRUCT",		(ClientData) OPC_ICA_TYPE_STRUCT},
{"OPC_ICF_TYPE_DOUBLE",		(ClientData) OPC_ICF_TYPE_DOUBLE},
{"OPC_ICF_TYPE_INTEGER",	(ClientData) OPC_ICF_TYPE_INTEGER},
{"OPC_ICF_TYPE_STRUCT",		(ClientData) OPC_ICF_TYPE_STRUCT},
{"OPC_ID_INHRT_COMMENTS",	(ClientData) OPC_ID_INHRT_COMMENTS},
{"OPC_ID_INHRT_DEFVAL",		(ClientData) OPC_ID_INHRT_DEFVAL},
{"OPC_ID_INHRT_LIMS",		(ClientData) OPC_ID_INHRT_LIMS},
{"OPC_ID_INHRT_UNITS",		(ClientData) OPC_ID_INHRT_UNITS},
{"OPC_ID_INHRT_VAL_RESTRICT",	(ClientData) OPC_ID_INHRT_VAL_RESTRICT},
{"OPC_INTRPT_METHOD_FORCED",	(ClientData) OPC_INTRPT_METHOD_FORCED},
{"OPC_INTRPT_METHOD_QUIET",	(ClientData) OPC_INTRPT_METHOD_QUIET},
{"OPC_INTRPT_METHOD_SCHED",	(ClientData) OPC_INTRPT_METHOD_SCHED},
{"OPC_NT_LINK_ALLPKS",		(ClientData) OPC_NT_LINK_ALLPKS},
{"OPC_NT_LINK_FMTPKALL",	(ClientData) OPC_NT_LINK_FMTPKALL},
{"OPC_NT_LINK_UNFMTALL",	(ClientData) OPC_NT_LINK_UNFMTALL},
{"OPC_PKF_TYPE_DOUBLE",		(ClientData) OPC_PKF_TYPE_DOUBLE},
{"OPC_PKF_TYPE_FLOATING_POINT",	(ClientData) OPC_PKF_TYPE_FLOATING_POINT},
{"OPC_PKF_TYPE_INFO",		(ClientData) OPC_PKF_TYPE_INFO},
{"OPC_PKF_TYPE_INTEGER",	(ClientData) OPC_PKF_TYPE_INTEGER},
{"OPC_PKF_TYPE_PACKET",		(ClientData) OPC_PKF_TYPE_PACKET},
{"OPC_PKF_TYPE_STRUCT",		(ClientData) OPC_PKF_TYPE_STRUCT},
{"OPC_TEXT_SIZE_LARGE",		(ClientData) OPC_TEXT_SIZE_LARGE},
{"OPC_TEXT_SIZE_MEDIUM",	(ClientData) OPC_TEXT_SIZE_MEDIUM},
{"OPC_TEXT_SIZE_SMALL",		(ClientData) OPC_TEXT_SIZE_SMALL},
{"OPC_TRIG_FALLING_EDGE",	(ClientData) OPC_TRIG_FALLING_EDGE},
{"OPC_TRIG_HIGH_THRESH",	(ClientData) OPC_TRIG_HIGH_THRESH},
{"OPC_TRIG_LOW_THRESH",		(ClientData) OPC_TRIG_LOW_THRESH},
{"OPC_TRIG_REPEAT",		(ClientData) OPC_TRIG_REPEAT},
{"OPC_TRIG_RISING_EDGE",	(ClientData) OPC_TRIG_RISING_EDGE},
{"OPC_TRIG_ZERO_CROSSING",	(ClientData) OPC_TRIG_ZERO_CROSSING},
{"OPC_VECTOR_TYPE_DOUBLE",	(ClientData) OPC_VECTOR_TYPE_DOUBLE},
{"OPC_VECTOR_TYPE_ENUM",	(ClientData) OPC_VECTOR_TYPE_ENUM}
};



static int hash_init()
{
	Tcl_HashEntry *entrypt;
	int new;
	int i;

	/*
	 * Set up the keyword hash table and populate it
	 */
	hashpt = (Tcl_HashTable *) Tcl_Alloc( sizeof(Tcl_HashTable));
	if(!hashpt)
		return -1;

	Tcl_InitHashTable(hashpt, TCL_STRING_KEYS);

	for(i = 0; i < (sizeof(maptab)/sizeof(struct hashmap)); i++){
		/*fprintf(stderr,"hashing (%s)\n", maptab[i].key);*/
		entrypt = Tcl_CreateHashEntry(hashpt, maptab[i].key, &new);
		if(!entrypt)
			return -1;
		Tcl_SetHashValue(entrypt, maptab[i].value);
	}

	/*
	 * Set up the name to object hash table.  It will be
	 * populated at runtime
	 */
	namehash = (Tcl_HashTable *) Tcl_Alloc( sizeof(Tcl_HashTable));
	if(!namehash)
		return -1;
	Tcl_InitHashTable(namehash, TCL_STRING_KEYS);

	return 0;
}

static int hash_lookup(char *key)
{
	Tcl_HashEntry *ept;
	ClientData val;

	ept = Tcl_FindHashEntry(hashpt, key);
	if(!ept)
		return -1;

	val = Tcl_GetHashValue(ept);

	return (int) val;
	
}


static int tkEma_Init(ClientData clientData, Tcl_Interp *interp, int objc,
	Tcl_Obj *CONST objv[])
{
	int i;
	char *argv[50];
	int argc = objc;

	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	for(i = 0; i < objc; i++){
		argv[i] = Tcl_GetStringFromObj(objv[i], NULL);
	}

	Ema_Init(EMAC_MODE_NONE, argc, argv);

	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	return TCL_OK;
}

static int tkEma_Model_Attr_Add(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{

	EmaT_Model_Id model_id;
	char *attr_name;
	EmaT_Object_Id attr_properties;
	int error;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 4){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Attr_Add model_id attr_name"
			" attr_properties");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!attr_name)
		return TCL_ERROR;

	error = Tcl_GetIntFromObj(interp, objv[3], &attr_properties);
	if(error != TCL_OK)
		return error;

	Ema_Model_Attr_Add(model_id, attr_name, attr_properties);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	return TCL_OK;
}

/*
 * We can accept variable length requests by making a mongo call
 * to Ema_Model_Attr_Get and putting in our EMAC_EOL in at the
 * appropriate place.
 *
 * Multiple values are returned as a list, but single values are
 * returned as a simple result (ie, not a 1 element list) in order to
 * be consistent with historical TkEMA programs.
 *
 * This will require some rethinking in a 64 bit world, but then
 * so will a lot of this.
 */

static int tkEma_Model_Attr_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *component_string;
	char *attr_type_string;
	int error;
	typedef union {
		int ival;
		double dval;
		char sval[8192];
	} contents_t;
	int i;
	int acv_index;
	char buf[8192];

	char *attr_names[MAX_EMA_ATTR_GET];
	char attr_types[MAX_EMA_ATTR_GET];
	int  components[MAX_EMA_ATTR_GET];
	int  extended_components[MAX_EMA_ATTR_GET];
	contents_t contents[MAX_EMA_ATTR_GET];


	Tcl_Obj *resultPtr;
	Tcl_Obj *newobj;

	resultPtr = Tcl_GetObjResult(interp);

	if((objc < 5) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Get model_id "
			"attr_name attr_type component [index] ...");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	for(i = 2, acv_index = 0; i < objc ; i++, acv_index++) {

		if(acv_index == MAX_EMA_ATTR_GET) {
			sprintf(buf,
				"Too many attributes, can only get %d at once!", MAX_EMA_ATTR_GET);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_names[acv_index] = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_names[acv_index]){
			sprintf(buf, "Tcl_GetStringFromObj: Can't get attr_name[%d]\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Out of arguments, need attr type!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_type_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_type_string) {
			sprintf(buf, "Tcl_GetStringFromObj: Can't get attr type!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_types[acv_index]  = hash_lookup(attr_type_string);

		if( (++i) == objc) {
			sprintf(buf, "Out of arguments, need component spec!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		component_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!component_string){
			sprintf(buf, "Tcl_GetStringFromObj: Can't get component spec!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}


		components[acv_index]  = hash_lookup(component_string);
		if(components[acv_index]  < 0){
			sprintf(buf, "acv_index %d, unhandled component_string (%s)", acv_index,
				component_string);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		/* default */
		extended_components[acv_index] = 0;
		if(components[acv_index]  & TKEMA_COMPOUND){

			if( (++i) == objc) {
				sprintf(buf, "Out of arguments, need array index!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;
			}

			components[acv_index]  &= ~TKEMA_COMPOUND;
			error = Tcl_GetIntFromObj(interp, objv[i], &extended_components[acv_index]);
			if(error != TCL_OK) {
				sprintf(buf, "Tcl_GetIntFromObj: Can't get component index!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
			}

		}
	}

	/*
	 * We need to put an EMAC_EOL in the right place now..
	 */
	attr_names[acv_index] = (char *) EMAC_EOL;

	/*
	 * We have MAX_EMA_ATTR_GET acvs below.  It would be nice if we
	 * didn't have to list them by hand, ie if they automatically
	 * respected MAX_EMA_ATTR_GET.  As is, increasing the constant
	 * won't help here (though decreasing it should be OK)
	 */
	Ema_Model_Attr_Get(model_id,
		attr_names[0], components[0], extended_components[0], &contents[0],
		attr_names[1], components[1], extended_components[1], &contents[1],
		attr_names[2], components[2], extended_components[2], &contents[2],
		attr_names[3], components[3], extended_components[3], &contents[3],
		attr_names[4], components[4], extended_components[4], &contents[4],
		attr_names[5], components[5], extended_components[5], &contents[5],
		attr_names[6], components[6], extended_components[6], &contents[6],
		attr_names[7], components[7], extended_components[7], &contents[7],
		attr_names[8], components[8], extended_components[8], &contents[8],
		attr_names[9], components[9], extended_components[9], &contents[9],
		attr_names[10], components[10], extended_components[10], &contents[10],
		attr_names[11], components[11], extended_components[11], &contents[11],
		attr_names[12], components[12], extended_components[12], &contents[12],
		attr_names[13], components[13], extended_components[13], &contents[13],
		attr_names[14], components[14], extended_components[14], &contents[14],
		attr_names[15], components[15], extended_components[15], &contents[15],
		attr_names[16], components[16], extended_components[16], &contents[16],
		attr_names[17], components[17], extended_components[17], &contents[17],
		attr_names[18], components[18], extended_components[18], &contents[18],
		attr_names[19], components[19], extended_components[19], &contents[19],
		attr_names[20], components[20], extended_components[20], &contents[20],
		attr_names[21], components[21], extended_components[21], &contents[21],
		attr_names[22], components[22], extended_components[22], &contents[22],
		attr_names[23], components[23], extended_components[23], &contents[23],
		attr_names[24], components[24], extended_components[24], &contents[24],
		attr_names[25], components[25], extended_components[25], &contents[25],
		attr_names[26], components[26], extended_components[26], &contents[26],
		attr_names[27], components[27], extended_components[27], &contents[27],
		attr_names[28], components[28], extended_components[28], &contents[28],
		attr_names[29], components[29], extended_components[29], &contents[29],
		attr_names[30], components[30], extended_components[30], &contents[30],
		attr_names[31], components[31], extended_components[31], &contents[31],
		attr_names[32], components[32], extended_components[32], &contents[32],
		attr_names[33], components[33], extended_components[33], &contents[33],
		attr_names[34], components[34], extended_components[34], &contents[34],
		attr_names[35], components[35], extended_components[35], &contents[35],
		attr_names[36], components[36], extended_components[36], &contents[36],
		attr_names[37], components[37], extended_components[37], &contents[37],
		attr_names[38], components[38], extended_components[38], &contents[38],
		attr_names[39], components[39], extended_components[39], &contents[39],
		attr_names[40], components[40], extended_components[40], &contents[40],
		attr_names[41], components[41], extended_components[41], &contents[41],
		attr_names[42], components[42], extended_components[42], &contents[42],
		attr_names[43], components[43], extended_components[43], &contents[43],
		attr_names[44], components[44], extended_components[44], &contents[44],
		attr_names[45], components[45], extended_components[45], &contents[45],
		attr_names[46], components[46], extended_components[46], &contents[46],
		attr_names[47], components[47], extended_components[47], &contents[47],
		attr_names[48], components[48], extended_components[48], &contents[48],
		attr_names[49], components[49], extended_components[49], &contents[49],
		attr_names[50], components[50], extended_components[50], &contents[50],
		attr_names[51], components[51], extended_components[51], &contents[51],
		attr_names[52], components[52], extended_components[52], &contents[52],
		attr_names[53], components[53], extended_components[53], &contents[53],
		attr_names[54], components[54], extended_components[54], &contents[54],
		attr_names[55], components[55], extended_components[55], &contents[55],
		attr_names[56], components[56], extended_components[56], &contents[56],
		attr_names[57], components[57], extended_components[57], &contents[57],
		attr_names[58], components[58], extended_components[58], &contents[58],
		attr_names[59], components[59], extended_components[59], &contents[59],
		attr_names[60], components[60], extended_components[60], &contents[60],
		attr_names[61], components[61], extended_components[61], &contents[61],
		attr_names[62], components[62], extended_components[62], &contents[62],
		attr_names[63], components[63], extended_components[63], &contents[63],
		attr_names[64], components[64], extended_components[64], &contents[64],
		attr_names[65], components[65], extended_components[65], &contents[65],
		attr_names[66], components[66], extended_components[66], &contents[66],
		attr_names[67], components[67], extended_components[67], &contents[67],
		attr_names[68], components[68], extended_components[68], &contents[68],
		attr_names[69], components[69], extended_components[69], &contents[69],
		attr_names[70], components[70], extended_components[70], &contents[70],
		attr_names[71], components[71], extended_components[71], &contents[71],
		attr_names[72], components[72], extended_components[72], &contents[72],
		attr_names[73], components[73], extended_components[73], &contents[73],
		attr_names[74], components[74], extended_components[74], &contents[74],
		attr_names[75], components[75], extended_components[75], &contents[75],
		attr_names[76], components[76], extended_components[76], &contents[76],
		attr_names[77], components[77], extended_components[77], &contents[77],
		attr_names[78], components[78], extended_components[78], &contents[78],
		attr_names[79], components[79], extended_components[79], &contents[79],
		attr_names[80], components[80], extended_components[80], &contents[80],
		attr_names[81], components[81], extended_components[81], &contents[81],
		attr_names[82], components[82], extended_components[82], &contents[82],
		attr_names[83], components[83], extended_components[83], &contents[83],
		attr_names[84], components[84], extended_components[84], &contents[84],
		attr_names[85], components[85], extended_components[85], &contents[85],
		attr_names[86], components[86], extended_components[86], &contents[86],
		attr_names[87], components[87], extended_components[87], &contents[87],
		attr_names[88], components[88], extended_components[88], &contents[88],
		attr_names[89], components[89], extended_components[89], &contents[89],
		attr_names[90], components[90], extended_components[90], &contents[90],
		attr_names[91], components[91], extended_components[91], &contents[91],
		attr_names[92], components[92], extended_components[92], &contents[92],
		attr_names[93], components[93], extended_components[93], &contents[93],
		attr_names[94], components[94], extended_components[94], &contents[94],
		attr_names[95], components[95], extended_components[95], &contents[95],
		attr_names[96], components[96], extended_components[96], &contents[96],
		attr_names[97], components[97], extended_components[97], &contents[97],
		attr_names[98], components[98], extended_components[98], &contents[98],
		attr_names[99], components[99], extended_components[99], &contents[99],
		EMAC_EOL);

		

	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	for(i = 0; i < acv_index; i++) {

		/*
		 * Add the attribute name to the result list
		 */
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate result list object for name!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}
		Tcl_SetStringObj(newobj, attr_names[i], -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);

		/*
		 * Add the attribute value to the result list
		 */
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate result list object for value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		switch(attr_types[i]){
			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:
				Tcl_SetIntObj(newobj, contents[i].ival);
				break;
	
			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:
				Tcl_SetDoubleObj(newobj, contents[i].dval);
				break;
	
			case EMAC_STRING:
			case EMAC_DATAFILE:
				Tcl_SetStringObj(newobj, contents[i].sval, -1);
				break;
	
			default:
				Tcl_SetStringObj(resultPtr,"Unhandled type ", -1);
				Tcl_AppendStringsToObj(resultPtr, attr_type_string,
					NULL);
				return TCL_ERROR;
				break;
		}
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);
	}

	/*
	 * To be backwards compatible with existing TkEMA code, if we
	 * were only asked for 1 value, we return it in resultPtr,
	 * instead of in a list
	 */
	if(acv_index == 1) {
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for single result value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}
		/* index 0 is name, index 1 is value */
		Tcl_ListObjIndex(interp, resultPtr, 1, &newobj);
		Tcl_SetObjResult(interp, newobj);
	}

	return TCL_OK;
}

static int tkEma_Model_Attr_Print(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{

	EmaT_Model_Id model_id;
	char *attr_name;
	int error;
	Tcl_Obj *resultPtr;
	int dolist = 0;
	FILE *tmpfp;
	char buf[8192];

	resultPtr = Tcl_GetObjResult(interp);

	if( (objc != 3) && (objc != 4) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Attr_Print model_id attr_name [list]");
		return TCL_ERROR;
	}

	if(objc == 4)
		dolist = 1;

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!attr_name)
		return TCL_ERROR;

	if(dolist)
		tmpfp = diddle_stdout();

	Ema_Model_Attr_Print(model_id, attr_name);
	if(tkEma_errchk(resultPtr) < 0) {
		if(dolist)
			undiddle_stdout(tmpfp);
		return TCL_ERROR;
	}

	if(dolist) {
		Tcl_SetStringObj(resultPtr,"", -1);

		rewind(tmpfp);
		while(fgets(buf, sizeof buf, tmpfp)){
			Tcl_AppendStringsToObj(resultPtr,buf, NULL);
		}

		undiddle_stdout(tmpfp);
	}

	return TCL_OK;

}

static int tkEma_Model_Attr_Nth(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{

	EmaT_Model_Id model_id;
	int index;
	int error;
	char buf[8192];
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Attr_Nth model_id index");
		return TCL_ERROR;
	}


	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &index);
	if(error != TCL_OK)
		return error;

	Ema_Model_Attr_Nth(model_id, index, buf);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetStringObj(resultPtr,buf, -1);

	return TCL_OK;

}

static int tkEma_Model_Read(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{


	char *model_name;
	char *model_type_string;
	EmaT_Model_Type model_type;
	Tcl_Obj *resultPtr;
	EmaT_Model_Id model_id;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Read model_type model_name");
		return TCL_ERROR;
	}

	model_type_string = Tcl_GetStringFromObj(objv[1], NULL);
	if(!model_type_string)
		return TCL_ERROR;

	model_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!model_name)
		return TCL_ERROR;

	model_type = hash_lookup(model_type_string);
	if(model_type < 0){
		Tcl_SetStringObj(resultPtr,"Unknown model type ", -1);
		Tcl_AppendStringsToObj(resultPtr,model_type_string, NULL);
		return TCL_ERROR;
	}

	model_id = Ema_Model_Read(model_type, model_name);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, model_id);
	return TCL_OK;
}

/*
 * Originally, only supported setting one attribute at a
 * time, now do MAX_EMA_ATTR_SET at "once".  Unfortunately,
 * this must be done with multiple calls to the underlying
 * Ema_Model_Attr_Set function, because dummying up the
 * argument list as in Ema_Model_Attr_Get wouldn't work
 * because it would screw up the varargs handling in the
 * OPNET code.  (Since not all the value types are the
 * same size.  If only we were passing _pointers_ to
 * the argument values, then it would work..)
 */
static int tkEma_Model_Attr_Set(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *component_string;
	char *arg_string;
	Tcl_Obj *resultPtr;
	char *attr_type_string;
	int error;
	typedef union {
		int ival;
		double dval;
		char *sval;
	} contents_t;


	int i;
	int acv_index;
	char buf[8192];

	char *attr_names[MAX_EMA_ATTR_SET];
	char attr_types[MAX_EMA_ATTR_SET];
	int  components[MAX_EMA_ATTR_SET];
	int  extended_components[MAX_EMA_ATTR_SET];
	contents_t contents[MAX_EMA_ATTR_SET];



	resultPtr = Tcl_GetObjResult(interp);

	if( (objc < 6) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Attr_Set model_id "
			"attr_name attr_type component [component index] "
			"value ...");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	/*
	 * Loop through and gather up all the attribute info
	 */
	for(i = 2, acv_index = 0; i < objc ; i++, acv_index++) {

		if(acv_index == MAX_EMA_ATTR_SET) {
			sprintf(buf, "Too many attributes, can only set %d at once!",
				MAX_EMA_ATTR_SET);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_names[acv_index] = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_names[acv_index]){
			sprintf(buf, "Can't get attribute name in acv %d!\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need attr type!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_type_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_type_string){
			sprintf(buf, "Can't get attribute type in acv %d!\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_types[acv_index] = hash_lookup(attr_type_string);
		if(attr_types[acv_index] < 0){
			sprintf(buf, "Acv %d: Unhandled attr_type (%s)!", acv_index,
				attr_type_string);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need attr component!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		component_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!component_string)
			return TCL_ERROR;

		components[acv_index] = hash_lookup(component_string);
		if(components[acv_index] < 0){
			sprintf(buf,"Acv %d: Unhandled component ", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		extended_components[acv_index] = 0;
		if(components[acv_index] & TKEMA_COMPOUND){

			if( (++i) == objc) {
				sprintf(buf, "Acv %d: Out of arguments, need component index!",
					acv_index);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;
			}

			components[acv_index] &= ~TKEMA_COMPOUND;
			error = Tcl_GetIntFromObj(interp, objv[i], &extended_components[acv_index]);
			if(error != TCL_OK){
				sprintf(buf,"Acv %d: Can't get component index", acv_index);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
			}
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need value!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		/*
		 * Since we don't yet handle the more complicated component types,
		 * we have to remember to plug in a 0 for the second slot.
		 * Ie, COMP_CONTENTS really #defines to COMP_TYPE_CONTENTS, 0
		 */
		switch(attr_types[acv_index]){
			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:

				arg_string = Tcl_GetStringFromObj(objv[i],NULL);
				if(!arg_string){
					sprintf(buf,"Acv %d: Can't get arg string", acv_index);
					Tcl_SetStringObj(resultPtr, buf, -1);
					return TCL_ERROR;
				}

				contents[acv_index].ival = hash_lookup(arg_string);
				if(contents[acv_index].ival == -1){
					error = Tcl_GetIntFromObj(interp,
						objv[i], &contents[acv_index].ival);

					if(error != TCL_OK){
						sprintf(buf,"Acv %d: Can't get integer from string"
							" (%s)!", acv_index, arg_string);
						Tcl_SetStringObj(resultPtr, buf, -1);
						return error;
					}
				}

				break;
	
			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:

				error = Tcl_GetDoubleFromObj(interp, objv[i],
					&contents[acv_index].dval);

				if(error != TCL_OK){
					sprintf(buf,"Acv %d: Can't get double value!", acv_index);
					Tcl_SetStringObj(resultPtr, buf, -1);
					return error;
				}

				break;
	
			case EMAC_STRING:
			case EMAC_DATAFILE:
				contents[acv_index].sval = Tcl_GetStringFromObj(objv[i], NULL);
	
				break;

	
			default:
				sprintf(buf, "Acv %d: Unhandled type %s!", acv_index,
					attr_type_string);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;

				break;
		}
	}

	for(i = 0; i < acv_index; i++) {
		switch(attr_types[i]){

			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:

				Ema_Model_Attr_Set(model_id, attr_names[i],
					components[i], extended_components[i],
					contents[i].ival,
					EMAC_EOL);

				break;

			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:

				Ema_Model_Attr_Set(model_id,  attr_names[i],
					components[i], extended_components[i],
					contents[i].dval,
					EMAC_EOL);

				break;

			case EMAC_STRING:
			case EMAC_DATAFILE:

				Ema_Model_Attr_Set(model_id, attr_names[i],
					components[i], extended_components[i],
					contents[i].sval,
					EMAC_EOL);

				break;

			default:
				sprintf(buf,"tkEma_Model_Attr_Set: Can't happen!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
				break;
		}
				
		if(tkEma_errchk(resultPtr) < 0){
			sprintf(buf,"Acv %d", i);
			Tcl_AppendStringsToObj(resultPtr, buf, NULL);
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}


static int tkEma_Model_Base_Print(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	FILE *tmpfp;
	int dolist = 0;
	char buf[8192];

	resultPtr = Tcl_GetObjResult(interp);

	if( (objc != 1)  && (objc != 2) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Base_Print [tclresult]");
		return TCL_ERROR;
	}

	if(objc == 2)
		dolist = 1;

	if(dolist)
		tmpfp = diddle_stdout();

	Ema_Model_Base_Print();
	if(tkEma_errchk(resultPtr) < 0){
		if(dolist)
			undiddle_stdout(tmpfp);
		return TCL_ERROR;
	}

	if(dolist) {
		Tcl_SetStringObj(resultPtr,"", -1);

		rewind(tmpfp);
		while(fgets(buf, sizeof buf, tmpfp)){
			Tcl_AppendStringsToObj(resultPtr,buf, NULL);
		}

		undiddle_stdout(tmpfp);
	}
		

	return TCL_OK;
}

static int tkEma_Model_Code_Gen(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *model_name;
	int error;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Code_Gen model_id model_name");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	model_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!model_name)
		return TCL_ERROR;

	Ema_Model_Code_Gen(model_id, model_name);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;
	return TCL_OK;
}

static int tkEma_Model_Copy(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Model_Id model2_id;
	Tcl_Obj *resultPtr;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Copy model_id");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	model2_id = Ema_Model_Copy(model_id);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, model2_id);
	return TCL_OK;
}

static int tkEma_Model_Create(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *model_type_string;
	int model_type;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Create model_type");
		return TCL_ERROR;
	}

	model_type_string = Tcl_GetStringFromObj(objv[1], NULL);
	if(!model_type_string)
		return TCL_ERROR;

	model_type = hash_lookup(model_type_string);
	if(model_type < 0){
		Tcl_SetStringObj(resultPtr,"Unhandled model type ", -1);
		Tcl_AppendStringsToObj(resultPtr, model_type_string, NULL);
		return TCL_ERROR;
	}

	model_id = Ema_Model_Create(model_type);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, model_id);

	return TCL_OK;
}

static int tkEma_Model_Destroy(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	int error;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Destroy");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	Ema_Model_Destroy(model_id);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	return TCL_OK;
}

static int tkEma_Model_Print(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	int error;
	Tcl_Obj *resultPtr;
	int dolist = 0;
	FILE *tmpfp;
	char buf[8192];

	resultPtr = Tcl_GetObjResult(interp);

	if( (objc != 2) && (objc != 3) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Print model_id [tclresult]");
		return TCL_ERROR;
	}
	if(objc == 3) {
		dolist = 1;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK) {
		return error;
	}

	if(dolist)
		tmpfp = diddle_stdout();

	Ema_Model_Print(model_id);
	if(tkEma_errchk(resultPtr) < 0){
		if(dolist)
			undiddle_stdout(tmpfp);
		return TCL_ERROR;
	}

	if(dolist) {
		Tcl_SetStringObj(resultPtr,"", -1);

		rewind(tmpfp);
		while(fgets(buf, sizeof buf, tmpfp)){
			Tcl_AppendStringsToObj(resultPtr,buf, NULL);
		}

		undiddle_stdout(tmpfp);
	}

	return TCL_OK;
}

static int tkEma_Model_Write(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	Tcl_Obj *resultPtr;
	char *model_name;
	int error;
	EmaT_Compcode ret;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Write model_id model_name");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	model_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!model_name)
		return TCL_ERROR;

	ret = Ema_Model_Write(model_id, model_name);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;
	switch(ret){
		case EMAC_COMPCODE_SUCCESS:
			return TCL_OK;
			break;

		case EMAC_COMPCODE_FAILURE:
			return TCL_ERROR;
			break;
		
		default:
			return TCL_ERROR;
			break;
	}
		
}

static int tkEma_Object_Attr_Add(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	Tcl_Obj *resultPtr;
	EmaT_Object_Id attr_properties;
	char *attr_name;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 5){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Add model_id object_id "
			"attr_name attr_properties");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[3], NULL);
	if(!attr_name)
		return TCL_ERROR;

	error = Tcl_GetIntFromObj(interp, objv[4], &attr_properties);
	if(error != TCL_OK)
		return error;

	Ema_Object_Attr_Add(model_id, object_id, attr_name, attr_properties);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;
	return TCL_OK;
}

/*
 * We can accept variable length requests by making a mongo call
 * to Ema_Object_Attr_Get and putting in our EMAC_EOL in at the
 * appropriate place.
 *
 * Multiple values are returned as a list, but single values are
 * returned as a simple result (ie, not a 1 element list) in order to
 * be consistent with historical TkEMA programs.
 *
 * This will require some rethinking in a 64 bit world, but then
 * so will a lot of this.
 */

static int tkEma_Object_Attr_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	char *component_string;
	char *attr_type_string;
	int error;
	typedef union {
		int ival;
		double dval;
		char sval[8192];
	} contents_t;
	int i;
	int acv_index;
	char buf[8192];

	char *attr_names[MAX_EMA_ATTR_GET];
	char attr_types[MAX_EMA_ATTR_GET];
	int  components[MAX_EMA_ATTR_GET];
	int  extended_components[MAX_EMA_ATTR_GET];
	contents_t contents[MAX_EMA_ATTR_GET];


	Tcl_Obj *resultPtr;
	Tcl_Obj *newobj;

	resultPtr = Tcl_GetObjResult(interp);

	if((objc < 6) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Get model_id object_id "
			"attr_name attr_type component [index] ...");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	for(i = 3, acv_index = 0; i < objc ; i++, acv_index++) {

		if(acv_index == MAX_EMA_ATTR_GET) {
			sprintf(buf,
				"Too many attributes, can only get %d at once!", MAX_EMA_ATTR_GET);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_names[acv_index] = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_names[acv_index]){
			sprintf(buf, "Tcl_GetStringFromObj: Can't get attr_name[%d]\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Out of arguments, need attr type!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_type_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_type_string) {
			sprintf(buf, "Tcl_GetStringFromObj: Can't get attr type!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_types[acv_index]  = hash_lookup(attr_type_string);

		if( (++i) == objc) {
			sprintf(buf, "Out of arguments, need component spec!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		component_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!component_string){
			sprintf(buf, "Tcl_GetStringFromObj: Can't get component spec!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}


		components[acv_index]  = hash_lookup(component_string);
		if(components[acv_index]  < 0){
			sprintf(buf, "acv_index %d, unhandled component_string (%s)", acv_index,
				component_string);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		/* default */
		extended_components[acv_index] = 0;
		if(components[acv_index]  & TKEMA_COMPOUND){

			if( (++i) == objc) {
				sprintf(buf, "Out of arguments, need array index!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;
			}

			components[acv_index]  &= ~TKEMA_COMPOUND;
			error = Tcl_GetIntFromObj(interp, objv[i], &extended_components[acv_index]);
			if(error != TCL_OK) {
				sprintf(buf, "Tcl_GetIntFromObj: Can't get component index!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
			}

		}
	}

	/*
	 * We need to put an EMAC_EOL in the right place now..
	 */
	attr_names[acv_index] = (char *) EMAC_EOL;

	/*
	 * We have MAX_EMA_ATTR_GET acvs below.  It would be nice if we
	 * didn't have to list them by hand, ie if they automatically
	 * respected MAX_EMA_ATTR_GET.  As is, increasing the constant
	 * won't help here (though decreasing it should be OK)
	 */
	Ema_Object_Attr_Get(model_id, object_id,
		attr_names[0], components[0], extended_components[0], &contents[0],
		attr_names[1], components[1], extended_components[1], &contents[1],
		attr_names[2], components[2], extended_components[2], &contents[2],
		attr_names[3], components[3], extended_components[3], &contents[3],
		attr_names[4], components[4], extended_components[4], &contents[4],
		attr_names[5], components[5], extended_components[5], &contents[5],
		attr_names[6], components[6], extended_components[6], &contents[6],
		attr_names[7], components[7], extended_components[7], &contents[7],
		attr_names[8], components[8], extended_components[8], &contents[8],
		attr_names[9], components[9], extended_components[9], &contents[9],
		attr_names[10], components[10], extended_components[10], &contents[10],
		attr_names[11], components[11], extended_components[11], &contents[11],
		attr_names[12], components[12], extended_components[12], &contents[12],
		attr_names[13], components[13], extended_components[13], &contents[13],
		attr_names[14], components[14], extended_components[14], &contents[14],
		attr_names[15], components[15], extended_components[15], &contents[15],
		attr_names[16], components[16], extended_components[16], &contents[16],
		attr_names[17], components[17], extended_components[17], &contents[17],
		attr_names[18], components[18], extended_components[18], &contents[18],
		attr_names[19], components[19], extended_components[19], &contents[19],
		attr_names[20], components[20], extended_components[20], &contents[20],
		attr_names[21], components[21], extended_components[21], &contents[21],
		attr_names[22], components[22], extended_components[22], &contents[22],
		attr_names[23], components[23], extended_components[23], &contents[23],
		attr_names[24], components[24], extended_components[24], &contents[24],
		attr_names[25], components[25], extended_components[25], &contents[25],
		attr_names[26], components[26], extended_components[26], &contents[26],
		attr_names[27], components[27], extended_components[27], &contents[27],
		attr_names[28], components[28], extended_components[28], &contents[28],
		attr_names[29], components[29], extended_components[29], &contents[29],
		attr_names[30], components[30], extended_components[30], &contents[30],
		attr_names[31], components[31], extended_components[31], &contents[31],
		attr_names[32], components[32], extended_components[32], &contents[32],
		attr_names[33], components[33], extended_components[33], &contents[33],
		attr_names[34], components[34], extended_components[34], &contents[34],
		attr_names[35], components[35], extended_components[35], &contents[35],
		attr_names[36], components[36], extended_components[36], &contents[36],
		attr_names[37], components[37], extended_components[37], &contents[37],
		attr_names[38], components[38], extended_components[38], &contents[38],
		attr_names[39], components[39], extended_components[39], &contents[39],
		attr_names[40], components[40], extended_components[40], &contents[40],
		attr_names[41], components[41], extended_components[41], &contents[41],
		attr_names[42], components[42], extended_components[42], &contents[42],
		attr_names[43], components[43], extended_components[43], &contents[43],
		attr_names[44], components[44], extended_components[44], &contents[44],
		attr_names[45], components[45], extended_components[45], &contents[45],
		attr_names[46], components[46], extended_components[46], &contents[46],
		attr_names[47], components[47], extended_components[47], &contents[47],
		attr_names[48], components[48], extended_components[48], &contents[48],
		attr_names[49], components[49], extended_components[49], &contents[49],
		attr_names[50], components[50], extended_components[50], &contents[50],
		attr_names[51], components[51], extended_components[51], &contents[51],
		attr_names[52], components[52], extended_components[52], &contents[52],
		attr_names[53], components[53], extended_components[53], &contents[53],
		attr_names[54], components[54], extended_components[54], &contents[54],
		attr_names[55], components[55], extended_components[55], &contents[55],
		attr_names[56], components[56], extended_components[56], &contents[56],
		attr_names[57], components[57], extended_components[57], &contents[57],
		attr_names[58], components[58], extended_components[58], &contents[58],
		attr_names[59], components[59], extended_components[59], &contents[59],
		attr_names[60], components[60], extended_components[60], &contents[60],
		attr_names[61], components[61], extended_components[61], &contents[61],
		attr_names[62], components[62], extended_components[62], &contents[62],
		attr_names[63], components[63], extended_components[63], &contents[63],
		attr_names[64], components[64], extended_components[64], &contents[64],
		attr_names[65], components[65], extended_components[65], &contents[65],
		attr_names[66], components[66], extended_components[66], &contents[66],
		attr_names[67], components[67], extended_components[67], &contents[67],
		attr_names[68], components[68], extended_components[68], &contents[68],
		attr_names[69], components[69], extended_components[69], &contents[69],
		attr_names[70], components[70], extended_components[70], &contents[70],
		attr_names[71], components[71], extended_components[71], &contents[71],
		attr_names[72], components[72], extended_components[72], &contents[72],
		attr_names[73], components[73], extended_components[73], &contents[73],
		attr_names[74], components[74], extended_components[74], &contents[74],
		attr_names[75], components[75], extended_components[75], &contents[75],
		attr_names[76], components[76], extended_components[76], &contents[76],
		attr_names[77], components[77], extended_components[77], &contents[77],
		attr_names[78], components[78], extended_components[78], &contents[78],
		attr_names[79], components[79], extended_components[79], &contents[79],
		attr_names[80], components[80], extended_components[80], &contents[80],
		attr_names[81], components[81], extended_components[81], &contents[81],
		attr_names[82], components[82], extended_components[82], &contents[82],
		attr_names[83], components[83], extended_components[83], &contents[83],
		attr_names[84], components[84], extended_components[84], &contents[84],
		attr_names[85], components[85], extended_components[85], &contents[85],
		attr_names[86], components[86], extended_components[86], &contents[86],
		attr_names[87], components[87], extended_components[87], &contents[87],
		attr_names[88], components[88], extended_components[88], &contents[88],
		attr_names[89], components[89], extended_components[89], &contents[89],
		attr_names[90], components[90], extended_components[90], &contents[90],
		attr_names[91], components[91], extended_components[91], &contents[91],
		attr_names[92], components[92], extended_components[92], &contents[92],
		attr_names[93], components[93], extended_components[93], &contents[93],
		attr_names[94], components[94], extended_components[94], &contents[94],
		attr_names[95], components[95], extended_components[95], &contents[95],
		attr_names[96], components[96], extended_components[96], &contents[96],
		attr_names[97], components[97], extended_components[97], &contents[97],
		attr_names[98], components[98], extended_components[98], &contents[98],
		attr_names[99], components[99], extended_components[99], &contents[99],
		EMAC_EOL);

		

	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	for(i = 0; i < acv_index; i++) {

		/*
		 * Add the attribute name to the result list
		 */
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate result list object for name!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}
		Tcl_SetStringObj(newobj, attr_names[i], -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);

		/*
		 * Add the attribute value to the result list
		 */
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate result list object for value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		switch(attr_types[i]){
			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:
				Tcl_SetIntObj(newobj, contents[i].ival);
				break;
	
			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:
				Tcl_SetDoubleObj(newobj, contents[i].dval);
				break;
	
			case EMAC_STRING:
			case EMAC_DATAFILE:
				Tcl_SetStringObj(newobj, contents[i].sval, -1);
				break;
	
			default:
				Tcl_SetStringObj(resultPtr,"Unhandled type ", -1);
				Tcl_AppendStringsToObj(resultPtr, attr_type_string,
					NULL);
				return TCL_ERROR;
				break;
		}
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);
	}

	/*
	 * To be backwards compatible with existing TkEMA code, if we
	 * were only asked for 1 value, we return it in resultPtr,
	 * instead of in a list
	 */
	if(acv_index == 1) {
		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for single result value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}
		/* index 0 is name, index 1 is value */
		Tcl_ListObjIndex(interp, resultPtr, 1, &newobj);
		Tcl_SetObjResult(interp, newobj);
	}

	return TCL_OK;
}

static int tkEma_Object_Attr_Nth(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	int index;
	char attr_name[1024];
	int error;
	Tcl_Obj *resultPtr;
	int res;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 4){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_nth model_id object_id "
			"index");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[3], &index);
	if(error != TCL_OK)
		return error;

	res = Ema_Object_Attr_Nth(model_id, object_id, index, attr_name);
	if( (tkEma_errchk(resultPtr) < 0) || (res == VOSC_FAILURE) )
		return TCL_ERROR;

	Tcl_SetStringObj(resultPtr,attr_name, -1);

	return TCL_OK;
}

static int tkEma_Object_Attr_Print(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	char *attr_name;
	int error;
	Tcl_Obj *resultPtr;
	char buf[8192];
	FILE *tmpfp;
	int dolist = 0;

	resultPtr = Tcl_GetObjResult(interp);

	if( (objc != 4) && (objc != 5) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Print model_id object_id "
			"attr_name [tclresult]");
		return TCL_ERROR;
	}

	if(objc == 5)
		dolist = 1;

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[3], NULL);
	if(!attr_name)
		return TCL_ERROR;

	if(dolist)
		tmpfp = diddle_stdout();

	Ema_Object_Attr_Print(model_id, object_id, attr_name);
	if(tkEma_errchk(resultPtr) < 0){
		if(dolist)
			undiddle_stdout(tmpfp);
		return TCL_ERROR;
	}

	if(dolist) {
		Tcl_SetStringObj(resultPtr,"", -1);

		rewind(tmpfp);
		while(fgets(buf, sizeof buf, tmpfp)){
			Tcl_AppendStringsToObj(resultPtr,buf, NULL);
		}

		undiddle_stdout(tmpfp);
	}

	return TCL_OK;
}


/*
 * Originally, only supported setting one attribute at a
 * time, now do MAX_EMA_ATTR_SET at "once".  Unfortunately,
 * this must be done with multiple calls to the underlying
 * Ema_Object_Attr_Set function, because dummying up the
 * argument list as in Ema_Object_Attr_Get wouldn't work
 * because it would screw up the varargs handling in the
 * OPNET code.  (Since not all the value types are the
 * same size.  If only we were passing _pointers_ to
 * the argument values, then it would work..)
 */
static int tkEma_Object_Attr_Set(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	char *component_string;
	char *arg_string;
	Tcl_Obj *resultPtr;
	char *attr_type_string;
	int error;
	typedef union {
		int ival;
		double dval;
		char *sval;
	} contents_t;


	int i;
	int acv_index;
	char buf[8192];

	char *attr_names[MAX_EMA_ATTR_SET];
	char attr_types[MAX_EMA_ATTR_SET];
	int  components[MAX_EMA_ATTR_SET];
	int  extended_components[MAX_EMA_ATTR_SET];
	contents_t contents[MAX_EMA_ATTR_SET];



	resultPtr = Tcl_GetObjResult(interp);

	if( (objc < 7) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Set model_id object_id "
			"attr_name attr_type component [component index] "
			"value ...");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	/*
	 * Loop through and gather up all the attribute info
	 */
	for(i = 3, acv_index = 0; i < objc ; i++, acv_index++) {

		if(acv_index == MAX_EMA_ATTR_SET) {
			sprintf(buf, "Too many attributes, can only set %d at once!",
				MAX_EMA_ATTR_SET);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_names[acv_index] = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_names[acv_index]){
			sprintf(buf, "Can't get attribute name in acv %d!\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need attr type!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_type_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!attr_type_string){
			sprintf(buf, "Can't get attribute type in acv %d!\n", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		attr_types[acv_index] = hash_lookup(attr_type_string);
		if(attr_types[acv_index] < 0){
			sprintf(buf, "Acv %d: Unhandled attr_type (%s)!", acv_index,
				attr_type_string);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need attr component!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		component_string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!component_string)
			return TCL_ERROR;

		components[acv_index] = hash_lookup(component_string);
		if(components[acv_index] < 0){
			sprintf(buf,"Acv %d: Unhandled component ", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		extended_components[acv_index] = 0;
		if(components[acv_index] & TKEMA_COMPOUND){

			if( (++i) == objc) {
				sprintf(buf, "Acv %d: Out of arguments, need component index!",
					acv_index);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;
			}

			components[acv_index] &= ~TKEMA_COMPOUND;
			error = Tcl_GetIntFromObj(interp, objv[i], &extended_components[acv_index]);
			if(error != TCL_OK){
				sprintf(buf,"Acv %d: Can't get component index", acv_index);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
			}
		}

		if( (++i) == objc) {
			sprintf(buf, "Acv %d: Out of arguments, need value!", acv_index);
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		/*
		 * Since we don't yet handle the more complicated component types,
		 * we have to remember to plug in a 0 for the second slot.
		 * Ie, COMP_CONTENTS really #defines to COMP_TYPE_CONTENTS, 0
		 */
		switch(attr_types[acv_index]){
			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:

				arg_string = Tcl_GetStringFromObj(objv[i],NULL);
				if(!arg_string){
					sprintf(buf,"Acv %d: Can't get arg string", acv_index);
					Tcl_SetStringObj(resultPtr, buf, -1);
					return TCL_ERROR;
				}

				contents[acv_index].ival = hash_lookup(arg_string);
				if(contents[acv_index].ival == -1){
					error = Tcl_GetIntFromObj(interp,
						objv[i], &contents[acv_index].ival);

					if(error != TCL_OK){
						sprintf(buf,"Acv %d: Can't get integer from string"
							" (%s)!", acv_index, arg_string);
						Tcl_SetStringObj(resultPtr, buf, -1);
						return error;
					}
				}

				break;
	
			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:

				error = Tcl_GetDoubleFromObj(interp, objv[i],
					&contents[acv_index].dval);

				if(error != TCL_OK){
					sprintf(buf,"Acv %d: Can't get double value!", acv_index);
					Tcl_SetStringObj(resultPtr, buf, -1);
					return error;
				}

				break;
	
			case EMAC_STRING:
			case EMAC_DATAFILE:
				contents[acv_index].sval = Tcl_GetStringFromObj(objv[i], NULL);
	
				break;

	
			default:
				sprintf(buf, "Acv %d: Unhandled type %s!", acv_index,
					attr_type_string);
				Tcl_SetStringObj(resultPtr, buf, -1);
				return TCL_ERROR;

				break;
		}
	}

	for(i = 0; i < acv_index; i++) {
		switch(attr_types[i]){

			case EMAC_INTEGER:
			case EMAC_TOGGLE:
			case EMAC_COMPOUND:

				Ema_Object_Attr_Set(model_id, object_id, attr_names[i],
					components[i], extended_components[i],
					contents[i].ival,
					EMAC_EOL);

				break;

			case EMAC_DOUBLE:
			case EMAC_TOGGLE_DOUBLE:

				Ema_Object_Attr_Set(model_id, object_id, attr_names[i],
					components[i], extended_components[i],
					contents[i].dval,
					EMAC_EOL);

				break;

			case EMAC_STRING:
			case EMAC_DATAFILE:

				Ema_Object_Attr_Set(model_id, object_id, attr_names[i],
					components[i], extended_components[i],
					contents[i].sval,
					EMAC_EOL);

				break;

			default:
				sprintf(buf,"tkEma_Object_Attr_Set: Can't happen!");
				Tcl_SetStringObj(resultPtr, buf, -1);
				return error;
				break;
		}
				
		if(tkEma_errchk(resultPtr) < 0){
			sprintf(buf,"Acv %d", i);
			Tcl_AppendStringsToObj(resultPtr, buf, NULL);
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}


static int tkEma_Object_Copy(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	EmaT_Object_Id object2_id;
	Tcl_Obj *resultPtr;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Copy model_id object_id");
		return TCL_ERROR;
	}

 	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
 	if(error != TCL_OK)
 		return error;
 
 	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
 	if(error != TCL_OK)
 		return error;
 
	object2_id = Ema_Object_Copy(model_id, object_id);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, object2_id);
	return TCL_OK;
}

static int tkEma_Object_Create(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	Tcl_Obj *resultPtr;
	char *object_type_string;
	int object_type;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Create model_id object_type");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	object_type_string = Tcl_GetStringFromObj(objv[2], NULL);
	if(!object_type_string)
		return TCL_ERROR;

	object_type = hash_lookup(object_type_string);
	if(object_type < 0){
		Tcl_SetStringObj(resultPtr,"Unhandled object type ", -1);
		Tcl_AppendStringsToObj(resultPtr, object_type_string, NULL);
		return TCL_ERROR;
	}

	object_id = Ema_Object_Create(model_id, object_type);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, object_id);

	return TCL_OK;
}

static int tkEma_Object_Destroy(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	int error;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Destroy model_id object_id");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	Ema_Object_Destroy(model_id, object_id);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	return TCL_OK;
}

static int tkEma_Object_Move(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])

{
	EmaT_Model_Id model_id;
	EmaT_Model_Id model2_id;
	EmaT_Model_Id new_model_id;
	EmaT_Object_Id object_id;
	Tcl_Obj *resultPtr;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 4){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Move src_model_id "
			"object_id dst_model_id");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[3], &model2_id);
	if(error != TCL_OK)
		return error;


	new_model_id = Ema_Object_Move(model_id, object_id, model2_id);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;
	Tcl_SetIntObj(resultPtr, new_model_id);

	return TCL_OK;
}

static int tkEma_Object_Nth(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Type object_type;
	EmaT_Object_Id ret;
	char *object_type_string;
	int n;
	Tcl_Obj *resultPtr;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 4){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Nth model_id object_type n");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	object_type_string = Tcl_GetStringFromObj(objv[2], NULL);
	if(!object_type_string)
		return TCL_ERROR;

	object_type = hash_lookup(object_type_string);
	if(object_type < 0){
		Tcl_SetStringObj(resultPtr,"Unhandled object type ", -1);
		Tcl_AppendStringsToObj(resultPtr, object_type_string, NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[3], &n);
	if(error != TCL_OK)
		return error;

	ret = Ema_Object_Nth(model_id, object_type, n);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;
	Tcl_SetIntObj(resultPtr, ret);
	return TCL_OK;
}


static int tkEma_Object_Print(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	int error;
	Tcl_Obj *resultPtr;
	char buf[8192];
	int dolist = 0;
	FILE *tmpfp;

	resultPtr = Tcl_GetObjResult(interp);

	if( (objc != 3) && (objc != 4) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Print model_id "
			"object_id [tclresult]");
		return TCL_ERROR;
	}

	if(objc == 4)
		dolist = 1;

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	if(dolist)
		tmpfp = diddle_stdout();

	Ema_Object_Print(model_id, object_id);
	if(tkEma_errchk(resultPtr) < 0) {
		if(dolist)
			undiddle_stdout(tmpfp);
		return TCL_ERROR;
	}

	if(dolist) {
		Tcl_SetStringObj(resultPtr,"", -1);

		rewind(tmpfp);
		while(fgets(buf, sizeof buf, tmpfp)){
			Tcl_AppendStringsToObj(resultPtr,buf, NULL);
		}

		undiddle_stdout(tmpfp);
	}

	return TCL_OK;
}

static int tkEma_Object_Prom_Attr_Set(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	EmaT_Object_Id attr_properties;
	char *attr_name;
	int error;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 5){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Prom_Attr_Set model_id object_id "
			"attr_name attr_properties");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[3], NULL);
	if(!attr_name)
		return TCL_ERROR;

	error = Tcl_GetIntFromObj(interp, objv[4], &attr_properties);
	if(error != TCL_OK)
		return error;

	Ema_Object_Prom_Attr_Set(model_id, object_id, attr_name,
		attr_properties);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	return TCL_OK;
}

/*
 * How many of a particular type of object is there?
 */
static int tkEma_Model_Num_Objects(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *object_type_string;
	EmaT_Object_Type object_type;
	int num;
	Tcl_Obj *resultPtr;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Num_Objects model_id object_type");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	object_type_string = Tcl_GetStringFromObj(objv[2], NULL);
	if(!object_type_string)
		return TCL_ERROR;

	object_type = hash_lookup(object_type_string);
	if(object_type < 0){
		Tcl_SetStringObj(resultPtr,"Unhandled object type ", -1);
		Tcl_AppendStringsToObj(resultPtr, object_type_string, NULL);
		return TCL_ERROR;
	}

	Ema_Model_Attr_Get(model_id, ATTR_EMA_NUM_OBJECTS,
		COMP_OBTYPE(object_type), &num,
		EMAC_EOL);
	if(tkEma_errchk(resultPtr) < 0)
		return TCL_ERROR;

	Tcl_SetIntObj(resultPtr, num);
	return TCL_OK;
}


static int tkEma_Vos_Textlist_Create(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 1){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Create");
		return TCL_ERROR;
	}

	tpt = Vos_Textlist_Create();

	/* warning */
	Tcl_SetLongObj(resultPtr, (long) tpt);

	return TCL_OK;
}

static int tkEma_Vos_Textlist_Append(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;
	int i;
	char *string;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc < 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Append textlist string..");
		return TCL_ERROR;
	}


	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	for(i = 2; i < objc; i++){
		
		string = Tcl_GetStringFromObj(objv[i], NULL);
		if(!string)
			return TCL_ERROR;
		Vos_Textlist_Append(tpt, string, VOSC_EOL);
	}

	return TCL_OK;
}

static int tkEma_Vos_Textlist_Copy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	VosT_Textlist *tpt2;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Copy textlist");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	tpt2 = Vos_Textlist_Copy(tpt);
	/* warning */
	Tcl_SetLongObj(resultPtr, (long) tpt2);

	return TCL_OK;
}

static int tkEma_Vos_Textlist_Destroy(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Destroy textlist");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	Vos_Textlist_Destroy(tpt);

	return TCL_OK;
}

static int tkEma_Vos_Textlist_Line_Count(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;
	int i;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Line_Count textlist");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	i = Vos_Textlist_Line_Count(tpt);

	Tcl_SetIntObj(resultPtr, i);
	return TCL_OK;
}

static int tkEma_Vos_Textlist_Line_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{

	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;
	int i;
	char buf[8192];

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Line_Get textlist linenum");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &i);
	if(error != TCL_OK)
		return error;

	Vos_Textlist_Line_Get(tpt, i, buf);

	Tcl_SetStringObj(resultPtr,buf, -1);

	return TCL_OK;
}


static int tkEma_Vos_Textlist_Line_Set(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{

	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;
	int i;
	char *string;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 4){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Line_Set textlist linenum string");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	error = Tcl_GetIntFromObj(interp, objv[2], &i);
	if(error != TCL_OK)
		return error;

	string = Tcl_GetStringFromObj(objv[3], NULL);
	if(!string)
		return TCL_ERROR;

	Vos_Textlist_Line_Set(tpt, i, string);

	return TCL_OK;
}



static int tkEma_Vos_Textlist_Print(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{

	Tcl_Obj *resultPtr;
	VosT_Textlist *tpt;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Vos_Textlist_Print textlist");
		return TCL_ERROR;
	}

	error = Tcl_GetLongFromObj(interp, objv[1], (long *)&tpt);
	if(error != TCL_OK)
		return error;

	Vos_Textlist_Print(tpt);

	return TCL_OK;
}



static int tkEma_Object_Store(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Object_Id object_id;
	char *name;
	int error;
	Tcl_Obj *resultPtr;
	Tcl_HashEntry *entrypt;
	int new;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Store object_id name");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &object_id);
	if(error != TCL_OK)
		return error;

	name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!name)
		return TCL_ERROR;

	if(Tcl_FindHashEntry(namehash, name)){
		Tcl_SetStringObj(resultPtr,"Name already in table", -1);
		return TCL_ERROR;
	}

	entrypt = Tcl_CreateHashEntry(namehash, name, &new);
	Tcl_SetHashValue(entrypt, (ClientData) object_id);

	return TCL_OK;
}



static int tkEma_Object_Get(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Object_Id object_id;
	char *name;
	Tcl_Obj *resultPtr;
	Tcl_HashEntry *entrypt;
	char buf[1024];

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Get name");
		return TCL_ERROR;
	}

	name = Tcl_GetStringFromObj(objv[1], NULL);
	if(!name)
		return TCL_ERROR;

	if(!(entrypt = Tcl_FindHashEntry(namehash, name))){
		sprintf(buf,"Name (%s) not in table", name);
		Tcl_SetStringObj(resultPtr, buf , -1);
		return TCL_ERROR;
	}

	object_id = (EmaT_Object_Id) Tcl_GetHashValue(entrypt);

	Tcl_SetLongObj(resultPtr, object_id);

	return TCL_OK;
}

/*
 * Return a numeric based on an EMA #defined constant
 */
static int tkEma_Const(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	char *const_name;
	int const_value;
	Tcl_Obj *resultPtr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 2){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Const symbolic_constant");
		return TCL_ERROR;
	}

	const_name = Tcl_GetStringFromObj(objv[1], NULL);
	if(!const_name)
		return TCL_ERROR;

	const_value = hash_lookup(const_name);
	if(const_value < 0) {
		Tcl_SetStringObj(resultPtr, "Unknown constant ", -1);
		Tcl_AppendStringsToObj(resultPtr, const_name, NULL);
		return TCL_ERROR;
	}

	Tcl_SetLongObj(resultPtr, const_value);

	return TCL_OK;
}


/*
 *
 * This function stolen from tims_ema:
 *
 * In opnet 3, attributes are now full-fledged objects.  This is a
 * pain and affects all Ema_Object_Attr_Add and Ema_Object_Prom_Attr_Set
 * calls.  To decrease our dolefulness, here is a function which can
 * be embedded in those calls to make them similar to before.  Maybe
 *
 */
static int tkEma_Attr_Make(ClientData clientData, Tcl_Interp *interp,
	int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	char *attr_name;
	char *attr_type_string;
	int attr_type;
	char *units;
	Tcl_Obj *resultPtr;
	int error;

	EmaT_Object_Id	props;
	VosT_Textlist   *comments;
	char	*svalue;
	double	dvalue;
	int		ivalue;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 6){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Make_Attr model_id name type units df_val");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK)
		return error;

	attr_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!attr_name)
		return TCL_ERROR;

	attr_type_string = Tcl_GetStringFromObj(objv[3], NULL);
	if(!attr_name)
		return TCL_ERROR;

	attr_type = hash_lookup(attr_type_string);
	if(attr_type < 0){
		Tcl_SetStringObj(resultPtr,"Unhandled attr type ", -1);
		Tcl_AppendStringsToObj(resultPtr, attr_type_string, NULL);
		return TCL_ERROR;
	}

	units = Tcl_GetStringFromObj(objv[4], NULL);
	if(!units)
		return TCL_ERROR;

	

	props = Ema_Object_Create(model_id, OBJ_ATTR_PROPS);
	comments = Vos_Textlist_Create();

	Ema_Object_Attr_Set(model_id, props,
		"public name",	COMP_CONTENTS,		attr_name,
		"units",	COMP_CONTENTS,		units,
		"comments",	COMP_CONTENTS,		comments,
		"high limit",	COMP_CONTENTS,		0.0,
		"low limit",	COMP_CONTENTS,		0.0,
		"symbol map list",	COMP_INTENDED,	EMAC_DISABLED,
		"flags",	COMP_CONTENTS,		0,
		"data type",	COMP_CONTENTS,		attr_type,
		"count properties",	COMP_INTENDED,	EMAC_DISABLED,
		"list attribute definitions",	COMP_INTENDED, EMAC_DISABLED,
		EMAC_EOL);



	switch(attr_type){
		
		case EMAC_INTEGER:
			error = Tcl_GetIntFromObj(interp, objv[5], &ivalue);
			if(error != TCL_OK)
				return error;
			
			Ema_Object_Attr_Set(model_id, props,
				"default value",COMP_CONTENTS_TYPE, attr_type,
				"default value",COMP_CONTENTS, ivalue,
				EMAC_EOL);

			if(tkEma_errchk(resultPtr) < 0)
				return TCL_ERROR;
			break;

		case EMAC_DOUBLE:
			error = Tcl_GetDoubleFromObj(interp, objv[5], &dvalue);
			if(error != TCL_OK)
				return error;

			Ema_Object_Attr_Set(model_id, props,
				"default value",COMP_CONTENTS_TYPE, attr_type,
				"default value",COMP_CONTENTS, dvalue,
				EMAC_EOL);

			if(tkEma_errchk(resultPtr) < 0)
				return TCL_ERROR;
			break;

		case EMAC_STRING:
			svalue = Tcl_GetStringFromObj(objv[5], NULL);
			if(!svalue)
				return TCL_ERROR;

			Ema_Object_Attr_Set(model_id, props,
				"default value",COMP_CONTENTS_TYPE, attr_type,
				"default value",COMP_CONTENTS, svalue,
				EMAC_EOL);

			if(tkEma_errchk(resultPtr) < 0)
				return TCL_ERROR;
			break;

		case EMAC_TOGGLE:
			error = Tcl_GetIntFromObj(interp, objv[5], &ivalue);
			if(error != TCL_OK)
				return error;

			Ema_Object_Attr_Set(model_id, props,
				"default value",COMP_CONTENTS_TYPE,EMAC_TOGGLE,
				"default value", COMP_TOGGLE, ivalue,
				/*"default value",COMP_TOGGLE, ivalue,*/
				/*"default value",COMP_CONTENTS, ivalue,*/
				EMAC_EOL);

			if(tkEma_errchk(resultPtr) < 0)
				return TCL_ERROR;
			break;

		case EMAC_DATAFILE:
			svalue = Tcl_GetStringFromObj(objv[5], NULL);
			if(!svalue)
				return TCL_ERROR;

			Ema_Object_Attr_Set(model_id, props,
				"default value",COMP_CONTENTS_TYPE, attr_type,
				"default value",COMP_CONTENTS, svalue,
				EMAC_EOL);

			if(tkEma_errchk(resultPtr) < 0)
				return TCL_ERROR;
			break;
				
		default:
			Tcl_SetStringObj(resultPtr,"Unhandled attr type ", -1);
			Tcl_AppendStringsToObj(resultPtr, attr_type_string,
				NULL);
			return TCL_ERROR;
			break;
	}

	Tcl_SetIntObj(resultPtr, props);
	return TCL_OK;
}


/*
 * Check for an Ema error and if found, add to the result
 */
static int tkEma_errchk(Tcl_Obj *resultPtr)
{
	if(EmaS_Oper_Status == EMAC_COMPCODE_FAILURE){
		if(strlen(EmaS_Error_Condition) > 0)
			Tcl_AppendToObj(resultPtr, EmaS_Error_Condition, -1);
		return -1;
	}

	return 0;
}

#if 0
/*
 * This section is strictly to get rid of some compiler warnings due to
 * some things declared in Opnet's header files, but never used anywhere
 */
static void never_call_me()
{
	Prohandle   fooProhandle;
	Evhandle fooEvhandle;
	Evhandle barEvhandle;
	Evhandle bazEvhandle;
	Evhandle quuxEvhandle;
	Stathandle  fooStathandle;






	fooProhandle = _SimI_Temp_Prohandle;
	fooEvhandle = _SimI_Ev_Temp_Evhandle;
	barEvhandle = _SimI_Irpt_Temp_Evhandle;
	bazEvhandle = _SimI_Pk_Temp_Evhandle;
	quuxEvhandle = _SimI_Esys_Temp_Evhandle;
	fooStathandle = _SimI_Temp_Stathandle;


}
#endif




/*
 * create a prg_list and return the pointer
 */
static int tkEma_prg_list_create(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	Prg_List *lptr;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 1){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: prt_list_create");
		return TCL_ERROR;
	}

	lptr = (Prg_List *) prg_list_create ();
	if(!lptr) {
		Tcl_SetStringObj(resultPtr, "Can't create list ", -1);
		return TCL_ERROR;
	}

	Tcl_SetLongObj(resultPtr, (long) lptr);

	return TCL_OK;
}



/*
 * Append strings to a prg_list.  Right now, we only handle 1 string..
 */
static int tkEma_prg_list_strings_append(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr;
	Prg_List *lptr;
	char *svalue;
	int error;

	resultPtr = Tcl_GetObjResult(interp);

	if(objc != 3){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: prt_list_strings_append lptr string");
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], (int *)&lptr);
	if(error != TCL_OK)
		return error;

	svalue = Tcl_GetStringFromObj(objv[2], NULL);
	if(!svalue)
		return TCL_ERROR;

	if( prg_list_strings_append(lptr, svalue, PRGC_NIL) != 
		PrgC_Compcode_Success){
			Tcl_SetStringObj(resultPtr, "Can't append string ", -1);
			Tcl_AppendStringsToObj(resultPtr, svalue, NULL);
			return TCL_ERROR;
	}

	Tcl_SetLongObj(resultPtr, (long) lptr);

	return TCL_OK;
}



/*
 * Return information about an object attribute as a
 * TCL list.  OPNET has made this much harder to do than
 * necessary.  We have to diddle stdout and use
 * Ema_Object_Attr_Print, plus the results are dependant
 * on the ASCII formatting not changing..
 */
static int tkEma_Object_Attr_Info(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	EmaT_Object_Id object_id;
	int error;
	char buf[8192];
	FILE *tmpfp;
	char *attr_name;
	char *key;
	char *value;
	int i;
	Tcl_Obj *resultPtr;
	Tcl_Obj *newobj;

	resultPtr = Tcl_GetObjResult(interp);

	if((objc != 4) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Object_Attr_Info model_id object_id "
			"attr_name");
		return TCL_ERROR;
	}

	/*
	 * Diddle stdout
	 */
	tmpfp = diddle_stdout();
	if(!tmpfp) {
		sprintf(buf, "Can't open diddle stdout!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK) {
		sprintf(buf, "Can't get model_id!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return error;
	}

	error = Tcl_GetIntFromObj(interp, objv[2], &object_id);
	if(error != TCL_OK){
		sprintf(buf, "Can't get object_id!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return error;
	}

	attr_name = Tcl_GetStringFromObj(objv[3], NULL);
	if(!attr_name) {
		sprintf(buf, "Tcl_GetStringFromObj: Can't get attr_name!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	error = 0;

	Ema_Object_Attr_Print(model_id, object_id, attr_name);

	/*
	 * Undiddle and return..
	 */
	if(tkEma_errchk(resultPtr) < 0){
		tmpfp = undiddle_stdout(tmpfp);
		if(!tmpfp) {
			sprintf(buf, "Can't reset stdout!");
			Tcl_AppendStringsToObj(resultPtr, buf, NULL);
		}
		return TCL_ERROR;
	}

	/*
	 * OK, if we got this far, then our information
	 * should be in our tempfile..
	 */

	rewind(tmpfp);
	
	while(fgets(buf, sizeof buf, tmpfp)){
		if(buf[0] == '-')
			continue;

		/*
		 * This will be true for arrays.
		 * We don't want the info for each element
		 * it will just confuse us
		 */
		if(buf[0] == ' ')
			break;

		for(i = 0; i < strlen(buf); i++){
			if( (buf[i] == '\n') || (buf[i] == '\r')){
				buf[i] = '\0';
			}
		}

		/*
		 * This is very bogus and depends on
		 * counting characters in the output
		 * I should probably just return the
		 * whole line and parse it at the Tcl level...
		 */

		key = &buf[7];
		for(i = 7; i < strlen(buf); i++){
			if(buf[i] == ':'){
				buf[i] = '\0';
				break;
			}
		}

		++i;
		while(buf[i]) {
			if(buf[i] != ' ')
				break;

			++i;
		}

		value = &buf[i];
		/*
		 * If we have "value" we just want value
		 */
		if( (value[0]) == '"' && (value[strlen(value) -1] == '"') ) {
			value[strlen(value) -1] = '\0';
			++value;
		}


		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for key value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		Tcl_SetStringObj(newobj, key, -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);


		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for value value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		Tcl_SetStringObj(newobj, value, -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);
	}

	/*
	 * Undiddle stdout
	 */
	tmpfp = undiddle_stdout(tmpfp);
	if(!tmpfp) {
		sprintf(buf, "Can't reset stdout!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	return TCL_OK;
	
}


/*
 * Return information about a model attribute as a
 * TCL list.  OPNET has made this much harder to do than
 * necessary.  We have to diddle stdout and use
 * Ema_Model_Attr_Print, plus the results are dependant
 * on the ASCII formatting not changing..
 */
static int tkEma_Model_Attr_Info(ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	EmaT_Model_Id model_id;
	int error;
	char buf[8192];
	FILE *tmpfp;
	char *attr_name;
	char *key;
	char *value;
	int i;
	Tcl_Obj *resultPtr;
	Tcl_Obj *newobj;

	resultPtr = Tcl_GetObjResult(interp);

	if((objc != 3) ){
		Tcl_WrongNumArgs(interp, 1, objv,
			"Usage: Ema_Model_Attr_Info model_id "
			"attr_name");
		return TCL_ERROR;
	}

	/*
	 * Diddle stdout
	 */
	tmpfp = diddle_stdout();
	if(!tmpfp) {
		sprintf(buf, "Can't open diddle stdout!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp, objv[1], &model_id);
	if(error != TCL_OK) {
		sprintf(buf, "Can't get model_id!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return error;
	}

	attr_name = Tcl_GetStringFromObj(objv[2], NULL);
	if(!attr_name) {
		sprintf(buf, "Tcl_GetStringFromObj: Can't get attr_name!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	error = 0;

	Ema_Model_Attr_Print(model_id, attr_name);

	/*
	 * Undiddle and return..
	 */
	if(tkEma_errchk(resultPtr) < 0){
		tmpfp = undiddle_stdout(tmpfp);
		if(!tmpfp) {
			sprintf(buf, "Can't reset stdout!");
			Tcl_AppendStringsToObj(resultPtr, buf, NULL);
		}
		return TCL_ERROR;
	}

	/*
	 * OK, if we got this far, then our information
	 * should be in our tempfile..
	 */

	rewind(tmpfp);
	
	while(fgets(buf, sizeof buf, tmpfp)){
		if(buf[0] == '-')
			continue;

		/*
		 * This will be true for arrays.
		 * We don't want the info for each element
		 * it will just confuse us
		 */
		if(buf[0] == ' ')
			break;

		for(i = 0; i < strlen(buf); i++){
			if( (buf[i] == '\n') || (buf[i] == '\r')){
				buf[i] = '\0';
			}
		}

		/*
		 * This is very bogus and depends on
		 * counting characters in the output
		 * I should probably just return the
		 * whole line and parse it at the Tcl level...
		 */

		key = &buf[7];
		for(i = 7; i < strlen(buf); i++){
			if(buf[i] == ':'){
				buf[i] = '\0';
				break;
			}
		}

		++i;
		while(buf[i]) {
			if(buf[i] != ' ')
				break;

			++i;
		}

		value = &buf[i];
		/*
		 * If we have "value" we just want value
		 */
		if( (value[0]) == '"' && (value[strlen(value) -1] == '"') ) {
			value[strlen(value) -1] = '\0';
			++value;
		}


		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for key value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		Tcl_SetStringObj(newobj, key, -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);


		newobj = Tcl_NewObj();
		if(!newobj) {
			sprintf(buf, "Tcl_NewObj: Can't allocate object for value value!");
			Tcl_SetStringObj(resultPtr, buf, -1);
			return TCL_ERROR;
		}

		Tcl_SetStringObj(newobj, value, -1);
		Tcl_ListObjAppendElement(interp, resultPtr, newobj);
	}

	/*
	 * Undiddle stdout
	 */
	tmpfp = undiddle_stdout(tmpfp);
	if(!tmpfp) {
		sprintf(buf, "Can't reset stdout!");
		Tcl_SetStringObj(resultPtr, buf, -1);
		return TCL_ERROR;
	}

	return TCL_OK;
	
}


/*
 * This is iffy I suspect using the dup()
 * interface on Windows..
 */
static int save_stdout_fd = -1;

static FILE *diddle_stdout()
{
	FILE *tmpfp;

	save_stdout_fd = dup(fileno(stdout));
	if(save_stdout_fd < 0){
		perror("dup"); /* won't get communicated back to tcl..*/
		return NULL;
	}

	fflush(stdout);
	fclose(stdout);

	/*
	 * Because it is lowest, tmpfp should replace
	 * stdout..
	 */
	tmpfp = tmpfile();
	if(!tmpfp) {
		return NULL;
	}

	return tmpfp;
}

static FILE *undiddle_stdout(FILE *tmpfp)
{
	int new_stdout_fd;

	fflush(tmpfp);
	fclose(tmpfp);

	new_stdout_fd = dup(save_stdout_fd);
	close(save_stdout_fd);
	tmpfp = fdopen(new_stdout_fd, "w");

	if(!tmpfp) {
		return NULL;
	}

	return tmpfp;
}
