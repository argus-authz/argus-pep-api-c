#include <stdio.h>
#include <string.h>

#include "argus/xacml.h"
#include "argus/io.h"
#include "hessian/hessian.h"
#include "util/buffer.h"
#include "util/log.h"

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
    

    return 0;
}
