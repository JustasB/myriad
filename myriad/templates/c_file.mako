## Add lib includes
% for lib in lib_includes:
#include <${lib}>
% endfor

## Add local includes
% for lib in local_includes:
#include "${lib}"
% endfor

#include "${obj_name}.h"

## Process instance methods
<%
instance_methods = [m.from_myriad_func(m, obj_name + "_" + m.ident) for m in myriad_methods.values()]
%>

## Print methods forward declarations
% for mtd in instance_methods:
${mtd.stringify_decl()};
% endfor

## Global vtables
% for method in own_methods:
#ifdef CUDA
__device__ ${method.typedef_name} ${obj_name}_${method.ident}_devp = ${obj_name}_${method.ident};
#endif

#ifdef CUDA
__constant__ const ${method.typedef_name} ${method.ident}_vtable[NUM_CU_CLASS] = { NULL };
#else
const ${method.typedef_name} ${method.ident}_vtable[NUM_CU_CLASS] = {
    ## For each class in all Myriad classes, use NULL if it's a non-child class and if
    ## the subclass has overwritten the method. Otherwise, use the subclass' version
    ## of our method
    % for mclass, subclasses in inheritors_dict.items():
        % if mclass.obj_name == obj_name or mclass in our_subclasses:
            &${mclass.obj_name}_${method.ident},
        % else:
            NULL,
        % endif
    % endfor
};
#endif
% endfor

## Method definitions
% for mtd in instance_methods:
${mtd.stringify_decl()}
{
${mtd.stringify_def()}
}

% endfor

## Method delegators
% for delg, super_delg in own_method_delgs:
${delg.stringify_def()}

${super_delg.stringify_def()}
% endfor

## Top-level init functions
${init_functions}
