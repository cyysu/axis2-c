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

#include <rampart_context.h>
#include <rampart_constants.h>
#include <oxs_axiom.h>

struct rampart_context_t
{
    /*****************************/
    axiom_node_t *policy_node;
    void *prv_key;
    axis2_key_type_t prv_key_type;
    void *certificate;
    axis2_key_type_t certificate_type;
    void *receiver_certificate;
    axis2_key_type_t receiver_certificate_type;
    axis2_char_t *user;
    axis2_char_t *password;
    axis2_char_t *prv_key_password;
    password_callback_fn pwcb_function;
    int ttl;
    axis2_char_t *password_type;

    /****************************/

    /*Rampart specific members*/
    rp_secpolicy_t *secpolicy;
    rampart_callback_t *password_callback_module;
    rampart_authn_provider_t *authn_provider;
    auth_password_func authenticate_with_password;
    auth_digest_func authenticate_with_digest;
            
};

/*void rampart_context_set_callback_fn(axis2_env_t *env, 
                                axis2_char_t *(*callback)(
                                    axis2_env_t  *env, 
                                    axis2_char_t *user ,
                                    void *ctx), void *ctx);

*/
/*private functions*/

rp_symmetric_asymmetric_binding_commons_t 
*rampart_context_get_symmetric_asymmetric_binding_commons(
    rampart_context_t *rampart_context,
    const axis2_env_t *env);

rp_binding_commons_t *rampart_context_get_binding_commons(
    rampart_context_t *rampart_context,
    const axis2_env_t *env);

axis2_bool_t rampart_context_use_username_token(
    rp_supporting_tokens_t *signed_supporting,
    const axis2_env_t *env);

axis2_bool_t rampart_context_validate_ut(
    rp_username_token_t *username_token,
    const axis2_env_t *env);

rp_supporting_tokens_t
*rampart_context_get_signed_supporting_from_binding(
    rampart_context_t *rampart_context,
    const axis2_env_t *env);

/*
axis2_status_t rampart_context_set_nodes_to_encrypt(
    rp_header_t *header,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt);
*/

axis2_status_t rampart_context_set_nodes_to_encrypt_or_sign(
    rp_header_t *header,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt_or_sign);

axis2_status_t AXIS2_CALL
rampart_context_get_nodes_to_protect(
    rampart_context_t *rampart_context,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_sign_or_encrypt,
    axis2_bool_t is_sign);

axis2_char_t *AXIS2_CALL
rampart_context_get_key_identifier_from_wss(
    rampart_context_t *rampart_context,
    const axis2_env_t *env);



AXIS2_EXTERN rampart_context_t *AXIS2_CALL 
rampart_context_create(const axis2_env_t *env)
{
    rampart_context_t *rampart_context = NULL;

    AXIS2_ENV_CHECK(env, NULL);

    rampart_context =  (rampart_context_t *) AXIS2_MALLOC (env->allocator,
    sizeof (rampart_context_t));

    if(rampart_context == NULL)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    rampart_context->policy_node = NULL;
    rampart_context->prv_key = NULL;
    rampart_context->prv_key_type = 0;
    rampart_context->certificate = NULL;
    rampart_context->certificate_type = 0;
    rampart_context->receiver_certificate = NULL;
    rampart_context->receiver_certificate_type = 0;
    rampart_context->user = 0;
    rampart_context->password = NULL;
    rampart_context->prv_key_password = NULL;
    rampart_context->pwcb_function = NULL;
    rampart_context->ttl = 0;
    rampart_context->password_type = NULL;

    rampart_context->secpolicy = NULL;
    rampart_context->password_callback_module = NULL;
    rampart_context->authn_provider = NULL;
    rampart_context->authenticate_with_password = NULL;
    rampart_context->authenticate_with_digest = NULL;
    
    return rampart_context;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_free(rampart_context_t *rampart_context,
        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if(rampart_context)
    {
        if(rampart_context->secpolicy)
        {
            rp_secpolicy_free(rampart_context->secpolicy,env);
            rampart_context->secpolicy = NULL;
        }
        AXIS2_FREE(env->allocator,rampart_context);
        rampart_context = NULL;
    }
    return AXIS2_SUCCESS;
}


/* Implementations */

/*Implementation of PHP-rampart interface */

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_policy_node(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axiom_node_t *policy_node)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,policy_node,AXIS2_FAILURE);

    rampart_context->policy_node = policy_node;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_prv_key(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            void *prv_key)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,prv_key,AXIS2_FAILURE);

    rampart_context->prv_key = prv_key;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_prv_key_type(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_key_type_t type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,type,AXIS2_FAILURE);

    rampart_context->prv_key_type = type;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_certificate(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            void *certificate)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,certificate,AXIS2_FAILURE);

    rampart_context->certificate = certificate;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_certificate_type(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_key_type_t type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,type,AXIS2_FAILURE);

    rampart_context->certificate_type = type;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_receiver_certificate(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            void *receiver_certificate)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,receiver_certificate,AXIS2_FAILURE);

    rampart_context->receiver_certificate = receiver_certificate;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_receiver_certificate_type(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_key_type_t type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,type,AXIS2_FAILURE);

    rampart_context->receiver_certificate_type = type;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_user(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_char_t *user)
{

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,user,AXIS2_FAILURE);

    rampart_context->user = user;
    return AXIS2_SUCCESS;

}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_password(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_char_t *password)
{

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,password,AXIS2_FAILURE);

    rampart_context->password = password;
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_prv_key_password(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_char_t *prv_key_password)
{

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,prv_key_password,AXIS2_FAILURE);

    rampart_context->prv_key_password = prv_key_password;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_pwcb_function(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            password_callback_fn pwcb_function,
            void *ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,pwcb_function,AXIS2_FAILURE);

    rampart_context->pwcb_function = pwcb_function;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_password_type(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            axis2_char_t *password_type)
{

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,password_type,AXIS2_FAILURE);

    rampart_context->password_type = password_type;
    return AXIS2_SUCCESS;

}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_ttl(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            int ttl)
{

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,ttl,AXIS2_FAILURE);

    rampart_context->ttl = ttl;
    return AXIS2_SUCCESS;
}

