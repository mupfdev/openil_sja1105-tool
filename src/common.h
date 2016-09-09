/******************************************************************************
 * Copyright (c) 2016, NXP Semiconductors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef _SJA1105_TOOL_COMMON_H
#define _SJA1105_TOOL_COMMON_H

#define SJA1105_CONF_FILE "/etc/sja1105/sja1105.conf"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define TRACE do { \
	printf("%s %d\n", __FILE__, __LINE__); \
} while(0);

#define min(x, y) (((x) < (y)) ? (x) : (y))

#define MAX_LINE_SIZE 2048

int   get_multiline_buf_width(char *buf);
int   get_entry_count_to_fit_screen(char **print_bufs, int count);
void  show_print_bufs(char **print_bufs, int count);
char *trimwhitespace(char *str);
int   matches(const char*, const char*);
int   get_match(const char*, const char**, int);
void  formatted_append(char *buffer, char *width_fmt, char *fmt, ...);
void  linewise_concat(char **buffers, int count);
void  print_array(char *print_buf, uint64_t *array, int count);
int   read_array(char *array_str, uint64_t *array_val, int max_count);
int   reliable_uint64_from_string(uint64_t *to, char *from, char**);

struct general_config {
	char *staging_area;
	int   screen_width;
	int   entries_per_line;
	int   verbose;
	int   debug;
};

extern struct general_config general_config;

#endif
