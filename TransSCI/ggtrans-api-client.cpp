/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2021.
 *
 *	ggtrans-api-client.c - automatic text translation using gòògl3 translate
 *	
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <curl/curl.h>
# include "ggtrans-api-client.h"


//#define URL "http://translate.googleapis.com/translate_a/single?client=gtx&ie=UTF-8&oe=UTF-8&dt=bd&dt=ex&dt=ld&dt=md&dt=rw&dt=rm&dt=ss&dt=t&dt=at&dt=qca&hl=it"

#define URL "http://translate.googleapis.com/translate_a/single?client=gtx&ie=UTF-8&oe=UTF-8&dt=t" //&hl=it"


# define DONT_VERIFY_PEER_SSL 0

CURL *curl = 0;

struct string
{
    char *ptr;
    int len;
};

#ifndef strtok_r
/* 
 * public domain strtok_r() by Charlie Gordon
 *
 *   from comp.lang.c  9/14/2007
 *
 *      http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
 *
 *     (Declaration that it's public domain):
 *      http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
 */

char* strtok_r(
    char *str, 
    const char *delim, 
    char **nextp)
{
    char *ret;

    if (str == NULL)
    {
        str = *nextp;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    ret = str;

    str += strcspn(str, delim);

    if (*str)
    {
        *str++ = '\0';
    }

    *nextp = str;

    return ret;
}

#endif


void init_string(struct string *s);
size_t write_callback(char *ptr, size_t size, size_t nmemb, struct string *s);
char *get_translated_text_from_response(char *response);

void ggtrans_init_client()
{
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
}

void ggtrans_cleanup_client()
{
    if (curl)
    {
        curl_easy_cleanup(curl);
        curl = 0;
    }
}



char *ggtrans_translate_text(char *text, const char *from, const char *to)
{
    //struct curl_slist *chunk = NULL;
    struct string response;

    char *url_to_send = NULL;

    char *text_encoded = curl_easy_escape( curl, text, 0);

    url_to_send = (char *) malloc (strlen(URL)+strlen(text_encoded)+50);

    sprintf(url_to_send,"%s&sl=%s&tl=%s&q=%s", URL, from, to, text_encoded);

    CURLcode res = CURLE_FAILED_INIT;

    init_string(&response);

   /* chunk = curl_slist_append(chunk, "Content-Type:application/json");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, DONT_VERIFY_PEER_SSL);
    curl_easy_setopt(curl, CURLOPT_URL, URL_WITH_KEY);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, build_json(text, from, to));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char*)&response);
*/
    //chunk = curl_slist_append(chunk, "Content-Type:application/json");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, DONT_VERIFY_PEER_SSL);
    curl_easy_setopt(curl, CURLOPT_URL, url_to_send);
    //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, build_json(text, from, to));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char*)&response);


    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        fprintf(stderr, "%s", curl_easy_strerror(res));

//    curl_slist_free_all(chunk);

    free(url_to_send);

    curl_free(text_encoded);
    return get_translated_text_from_response(response.ptr);
}


char *ggtrans_translate_multiline_text(char *text, const char *from, const char *to)
{
    char *str = NULL;
    char *text_copy = (char *) malloc(strlen(text)+10);
    char *dest = (char *) malloc(1);
    char *rest = NULL;
    char *translated = NULL;

    dest[0] = 0;

    strcpy(text_copy, text);
    rest = text_copy;

    while (str = strtok_r(NULL,"\n",&rest))
    {
        translated = ggtrans_translate_text(str, from, to);

        if (translated != NULL)
        {
            dest = (char *) realloc(dest, strlen(dest)+strlen(translated)+10);
            if (dest == NULL)
                return NULL;

            if (dest[0])
                strcat(dest, "\r\n");
            strcat(dest, translated);

        }
        
        Sleep(2000);    //to avoid gòòg13 blocking us
    
    }

    free(text_copy);

    return dest;
}


void init_string(struct string *s)
{
    s->len = 0;
    s->ptr = (char *) malloc(1);

    if(s->ptr == NULL) {
        fprintf(stderr, "malloc() failed.\n");
        exit(1);
    }

    s->ptr[0] = '\0';
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = (char *) realloc(s->ptr, new_len+1);

    if (s->ptr == NULL) {
      fprintf(stderr, "realloc() failed\n");
      exit(1);
    }

    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

//function to replace chars
char *replace_chars(char *line, const char *search, const char *replace)
{
	int sLen = strlen(search);
	int rLen = strlen(replace);
	int count = 1;
	char *sp = line; // start of pattern

	if (sLen < rLen) {
		// string enlargement not supported
		return line;
	}
	
	do
	{
		if ((sp = strstr(sp, search)) != NULL)
		{
			
			if (sLen > rLen)
			{
				// move from right to left
				char *src = sp + sLen;
				char *dst = sp + rLen;
				while((*dst = *src) != '\0') 
				{ 
					dst++; 
					src++;
				}
			} 
			
			memcpy(sp, replace, rLen);

			sp += rLen;
		}
	} while (sp);
	
	return line;


}

char *get_translated_text_from_response(char *response)
{
	//try to avoid crashes
	if (response == NULL)
	{
		return (char*)"";
	}

	response = replace_chars(response, "\\\"", "#$");

	char *returned_string = (char *) malloc(strlen(response));
    char *rest = response;
    char *res = strtok_r(response, "\"", &rest);
	char *old_res;
	
	returned_string[0] = 0;
	
    if (res)
    {
        res = strtok_r(NULL, "\"", &rest);
		old_res = res;
		
		while (res)
    	{
			int i = 0;
			
			for (i=0; i<8; ++i)
			{
				res = strtok_r(NULL, "\"", &rest);
				if (res == NULL)
					break;
		
				if (i==2)
				{	
					if (strstr(res, ",0") || strstr(res, ",1") || strstr(res, ",2"))
					{
						//alternative cases that occur when audio spelling misses, or so it seems
						res = strtok_r(NULL, "\"", &rest);
						break;
					}
				}
			}

			if (res)
			{
				if (returned_string[0] != 0)
				{
					strcat(returned_string, " ");	
				}
				strcat(returned_string, old_res);	

				old_res = res;
			}
			else if (returned_string[0] == 0)
			{
				strcpy(returned_string, old_res);
			}

		}
		
	}

	
	returned_string = replace_chars(returned_string, "#$", "\"");

	
	printf("Result: %s\n", returned_string);


    return returned_string;
}