/*End of implementation*/

/*Getters of the PHP-RAMPART interface*/

AXIS2_EXTERN axiom_node_t *AXIS2_CALL
rampart_context_get_policy_node(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
   
    return rampart_context->policy_node; 
}

AXIS2_EXTERN void *AXIS2_CALL
rampart_context_get_prv_key(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->prv_key;
}

AXIS2_EXTERN axis2_key_type_t AXIS2_CALL
rampart_context_get_prv_key_type(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    return rampart_context->prv_key_type;
}

AXIS2_EXTERN void *AXIS2_CALL
rampart_context_get_certificate(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->certificate;
}

AXIS2_EXTERN axis2_key_type_t AXIS2_CALL
rampart_context_get_certificate_type(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    return rampart_context->certificate_type;
}


AXIS2_EXTERN void *AXIS2_CALL
rampart_context_get_receiver_certificate(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->receiver_certificate;
}

AXIS2_EXTERN axis2_key_type_t AXIS2_CALL
rampart_context_get_receiver_certificate_type(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    return rampart_context->receiver_certificate_type;
}



AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_user(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    return rampart_context->user;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_password(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->password;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_prv_key_password(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->prv_key_password;
}

AXIS2_EXTERN password_callback_fn AXIS2_CALL
rampart_context_get_pwcb_function(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return rampart_context->pwcb_function;
}

AXIS2_EXTERN int AXIS2_CALL
rampart_context_get_ttl(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env,AXIS2_FAILURE);

    return rampart_context->ttl;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_password_type(
            rampart_context_t *rampart_context,
            const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env,NULL);

    return rampart_context->password_type;
}

/*End of getters*/

AXIS2_EXTERN rp_secpolicy_t *AXIS2_CALL
rampart_context_get_secpolicy(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    return rampart_context->secpolicy;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_secpolicy(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            rp_secpolicy_t *secpolicy)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,secpolicy,AXIS2_FAILURE);

    rampart_context->secpolicy = secpolicy;
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN rampart_callback_t *AXIS2_CALL
rampart_context_get_password_callback(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    return rampart_context->password_callback_module;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_password_callback(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            rampart_callback_t *password_callback_module)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,password_callback_module,AXIS2_FAILURE);

    rampart_context->password_callback_module = password_callback_module;
    return AXIS2_SUCCESS;
}

/*The 4 functions below deals with setting and getting authentication 
 *module function pointers */

AXIS2_EXTERN auth_password_func AXIS2_CALL
rampart_context_get_auth_password_function(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    return rampart_context->authenticate_with_password;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_auth_password_function(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            auth_password_func authenticate_with_password)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,authenticate_with_password,AXIS2_FAILURE);

    rampart_context->authenticate_with_password = authenticate_with_password;
    return AXIS2_SUCCESS;
}



AXIS2_EXTERN auth_digest_func AXIS2_CALL
rampart_context_get_auth_digest_function(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    return rampart_context->authenticate_with_digest;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_auth_digest_function(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            auth_digest_func authenticate_with_digest)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,authenticate_with_digest,AXIS2_FAILURE);

    rampart_context->authenticate_with_digest = authenticate_with_digest;
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN rampart_authn_provider_t *AXIS2_CALL
rampart_context_get_authn_provider(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    return rampart_context->authn_provider;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_authn_provider(rampart_context_t *rampart_context,
            const axis2_env_t *env,
            rampart_authn_provider_t *authn_provider)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error,authn_provider,AXIS2_FAILURE);

    rampart_context->authn_provider = authn_provider;
    return AXIS2_SUCCESS;
}



AXIS2_EXTERN int AXIS2_CALL
rampart_context_get_binding_type(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_property_t *property = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    property = rp_secpolicy_get_binding(rampart_context->secpolicy,env);
    if(!property)
        return -1;

    return rp_property_get_type(property,env);   
}

