/*	TraduSCI - Sierra SCI1.1/SCI32 games translator
 *  Copyright (C) Enrico Rolfi 'Endroz', 2003-2008.
 *
 *	graphs.cpp - Used to draw graphs in VCG tool
 *	
 */
 
 
#include "common.h"

void printGraphString(FILE *gfile, const char *source)
{
     bool carriage = false;
     
     if (source == 0)
        return;
 
     for(long k=0; k<strlen(source); k++)
     {
         
        if (source[k] < 32 || source[k] > 126)     
        //for ANSI extended characters conversion 
            fprintf(gfile, "\\fi%03d", (unsigned char)source[k]);   
        else    
                    switch (source[k])
                    {
                      case '\"':
                      case '\\':
                      //CHECK are there other special characters to handle?
                             fprintf(gfile, "\\%c", source[k]);
                             break;
                      case ' ':
                      if (carriage)
                      {
                       carriage = false;
                       fprintf(gfile, "\n");
                      }
                      default:
                              fputc(source[k], gfile);
                    }
        
        if ((k+1) % 45 == 0)    //to break long labels
           carriage = true;
           
     
     }
     

}

unsigned char talkercolors[32];
int colorcount=1;
unsigned char gimmieacolorfultalker(unsigned char imtalker)
{       

         unsigned char mytalkercolor = 0;
         
         talkercolors[0]=99;

            //search color
            int col = 0;
            while (col<colorcount)
            {
                if (talkercolors[col]==imtalker)
                {
                   mytalkercolor=col;
                   break;
                }   
                col++;
             }   
            if (col>=colorcount)
            {  
               if (colorcount<32)
               {
                   talkercolors[colorcount] = imtalker;
                   mytalkercolor=colorcount;
                   colorcount++;
               }
               else   //more than 32 unique talkers!! but we only set 32 useable colors in VCG
                  {} //NOTE nothing has to be done.... 0 is returned
            }

         
         return mytalkercolor;
}


void printTalkerLabel(FILE *gfile, unsigned char talker)
{
    char *tname = msgObj->getTalkerName(talker);  
    if (tname)
    {
              fprintf(gfile, "\\fu");
              printGraphString(gfile, tname);
              fprintf(gfile, "\\fn"); 
              printGraphString(gfile, ": ");
    }

}


void AddClonedStrings(SortedMsg csmsg, const char *strapp, FILE *graphile, bool translmsg)
{           //RECURSIVE function to resolve clones
              
              fprintf(graphile, GRAPH_CLONEOF, csmsg.noun, csmsg.verb, csmsg.ncase); 
              
              unsigned short k = 0;
              while (k < msgObj->getPhrasesCount())
              {
                  if ((msgObj->getNoun(k) == csmsg.noun) &&
                      (msgObj->getVerb(k) == csmsg.verb) &&
                      (msgObj->getCase(k) == csmsg.ncase))
                      break;
                  k++;
              }
              
              
              unsigned short z = k;
              while (z < msgObj->getPhrasesCount()) //has been found
              {
               //SEARCH AND ADD STUFF HERE
               if ((msgObj->getNoun(z) != csmsg.noun) ||
                   (msgObj->getVerb(z) != csmsg.verb) ||
                   (msgObj->getCase(z) != csmsg.ncase))
                   break;
               
                SortedMsg clonechildmsg;
                clonechildmsg.noun = msgObj->getCloneNoun(z);
                clonechildmsg.verb = msgObj->getCloneVerb(z);
                clonechildmsg.ncase = msgObj->getCloneCase(z); 
                if (!(clonechildmsg.noun+clonechildmsg.verb+clonechildmsg.ncase))
                {
                   fprintf(graphile, "\\f31\n\n\\fu(%d)\n\\f%02d",msgObj->getFileIndex(z)+1,gimmieacolorfultalker(msgObj->getTalker(z)));
                   strapp = (translmsg ? msgObj->getTranslation(z) : msgObj->getPhrase(z));
                   printTalkerLabel(graphile, msgObj->getTalker(z));
                   printGraphString(graphile, strapp);
                } else {
                  fprintf(graphile, "\n");
                  AddClonedStrings(clonechildmsg, strapp, graphile, translmsg);  
                }
                z++;
              }
}

