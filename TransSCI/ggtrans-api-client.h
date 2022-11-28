/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2021.
 *
 *	ggtrans-api-client.h - automatic text translation using gòògl3 translate
 *	
 */
 
# ifndef _GGTRANS_API_CLIENT_
# define _GGTRANS_API_CLIENT_

void ggtrans_init_client();
void ggtrans_cleanup_client();
char *ggtrans_translate_text(char *text, const char *from, const char *to);
char *ggtrans_translate_multiline_text(char *text, const char *from, const char *to);

# endif