rp_symmetric_asymmetric_binding_commons_t 
*rampart_context_get_symmetric_asymmetric_binding_commons(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_property_t *property = NULL;
    property = rp_secpolicy_get_binding(rampart_context->secpolicy,env);
    if(!property)
        return NULL;

    if(rp_property_get_type(property,env)==RP_BINDING_ASYMMETRIC)
    {
        rp_asymmetric_binding_t *asymmetric_binding = NULL;
        asymmetric_binding = (rp_asymmetric_binding_t*)rp_property_get_value(property,env);
        if(!asymmetric_binding)
            return NULL;

        return rp_asymmetric_binding_get_symmetric_asymmetric_binding_commons(asymmetric_binding,env);
    }
    else if(rp_property_get_type(property,env)==RP_BINDING_SYMMETRIC)
    {
        rp_symmetric_binding_t *symmetric_binding = NULL;
        symmetric_binding = (rp_symmetric_binding_t*)rp_property_get_value(property,env);
        if(!symmetric_binding)
            return NULL;

        return rp_symmetric_binding_get_symmetric_asymmetric_binding_commons(symmetric_binding,env);
    }
    else
        return NULL;
}

rp_binding_commons_t 
*rampart_context_get_binding_commons(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_property_t *property = NULL;
    property = rp_secpolicy_get_binding(rampart_context->secpolicy,env);
    if(!property)
        return NULL;

    if(rp_property_get_type(property,env)==RP_BINDING_ASYMMETRIC)
    {
        rp_asymmetric_binding_t *asymmetric_binding = NULL;
        rp_symmetric_asymmetric_binding_commons_t *sym_asym_commons = NULL;
        asymmetric_binding = (rp_asymmetric_binding_t*)rp_property_get_value(property,env);
        if(!asymmetric_binding)
            return NULL;

        sym_asym_commons = rp_asymmetric_binding_get_symmetric_asymmetric_binding_commons(asymmetric_binding,env);
        if(!sym_asym_commons)
            return NULL;

        return rp_symmetric_asymmetric_binding_commons_get_binding_commons(sym_asym_commons,env);
    }
    else if(rp_property_get_type(property,env)==RP_BINDING_SYMMETRIC)
    {
        rp_symmetric_binding_t *symmetric_binding = NULL;
        rp_symmetric_asymmetric_binding_commons_t *sym_asym_commons = NULL;
        symmetric_binding = (rp_symmetric_binding_t*)rp_property_get_value(property,env);
        if(!symmetric_binding)
            return NULL;

        sym_asym_commons = rp_symmetric_binding_get_symmetric_asymmetric_binding_commons(symmetric_binding,env);
        if(!sym_asym_commons)
            return NULL;

        return rp_symmetric_asymmetric_binding_commons_get_binding_commons(sym_asym_commons,env);
        
    }
    else if(rp_property_get_type(property,env)==RP_BINDING_TRANSPORT)
    {
        rp_transport_binding_t *transport_binding = NULL;
        transport_binding = (rp_transport_binding_t*)rp_property_get_value(property,env);
        if(!transport_binding)
            return NULL;

        return rp_transport_binding_get_binding_commons(transport_binding,env);
    }
    else
        return NULL;
}
/*supporting tokens may contain under binding*/

rp_supporting_tokens_t  
*rampart_context_get_signed_supporting_from_binding(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_binding_commons_t *commons = NULL;
    commons = rampart_context_get_binding_commons(rampart_context,env);
    if(!commons)
        return NULL;
    return rp_binding_commons_get_signed_supporting_tokens(commons,env);     
}

axis2_bool_t
rampart_context_validate_ut(
    rp_username_token_t *username_token,
    const axis2_env_t *env)
{
    axis2_char_t *inclusion = NULL;
    axis2_bool_t bval = AXIS2_FALSE;

    bval = rp_username_token_get_useUTprofile10(username_token,env);
    if(bval)
    {
        inclusion = rp_username_token_get_inclusion(username_token,env);
        if((axis2_strcmp(inclusion,RP_INCLUDE_ALWAYS)==0)||
           (axis2_strcmp(inclusion,RP_INCLUDE_ONCE)==0)||
           (axis2_strcmp(inclusion,RP_INCLUDE_ALWAYS_TO_RECIPIENT)==0))
            return bval;
        else
            bval = AXIS2_FALSE;
    }
    return bval;
}



