/*
 *  CMFL Event Scheduler
 *  Copyright (c) 2013 David M. Syzdek <david@syzdek.net>.
 *
 *  @CMFL_BSD_LICENSE_START@
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of David M. Syzdek nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  @CMFL_BSD_LICENSE_END@
 */
/**
 *  Displays variables available to the CGIs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern char **environ;

const char * html_unsafe_printf(char * dst, const char * src, size_t len);


const char * html_unsafe_printf(char * dst, const char * src, size_t len)
{
   size_t i;
   size_t o;

   dst[0] = '\0';
   len--;

   o = 0;
   for(i = 0; ((src[i] != '\0') && (o < len)); i++)
   {
      switch(src[i])
      {
         case ' ':
         strncat(&dst[o], "&nbsp;", len-o);
         o += 6;
         break;

         case '&':
         strncat(&dst[o], "&amp;", len-o);
         o += 5;
         break;

         case '<':
         strncat(&dst[o], "&lt;", len-o);
         o += 4;
         break;

         case '>':
         strncat(&dst[o], "&gt;", len-o);
         o += 4;
         break;

         case '"':
         strncat(&dst[o], "&quot;", len-o);
         o += 6;
         break;

         case '\'':
         strncat(&dst[o], "&#x27;", len-o);
         o += 6;
         break;

         case '/':
         strncat(&dst[o], "&#x2F;", len-o);
         o += 6;
         break;

         default:
         dst[o+0] = src[i];
         dst[o+1] = '\0';
         o++;
         break;
      };
   };

   o = (o >= len) ? len : o;

   dst[o] = '\0';

   return(dst);
}


int main(void)
{
   char       * string;
   char       * namep;
   char       * valuep;
   char         name[1024];
   char         value[1024];
   char       * ptr;
   int          i;

   // prints HTML header
   printf("Content-Type: text/html;charset=us-ascii\n\n");

   // prints table start
   printf("<html>\n");
   printf("  <header>\n");
   printf("    <title>CGI Variables</title>\n");
   printf("  </header>\n");
   printf("  <body>\n");

   // prints variables
   printf("    <h2>CGI/FCGI Variables</h2>\n");
   printf("    <table border=1>\n");
   printf("      <tr><td align=center>Name</td><td align=center>Value</td></tr>\n");
   for(i = 0; environ[i]; i++)
   {
      // copies value
      if ((string = strdup(environ[i])) == NULL)
      {
         perror("strdup()");
         return(1);
      };

      // splits value
      if ((ptr = index(string, '=')) == NULL)
      {
         free(string);
         continue;
      };
      ptr[0] = '\0';

      // prints values
      printf("      <tr>\n");
      printf("        <td>%s</td>\n", html_unsafe_printf(name,  string,  1024));
      printf("        <td>%s</td>\n", html_unsafe_printf(value, &ptr[1], 1024));
      printf("      </tr>\n");

      free(string);
   };
   printf("    </table>\n");

   // prints query string
   printf("    <h2>Query String</h2>\n");
   printf("    <table border=1>\n");
   printf("      <tr><td align=center>Name</td><td align=center>Value</td></tr>\n");
   string = getenv("QUERY_STRING");
   while((string))
   {
      // splits value
      if ((valuep = index(string, '=')) == NULL)
      {
         string = index(string, '&');
         continue;
      };
      valuep[0] = '\0';
      namep     = string;
      valuep    = &valuep[1];

      if ((string = index(valuep, '&')) != NULL)
      {
         string[0] = '\0';
         string    = &string[1];
      };

      // prints values
      printf("      <tr>\n");
      printf("        <td>%s</td>\n", html_unsafe_printf(name,  namep,  1024));
      printf("        <td>%s</td>\n", html_unsafe_printf(value, valuep, 1024));
      printf("      </tr>\n");
   };
   printf("    </table>\n");

   // prints table start
   printf("  </body>\n");
   printf("</html>\n\n");

   return(0);
}

/* end of source */