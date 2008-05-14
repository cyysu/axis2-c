/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <axiom_mime_parser.h>
#include <axutil_string.h>
#include <axiom_data_handler.h>
#include <stdio.h>
#include <axutil_http_chunked_stream.h>
#include <ctype.h>

struct axiom_mime_parser
{
    axutil_hash_t *mime_parts_map;
    int soap_body_len;
    axis2_char_t *soap_body_str;
    int chunk_buffer_size;
    int max_chunk_buffers;
};

AXIS2_EXTERN axiom_mime_parser_t *AXIS2_CALL
axiom_mime_parser_create(
    const axutil_env_t *env)
{
    axiom_mime_parser_t *mime_parser = NULL;

    AXIS2_ENV_CHECK(env, NULL);
    mime_parser = (axiom_mime_parser_t *)
        AXIS2_MALLOC(env->allocator, sizeof(axiom_mime_parser_t));

    if (!mime_parser)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
            "No memory. Cannot create MIME parser");
        return NULL;
    }

    mime_parser->mime_parts_map = NULL;
    mime_parser->soap_body_len = 0;
    mime_parser->soap_body_str = NULL;
    mime_parser->chunk_buffer_size = 1;
    mime_parser->max_chunk_buffers = AXIOM_MIME_PARSER_MAX_CHUNK_BUFFERS;

    mime_parser->mime_parts_map = axutil_hash_make(env);
    if (!(mime_parser->mime_parts_map))
    {
        axiom_mime_parser_free(mime_parser, env);
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
            "Failed in creating MIME parts map. Cannot create MIME parser");
        return NULL;
    }

    return mime_parser;
}

AXIS2_EXTERN void AXIS2_CALL
axiom_mime_parser_free(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env)
{
    /* This map is passed on to SOAP builder, and SOAP builder take over the
     * ownership of the map
     */
    mime_parser->mime_parts_map = NULL;

    /* The MIME parser doesn't own the SOAP Body string */
    mime_parser->soap_body_str = NULL;

    if (mime_parser)
    {
        AXIS2_FREE(env->allocator, mime_parser);
    }

    return;
}