axis2_bool_t 
rampart_context_use_username_token(
    rp_supporting_tokens_t *signed_supporting,
    const axis2_env_t *env)
{
    axis2_array_list_t *array_list = NULL;
    axis2_bool_t bvalidate = AXIS2_FALSE;
    array_list = rp_supporting_tokens_get_tokens(signed_supporting,env);
    if(!array_list)
        return AXIS2_FALSE;

    else
    {
        int i = 0;
        for (i = 0; i < axis2_array_list_size(array_list,env); i++)
        {
            rp_property_t *token = NULL;
            token = (rp_property_t *)
                   axis2_array_list_get(array_list,env, i);
            if (token)
            {
                if(rp_property_get_type(token,env)==RP_TOKEN_USERNAME)
                {
                    rp_username_token_t *username_token = 
                        (rp_username_token_t *)rp_property_get_value(token,env);
                    bvalidate = rampart_context_validate_ut(username_token,env);
                    break;
                }
            }
        }
    }
    return bvalidate;
}
/*

axis2_status_t rampart_context_set_nodes_to_encrypt(
    rp_header_t *header,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt)
{
    axis2_char_t *namespace = NULL;
    axis2_char_t *local_name = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_node_t *header_node = NULL;

    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope,env);
    if(!soap_header)
        return AXIS2_FAILURE;

    namespace = (axis2_char_t *) rp_header_get_namespace(header,env);
    if(!namespace)
        return AXIS2_FAILURE;

    if(axis2_strcmp(namespace,RP_SECURITY_NS)==0)
    {
        AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] We do not encrypt security namespace headers");
        return AXIS2_FAILURE;
    }

    local_name = (axis2_char_t*) rp_header_get_name(header,env);
    if(!local_name)
    {
        axis2_array_list_t *soap_header_blocks = NULL;
        int i = 0;
        soap_header_blocks = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(soap_header,env,namespace);
        if(!soap_header_blocks)
            return AXIS2_FAILURE;

        for(i=0 ; i<axis2_array_list_size(soap_header_blocks,env); i++)
        {
            axiom_soap_header_block_t *header_block = NULL;
            axiom_node_t *node = NULL;
            header_block = (axiom_soap_header_block_t *)axis2_array_list_get(soap_header_blocks,env,i);
            if(header_block)
            {
                node = AXIOM_SOAP_HEADER_BLOCK_GET_BASE_NODE(header_block,env);
                if(node)
                {    
                    axis2_array_list_add(nodes_to_encrypt,env,node);
                    return AXIS2_SUCCESS;
                }
            }
            
        }
    }
    else if(axis2_strcmp(local_name,"Security")==0)
    {
        AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] We do not encrypt %s", local_name);
        return AXIS2_FAILURE;
    }
    else
    {
        axiom_node_t *ret_node = NULL;
        header_node = AXIOM_SOAP_HEADER_GET_BASE_NODE(soap_header,env);
        if(header_node)
        {
            ret_node = oxs_axiom_get_node_by_local_name(env,header_node,local_name);
            if(ret_node)
            {
                axiom_element_t *ret_node_ele = NULL;
                ret_node_ele = (axiom_element_t *)
                           AXIOM_NODE_GET_DATA_ELEMENT(ret_node, env);
                if(ret_node_ele)
                {
                    axiom_namespace_t *ns = NULL;
                    axis2_char_t *namespace_uri = NULL;
                    ns = axiom_element_get_namespace(ret_node_ele, env,ret_node);
                    if(ns)
                    {
                        namespace_uri = axiom_namespace_get_uri(ns, env);
                        if (axis2_strcmp(namespace_uri,namespace) == 0)
                        {
                            axis2_array_list_add(nodes_to_encrypt,env,ret_node);
                            return AXIS2_SUCCESS;
                        }

                    }                

                }
        
            }
        }
    }
    return AXIS2_FAILURE;
}
*/

axis2_status_t rampart_context_set_nodes_to_encrypt_or_sign(
    rp_header_t *header,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt_or_sign)
{
    axis2_char_t *namespace = NULL;
    axis2_char_t *local_name = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axiom_node_t *header_node = NULL;

    soap_header = AXIOM_SOAP_ENVELOPE_GET_HEADER(soap_envelope,env);
    if(!soap_header)
        return AXIS2_FAILURE;

    namespace = (axis2_char_t *) rp_header_get_namespace(header,env);
    if(!namespace)
        return AXIS2_FAILURE;

    if(axis2_strcmp(namespace,RP_SECURITY_NS)==0)
    {
        AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] We do not sign or encrypt security namespace headers");
        return AXIS2_FAILURE;
    }

    local_name = (axis2_char_t*) rp_header_get_name(header,env);
    if(!local_name)
    {
        axis2_array_list_t *soap_header_blocks = NULL;
        int i = 0;
        soap_header_blocks = AXIOM_SOAP_HEADER_GET_HEADER_BLOCKS_WITH_NAMESPACE_URI(soap_header,env,namespace);
        if(!soap_header_blocks)
            return AXIS2_FAILURE;

        for(i=0 ; i<axis2_array_list_size(soap_header_blocks,env); i++)
        {
            axiom_soap_header_block_t *header_block = NULL;
            axiom_node_t *node = NULL;
            header_block = (axiom_soap_header_block_t *)axis2_array_list_get(soap_header_blocks,env,i);
            if(header_block)
            {
                node = AXIOM_SOAP_HEADER_BLOCK_GET_BASE_NODE(header_block,env);
                if(node)
                {    
                    axis2_array_list_add(nodes_to_encrypt_or_sign,env,node);
                    return AXIS2_SUCCESS;
                }
            }
            
        }
    }
    else if(axis2_strcmp(local_name,"Security")==0)
    {
        AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] We do not sign or encrypt %s", local_name);
        return AXIS2_FAILURE;
    }
    else
    {
        axiom_node_t *ret_node = NULL;
        header_node = AXIOM_SOAP_HEADER_GET_BASE_NODE(soap_header,env);
        if(header_node)
        {
            ret_node = oxs_axiom_get_node_by_local_name(env,header_node,local_name);
            if(ret_node)
            {
                axiom_element_t *ret_node_ele = NULL;
                ret_node_ele = (axiom_element_t *)
                           AXIOM_NODE_GET_DATA_ELEMENT(ret_node, env);
                if(ret_node_ele)
                {
                    axiom_namespace_t *ns = NULL;
                    axis2_char_t *namespace_uri = NULL;
                    ns = axiom_element_get_namespace(ret_node_ele, env,ret_node);
                    if(ns)
                    {
                        namespace_uri = axiom_namespace_get_uri(ns, env);
                        if (axis2_strcmp(namespace_uri,namespace) == 0)
                        {
                            axis2_array_list_add(nodes_to_encrypt_or_sign,env,ret_node);
                            return AXIS2_SUCCESS;
                        }

                    }                

                }
        
            }
        }
    }
    return AXIS2_FAILURE;
}


