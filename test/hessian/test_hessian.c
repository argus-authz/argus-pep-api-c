#include <stdio.h>
#include <string.h>

#include "argus/xacml.h"
#include "argus/io.h"
#include "argus/pep.h"
#include "hessian/hessian.h"
#include "util/buffer.h"
#include "util/log.h"
#include "util/base64.h"

#define PEP_IO_ERROR -1
#define PEP_IO_OK 0


static int xacml_attribute_unmarshal(xacml_attribute_t ** attr, const hessian_object_t * h_attribute) {
    const char * map_type;
    xacml_attribute_t * attribute;
    size_t map_l;
    int i;
    if (hessian_gettype(h_attribute) != HESSIAN_MAP) {
        log_error("xacml_attribute_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_attribute), hessian_getclassname(h_attribute));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_attribute);
    if (map_type == NULL) {
        log_error("xacml_attribute_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_ATTRIBUTE_CLASSNAME,map_type) != 0) {
        log_error("xacml_attribute_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    attribute= xacml_attribute_create(NULL);
    if (attribute == NULL) {
        log_error("xacml_attribute_unmarshal: can't create XACML attribute.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_attribute);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_attribute,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_attribute_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_attribute_delete(attribute);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_attribute_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_attribute_delete(attribute);
            return PEP_IO_ERROR;
        }

        /* id (mandatory) */
        if (strcmp(XACML_HESSIAN_ATTRIBUTE_ID,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            const char * id;
            if (hessian_gettype(h_string) != HESSIAN_STRING) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            id= hessian_string_getstring(h_string);
            if (xacml_attribute_setid(attribute,id) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set id: %s to XACML attribute at: %d",id,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
        }
        /* datatype (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_DATATYPE,key) == 0) {
            const char * datatype= NULL;
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            if (h_string_type == HESSIAN_STRING) {
                datatype= hessian_string_getstring(h_string);
            }
            if (xacml_attribute_setdatatype(attribute,datatype) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set datatype: %s to XACML attribute at: %d",datatype,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }

        }
        /* issuer (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_ISSUER,key) == 0) {
            const char * issuer = NULL;
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            if (h_string_type == HESSIAN_STRING) {
                issuer= hessian_string_getstring(h_string);
            }
            if (xacml_attribute_setissuer(attribute,issuer) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set issuer: %s to XACML attribute at: %d",issuer,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }

        }
        /* values list */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_VALUES,key) == 0) {
            hessian_object_t * h_values= hessian_map_getvalue(h_attribute,i);
            size_t h_values_l;
            int j;
            if (hessian_gettype(h_values) != HESSIAN_LIST) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian list.",key);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            h_values_l= hessian_list_length(h_values);
            for(j= 0; j<h_values_l; j++) {
                const char * value;
                hessian_object_t * h_value= hessian_list_get(h_values,j);
                if (hessian_gettype(h_value) != HESSIAN_STRING) {
                    log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
                value= hessian_string_getstring(h_value);
                if (xacml_attribute_addvalue(attribute,value) != PEP_XACML_OK) {
                    log_error("xacml_attribute_unmarshal: can't add value: %s to XACML attribute at: %d",value,j);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }

        }
        else {
            log_warn("xacml_attribute_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *attr= attribute;
    return PEP_IO_OK;
}

static int xacml_resource_unmarshal(xacml_resource_t ** res, const hessian_object_t * h_resource) {
    const char * map_type;
    xacml_resource_t * resource;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_resource) != HESSIAN_MAP) {
        log_error("xacml_resource_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_resource), hessian_getclassname(h_resource));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_resource);
    if (map_type == NULL) {
        log_error("xacml_resource_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_RESOURCE_CLASSNAME,map_type) != 0) {
        log_error("xacml_resource_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    resource= xacml_resource_create();
    if (resource == NULL) {
        log_error("xacml_resource_unmarshal: can't create XACML resource.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_resource);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_resource,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_resource_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_resource_delete(resource);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_resource_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_resource_delete(resource);
            return PEP_IO_ERROR;
        }
        /* content (can be null) */
        if (strcmp(XACML_HESSIAN_RESOURCE_CONTENT,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_resource,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * content;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_resource_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
            content= NULL;
            if (h_string_type == HESSIAN_STRING) {
                content= hessian_string_getstring(h_string);
            }
            if (xacml_resource_setcontent(resource,content) != PEP_XACML_OK) {
                log_error("xacml_resource_unmarshal: can't set content: %s to XACML resource.",content);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
        }
        /* attributes list */
        else if (strcmp(XACML_HESSIAN_RESOURCE_ATTRIBUTES,key) == 0) {
            hessian_object_t * h_attributes= hessian_map_getvalue(h_resource,i);
            size_t h_attributes_l;
            if (hessian_gettype(h_attributes) != HESSIAN_LIST) {
                log_error("xacml_resource_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
            h_attributes_l= hessian_list_length(h_attributes);
            for(j= 0; j<h_attributes_l; j++) {
                hessian_object_t * h_attr= hessian_list_get(h_attributes,j);
                xacml_attribute_t * attribute= NULL;
                if (xacml_attribute_unmarshal(&attribute,h_attr) != PEP_IO_OK) {
                    log_error("xacml_resource_unmarshal: can't unmarshal XACML attribute at: %d.",j);
                    xacml_resource_delete(resource);
                    return PEP_IO_ERROR;
                }
                if (xacml_resource_addattribute(resource,attribute) != PEP_XACML_OK) {
                    log_error("xacml_resource_unmarshal: can't add XACML attribute to XACML resource at: %d",j);
                    xacml_resource_delete(resource);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_resource_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *res= resource;
    return PEP_IO_OK;
}


static const char * decision_str(int decision) {
    switch(decision) {
    case 0:
        return "Deny";
        break;
    case 1:
        return "Permit";
        break;
    case 2:
        return "Indeterminate";
        break;
    case 3:
        return "Not Applicable";
        break;
    default:
        return "Unknown!?!";
        break;
    }
}


/**
 * Dump a XACML request into a file. NULL values are not displayed.
 */
static int dump_xacml_request(FILE * fout, xacml_request_t * request) {
    if (request == NULL) {
        fprintf(fout,"ERROR: dump_xacml_request: request is NULL\n");
        return 1;
    }
    size_t subjects_l= xacml_request_subjects_length(request);
    fprintf(fout,"request: %d subjects\n", (int)subjects_l);
    int i= 0;
    for (i= 0; i<subjects_l; i++) {
        xacml_subject_t * subject= xacml_request_getsubject(request,i);
        const char * category= xacml_subject_getcategory(subject);
        if (category)
            fprintf(fout,"request.subject[%d].category= %s\n", i, category);
        size_t attrs_l= xacml_subject_attributes_length(subject);
        fprintf(fout,"request.subject[%d]: %d attributes\n", i, (int)attrs_l);
        int j= 0;
        for(j= 0; j<attrs_l; j++) {
            xacml_attribute_t * attr= xacml_subject_getattribute(subject,j);
            const char * attr_id= xacml_attribute_getid(attr);
            if (attr_id)
                fprintf(fout,"request.subject[%d].attribute[%d].id= %s\n", i,j,attr_id);
            const char * attr_datatype= xacml_attribute_getdatatype(attr);
            if (attr_datatype)
                fprintf(fout,"request.subject[%d].attribute[%d].datatype= %s\n", i,j,attr_datatype);
            const char * attr_issuer= xacml_attribute_getissuer(attr);
            if (attr_issuer)
                fprintf(fout,"request.subject[%d].attribute[%d].issuer= %s\n", i,j,attr_issuer);
            size_t values_l= xacml_attribute_values_length(attr);
            //fprintf(fout,"request.subject[%d].attribute[%d]: %d values", i,j,(int)values_l);
            int k= 0;
            for (k= 0; k<values_l; k++) {
                const char * attr_value= xacml_attribute_getvalue(attr,k);
                fprintf(fout,"request.subject[%d].attribute[%d].value[%d]= %s\n", i,j,k,attr_value);
            }
        }
    }
    size_t resources_l= xacml_request_resources_length(request);
    fprintf(fout,"request: %d resources\n", (int)resources_l);
    for (i= 0; i<resources_l; i++) {
        xacml_resource_t * resource= xacml_request_getresource(request,i);
        const char * res_content= xacml_resource_getcontent(resource);
        if (res_content)
            fprintf(fout,"request.resource[%d].content= %s\n", i, res_content);
        size_t attrs_l= xacml_resource_attributes_length(resource);
        fprintf(fout,"request.resource[%d]: %d attributes\n", i, (int)attrs_l);
        int j= 0;
        for(j= 0; j<attrs_l; j++) {
            xacml_attribute_t * attr= xacml_resource_getattribute(resource,j);
            const char * attr_id= xacml_attribute_getid(attr);
            if (attr_id)
                fprintf(fout,"request.resource[%d].attribute[%d].id= %s\n", i,j,attr_id);
            const char * attr_datatype= xacml_attribute_getdatatype(attr);
            if (attr_datatype)
                fprintf(fout,"request.resource[%d].attribute[%d].datatype= %s\n", i,j,attr_datatype);
            const char * attr_issuer= xacml_attribute_getissuer(attr);
            if (attr_issuer)
                fprintf(fout,"request.resource[%d].attribute[%d].issuer= %s\n", i,j,attr_issuer);
            size_t values_l= xacml_attribute_values_length(attr);
            //fprintf(fout,"request.resource[%d].attribute[%d]: %d values", i,j,(int)values_l);
            int k= 0;
            for (k= 0; k<values_l; k++) {
                const char * attr_value= xacml_attribute_getvalue(attr,k);
                if (attr_value)
                    fprintf(fout,"request.resource[%d].attribute[%d].value[%d]= %s\n", i,j,k,attr_value);
            }
        }
    }
    int j= 0;
    xacml_action_t * action= xacml_request_getaction(request);
    if (action) {
        size_t act_attrs_l= xacml_action_attributes_length(action);
        fprintf(fout,"request.action: %d attributes\n",(int)act_attrs_l);
        for (j= 0; j<act_attrs_l; j++) {
            xacml_attribute_t * attr= xacml_action_getattribute(action,j);
            const char * attr_id= xacml_attribute_getid(attr);
            if (attr_id)
                fprintf(fout,"request.action.attribute[%d].id= %s\n", j,attr_id);
            const char * attr_datatype= xacml_attribute_getdatatype(attr);
            if (attr_datatype)
                fprintf(fout,"request.action.attribute[%d].datatype= %s\n", j,attr_datatype);
            const char * attr_issuer= xacml_attribute_getissuer(attr);
            if (attr_issuer)
                fprintf(fout,"request.action.attribute[%d].issuer= %s\n", j,attr_issuer);
            size_t values_l= xacml_attribute_values_length(attr);
            //fprintf(fout,"request.action.attribute[%d]: %d values", j,(int)values_l);
            int k= 0;
            for (k= 0; k<values_l; k++) {
                const char * attr_value= xacml_attribute_getvalue(attr,k);
                if (attr_value)
                    fprintf(fout,"request.action.attribute[%d].value[%d]= %s\n",j,k,attr_value);
            }
        }
    }
    xacml_environment_t * env= xacml_request_getenvironment(request);
    if (env) {
        size_t env_attrs_l= xacml_environment_attributes_length(env);
        fprintf(fout,"request.environment: %d attributes\n",(int)env_attrs_l);
        for (j= 0; j<env_attrs_l; j++) {
            xacml_attribute_t * attr= xacml_environment_getattribute(env,j);
            const char * attr_id= xacml_attribute_getid(attr);
            if (attr_id)
                fprintf(fout,"request.environment.attribute[%d].id= %s\n", j,attr_id);
            const char * attr_datatype= xacml_attribute_getdatatype(attr);
            if (attr_datatype)
                fprintf(fout,"request.environment.attribute[%d].datatype= %s\n", j,attr_datatype);
            const char * attr_issuer= xacml_attribute_getissuer(attr);
            if (attr_issuer)
                fprintf(fout,"request.environment.attribute[%d].issuer= %s\n", j,attr_issuer);
            size_t values_l= xacml_attribute_values_length(attr);
            //fprintf(fout,"request.environment.attribute[%d]: %d values", j,(int)values_l);
            int k= 0;
            for (k= 0; k<values_l; k++) {
                const char * attr_value= xacml_attribute_getvalue(attr,k);
                if (attr_value)
                    fprintf(fout,"request.environment.attribute[%d].value[%d]= %s\n",j,k,attr_value);
            }
        }
    }
    return 0;
}

static int dump_xacml_response(FILE * fout,xacml_response_t * response) {
    if (response == NULL) {
        fprintf(fout,"ERROR: dump_xacml_response: response is NULL\n");
        return 1;
    }
    size_t results_l= xacml_response_results_length(response);
    fprintf(fout,"response: %d results\n", (int)results_l);
    int i= 0;
    for(i= 0; i<results_l; i++) {
        xacml_result_t * result= xacml_response_getresult(response,i);
        fprintf(fout,"response.result[%d].decision= %s\n", i, decision_str(xacml_result_getdecision(result)));

        fprintf(fout,"response.result[%d].resourceid= %s\n", i, xacml_result_getresourceid(result));
        xacml_status_t * status= xacml_result_getstatus(result);
        fprintf(fout,"response.result[%d].status.message= %s\n", i, xacml_status_getmessage(status));
        xacml_statuscode_t * statuscode= xacml_status_getcode(status);
        fprintf(fout,"response.result[%d].status.code.value= %s\n", i, xacml_statuscode_getvalue(statuscode));
        xacml_statuscode_t * subcode= xacml_statuscode_getsubcode(statuscode);
        if (subcode != NULL) {
            fprintf(fout,"response.result[%d].status.code.subcode.value= %s\n", i, xacml_statuscode_getvalue(subcode));
        }
        size_t obligations_l= xacml_result_obligations_length(result);
        fprintf(fout,"response.result[%d]: %d obligations\n", i, (int)obligations_l);
        int j=0;
        for(j= 0; j<obligations_l; j++) {
            xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
            fprintf(fout,"response.result[%d].obligation[%d].id= %s\n",i,j, xacml_obligation_getid(obligation));
            fprintf(fout,"response.result[%d].obligation[%d].fulfillOn= %s\n",i,j, decision_str(xacml_obligation_getfulfillon(obligation)));
            size_t attrs_l= xacml_obligation_attributeassignments_length(obligation);
            fprintf(fout,"response.result[%d].obligation[%d]: %d attribute assignments\n",i,j,(int)attrs_l);
            int k= 0;
            for (k= 0; k<attrs_l; k++) {
                xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
                fprintf(fout,"response.result[%d].obligation[%d].attributeassignment[%d].id= %s\n",i,j,k,xacml_attributeassignment_getid(attr));
                fprintf(fout,"response.result[%d].obligation[%d].attributeassignment[%d].datatype= %s\n",i,j,k,xacml_attributeassignment_getdatatype(attr));
                fprintf(fout,"response.result[%d].obligation[%d].attributeassignment[%d].value= %s\n",i,j,k,xacml_attributeassignment_getvalue(attr));
            }
        }
    }
    return 0;
}



int main(void) {
    BUFFER * buffer;
    double din1, din2, dout1, dout2;
    int64_t lin1, lin2, lout1, lout2;
    char * sin1, *sin2;
    const char * sout1, * sout2;
    hessian_object_t * h_din1, * h_din2, * h_dout1, * h_dout2, * h_lin1, * h_lin2, * h_lout1, * h_lout2, * h_sin1, * h_sin2, * h_sout2, * h_sout1;
    din1= -0.678687687;
    din2= 123423.3423;
    lin1= 979798797798;
    lin2= -65422454;
    sin1= "";
    sin2= "heelo";
    printf("din1: %f\n",din1);
    printf("din2: %f\n",din2);
    h_din1= hessian_create(HESSIAN_DOUBLE, din1);
    h_din2= hessian_create(HESSIAN_DOUBLE, din2);
    printf("lin1: %ld\n",lin1);
    printf("lin2: %ld\n",lin2);
    h_lin1= hessian_create(HESSIAN_LONG, lin1);
    h_lin2= hessian_create(HESSIAN_LONG, lin2);
    printf("sin1: '%s'\n",sin1); 
    printf("sin2: '%s'\n",sin2); 
    h_sin1= hessian_create(HESSIAN_STRING, sin1);
    h_sin2= hessian_create(HESSIAN_STRING, sin2);
    buffer= buffer_create(1024);
    
    printf("serialize...\n");
    hessian_serialize(h_din1,buffer);
    hessian_serialize(h_din2,buffer);
    hessian_serialize(h_sin1,buffer);
    hessian_serialize(h_sin2,buffer);
    hessian_serialize(h_lin1,buffer);
    hessian_serialize(h_lin2,buffer);
    
    printf("deserialize...\n");
    h_dout1= hessian_deserialize(buffer);
    h_dout2= hessian_deserialize(buffer);
    h_sout1= hessian_deserialize(buffer);
    h_sout2= hessian_deserialize(buffer);
    h_lout1= hessian_deserialize(buffer);
    h_lout2= hessian_deserialize(buffer);

    dout1= hessian_double_getvalue(h_dout1);
    dout2= hessian_double_getvalue(h_dout2);
    lout1= hessian_long_getvalue(h_lout1);
    lout2= hessian_long_getvalue(h_lout2);
    sout1= hessian_string_getstring(h_sout1);
    sout2= hessian_string_getstring(h_sout2);
    printf("dout1: %f\n",dout1);
    printf("dout2: %f\n",dout2);
    printf("lout1: %ld\n",lout1);
    printf("lout2: %ld\n",lout2);
    printf("sout1: '%s'\n",sout1);
    printf("sout2: '%s'\n",sout2);
    
    printf("tests...\n");
    if (din1 != dout1) {
        printf("din1 (%f) != dout1 (%f)\n",din1,dout1);
        return 1;
    }
    if (din2 != dout2) {
        printf("din2 (%f) != dout2 (%f)\n",din2,dout2);
        return 1;
    }
    if (lin1 != lout1) {
        printf("lin1 (%ld) != lout1 (%ld)\n",lin1,lout1);
        return 2;
    }
    if (lin2 != lout2) {
        printf("lin2 (%ld) != lout2 (%ld)\n",lin2,lout2);
        return 2;
    }
    if (strcmp(sin1,sout1) != 0) {
        printf("sin1 '%s' != sout1 '%s'\n",sin1,sout1);
        return 3;
    }
    if (strcmp(sin2,sout2) != 0) {
        printf("sin2 '%s' != sout2 '%s'\n",sin2,sout2);
        return 3;
    }

/*
    const char * filename= "/tmp/java_hessian_resource.data";
    FILE * file;
    printf("test deserialization from Java serialization. Data: %s\n", filename);
    file= fopen(filename,"r");
    if (file==NULL) {
        printf("failed to open file: %s\n", filename);
        return 4;
    }
    buffer_reset(buffer);
    size_t size= buffer_fread(buffer,file);
    printf("%d bytes read from %s\n", (int)size, filename);
    
    log_setout(stderr);
    log_setlevel(LOG_LEVEL_TRACE);
    
    printf("deserialize hessian resource...\n");

    hessian_object_t * h_resource= hessian_deserialize(buffer);
    xacml_resource_t * resource;
    int io_rc= xacml_resource_unmarshal(&resource, h_resource);
    printf("result: %d\n",io_rc);
*/    



    printf("using: %s\n",pep_version());
    printf("base64 decoding and hessian deserialization test...\n");

    log_setout(stderr);
    log_setlevel(LOG_LEVEL_TRACE);

    const char * b64filename= "b64input1";
    FILE * b64file= fopen(b64filename,"r");
    if (b64file==NULL) {
        printf("failed to open b64 file: %s\n", b64filename);
        return 5;
    }
    BUFFER * b64input= buffer_create(1024);
    if (b64input==NULL) {
        printf("failed to create b64input buffer\n");
        return 5;
    }
    size_t size= buffer_fread(b64input,b64file);
    printf("%d bytes read from %s\n", (int)size, b64filename);
    
    BUFFER * input= buffer_create(1024);
    if (input==NULL) {
        printf("failed to create input buffer\n");
        return 5;
    }
    
    printf("base64 decode input buffer...\n");
    base64_decode(b64input,input);
    size= buffer_length(input);
    printf("%d bytes available in input buffer\n", (int)size);

    /* unmarshal the PEP response */
    xacml_response_t * response;
    pep_error_t unmarshal_rc= xacml_response_unmarshalling(&response,input);
    if ( unmarshal_rc != PEP_OK) {
        printf("pep_authorize: can't unmarshal the XACML response: %s.", pep_strerror(unmarshal_rc));
        buffer_delete(b64input);
        buffer_delete(input);
        return unmarshal_rc;
    }

    printf("hessian response deserialized from: %s.\n", b64filename);
    dump_xacml_request(stdout, xacml_response_getrequest(response));
    dump_xacml_response(stdout,response);


    printf("buffer tests...\n");
    BUFFER * buf= buffer_create(1024);
    buffer_delete(buf);
    if (buf == NULL) printf("buf is NULL\n");
    else printf("buf not NULL\n");

    return 0;
}
