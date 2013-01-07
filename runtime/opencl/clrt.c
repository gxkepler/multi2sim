/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "clrt.h"
#include "debug.h"
#include "event.h"
#include "mhandle.h"
#include "object.h"



/* Error messages */

char *opencl_err_not_impl =
	"\tThis error message is reported by the Multi2Sim OpenCL Runtime library linked\n"
	"\tto your OpenCL application. The runtime only supports partial implementation\n"
	"\tof OpenCL. To request support for this feature, please email\n"
	"\t'development@multi2sim.org'.\n";

char *opencl_err_note =
	"\tThis error message is generated by the Multi2Sim OpenCL Runtime library linked\n"
	"\twith your OpenCL host application.\n"
	"\tThis implementation only provides a subset of the OpenCL specification. Please\n"
	"\temail 'development@multi2sim.org' for further support.\n";

char *opencl_err_param_note =
	"\tThis error message is generated by the Multi2Sim OpenCL Runtime library linked\n"
	"\twith your OpenCL host application.\n"
	"\tWhile a complete OpenCL implementation would return an error code to your\n"
	"\tapplication, the Multi2Sim OpenCL library will make your program fail with an\n"
	"\terror code.\n";


/* Native mode */
int opencl_native_mode;



/*
 * Debug
 *
 * If environment variable 'opencl_DEBUG' is set, the Multi2Sim OpenCL Runtime
 * library will dump debug information about OpenCL calls, argument values,
 * intermeidate actions, and return values.
 */

static int opencl_debug_initialized;
static int opencl_debugging;

void opencl_debug(char *fmt, ...)
{
	va_list va;
	char *value;

	/* Initialize debug */
	if (!opencl_debug_initialized)
	{
		opencl_debug_initialized = 1;
		value = getenv("OPENCL_DEBUG");
		if (value && !strcmp(value, "1"))
			opencl_debugging = 1;
	}

	/* Exit if not debugging */
	if (!opencl_debugging)
		return;

	/* Dump debug message */
	va_start(va, fmt);
	fprintf(stderr, "[libm2s-opencl] ");
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
}


cl_int opencl_set_param(const void *src_value, size_t src_size,
	size_t dest_size, void *dest_value, size_t *size_ret)
{
	assert(src_value);
	if (dest_value && src_size > dest_size)
		return CL_INVALID_VALUE;
	if (size_ret)
		*size_ret = src_size;
	if (dest_value)
		memcpy(dest_value, src_value, src_size);
	return CL_SUCCESS;
}


cl_int opencl_set_string(const char *src_string, size_t dest_size,
	void *dest_string, size_t *size_ret)
{
	size_t src_size = strlen(src_string) + 1;
	return opencl_set_param(src_string, src_size, dest_size,
		dest_string, size_ret);
}


int opencl_is_valid_device_type(cl_device_type device_type)
{
	return device_type == CL_DEVICE_TYPE_ALL
		|| (device_type & 
			(CL_DEVICE_TYPE_GPU 
			| CL_DEVICE_TYPE_CPU 
			| CL_DEVICE_TYPE_ACCELERATOR 
			| CL_DEVICE_TYPE_DEFAULT));
}


int opencl_event_wait_list_check(
	unsigned int num_events, 
	const cl_event *event_wait_list)
{
	int i;

	if ((!event_wait_list && num_events) 
		|| (event_wait_list && !num_events))
		return CL_INVALID_EVENT_WAIT_LIST;

	/* Verify that the parameter list is valid up-front */
	for (i = 0; i < num_events; i++)
	{
		if (!opencl_object_verify(event_wait_list[i], OPENCL_OBJECT_EVENT))
			return CL_INVALID_EVENT_WAIT_LIST;
	}
	return CL_SUCCESS;
}








/*
 * OpenCL Interface Functions
 */

void *clGetExtensionFunctionAddress(
	const char *func_name)
{
	__OPENCL_NOT_IMPL__
	return NULL;
}

