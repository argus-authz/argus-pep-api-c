/*
 * Copyright 2008 Members of the EGEE Collaboration.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id: xml.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include "hessian/hessian.h"

/**
 * Uses string generic methods
 */
extern OBJECT_CTOR(hessian_string);
extern OBJECT_DTOR(hessian_string);
extern OBJECT_SERIALIZE(hessian_string);
extern OBJECT_DESERIALIZE(hessian_string);

/**
 * Initializes and registers the HessianXml class.
 */
static const hessian_class_t _hessian_xml_descr = {
    HESSIAN_XML,
    "hessian.Xml",
    sizeof(hessian_xml_t),
    'X', 'x',
    hessian_string_ctor,
    hessian_string_dtor,
    hessian_string_serialize,
    hessian_string_deserialize
};

const void * hessian_xml_class = &_hessian_xml_descr;

size_t hessian_xml_utf8_length(const hessian_object_t * xml) {
	return hessian_string_utf8_length(xml);
}
size_t hessian_xml_length(const hessian_object_t * xml) {
	return hessian_string_length(xml);
}
const char * hessian_xml_getxml(const hessian_object_t *xml) {
	return hessian_string_getstring(xml);
}
