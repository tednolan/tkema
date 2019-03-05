#
#	$Id: Makefile,v 1.2 2001/06/12 17:59:14 ted Exp $
#
# Create html, PostScript and pdf from the manual pages.  Probably should be
# an install option
#
GS=/usr/local/cluster/gs5.10/bin/gs

.SUFFIXES: .html .n

# suffix rule
.n.html:
		../bin/make_html $<


SRC= \
Ema_Attr_Make.n \
Ema_Const.n \
Ema_Init.n \
Ema_Model_Attr_Add.n \
Ema_Model_Attr_Get.n \
Ema_Model_Attr_Nth.n \
Ema_Model_Attr_Print.n \
Ema_Model_Attr_Set.n \
Ema_Model_Base_Print.n \
Ema_Model_Code_Gen.n \
Ema_Model_Copy.n \
Ema_Model_Create.n \
Ema_Model_Destroy.n \
Ema_Model_Print.n \
Ema_Model_Read.n \
Ema_Model_Write.n \
Ema_Object_Attr_Add.n \
Ema_Object_Attr_Get.n \
Ema_Object_Attr_Nth.n \
Ema_Object_Attr_Print.n \
Ema_Object_Attr_Set.n \
Ema_Object_Copy.n \
Ema_Object_Create.n \
Ema_Object_Destroy.n \
Ema_Object_Get.n \
Ema_Object_Move.n \
Ema_Object_Nth.n \
Ema_Object_Print.n \
Ema_Object_Prom_Attr_Set.n \
Ema_Object_Store.n \
Vos_Textlist_Append.n \
Vos_Textlist_Copy.n \
Vos_Textlist_Create.n \
Vos_Textlist_Destroy.n \
Vos_Textlist_Line_Count.n \
Vos_Textlist_Line_Get.n \
Vos_Textlist_Line_Set.n \
Vos_Textlist_Print.n \
prg_list_create.n \
prg_list_strings_append.n \
Ema_Object_Attr_Info.n \
Ema_Model_Attr_Info.n 

HTML= \
Ema_Attr_Make.html \
Ema_Const.html \
Ema_Init.html \
Ema_Model_Attr_Add.html \
Ema_Model_Attr_Get.html \
Ema_Model_Attr_Nth.html \
Ema_Model_Attr_Print.html \
Ema_Model_Attr_Set.html \
Ema_Model_Base_Print.html \
Ema_Model_Code_Gen.html \
Ema_Model_Copy.html \
Ema_Model_Create.html \
Ema_Model_Destroy.html \
Ema_Model_Print.html \
Ema_Model_Read.html \
Ema_Model_Write.html \
Ema_Object_Attr_Add.html \
Ema_Object_Attr_Get.html \
Ema_Object_Attr_Nth.html \
Ema_Object_Attr_Print.html \
Ema_Object_Attr_Set.html \
Ema_Object_Copy.html \
Ema_Object_Create.html \
Ema_Object_Destroy.html \
Ema_Object_Get.html \
Ema_Object_Move.html \
Ema_Object_Nth.html \
Ema_Object_Print.html \
Ema_Object_Prom_Attr_Set.html \
Ema_Object_Store.html \
Vos_Textlist_Append.html \
Vos_Textlist_Copy.html \
Vos_Textlist_Create.html \
Vos_Textlist_Destroy.html \
Vos_Textlist_Line_Count.html \
Vos_Textlist_Line_Get.html \
Vos_Textlist_Line_Set.html \
Vos_Textlist_Print.html \
prg_list_create.html \
prg_list_strings_append.html \
Ema_Object_Attr_Info.html \
Ema_Model_Attr_Info.html 

all: html tkEMA_Man.ps tkEMA_Man.pdf index.html

tkEMA_Man.ps: $(SRC)
	proff -man $(SRC) > tkEMA_Man.ps

tkEMA_Man.pdf: tkEMA_Man.ps
	cat tkEMA_Man.ps | $(GS) -q -sDEVICE=pdfwrite -sOutputFile=tkEMA_Man.pdf -

html: $(HTML)

index.html: $(HTML)
	../bin/make_index

install:
	cp *.html ../html

clean:
	/bin/rm -f *.html tkEMA_Man.ps tkEMA_Man.pdf