rp_algorithmsuite_t *AXIS2_CALL 
rampart_context_get_algorithmsuite(
        rampart_context_t *rampart_context,
        const axis2_env_t *env)
{
    rp_binding_commons_t *binding_commons = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    binding_commons = rampart_context_get_binding_commons(rampart_context,env);

    if(!binding_commons)
        return AXIS2_FALSE;

    return rp_binding_commons_get_algorithmsuite(binding_commons,env);
}

axis2_char_t *AXIS2_CALL 
rampart_context_get_key_identifier_from_wss(
        rampart_context_t *rampart_context,
        const axis2_env_t *env)
{
    rp_property_t *wss = NULL;
    axis2_char_t *identifier = NULL;
             
    wss = rp_secpolicy_get_wss(rampart_context->secpolicy,env);
    if(!wss)
    {
        identifier = RAMPART_STR_DIRECT_REFERENCE;
        return identifier;    
    }

    if(rp_property_get_type(wss,env)==RP_WSS_WSS10)
    {
        rp_wss10_t *wss10 = NULL;
        wss10 = rp_property_get_value(wss,env);
        if(!wss10)
            return NULL;
            
        if(rp_wss10_get_must_support_ref_key_identifier(wss10,env))
            identifier = RAMPART_STR_KEY_IDENTIFIER;
        else if(rp_wss10_get_must_support_ref_issuer_serial(wss10,env))
            identifier = RAMPART_STR_ISSUER_SERIAL;
        else if(rp_wss10_get_must_support_ref_external_uri(wss10,env))
            identifier = RAMPART_STR_EXTERNAL_URI;
        else if(rp_wss10_get_must_support_ref_embedded_token(wss10,env))
            identifier = RAMPART_STR_EMBEDDED;
        else
            identifier = RAMPART_STR_DIRECT_REFERENCE;
        
        return identifier;                                         
    }
    else if(rp_property_get_type(wss,env)==RP_WSS_WSS11)
    {
        rp_wss11_t *wss11 = NULL;
        wss11 = rp_property_get_value(wss,env);
        if(!wss11)
            return NULL;
            
        if(rp_wss11_get_must_support_ref_key_identifier(wss11,env))
            identifier = RAMPART_STR_KEY_IDENTIFIER;
        else if(rp_wss11_get_must_support_ref_issuer_serial(wss11,env))
            identifier = RAMPART_STR_ISSUER_SERIAL;
        else if(rp_wss11_get_must_support_ref_external_uri(wss11,env))
            identifier = RAMPART_STR_EXTERNAL_URI;
        else if(rp_wss11_get_must_support_ref_embedded_token(wss11,env))
            identifier = RAMPART_STR_EMBEDDED;
        else if(rp_wss11_get_must_support_ref_thumbprint(wss11,env))
            identifier = RAMPART_STR_THUMB_PRINT;
        else if(rp_wss11_get_must_support_ref_encryptedkey(wss11,env))
            identifier = RAMPART_STR_ENCRYPTED_KEY;
        else
            identifier = RAMPART_STR_DIRECT_REFERENCE;                                         

        return identifier;
    }
    else return NULL;
} 


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_is_include_timestamp(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_binding_commons_t *binding_commons = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    binding_commons = rampart_context_get_binding_commons(rampart_context,env);

    if(!binding_commons)
        return AXIS2_FALSE;

    return rp_binding_commons_get_include_timestamp(binding_commons,env);
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_is_include_username_token(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    /*Username tokens should be in signed supporting tikens.
      Otherwise no meaning  */

    rp_supporting_tokens_t *signed_supporting = NULL;

    /*First we should check in the direct policy members*/
    signed_supporting = rp_secpolicy_get_signed_supporting_tokens(rampart_context->secpolicy,env);
    /*If not there then we should ckeck in the binding*/
    if(!signed_supporting)
    {
        signed_supporting = rampart_context_get_signed_supporting_from_binding(rampart_context,env);
        if(!signed_supporting)
            return AXIS2_FALSE;
    }
    /*Now we have signed supporting tokens*/
    /*Get the user name token if available and check the validity*/
    return rampart_context_use_username_token(signed_supporting,env); 
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_user_from_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{

    rp_rampart_config_t *config = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return AXIS2_FAILURE;    

    rampart_context->user = rp_rampart_config_get_user(config,env);
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_password_callback_class(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *config = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return NULL;

    return rp_rampart_config_get_password_callback_class(config,env);
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_authn_module_name(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *config = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return NULL;

    return rp_rampart_config_get_authenticate_module(config,env);
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_password_type_from_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *config = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return AXIS2_FAILURE;    

    rampart_context->password_type = rp_rampart_config_get_password_type(config,env);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_set_ttl_from_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *config = NULL;
    axis2_char_t *time_to_live = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return AXIS2_FAILURE;

    time_to_live = rp_rampart_config_get_time_to_live(config,env);
    if(!time_to_live)
        rampart_context->ttl = 300;
    else
        rampart_context->ttl = axis2_atoi(time_to_live);    

    return AXIS2_SUCCESS;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_is_encrypt_before_sign(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    axis2_char_t *protection_order = NULL;
    rp_symmetric_asymmetric_binding_commons_t *sym_asym_commons = NULL;
    sym_asym_commons = rampart_context_get_symmetric_asymmetric_binding_commons(rampart_context,env);

    if(!sym_asym_commons)
        return AXIS2_FALSE;
    else
    {
        protection_order = rp_symmetric_asymmetric_binding_commons_get_protection_order(sym_asym_commons,env);       
        if(!protection_order || axis2_strcmp(protection_order,RP_SIGN_BEFORE_ENCRYPTING)==0)
            return AXIS2_FALSE;
 
        else if(axis2_strcmp(protection_order,RP_ENCRYPT_BEFORE_SIGNING)==0)
            return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}

/*This method will return all the parts in the soap message
outside the security header which needs to be encrypted.*/
/*
AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_get_nodes_to_encrypt(
    rampart_context_t *rampart_context,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt)
{
    rp_signed_encrypted_parts_t *encrypted_parts = NULL;    
    axis2_array_list_t *parts = NULL;
    axis2_status_t status = AXIS2_FAILURE;

    encrypted_parts = rp_secpolicy_get_encrypted_parts(rampart_context->secpolicy,env);
    if(!encrypted_parts)
        return AXIS2_FAILURE;
    
    parts = rp_signed_encrypted_parts_get_headers(encrypted_parts,env);
    if(!parts || (axis2_array_list_size(parts,env)==0))
    {
        if(rp_signed_encrypted_parts_get_body(encrypted_parts,env))
        {
            axiom_soap_body_t *body = NULL;
            axiom_node_t *body_node = NULL;
            axiom_node_t *body_child_node = NULL;
            
            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] No encryption parts specified. Using the body.");
            body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
            body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(body, env);
            body_child_node = axiom_node_get_first_element(body_node, env);
            axis2_array_list_add(nodes_to_encrypt, env, body_child_node);
            return AXIS2_SUCCESS;
        }
        else
        {
            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] Nothing to encrypt");
            return AXIS2_FAILURE;
        }
    }
    else
    {
        int i = 0;
        for(i=0; i<axis2_array_list_size(parts,env); i++)
        {
            rp_header_t *header = NULL;
            header = (rp_header_t *)axis2_array_list_get(parts,env,i);
            if(header)
            {
                status = rampart_context_set_nodes_to_encrypt(header,env,soap_envelope,nodes_to_encrypt);
                if(status!=AXIS2_FAILURE)
                    return AXIS2_FAILURE;
            }
            
        }
        if(rp_signed_encrypted_parts_get_body(encrypted_parts,env))
        {
            axiom_soap_body_t *body = NULL;
            axiom_node_t *body_node = NULL;
            axiom_node_t *body_child_node = NULL;

            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] Including the body the body.");
            body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
            body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(body, env);
            body_child_node = axiom_node_get_first_element(body_node, env);
            axis2_array_list_add(nodes_to_encrypt, env, body_child_node);
            return AXIS2_SUCCESS;
        }
        
    }
    return AXIS2_FAILURE;
}
*/

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_get_nodes_to_encrypt(
    rampart_context_t *rampart_context,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_encrypt)
{
    
    return rampart_context_get_nodes_to_protect(rampart_context,env,soap_envelope,nodes_to_encrypt,AXIS2_FALSE);
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
rampart_context_get_nodes_to_sign(
    rampart_context_t *rampart_context,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_sign)
{
    
    return rampart_context_get_nodes_to_protect(rampart_context,env,soap_envelope,nodes_to_sign,AXIS2_TRUE);
}



axis2_status_t AXIS2_CALL
rampart_context_get_nodes_to_protect(
    rampart_context_t *rampart_context,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope,
    axis2_array_list_t *nodes_to_sign_or_encrypt,
    axis2_bool_t is_sign)
{
    rp_signed_encrypted_parts_t *signed_encrypted_parts = NULL;    
    axis2_array_list_t *parts = NULL;
    axis2_status_t status = AXIS2_FAILURE;

    if(is_sign)
        signed_encrypted_parts = rp_secpolicy_get_signed_parts(rampart_context->secpolicy,env);
    else
        signed_encrypted_parts = rp_secpolicy_get_encrypted_parts(rampart_context->secpolicy,env);
  
    if(!signed_encrypted_parts)
        return AXIS2_FAILURE;
    
    parts = rp_signed_encrypted_parts_get_headers(signed_encrypted_parts,env);
    if(!parts || (axis2_array_list_size(parts,env)==0))
    {
        if(rp_signed_encrypted_parts_get_body(signed_encrypted_parts,env))
        {
            axiom_soap_body_t *body = NULL;
            axiom_node_t *body_node = NULL;
            axiom_node_t *body_child_node = NULL;
            
            if(is_sign)
                AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] No Signed parts specified. Using the body.");
            else
                AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] No Encrypted parts specified. Using the body.");

            body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
            body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(body, env);
            body_child_node = axiom_node_get_first_element(body_node, env);
            axis2_array_list_add(nodes_to_sign_or_encrypt, env, body_child_node);
            return AXIS2_SUCCESS;
        }
        else
        {
            if(is_sign)
                AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] Nothing to sign outside Secyrity header.");
            else
                AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] Nothing to encrypt outside Secyrity header.");
            
            return AXIS2_FAILURE;
        }
    }
    else
    {
        int i = 0;
        for(i=0; i<axis2_array_list_size(parts,env); i++)
        {
            rp_header_t *header = NULL;
            header = (rp_header_t *)axis2_array_list_get(parts,env,i);
            if(header)
            {
                status = rampart_context_set_nodes_to_encrypt_or_sign(header,env,soap_envelope,nodes_to_sign_or_encrypt);
                if(status!=AXIS2_FAILURE)
                    return AXIS2_FAILURE;
            }
            
        }
        if(rp_signed_encrypted_parts_get_body(signed_encrypted_parts,env))
        {
            axiom_soap_body_t *body = NULL;
            axiom_node_t *body_node = NULL;
            axiom_node_t *body_child_node = NULL;

            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context] Including the body.");
            body = AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope, env);
            body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(body, env);
            body_child_node = axiom_node_get_first_element(body_node, env);
            axis2_array_list_add(nodes_to_sign_or_encrypt, env, body_child_node);
            return AXIS2_SUCCESS;
        }
        
    }
    return AXIS2_FAILURE;
}


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_check_whether_to_encrypt(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_signed_encrypted_parts_t *encrypted_parts = NULL;    
    axis2_array_list_t *parts = NULL;

    encrypted_parts = rp_secpolicy_get_encrypted_parts(rampart_context->secpolicy,env);
    if(!encrypted_parts)
        return AXIS2_FALSE;
    
    parts = rp_signed_encrypted_parts_get_headers(encrypted_parts,env);
    if(!parts || (axis2_array_list_size(parts,env)==0))
    {
        if(rp_signed_encrypted_parts_get_body(encrypted_parts,env))
            return AXIS2_TRUE;       
        
        else
        {
            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context]No encryption parts specified Nothing to decrypt");
            return AXIS2_FALSE;
        }
    }
    return AXIS2_TRUE;
}        


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_check_whether_to_sign(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_signed_encrypted_parts_t *signed_parts = NULL;    
    axis2_array_list_t *parts = NULL;

    signed_parts = rp_secpolicy_get_signed_parts(rampart_context->secpolicy,env);
    if(!signed_parts)
        return AXIS2_FALSE;
    
    parts = rp_signed_encrypted_parts_get_headers(signed_parts,env);
    if(!parts || (axis2_array_list_size(parts,env)==0))
    {
        if(rp_signed_encrypted_parts_get_body(signed_parts,env))
            return AXIS2_TRUE;       
        
        else
        {
            AXIS2_LOG_INFO(env->log, "[rampart][rampart_context]No Signed parts specified Nothing to Verify");
            return AXIS2_FALSE;
        }
    }
    return AXIS2_TRUE;
}        