AXIS2_EXTERN axutil_hash_t *AXIS2_CALL
axiom_mime_parser_parse(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env,
    AXIS2_READ_INPUT_CALLBACK callback,
    void *callback_ctx,
    axis2_char_t *mime_boundary)
{
    axis2_char_t *buffer = NULL;
    int size = AXIOM_MIME_PARSER_BUFFER_SIZE;
    int len = 0;
    axis2_char_t *root_mime = NULL;
    axis2_char_t *soap_body_str = NULL;
    int soap_body_len = 0;
    axis2_char_t *body_mime = NULL;
    axis2_char_t *mime_binary = NULL;
    int mime_binary_len = 0;
    axis2_char_t *pos = NULL;
    axis2_bool_t end_of_mime = AXIS2_FALSE;
    int count = 0;
    int read = 0;
    int buf_num = 0;    
    int buf_len = 0;
    axis2_callback_info_t *cb_ctx = NULL;
    axis2_char_t **buf_array = NULL;
    int *len_array = NULL;

    cb_ctx = (axis2_callback_info_t *) callback_ctx;

    if (cb_ctx->chunked_stream)
    {
        buf_array = AXIS2_MALLOC(env->allocator,
            sizeof(axis2_char_t *) * (mime_parser->max_chunk_buffers));

        if (!buf_array)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "No memory. Failed in creating buffer array");
            return NULL;
        }

        len_array = AXIS2_MALLOC(env->allocator, 
            sizeof(int) * (mime_parser->max_chunk_buffers));
 
        if (!len_array)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "No memory. Failed in creating length array");
            return NULL;
        }

        size = (mime_parser->chunk_buffer_size) * AXIOM_MIME_PARSER_BUFFER_SIZE;

        while (!axutil_http_chunked_stream_get_end_of_chunks(
            cb_ctx->chunked_stream, env))
        {
            read = 0;
            
            if (buf_num > (mime_parser->max_chunk_buffers - 1))
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                    "Attachment size exceeds "
                    "the system MTOM configuration parameters");
                return NULL;
            }

            buf_array[buf_num] = AXIS2_MALLOC(env->allocator,
                sizeof(axis2_char_t) * (size + 1));
            if (!buf_array[buf_num])
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                    "No memory. Failed in creating buffer");
                return NULL;
            }
            do
            {
                read += len;
                len = callback(buf_array[buf_num] + read, size - read,
                    (void *) callback_ctx);
            }
            while ((read < (size-1)) && (len > 0));

            *(buf_array[buf_num] + read) = '\0';
            len_array[buf_num] = read;
            buf_num++;
        }
        /* If we have only one buffer */
        if (buf_num == 1)
        {
            buffer = buf_array[buf_num - 1];
        }
        else
        {
            int i = 0;
            int temp = 0;
            buf_len = 0;

            for (i = 0; i < buf_num; i++)
            {
                if (buf_array[i])
                {
                    buf_len += len_array[i];    
                }
            }
            buffer = AXIS2_MALLOC(env->allocator, sizeof(char) * (buf_len + 1));

            if (!buffer)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                    "No memory. Failed in creating buffer");
                return NULL;
            }

            for (i = 0; i < buf_num; i++)
            {
                if (buf_array[i])
                {
                    memcpy(buffer + temp, buf_array[i], len_array[i]);
                    temp += len_array[i];
                    AXIS2_FREE(env->allocator, buf_array[i]);
                    buf_array[i] = NULL;
                }
            }
            *(buffer + temp) = '\0';
        }
    }
    else
    {
        size = cb_ctx->content_length + 100;
        buffer = AXIS2_MALLOC(env->allocator,
            sizeof(axis2_char_t) * (size + 1));
        if (!buffer)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "No memory. Failed in creating buffer");
            return NULL;
        }
        do
        {
            read += len;
            len = callback(buffer + read, size - read, (void *) callback_ctx);
        }
        while ((read < size) && (len > 0));

        buffer[read+1] = '\0';
        buf_len = read;

        if (buf_len < cb_ctx->content_length)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "Some data in the message was not recieved");
            return NULL;
        }
    }

    if (buffer)
    {
        pos = axutil_strstr(buffer, AXIS2_CRLF AXIS2_CRLF);
        if (pos)
        {
            root_mime = pos + 4;
            pos = NULL;
        }
    }

    if (root_mime)
    {
        pos = axutil_strstr(root_mime, mime_boundary);
    }

    if (pos)
    {
        pos -= 2;
        soap_body_len = pos - root_mime;
        soap_body_str = AXIS2_MALLOC(env->allocator,
            sizeof(char) * (soap_body_len + 1));
        if (!soap_body_str)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                "No memory. Failed in SOAP body string");
            return NULL;
        }
        memcpy(soap_body_str, root_mime, soap_body_len);
        soap_body_str[soap_body_len] = '\0';
        body_mime = pos;
        mime_parser->soap_body_len = soap_body_len;
        mime_parser->soap_body_str = soap_body_str;
    }
    
    while (!end_of_mime && count < AXIOM_MIME_PARSER_END_OF_MIME_MAX_COUNT)
    {
        axis2_char_t *temp_body_mime = NULL;
        axis2_char_t *temp_mime_binary = NULL;
        axis2_char_t *temp_pos = NULL;

        pos = NULL;

        /* Keep track of the count to ensure that we do not go in an infinite 
         * loop. It is possible that we could fall into an infinite loop if 
         * there are problems in sender's message format
         */
        count++;

        if (body_mime)
        {
            pos = axutil_strstr(body_mime, AXIS2_CRLF AXIS2_CRLF);
        }

        if (pos)
        {
            axis2_char_t *old_pos = NULL;

            temp_mime_binary = pos + 4;
            old_pos = temp_mime_binary;    
            do
            {
                pos = NULL;
            
                if (old_pos) 
                {
                    pos = memchr(old_pos, AXIOM_MIME_BOUNDARY_BYTE,
                        buffer + buf_len - old_pos); 
                }

                if (pos)
                {
                    old_pos = pos + 1;
                    temp_pos = axutil_strstr(pos + 1, mime_boundary);
                }
            }
            while (pos && (*(pos + 1) != AXIOM_MIME_BOUNDARY_BYTE ||
                temp_pos != pos + 2));
        }
                
        if (pos && (*(pos + 1) == AXIOM_MIME_BOUNDARY_BYTE) && 
            (temp_pos == pos + 2))
        {
            mime_binary_len = (int)(pos - temp_mime_binary);
            temp_pos = pos + 2 + axutil_strlen(mime_boundary);

            end_of_mime = (AXIOM_MIME_BOUNDARY_BYTE == *(temp_pos)) &&
                (AXIOM_MIME_BOUNDARY_BYTE == *(temp_pos + 1));

            temp_body_mime = pos;
            mime_binary = AXIS2_MALLOC(env->allocator,
                sizeof(char) * (mime_binary_len + 1));
            if (!mime_binary)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                    "No memory. Failed in creating binary stream");
                return NULL;
            }
            memcpy(mime_binary, temp_mime_binary, mime_binary_len);
            mime_binary[mime_binary_len] = '\0';
        }

        if (mime_parser->mime_parts_map)
        {
            axis2_char_t *id = NULL;
            axis2_char_t *type = NULL;

            /* Get the MIME ID */
            if (body_mime)
            {
                id = axutil_strcasestr(body_mime, AXIOM_MIME_HEADER_CONTENT_ID);
                type = axutil_strcasestr(body_mime,
                    AXIOM_MIME_HEADER_CONTENT_TYPE);
                if (type)
                {
                    axis2_char_t *end = NULL;
                    axis2_char_t *temp_type = NULL;
                    type += axutil_strlen(AXIOM_MIME_HEADER_CONTENT_TYPE);
                    while (type && *type && *type != ':')
                    {
                        type++;
                    }
                    type++;
                    while (type && *type && *type == ' ')
                    {
                        type++;
                    }
                    end = type;
                    while (end && *end && !isspace(*end))
                    {
                        end++;
                    }
                    if ((end - type) > 0)
                    {
                        temp_type = AXIS2_MALLOC(env->allocator,
                            sizeof(axis2_char_t) * ((end - type) + 1));
                        if (!temp_type)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                                "No memory. Failed in creating Content-Type");
                            return NULL;
                        }
                        memcpy(temp_type, type, (end - type));
                        temp_type[end - type] = '\0';
                        type = temp_type;
                    }
                }
            }
            if (id)
            {
                id += axutil_strlen(AXIOM_MIME_HEADER_CONTENT_ID);
                while (id && *id && *id != ':')
                {
                    id++;
                }
                if (id)
                {
                    while (id && *id && *id != '<')
                    {
                        id++;
                    }
                    id++;
                    pos = axutil_strstr(id, ">");
                    if (pos)
                    {
                        axis2_char_t *mime_id = NULL;
                        int mime_id_len = 0;
                        mime_id_len = (int)(pos - id);
                        mime_id = AXIS2_MALLOC(env->allocator,
                            sizeof(axis2_char_t) * mime_id_len + 1); 
                        /* The MIME ID will be freed by the SOAP builder */
                        if (mime_id)
                        {
                            axiom_data_handler_t *data_handler = NULL;
                            memcpy(mime_id, id, mime_id_len);
                            mime_id[mime_id_len] = '\0';
                            data_handler =
                                axiom_data_handler_create(env, NULL, type);

                            AXIS2_FREE(env->allocator, type);

                            axiom_data_handler_set_binary_data(data_handler,
                                env, mime_binary, mime_binary_len - 2);
                            axutil_hash_set(mime_parser->mime_parts_map,
                                mime_id, AXIS2_HASH_KEY_STRING, data_handler);
                        }
                        else
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                                "No memory. Failed in creating MIME ID");
                            return NULL;
                        }
                    }
                }
            }
            else
            {
                axis2_char_t temp_boundry[1024];
                sprintf(temp_boundry, "--%s--", mime_boundary);
                if (body_mime && axutil_strstr(body_mime, temp_boundry))
                {
                    break;
                }
            }
            body_mime = temp_body_mime;
        }
    }                           /* end while (!end_of_mime) */

    if (buf_array)
    {
        AXIS2_FREE(env->allocator, buf_array);
        buf_array = NULL;
    }

    if (len_array)
    {
        AXIS2_FREE(env->allocator, len_array);
        len_array = NULL;
    }

    AXIS2_FREE(env->allocator, buffer);

    return mime_parser->mime_parts_map;
}

AXIS2_EXTERN axutil_hash_t *AXIS2_CALL
axiom_mime_parser_get_mime_parts_map(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env)
{
    return mime_parser->mime_parts_map;
}

AXIS2_EXTERN int AXIS2_CALL
axiom_mime_parser_get_soap_body_len(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env)
{
    return mime_parser->soap_body_len;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
axiom_mime_parser_get_soap_body_str(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env)
{
    return mime_parser->soap_body_str;
}

AXIS2_EXTERN void AXIS2_CALL
axiom_mime_parser_set_chunk_buffer_size(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env,
    int size)
{
    mime_parser->chunk_buffer_size = size;
}

AXIS2_EXTERN void AXIS2_CALL
axiom_mime_parser_set_max_chunk_buffers(
    axiom_mime_parser_t *mime_parser,
    const axutil_env_t *env,
    int num)
{
    mime_parser->max_chunk_buffers = num;
}