void DrawNounGraph(HWND hwnd, int sorting, bool colors)
{
   
   char oldsorting = msgObj->sorting();
   unsigned short tindex = msgObj->getFileIndex(msgObj->currentString());
   if (oldsorting != sorting)
      msgObj->sortBy(sorting);
   
   
   char graphilepath[MAX_PATH];
   sprintf(graphilepath, "%s\\graph.gdl", szAppDir);                                           
   FILE *graphile = fopen(graphilepath, "wb");     
   if (graphile!=NULL)
   {
     fprintf(graphile, "graph: { \n title : \"%s\" \n orientation: left_to_right \n node_alignment: top \n xspace: 5 \n yspace: 15 \n layoutalgorithm : tree \n smanhattan_edges: yes spreadlevel: 6 \n finetuning:no \n ", strrchr(szFileName, '\\')+1);
     fprintf(graphile, "color: lightgrey \n colorentry 21: 90 20 200 \n colorentry 7: 200 110 0 \n colorentry 8: 170 150 120 \n colorentry 1: 115 180 255 \n");                                        //CHECK leave finetuning off?
     fprintf(graphile, "colorentry 22: 160 80 0 \n colorentry 24: 65 65 65 \n colorentry 25: 0 0 128 \n colorentry 26: 128 0 0 \n colorentry 27: 0 128 0 \n");
     fprintf(graphile, "colorentry 28: 128 128 0 \n colorentry 29: 128 0 128 \n colorentry 30: 0 128 128 \n colorentry 20: 180 32 240 \n colorentry 10: 154 205 50 \n");
     fprintf(graphile, "colorentry 11: 255 192 203 \n colorentry 12: 255 165 0 \n colorentry 13: 218 165 214 \n colorentry 32:100 100 100 \n colorentry 9: 255 150 255 \n");

     fprintf(graphile, "node: {title: \"Base 0\" label: \"%s\" color: 32 textcolor: white vertical_order: %d}\n", strrchr(szFileName, '\\')+1, (sorting == ByTalker ?0:1));
    
      SortedMsg smsg, oldsmsg, clonesmsg;
     const char *strapp = 0;
     unsigned short actualnoun, actualverb, actualcase, baseid = 0;
     char basename[10];
     strcpy(basename, ((sorting == ByTalker) ? "Talker" : "Base"));   
     
                         
     for (unsigned short i = 0; i<msgObj->getPhrasesCount(); i++)
     {
         oldsmsg = smsg;
         
         smsg.index = msgObj->getFileIndex(i);
         smsg.noun = msgObj->getNoun(i);
         smsg.verb = msgObj->getVerb(i);
         smsg.ncase = msgObj->getCase(i);
         smsg.seq = msgObj->getSeq(i);
         smsg.talker = msgObj->getTalker(i);
     
         //hack for the first node to be inserted
         if (i==0)
         {  
           oldsmsg.noun = 255 - smsg.noun;   //so it won't match smsg in the switch case:)
           oldsmsg.verb = 255 - smsg.verb;   //so it won't match :)
           oldsmsg.ncase = 255 - smsg.ncase;   //so it won't match  :)
           oldsmsg.talker = 255 - smsg.talker;   //so it won't match  :)
         }
         
         switch (sorting)
         {
         case ByIndex:
              //nothing has to be done
              break;
         case ByTalker:
              if (smsg.talker != oldsmsg.talker)
              {
                 fprintf(graphile, "node: {title: \"Talker %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 1 horizontal_order: %d}\n",
                                    i, GRAPH_TALKER, smsg.talker, (msgObj->getTalkerName(smsg.talker)?msgObj->getTalkerName(smsg.talker):""), smsg.talker);
                 fprintf(graphile, "edge: {sourcename: \"Base 0\" targetname: \"Talker %d\"}\n", i);  
                 baseid = i;       //actual talker    
              }
         case ByNoun:               
              if ((smsg.noun != oldsmsg.noun)||((sorting == ByTalker) && (smsg.talker != oldsmsg.talker)))
              {
                 fprintf(graphile, "node: {title: \"Noun %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 2 horizontal_order: %d}\n", 
                                   i, GRAPH_NOUN, smsg.noun, (msgObj->getNounName(smsg.noun)?msgObj->getNounName(smsg.noun):""), smsg.noun);
                 fprintf(graphile, "edge: {sourcename: \"%s %d\" targetname: \"Noun %d\"}\n", basename, baseid, i);  
                 actualnoun = i;           
              }
         
              if ((smsg.verb != oldsmsg.verb)||(smsg.noun != oldsmsg.noun))
              {
                 fprintf(graphile, "node: {title: \"Verb %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 3 horizontal_order: %d}\n", 
                                   i, GRAPH_VERB, smsg.verb, (msgObj->getVerbName(smsg.verb)?msgObj->getVerbName(smsg.verb):""), smsg.verb);  
                 fprintf(graphile, "edge: {sourcename: \"Noun %d\" targetname: \"Verb %d\"}\n", actualnoun, i);  
                 actualverb = i;                     
              }
                 
              if ((smsg.ncase != oldsmsg.ncase)||(smsg.verb != oldsmsg.verb)||(smsg.noun != oldsmsg.noun))
              {                
                 fprintf(graphile, "node: {title: \"Case %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 4 horizontal_order: %d}\n", 
                                   i, GRAPH_CASE, smsg.ncase, (msgObj->getCaseName(smsg.ncase)?msgObj->getCaseName(smsg.ncase):""), smsg.ncase);              
                 fprintf(graphile, "edge: {sourcename: \"Verb %d\" targetname: \"Case %d\"}\n", actualverb, i);
                 actualcase = i;                 
              } 
              
              break;
              
         case ByVerb:
              if (smsg.verb != oldsmsg.verb)
              {
                 fprintf(graphile, "node: {title: \"Verb %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 2 horizontal_order: %d}\n", 
                                   i, GRAPH_VERB, smsg.verb, (msgObj->getVerbName(smsg.verb)?msgObj->getVerbName(smsg.verb):""), smsg.verb);
                 fprintf(graphile, "edge: {sourcename: \"Base 0\" targetname: \"Verb %d\"}\n", i); 
                 actualverb = i;           
              }
         
              if ((smsg.verb != oldsmsg.verb)||(smsg.noun != oldsmsg.noun))
              {
                 fprintf(graphile, "node: {title: \"Noun %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 3 horizontal_order: %d}\n", 
                                   i, GRAPH_NOUN, smsg.noun, (msgObj->getNounName(smsg.noun)?msgObj->getNounName(smsg.noun):""), smsg.noun);  
                 fprintf(graphile, "edge: {sourcename: \"Verb %d\" targetname: \"Noun %d\"}\n", actualverb, i);  
                 actualnoun = i;                     
              }
                 
              if ((smsg.ncase != oldsmsg.ncase)||(smsg.verb != oldsmsg.verb)||(smsg.noun != oldsmsg.noun))
              {                
                 fprintf(graphile, "node: {title: \"Case %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 4 horizontal_order: %d}\n", 
                                   i, GRAPH_CASE, smsg.ncase, (msgObj->getCaseName(smsg.ncase)?msgObj->getCaseName(smsg.ncase):""), smsg.ncase);              
                 fprintf(graphile, "edge: {sourcename: \"Noun %d\" targetname: \"Case %d\"}\n", actualnoun, i);
                 actualcase = i;                 
              } 
              
              break;
              
         case ByCase:
              if (smsg.ncase != oldsmsg.ncase)
              {
                 fprintf(graphile, "node: {title: \"Case %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 2 horizontal_order: %d}\n", 
                                   i, GRAPH_CASE, smsg.ncase, (msgObj->getCaseName(smsg.ncase)?msgObj->getCaseName(smsg.ncase):""), smsg.ncase);
                 fprintf(graphile, "edge: {sourcename: \"Base 0\" targetname: \"Case %d\"}\n", i); 
                 actualcase = i;           
              }
         
              if ((smsg.verb != oldsmsg.verb)||(smsg.ncase != oldsmsg.ncase))
              {
                 fprintf(graphile, "node: {title: \"Verb %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 3 horizontal_order: %d}\n", 
                                   i, GRAPH_VERB, smsg.verb, (msgObj->getVerbName(smsg.verb)?msgObj->getVerbName(smsg.verb):""), smsg.verb);  
                 fprintf(graphile, "edge: {sourcename: \"Case %d\" targetname: \"Verb %d\"}\n", actualcase, i);  
                 actualverb = i;                     
              }
                 
              if ((smsg.ncase != oldsmsg.ncase)||(smsg.verb != oldsmsg.verb)||(smsg.noun != oldsmsg.noun))
              {                
                 fprintf(graphile, "node: {title: \"Noun %d\" label: \"%s %d\n%s\" color: 32 textcolor: white vertical_order: 4 horizontal_order: %d}\n", 
                                   i, GRAPH_NOUN, smsg.noun, (msgObj->getNounName(smsg.noun)?msgObj->getNounName(smsg.noun):""), smsg.noun);              
                 fprintf(graphile, "edge: {sourcename: \"Verb %d\" targetname: \"Noun %d\"}\n", actualverb, i);
                 actualnoun = i;                 
              } 
              
              break;
         }
         

         gimmieacolorfultalker(smsg.talker);
       
         fprintf(graphile, "node: {title: \"Text %d\" %s label: \"%d\n\\f%02d", i, (tindex==smsg.index ?"bordercolor: red borderwidth: 4":""),smsg.index+1, gimmieacolorfultalker(smsg.talker));           
         //NOTE the whole cloned sequence is shown
                  
         clonesmsg.noun = msgObj->getCloneNoun(i);
         clonesmsg.verb = msgObj->getCloneVerb(i);
         clonesmsg.ncase = msgObj->getCloneCase(i);

         if (((msgObj->getVersion() & 0xFFFFFF00) >= SCI_05) && 
         (clonesmsg.noun + clonesmsg.verb + clonesmsg.ncase))  //if clones are supported and this is a clone
         {     //ADD CLONED STRINGS HERE
              
             AddClonedStrings(clonesmsg, strapp, graphile, false);              
             
             strapp = 0;
              
               ///////// 
         }else
         {
           printTalkerLabel(graphile, smsg.talker);
           strapp = msgObj->getPhrase(i);
         }
         
         printGraphString(graphile, strapp);
         fprintf(graphile, "\" color: 32 textcolor: white horizontal_order:%d}\n", i);
            
                   
                       
         if ((smsg.noun == oldsmsg.noun) && (smsg.verb == oldsmsg.verb) && (smsg.ncase == oldsmsg.ncase))                                       
             fprintf(graphile, "nearedge: {sourcename: \"Text %d\" targetname: \"Text %d\" linestyle: invisible}\n", i-1, i); 
         else                                                                              
             switch (sorting) 
             {
             case ByIndex:
                  fprintf(graphile, "edge: {sourcename: \"Base 0\" targetname: \"Text %d\"}\n", i);                              
                  break;
             case ByNoun:
             case ByVerb:
             case ByTalker:
                  fprintf(graphile, "edge: {sourcename: \"Case %d\" targetname: \"Text %d\"}\n", actualcase, i);                              
                  break;
             case ByCase:
                  fprintf(graphile, "edge: {sourcename: \"Noun %d\" targetname: \"Text %d\"}\n", actualnoun, i);                              
                  break;

             }
             /////////////////////  translation
         
                 
         if (strapp || (clonesmsg.noun + clonesmsg.verb + clonesmsg.ncase))  
         {
            fprintf(graphile, "node: {title: \"Translation %d\" %s label: \"%d\n\\f%02d", i, (tindex==smsg.index ?"bordercolor: red borderwidth: 4":""), smsg.index+1, gimmieacolorfultalker(smsg.talker));           
            if (((msgObj->getVersion() & 0xFFFFFF00) >= SCI_05) && 
                (clonesmsg.noun + clonesmsg.verb + clonesmsg.ncase))
             {                                   
                               
              AddClonedStrings(clonesmsg, strapp, graphile, true);
              
              strapp = 0;
    
            } else
            {
              printTalkerLabel(graphile, smsg.talker);
              strapp = msgObj->getTranslation(i);           
            }                  
            printGraphString(graphile, strapp);
            fprintf(graphile, "\" color: 32 textcolor: white}\n");
            fprintf(graphile, "edge: {sourcename: \"Text %d\" targetname: \"Translation %d\"}\n", i, i); 
         }
         ///////////////////////////////////////////////////
                  
      }
     
         
     fprintf(graphile, "}");
     fclose(graphile);
     
     //reset initial sorting    
     if (oldsorting != sorting)
     {
        msgObj->sortBy(oldsorting);
        msgObj->currentString(msgObj->findListIndex(tindex)); //or the updatestrindex will overwrite the ACTUAL index
        updateStrIndex(hwnd, msgObj->currentString());
     }
      
     char szGraphPath[MAX_PATH];
     sprintf(szGraphPath, "%s\\wingraph32.exe", szAppDir);
     if (colors)
        ShellExecute(hwnd, "open", szGraphPath, "graph.gdl", szAppDir, SW_MAXIMIZE);
     else
        ShellExecute(hwnd, "open", szGraphPath, " -blackwhite graph.gdl", szAppDir, SW_MAXIMIZE);
  
   }
   else
   {
    MessageBox(hwnd, GRAPH_CANTCREATEFILE, ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
   }
   
}