AXIS2_EXTERN rp_property_t *AXIS2_CALL 
rampart_context_get_token(
        rampart_context_t *rampart_context,
        const axis2_env_t *env,
        axis2_bool_t for_encryption,
        axis2_bool_t server_side)
{
    rp_property_t *binding = NULL;
    binding = rp_secpolicy_get_binding(rampart_context->secpolicy,env);
    if(!binding)
        return NULL;

    if(rp_property_get_type(binding,env)==RP_BINDING_ASYMMETRIC)
    {
        rp_asymmetric_binding_t *asym_binding = NULL;
        asym_binding = (rp_asymmetric_binding_t *)rp_property_get_value(binding,env);
        if(asym_binding)
        {
            if((for_encryption && server_side) || (!for_encryption && !server_side))
            {
                return rp_asymmetric_binding_get_initiator_token(asym_binding,env);
            }    
            else if((for_encryption && !server_side) || (!for_encryption && server_side))
            {
                return rp_asymmetric_binding_get_recipient_token(asym_binding,env);
            }
            else return NULL;
        }
        else
            return NULL;
    }
    /*In symmetric binding same tokens are used in the client and server sides.*/
    else if(rp_property_get_type(binding,env)==RP_BINDING_SYMMETRIC)
    {
        rp_symmetric_binding_t *sym_binding = NULL;
        rp_property_t *token = NULL;
        sym_binding = (rp_symmetric_binding_t *)rp_property_get_value(binding,env);
        if(sym_binding)
        {
            /*First check protection tokens have being specified.*/
            token = rp_symmetric_binding_get_protection_token(sym_binding,env);
            if(token)
                return token;    
            
            else
            {
                if(for_encryption)
                {
                    return rp_symmetric_binding_get_encryption_token(sym_binding,env);
                }
                else
                {
                    return rp_symmetric_binding_get_signature_token(sym_binding,env);
                }
            }
        }
        else
            return NULL;
    }
    else if(rp_property_get_type(binding,env)==RP_BINDING_TRANSPORT)
    {
        rp_transport_binding_t *transport_binding = NULL;
        transport_binding = (rp_transport_binding_t *)rp_property_get_value(binding,env);
        if(transport_binding)
        {
            return rp_transport_binding_get_transport_token(transport_binding,env);
        }
        else return NULL;                    
    }
    else return NULL;
}

AXIS2_EXTERN axis2_bool_t AXIS2_CALL
rampart_context_check_is_derived_keys(
    const axis2_env_t *env,
    rp_property_t *token)
{
    if(rp_property_get_type(token,env)==RP_TOKEN_X509)
    {
        rp_x509_token_t *x509_token = NULL;
        x509_token = (rp_x509_token_t *)rp_property_get_value(token,env);
        return rp_x509_token_get_derivedkeys(x509_token,env);
    }        
    /*This can be extended when we are supporting other token types.*/
    else
        return AXIS2_FALSE;        
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_enc_sym_algo(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_algorithmsuite_t *algosuite = NULL;
  
    algosuite = rampart_context_get_algorithmsuite(rampart_context,env); 
    if(algosuite)
    {
        return rp_algorithmsuite_get_encryption(algosuite,env);
    }
    else
        return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_enc_asym_algo(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_algorithmsuite_t *algosuite = NULL;
  
    algosuite = rampart_context_get_algorithmsuite(rampart_context,env); 
    if(algosuite)
    {
        return rp_algorithmsuite_get_asymmetrickeywrap(algosuite,env);
    }
    else
        return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_asym_sig_algo(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_algorithmsuite_t *algosuite = NULL;
  
    algosuite = rampart_context_get_algorithmsuite(rampart_context,env); 
    if(algosuite)
    {
        return rp_algorithmsuite_get_asymmetric_signature(algosuite,env);
    }
    else
        return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_digest_mtd(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_algorithmsuite_t *algosuite = NULL;
  
    algosuite = rampart_context_get_algorithmsuite(rampart_context,env); 
    if(algosuite)
    {
        return rp_algorithmsuite_get_digest(algosuite,env);
    }
    else
        return NULL;
}


AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_certificate_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *rampart_config = NULL;

    rampart_config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(rampart_config)
    {
        return rp_rampart_config_get_certificate_file(rampart_config,env);
    }
    else
        return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_receiver_certificate_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *rampart_config = NULL;

    rampart_config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(rampart_config)
    {
        return rp_rampart_config_get_receiver_certificate_file(rampart_config,env);
    }
    else
        return NULL;
}


AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_private_key_file(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *rampart_config = NULL;

    rampart_config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(rampart_config)
    {
        return rp_rampart_config_get_private_key_file(rampart_config,env);
    }
    else
        return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_encryption_user(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_rampart_config_t *config = NULL;
    config = rp_secpolicy_get_rampart_config(rampart_context->secpolicy,env);
    if(!config)
        return NULL;

    return rp_rampart_config_get_encryption_user(config,env);

}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_enc_key_identifier(
    rampart_context_t *rampart_context,
    rp_property_t *token,
    axis2_bool_t server_side,
    const axis2_env_t *env)
{
    axis2_char_t *inclusion = NULL;    
    axis2_bool_t include = AXIS2_TRUE;
    axis2_char_t *identifier = NULL;

    if(rp_property_get_type(token,env)==RP_TOKEN_X509)
    {
        rp_x509_token_t *x509_token = NULL;
        x509_token = (rp_x509_token_t *)rp_property_get_value(token,env);
        inclusion = rp_x509_token_get_inclusion(x509_token,env);

        if(server_side)
            include = ((axis2_strcmp(inclusion,RP_INCLUDE_ALWAYS)==0)||
                        (axis2_strcmp(inclusion,RP_INCLUDE_ONCE)==0));
        else
            include = ((axis2_strcmp(inclusion,RP_INCLUDE_ALWAYS)==0)||
           (axis2_strcmp(inclusion,RP_INCLUDE_ONCE)==0)||
           (axis2_strcmp(inclusion,RP_INCLUDE_ALWAYS_TO_RECIPIENT)==0));
        
        if(include)
        {
           if(rp_x509_token_get_require_key_identifier_reference(x509_token,env))
                identifier = RAMPART_STR_KEY_IDENTIFIER;
           else if(rp_x509_token_get_require_issuer_serial_reference(x509_token,env))
                identifier = RAMPART_STR_ISSUER_SERIAL;                        
           else if(rp_x509_token_get_require_embedded_token_reference(x509_token,env))
                identifier = RAMPART_STR_EMBEDDED; 
           else if(rp_x509_token_get_require_thumb_print_reference(x509_token,env))
                identifier = RAMPART_STR_THUMB_PRINT;
           else
                return rampart_context_get_key_identifier_from_wss(rampart_context,env);                             

           return identifier;              
        }
        else return NULL;
                            
    }
    /*This can be extended when we are supporting other token types.*/
    else return NULL;
}

AXIS2_EXTERN axis2_char_t *AXIS2_CALL
rampart_context_get_layout(
    rampart_context_t *rampart_context,
    const axis2_env_t *env)
{
    rp_binding_commons_t *binding_commons = NULL;
    rp_layout_t *layout = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    binding_commons = rampart_context_get_binding_commons(rampart_context,env);

    if(!binding_commons)
        return NULL;
    
    layout = rp_binding_commons_get_layout(binding_commons,env);
    if(!layout)
        return RP_LAYOUT_STRICT;

    return rp_layout_get_value(layout,env);
}
